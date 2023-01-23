/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include "srsgnb/gtpu/gtpu_demux.h"
#include "srsgnb/gtpu/gtpu_tunnel_tx.h"
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>

constexpr auto default_wait_timeout = std::chrono::seconds(3);

namespace srsgnb {

/// Dummy GTP-U Rx Demux
class dummy_gtpu_demux_ctrl : public gtpu_demux_ctrl
{
public:
  dummy_gtpu_demux_ctrl()  = default;
  ~dummy_gtpu_demux_ctrl() = default;

  bool add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override { return true; }

  bool remove_tunnel(uint32_t teid) override { return true; }
};

/// Dummy adapter between GTP-U and Network Gateway
class dummy_gtpu_network_gateway_adapter final : public gtpu_tunnel_tx_upper_layer_notifier
{
public:
  dummy_gtpu_network_gateway_adapter()           = default;
  ~dummy_gtpu_network_gateway_adapter() override = default;

  void on_new_pdu(byte_buffer /*buf*/, const ::sockaddr_storage& /*addr*/) override {}
};

class dummy_f1u_bearer final : public srs_cu_up::f1u_bearer,
                               public srs_cu_up::f1u_rx_pdu_handler,
                               public srs_cu_up::f1u_tx_sdu_handler
{
private:
  srs_cu_up::f1u_rx_sdu_notifier* rx_sdu_notifier = nullptr;

  std::list<pdcp_tx_pdu>  tx_sdu_list;
  std::mutex              tx_sdu_mutex;
  std::condition_variable tx_sdu_cv;

public:
  std::list<uint32_t> tx_discard_sdu_list;

  dummy_f1u_bearer()  = default;
  ~dummy_f1u_bearer() = default;

  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() override { return *this; }
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() override { return *this; }

  void connect_f1u_rx_sdu_notifier(srs_cu_up::f1u_rx_sdu_notifier& rx_sdu_notifier_)
  {
    rx_sdu_notifier = &rx_sdu_notifier_;
  }

  void handle_pdu(nru_ul_message msg) final
  {
    // Forward T-PDU to PDCP
    srsgnb_assert(rx_sdu_notifier != nullptr, "The rx_sdu_notifier must not be a nullptr!");
    rx_sdu_notifier->on_new_sdu(std::move(msg.t_pdu));
  }

  void discard_sdu(uint32_t count) final { tx_discard_sdu_list.push_back(count); };

  void handle_sdu(pdcp_tx_pdu sdu) final
  {
    std::lock_guard<std::mutex> lock(tx_sdu_mutex);
    tx_sdu_list.push_back(std::move(sdu));
    tx_sdu_cv.notify_one();
  }

  pdcp_tx_pdu wait_tx_sdu()
  {
    std::unique_lock<std::mutex> lock(tx_sdu_mutex);
    if (tx_sdu_cv.wait_for(lock, default_wait_timeout, [&]() { return !tx_sdu_list.empty(); })) {
      // success
      pdcp_tx_pdu sdu = std::move(tx_sdu_list.front());
      tx_sdu_list.pop_front();
      return sdu;
    } else {
      // timeout
      pdcp_tx_pdu sdu = {};
      return sdu;
    }
  }

  bool have_tx_sdu()
  {
    std::lock_guard<std::mutex> lock(tx_sdu_mutex);
    return !tx_sdu_list.empty();
  }
};

class dummy_f1u_gateway final : public f1u_cu_up_gateway
{
private:
  dummy_f1u_bearer& bearer;

public:
  explicit dummy_f1u_gateway(dummy_f1u_bearer& bearer_) : bearer(bearer_) {}
  ~dummy_f1u_gateway() override = default;

  srs_cu_up::f1u_bearer* create_cu_bearer(uint32_t                             ue_index,
                                          uint32_t                             ul_teid,
                                          srs_cu_up::f1u_rx_delivery_notifier& cu_delivery,
                                          srs_cu_up::f1u_rx_sdu_notifier&      cu_rx) override
  {
    created_ul_teid_list.push_back(ul_teid);
    bearer.connect_f1u_rx_sdu_notifier(cu_rx);
    return &bearer;
  };
  void attach_dl_teid(uint32_t ul_teid, uint32_t dl_teid) override { attached_ul_teid_list.push_back(ul_teid); };
  void remove_cu_bearer(uint32_t ul_teid) override { removed_ul_teid_list.push_back(ul_teid); };

