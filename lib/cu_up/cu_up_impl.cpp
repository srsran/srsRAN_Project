/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_impl.h"
#include "srsgnb/e1/cu_up/e1_config_converters.h"
#include "srsgnb/e1/cu_up/e1_cu_up_factory.h"
#include "srsgnb/gateways/network_gateway_factory.h"
#include "srsgnb/gtpu/gtpu_demux_factory.h"
#include "srsgnb/support/io_broker/io_broker_factory.h"

using namespace srsgnb;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsgnb_assert(cfg.cu_up_executor != nullptr, "Invalid CU-UP executor");
  srsgnb_assert(cfg.e1_notifier != nullptr, "Invalid E1 notifier");
  srsgnb_assert(cfg.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsgnb_assert(cfg.epoll_broker != nullptr, "Invalid IO broker");
}

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  /// > Create upper layers

  // Create NG-U gateway
  // TODO: Refactor to use UPF IP that we get from E1
  network_gateway_config ngu_gw_config = {};
  ngu_gw_config.type                   = network_gateway_type::udp;
  ngu_gw_config.connect_address        = cfg.upf_addr;
  ngu_gw_config.connect_port           = 2152; ///< TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs
  ngu_gw_config.bind_address           = cfg.gtp_bind_addr;
  ngu_gw_config.bind_port              = 2152; ///< TS 29.281 Sec. 4.4.2.3 Encapsulated T-PDUs
  // other params

  network_gateway_creation_message ngu_gw_msg = {ngu_gw_config, gw_ctrl_gtpu_demux_adapter, gw_data_gtpu_demux_adapter};
  ngu_gw                                      = create_network_gateway(ngu_gw_msg);
  if (not ngu_gw->create_and_bind()) {
    logger.error("Failed to create and connect NG-U gateway.");
  }
  cfg.epoll_broker->register_fd(ngu_gw->get_socket_fd(), [this](int fd) { ngu_gw->receive(); });
  gtpu_gw_adapter.connect_network_gateway(*ngu_gw);

  // Create GTP-U demux
  ngu_demux = create_gtpu_demux();

  /// > Connect layers

  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(logger, timers, *cfg.f1u_gateway, gtpu_gw_adapter, *ngu_demux);

  // create e1ap
  e1 = create_e1(*cfg.e1_notifier, e1_cu_up_ev_notifier);
  e1_cu_up_ev_notifier.connect_cu_up(*this);
}

cu_up::~cu_up()
{
  if (ngu_gw) {
    cfg.epoll_broker->unregister_fd(ngu_gw->get_socket_fd());
  }
}

cu_cp_e1_setup_response cu_up::handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& msg)
{
  cu_cp_e1_setup_response response;
  response.success = true;

  response.response->gnb_cu_up_id.value     = cfg.cu_up_id;
  response.response->gnb_cu_up_name_present = true;
  response.response->gnb_cu_up_name.value.from_string(cfg.cu_up_name);

  // We only support 5G
  response.response->cn_support.value = asn1::e1ap::cn_support_opts::options::c_5gc;

  asn1::e1ap::supported_plmns_item_s plmn_item;
  plmn_item.plmn_id.from_string(cfg.plmn);
  response.response->supported_plmns.value.push_back(plmn_item);

  return response;
}

e1ap_bearer_context_setup_response
cu_up::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
{
  e1ap_bearer_context_setup_response response = {};
  response.ue_index                           = INVALID_UE_INDEX;
  response.success                            = false;

  // 1. Create new UE context
  ue_context* ue_ctxt = ue_mng->add_ue();
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  logger.info("UE Created (ue_index={})", ue_ctxt->get_index());

  // 2. Handle bearer context setup request
  for (const auto& bearer_ctx_setup_request : msg.request.ng_ran_bearer_context_setup_request()) {
    const auto& pdu_session_list = bearer_ctx_setup_request->pdu_session_res_to_setup_list();
    for (const auto& pdu_session : pdu_session_list) {
      pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session);
    }
  }

  // 3. Create response
  response.ue_index = ue_ctxt->get_index();

  response.success = true;

  return response;
}

e1ap_bearer_context_modification_response
cu_up::handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg)
{
  e1ap_bearer_context_modification_response response = {};
  response.ue_index                                  = INVALID_UE_INDEX;
  response.success                                   = false;

  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return response;
  }

  const auto& bearer_ctx_modification_request = msg.request.ng_ran_bearer_context_mod_request();

  // pdu session resource to modify list
  if (bearer_ctx_modification_request.pdu_session_res_to_modify_list_present) {
    for (const auto& pdu_session_item : bearer_ctx_modification_request.pdu_session_res_to_modify_list.value) {
      pdu_session_modification_result result = ue_ctxt->modify_pdu_session(pdu_session_item);
    }
  }

  // 3. Create response
  response.ue_index = ue_ctxt->get_index();

  response.success = true;

  return response;
}

void cu_up::on_e1_connection_establish()
{
  e1_connected = true;
}

void cu_up::on_e1_connection_drop()
{
  e1_connected = false;
}
