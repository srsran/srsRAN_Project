/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "cu_up_impl.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/gateways/udp_network_gateway_factory.h"
#include "srsran/gtpu/gtpu_demux_factory.h"
#include "srsran/gtpu/gtpu_teid_pool_factory.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/support/io/io_broker.h"

using namespace srsran;
using namespace srs_cu_up;

void assert_cu_up_configuration_valid(const cu_up_configuration& cfg)
{
  srsran_assert(cfg.cu_up_executor != nullptr, "Invalid CU-UP executor");
  srsran_assert(cfg.e1ap_notifier != nullptr, "Invalid E1AP notifier");
  srsran_assert(cfg.f1u_gateway != nullptr, "Invalid F1-U connector");
  srsran_assert(cfg.epoll_broker != nullptr, "Invalid IO broker");
}

cu_up::cu_up(const cu_up_configuration& config_) : cfg(config_), main_ctrl_loop(128)
{
  assert_cu_up_configuration_valid(cfg);

  /// > Create upper layers

  // Create NG-U gateway
  udp_network_gateway_config ngu_gw_config = {};
  ngu_gw_config.bind_address               = cfg.net_cfg.n3_bind_addr;
  ngu_gw_config.bind_port                  = cfg.net_cfg.n3_bind_port;
  // other params

  udp_network_gateway_creation_message ngu_gw_msg = {ngu_gw_config, gw_data_gtpu_demux_adapter};
  ngu_gw                                          = create_udp_network_gateway(ngu_gw_msg);
  if (not ngu_gw->create_and_bind()) {
    logger.error("Failed to create and connect NG-U gateway.");
  }
  cfg.epoll_broker->register_fd(ngu_gw->get_socket_fd(), [this](int fd) { ngu_gw->receive(); });
  gtpu_gw_adapter.connect_network_gateway(*ngu_gw);

  // Create GTP-U demux and TEID allocator
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cu_up_exec                  = cfg.gtpu_pdu_executor;
  ngu_demux                             = create_gtpu_demux(demux_msg);

  gtpu_allocator_creation_request f1u_alloc_msg = {};
  f1u_alloc_msg.max_nof_teids                   = MAX_NOF_UES * MAX_NOF_PDU_SESSIONS;
  f1u_teid_allocator                            = create_gtpu_allocator(f1u_alloc_msg);

  /// > Connect layers
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  /// > Create e1ap
  e1ap = create_e1ap(*cfg.e1ap_notifier, e1ap_cu_up_ev_notifier, *cfg.cu_up_executor);
  e1ap_cu_up_ev_notifier.connect_cu_up(*this);

  /// > Create UE manager
  ue_mng = std::make_unique<ue_manager>(cfg.net_cfg,
                                        *e1ap,
                                        *cfg.timers,
                                        *cfg.f1u_gateway,
                                        gtpu_gw_adapter,
                                        *ngu_demux,
                                        *f1u_teid_allocator,
                                        *cfg.cu_up_executor,
                                        logger);
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

  response.gnb_cu_up_id   = cfg.cu_up_id;
  response.gnb_cu_up_name = cfg.cu_up_name;

  // We only support 5G
  response.cn_support = "c-5gc";

  supported_plmns_item_t plmn_item;
  plmn_item.plmn_id = plmn_string_to_bcd(cfg.plmn);
  response.supported_plmns.push_back(plmn_item);

  return response;
}