  std::list<uint32_t> created_ul_teid_list  = {};
  std::list<uint32_t> attached_ul_teid_list = {};
  std::list<uint32_t> removed_ul_teid_list  = {};
};

e1_message generate_bearer_context_setup_request_msg(unsigned int cu_cp_ue_e1_id)
{
  e1_message bearer_context_setup_request = {};

  bearer_context_setup_request.pdu.set_init_msg();
  bearer_context_setup_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_req = bearer_context_setup_request.pdu.init_msg().value.bearer_context_setup_request();
  bearer_context_setup_req->gnb_cu_cp_ue_e1ap_id.value = cu_cp_ue_e1_id;
  bearer_context_setup_req->security_info.value.security_algorithm.ciphering_algorithm =
      asn1::e1ap::ciphering_algorithm_e::nea0;
  bearer_context_setup_req->security_info.value.up_securitykey.encryption_key.from_string(
      "a6ae39efbe0d424cd85f4a9c3aee0414");
  bearer_context_setup_req->ue_dl_aggr_max_bit_rate.value.value = 1000000000U;
  bearer_context_setup_req->serving_plmn.value.from_string("02f899");
  bearer_context_setup_req->activity_notif_level.value = asn1::e1ap::activity_notif_level_e::ue;

  bearer_context_setup_req->sys_bearer_context_setup_request.id   = ASN1_E1AP_ID_SYS_BEARER_CONTEXT_SETUP_REQUEST;
  bearer_context_setup_req->sys_bearer_context_setup_request.crit = asn1::crit_opts::reject;
  bearer_context_setup_req->sys_bearer_context_setup_request.value.set_ng_ran_bearer_context_setup_request();

  auto& ng_ran_bearer_context_setup_req =
      bearer_context_setup_req->sys_bearer_context_setup_request.value.ng_ran_bearer_context_setup_request();

  ng_ran_bearer_context_setup_req.resize(1);

  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_res_to_setup_item = {};
  pdu_session_res_to_setup_item.pdu_session_id                              = 1;
  pdu_session_res_to_setup_item.pdu_session_type                            = asn1::e1ap::pdu_session_type_e::ipv4;
  pdu_session_res_to_setup_item.snssai.sst.from_number(1);
  pdu_session_res_to_setup_item.security_ind.integrity_protection_ind =
      asn1::e1ap::integrity_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.security_ind.confidentiality_protection_ind =
      asn1::e1ap::confidentiality_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr_present = true;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr         = 100000000U;

  pdu_session_res_to_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel();
  auto& gtp_tunnel = pdu_session_res_to_setup_item.ng_ul_up_tnl_info.gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_string("01111111000000000000000000000001");
  gtp_tunnel.gtp_teid.from_string("00000036");

  asn1::e1ap::drb_to_setup_item_ng_ran_s drb_to_setup_item_ng_ran;
  drb_to_setup_item_ng_ran.drb_id = 4;

  drb_to_setup_item_ng_ran.sdap_cfg.default_drb = asn1::e1ap::default_drb_e::true_value;
  drb_to_setup_item_ng_ran.sdap_cfg.sdap_hdr_ul = asn1::e1ap::sdap_hdr_ul_e::present;
  drb_to_setup_item_ng_ran.sdap_cfg.sdap_hdr_dl = asn1::e1ap::sdap_hdr_dl_e::absent;

  drb_to_setup_item_ng_ran.pdcp_cfg.pdcp_sn_size_ul                 = asn1::e1ap::pdcp_sn_size_e::s_neg18;
  drb_to_setup_item_ng_ran.pdcp_cfg.pdcp_sn_size_dl                 = asn1::e1ap::pdcp_sn_size_e::s_neg18;
  drb_to_setup_item_ng_ran.pdcp_cfg.rlc_mode                        = asn1::e1ap::rlc_mode_e::rlc_am;
  drb_to_setup_item_ng_ran.pdcp_cfg.t_reordering_timer_present      = true;
  drb_to_setup_item_ng_ran.pdcp_cfg.t_reordering_timer.t_reordering = asn1::e1ap::t_reordering_e::ms100;
  drb_to_setup_item_ng_ran.pdcp_cfg.discard_timer_present           = true;
  drb_to_setup_item_ng_ran.pdcp_cfg.discard_timer                   = asn1::e1ap::discard_timer_e::infinity;

  asn1::e1ap::cell_group_info_item_s cell_group_info_item = {};
  cell_group_info_item.cell_group_id                      = 0;
  drb_to_setup_item_ng_ran.cell_group_info.push_back(cell_group_info_item);

  asn1::e1ap::qos_flow_qos_param_item_s qos_flow_qos_param_item = {};
  qos_flow_qos_param_item.qos_flow_id                           = 1;
  qos_flow_qos_param_item.qos_flow_level_qos_params.qos_characteristics.set_non_dyn_5qi();
  auto& qos_characteristics   = qos_flow_qos_param_item.qos_flow_level_qos_params.qos_characteristics.non_dyn_5qi();
  qos_characteristics.five_qi = 9;

  qos_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.prio_level = 15;
  qos_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_cap =
      asn1::e1ap::pre_emption_cap_e::shall_not_trigger_pre_emption;

  qos_flow_qos_param_item.qos_flow_level_qos_params.ngra_nalloc_retention_prio.pre_emption_vulnerability =
      asn1::e1ap::pre_emption_vulnerability_e::pre_emptable;

  drb_to_setup_item_ng_ran.qos_flow_info_to_be_setup.push_back(qos_flow_qos_param_item);

  pdu_session_res_to_setup_item.drb_to_setup_list_ng_ran.push_back(drb_to_setup_item_ng_ran);

  ng_ran_bearer_context_setup_req[0].value().pdu_session_res_to_setup_list().push_back(pdu_session_res_to_setup_item);

  return bearer_context_setup_request;
}

} // namespace srsgnb