void process_successful_pdu_resource_setup_mod_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_setup_modification_item>&
                                    pdu_session_resource_setup_list,
    const pdu_session_setup_result& result)
{
  if (result.success) {
    e1ap_pdu_session_resource_setup_modification_item res_setup_item;
    res_setup_item.pdu_session_id    = result.pdu_session_id;
    res_setup_item.ng_dl_up_tnl_info = result.gtp_tunnel;
    for (const auto& drb_setup_item : result.drb_setup_results) {
      if (drb_setup_item.success) {
        e1ap_drb_setup_item_ng_ran res_drb_setup_item;
        res_drb_setup_item.drb_id = drb_setup_item.drb_id;

        e1ap_up_params_item up_param_item;
        up_param_item.up_tnl_info = drb_setup_item.gtp_tunnel;
        res_drb_setup_item.ul_up_transport_params.push_back(up_param_item);

        for (const auto& flow_item : drb_setup_item.qos_flow_results) {
          if (flow_item.success) {
            e1ap_qos_flow_item res_flow_setup_item;
            res_flow_setup_item.qos_flow_id = flow_item.qos_flow_id;
            res_drb_setup_item.flow_setup_list.emplace(flow_item.qos_flow_id, res_flow_setup_item);
          } else {
            e1ap_qos_flow_failed_item res_flow_failed_item;
            res_flow_failed_item.qos_flow_id = flow_item.qos_flow_id;
            res_flow_failed_item.cause       = flow_item.cause;
            res_drb_setup_item.flow_failed_list.emplace(flow_item.qos_flow_id, res_flow_failed_item);
          }
        }
        res_setup_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, res_drb_setup_item);
      } else {
        e1ap_drb_failed_item_ng_ran asn1_drb_failed_item;
        asn1_drb_failed_item.drb_id = drb_setup_item.drb_id;
        asn1_drb_failed_item.cause  = drb_setup_item.cause;

        res_setup_item.drb_failed_list_ng_ran.emplace(drb_setup_item.drb_id, asn1_drb_failed_item);
      }
    }
    pdu_session_resource_setup_list.emplace(result.pdu_session_id, res_setup_item);
  }
}

void process_successful_pdu_resource_modification_outcome(
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_modified_item>& pdu_session_resource_modified_list,
    slotted_id_vector<pdu_session_id_t, e1ap_pdu_session_resource_failed_item>&
                                           pdu_session_resource_failed_to_modify_list,
    const pdu_session_modification_result& result,
    const srslog::basic_logger&            logger)
{
  if (result.success) {
    e1ap_pdu_session_resource_modified_item modified_item;
    modified_item.pdu_session_id = result.pdu_session_id;

    for (const auto& drb_setup_item : result.drb_setup_results) {
      logger.debug("Adding DRB setup result item. {}, success={}", drb_setup_item.drb_id, drb_setup_item.success);
      if (drb_setup_item.success) {
        e1ap_drb_setup_item_ng_ran res_drb_setup_item;
        res_drb_setup_item.drb_id = drb_setup_item.drb_id;

        e1ap_up_params_item up_param_item;
        up_param_item.up_tnl_info = drb_setup_item.gtp_tunnel;
        res_drb_setup_item.ul_up_transport_params.push_back(up_param_item);

        for (const auto& flow_item : drb_setup_item.qos_flow_results) {
          if (flow_item.success) {
            e1ap_qos_flow_item res_flow_setup_item;
            res_flow_setup_item.qos_flow_id = flow_item.qos_flow_id;
            res_drb_setup_item.flow_setup_list.emplace(flow_item.qos_flow_id, res_flow_setup_item);
          } else {
            e1ap_qos_flow_failed_item res_flow_failed_item;
            res_flow_failed_item.qos_flow_id = flow_item.qos_flow_id;
            res_flow_failed_item.cause       = flow_item.cause;
            res_drb_setup_item.flow_failed_list.emplace(flow_item.qos_flow_id, res_flow_failed_item);
          }
        }
        modified_item.drb_setup_list_ng_ran.emplace(drb_setup_item.drb_id, res_drb_setup_item);
      } else {
        e1ap_drb_failed_item_ng_ran asn1_drb_failed_item;
        asn1_drb_failed_item.drb_id = drb_setup_item.drb_id;
        asn1_drb_failed_item.cause  = drb_setup_item.cause;

        modified_item.drb_failed_list_ng_ran.emplace(drb_setup_item.drb_id, asn1_drb_failed_item);
      }
    }
    for (const auto& drb_modified_item : result.drb_modification_results) {
      logger.debug(
          "Adding DRB modified result item. {}, success={}", drb_modified_item.drb_id, drb_modified_item.success);
      e1ap_drb_modified_item_ng_ran e1ap_mod_item;
      e1ap_mod_item.drb_id = drb_modified_item.drb_id;

      e1ap_up_params_item up_param_item;
      up_param_item.up_tnl_info = drb_modified_item.gtp_tunnel;
      e1ap_mod_item.ul_up_transport_params.push_back(up_param_item);
      modified_item.drb_modified_list_ng_ran.emplace(e1ap_mod_item.drb_id, e1ap_mod_item);
    }

    pdu_session_resource_modified_list.emplace(modified_item.pdu_session_id, modified_item);
  } else {
    e1ap_pdu_session_resource_failed_item failed_item;
    failed_item.pdu_session_id = result.pdu_session_id;
    failed_item.cause          = cause_t::radio_network;
    pdu_session_resource_failed_to_modify_list.emplace(failed_item.pdu_session_id, failed_item);
  }
}

e1ap_bearer_context_setup_response
cu_up::handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg)
{
  e1ap_bearer_context_setup_response response = {};
  response.ue_index                           = INVALID_UE_INDEX;
  response.success                            = false;

  // 1. Create new UE context
  ue_context_cfg ue_cfg        = {};
  ue_cfg.activity_level        = msg.activity_notif_level;
  ue_cfg.ue_inactivity_timeout = msg.ue_inactivity_timer;
  ue_context* ue_ctxt          = ue_mng->add_ue(ue_cfg);
  if (ue_ctxt == nullptr) {
    logger.error("Could not create UE context");
    return response;
  }
  logger.info("ue={} UE Created", ue_ctxt->get_index());

  // 2. Handle bearer context setup request
  for (const auto& pdu_session : msg.pdu_session_res_to_setup_list) {
    pdu_session_setup_result result = ue_ctxt->setup_pdu_session(pdu_session);
    if (result.success) {
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, result);
    } else {
      e1ap_pdu_session_resource_failed_item res_failed_item;

      res_failed_item.pdu_session_id = result.pdu_session_id;
      res_failed_item.cause          = result.cause;

      response.pdu_session_resource_failed_list.emplace(result.pdu_session_id, res_failed_item);
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
  logger.debug("Handling bearer context modification request ue={}", msg.ue_index);

  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("Could not find UE context");
    return {};
  }

  e1ap_bearer_context_modification_response response = {};
  response.ue_index                                  = ue_ctxt->get_index();
  response.success                                   = true;

  bool new_ul_tnl_info_required = msg.new_ul_tnl_info_required == std::string("required");

  if (msg.ng_ran_bearer_context_mod_request.has_value()) {
    // Traverse list of PDU sessions to be setup/modified
    for (const auto& pdu_session_item :
         msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_setup_mod_list) {
      logger.debug("Setup/Modification of PDU session id {}", pdu_session_item.pdu_session_id);
      pdu_session_setup_result session_result = ue_ctxt->setup_pdu_session(pdu_session_item);
      process_successful_pdu_resource_setup_mod_outcome(response.pdu_session_resource_setup_list, session_result);
      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be modified.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_modify_list) {
      logger.debug("Modifying PDU session id {}", pdu_session_item.pdu_session_id);
      pdu_session_modification_result session_result =
          ue_ctxt->modify_pdu_session(pdu_session_item, new_ul_tnl_info_required);
      process_successful_pdu_resource_modification_outcome(response.pdu_session_resource_modified_list,
                                                           response.pdu_session_resource_failed_to_modify_list,
                                                           session_result,
                                                           logger);
      logger.debug("Modification {}", session_result.success ? "successful" : "failed");

      response.success &= session_result.success; // Update final result.
    }

    // Traverse list of PDU sessions to be removed.
    for (const auto& pdu_session_item : msg.ng_ran_bearer_context_mod_request.value().pdu_session_res_to_rem_list) {
      logger.info("Removing PDU session id {}", pdu_session_item);
      ue_ctxt->remove_pdu_session(pdu_session_item);
    }
  } else {
    logger.warning("Ignoring empty Bearer Context Modification Request.");
  }

  // 3. Create response

  response.success = true;

  return response;
}

void cu_up::handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg)
{
  ue_context* ue_ctxt = ue_mng->find_ue(msg.ue_index);
  if (ue_ctxt == nullptr) {
    logger.error("ue={} not found. Discarding E1 Bearer Context Release Command.", msg.ue_index);
    return;
  }
  ue_mng->remove_ue(msg.ue_index);
}

void cu_up::on_e1ap_connection_establish()
{
  e1ap_connected = true;
}

void cu_up::on_e1ap_connection_drop()
{
  e1ap_connected = false;
}
