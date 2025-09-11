/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "lib/cu_up/ngu_session_manager.h"
#include "srsran/asn1/e1ap/e1ap_pdu_contents.h"
#include "srsran/cu_up/cu_up_executor_mapper.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"
#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/gtpu/gtpu_gateway.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/gtpu/gtpu_tunnel_common_tx.h"
#include <chrono>
#include <condition_variable>
#include <list>
#include <mutex>
#include <utility>

constexpr auto default_wait_timeout = std::chrono::seconds(3);

namespace srsran {

/// Dummy CU-UP executor pool used for testing
class dummy_cu_up_executor_mapper final : public srs_cu_up::cu_up_executor_mapper
{
  class dummy_pdu_session_executor_mapper_impl : public srs_cu_up::ue_executor_mapper
  {
  public:
    dummy_pdu_session_executor_mapper_impl(task_executor& exec_) : exec(&exec_) {}

    task_executor& ctrl_executor() override { return *exec; }
    task_executor& ul_pdu_executor() override { return *exec; }
    task_executor& dl_pdu_executor() override { return *exec; }
    task_executor& crypto_executor() override { return *exec; }

    async_task<void> stop() override
    {
      return launch_async([](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_RETURN();
      });
    }

    task_executor* exec;
  };

public:
  dummy_cu_up_executor_mapper(task_executor* test_executor_) : test_executor(test_executor_) {}

  task_executor& ctrl_executor() override { return *test_executor; }

  task_executor& io_ul_executor() override { return *test_executor; }

  task_executor& e2_executor() override { return *test_executor; }

  task_executor& n3_rx_executor() override { return *test_executor; }

  task_executor& e1_rx_executor() override { return *test_executor; }

  task_executor& e2_rx_executor() override { return *test_executor; }

  task_executor& f1u_rx_executor() override { return *test_executor; }

  std::unique_ptr<srs_cu_up::ue_executor_mapper> create_ue_executor_mapper() override
  {
    return std::make_unique<dummy_pdu_session_executor_mapper_impl>(*test_executor);
  }

private:
  task_executor* test_executor;
};

/// Dummy GTP-U Rx Demux
class dummy_gtpu_demux_ctrl final : public gtpu_demux_ctrl
{
public:
  dummy_gtpu_demux_ctrl() : logger(srslog::fetch_basic_logger("GTPU")) {}
  ~dummy_gtpu_demux_ctrl() = default;

  expected<std::unique_ptr<gtpu_demux_dispatch_queue>>
  add_tunnel(gtpu_teid_t teid, task_executor& tunnel_exec, gtpu_tunnel_common_rx_upper_layer_interface* tunnel) override
  {
    created_teid_list.push_back(teid);
    return std::make_unique<gtpu_demux_dispatch_queue>(
        8192, tunnel_exec, logger, [](span<gtpu_demux_pdu_ctx_t>) {}, 256);
  }

  bool remove_tunnel(gtpu_teid_t teid) override
  {
    removed_teid_list.push_back(teid);
    return true;
  }

  void apply_test_teid(gtpu_teid_t teid) override {}

  void stop() override {}

  std::list<gtpu_teid_t> created_teid_list = {};
  std::list<gtpu_teid_t> removed_teid_list = {};

private:
  srslog::basic_logger& logger;
};

/// Dummy GTP-U TEID pool
class dummy_gtpu_teid_pool final : public gtpu_teid_pool
{
public:
  dummy_gtpu_teid_pool()           = default;
  ~dummy_gtpu_teid_pool() override = default;

  [[nodiscard]] expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> teid = gtpu_teid_t{next_teid++};
    return teid;
  }

  [[nodiscard]] bool release_teid(gtpu_teid_t teid) override { return true; }

  [[nodiscard]] bool full() const override { return true; }

  uint32_t get_max_nof_teids() override { return UINT32_MAX; }

  uint32_t next_teid = 1;
};

/// Dummy adapter between GTP-U and Network Gateway
class dummy_gtpu_network_gateway_adapter final : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  dummy_gtpu_network_gateway_adapter()           = default;
  ~dummy_gtpu_network_gateway_adapter() override = default;

  void on_new_pdu(byte_buffer /*buf*/, const ::sockaddr_storage& /*addr*/) override {}
};

class dummy_inner_f1u_bearer final : srs_cu_up::f1u_tx_pdu_notifier
{
private:
  f1u_cu_up_gateway_bearer_rx_notifier* rx_sdu_notifier = nullptr;

  std::list<nru_dl_message> tx_sdu_list;
  std::mutex                tx_sdu_mutex;
  std::condition_variable   tx_sdu_cv;

public:
  std::list<uint32_t> tx_discard_sdu_list;

  dummy_inner_f1u_bearer()           = default;
  ~dummy_inner_f1u_bearer() override = default;

  void connect_f1u_rx_sdu_notifier(f1u_cu_up_gateway_bearer_rx_notifier& rx_sdu_notifier_)
  {
    rx_sdu_notifier = &rx_sdu_notifier_;
  }

  void handle_pdu(nru_ul_message msg)
  {
    // Forward T-PDU to PDCP
    srsran_assert(rx_sdu_notifier != nullptr, "The rx_sdu_notifier must not be a nullptr!");
    rx_sdu_notifier->on_new_pdu(std::move(msg));
  }

  // This should be handle_sdu
  void on_new_pdu(nru_dl_message sdu) final
  {
    std::lock_guard<std::mutex> lock(tx_sdu_mutex);
    tx_sdu_list.push_back(std::move(sdu));
    tx_sdu_cv.notify_one();
  }

  nru_dl_message wait_tx_sdu()
  {
    std::unique_lock<std::mutex> lock(tx_sdu_mutex);
    if (tx_sdu_cv.wait_for(lock, default_wait_timeout, [&]() { return !tx_sdu_list.empty(); })) {
      // success
      nru_dl_message sdu = std::move(tx_sdu_list.front());
      tx_sdu_list.pop_front();
      return sdu;
    }
    // timeout
    nru_dl_message sdu = {};
    return sdu;
  }

  bool have_tx_sdu()
  {
    std::lock_guard<std::mutex> lock(tx_sdu_mutex);
    return !tx_sdu_list.empty();
  }
};

class dummy_f1u_gateway_bearer final : public f1u_cu_up_gateway_bearer
{
public:
  dummy_f1u_gateway_bearer(dummy_inner_f1u_bearer&             inner_,
                           srs_cu_up::f1u_bearer_disconnector& disconnector_,
                           const up_transport_layer_info&      ul_up_tnl_info_) :
    inner(inner_), disconnector(disconnector_), ul_up_tnl_info(ul_up_tnl_info_)
  {
  }

  ~dummy_f1u_gateway_bearer() override { stop(); }

  void stop() override
  {
    if (not stopped) {
      disconnector.disconnect_cu_bearer(ul_up_tnl_info);
    }
    stopped = true;
  }

  void connect_f1u_rx_sdu_notifier(f1u_cu_up_gateway_bearer_rx_notifier& rx_sdu_notifier_)
  {
    inner.connect_f1u_rx_sdu_notifier(rx_sdu_notifier_);
  }

  void on_new_pdu(nru_dl_message sdu) final { inner.on_new_pdu(std::move(sdu)); }

  expected<std::string> get_bind_address() const override { return "127.0.0.2"; }

private:
  bool                                stopped = false;
  dummy_inner_f1u_bearer&             inner;
  srs_cu_up::f1u_bearer_disconnector& disconnector;
  up_transport_layer_info             ul_up_tnl_info;
};

class dummy_f1u_gateway final : public f1u_cu_up_gateway
{
private:
  dummy_inner_f1u_bearer& bearer;
  std::string             bind_ip_addr = "127.0.0.1";

public:
  explicit dummy_f1u_gateway(dummy_inner_f1u_bearer& bearer_) : bearer(bearer_) {}
  ~dummy_f1u_gateway() override = default;

  std::unique_ptr<f1u_cu_up_gateway_bearer> create_cu_bearer(uint32_t                              ue_index,
                                                             drb_id_t                              drb_id,
                                                             five_qi_t                             five_qi,
                                                             const srs_cu_up::f1u_config&          config,
                                                             const gtpu_teid_t&                    ul_teid,
                                                             f1u_cu_up_gateway_bearer_rx_notifier& rx_notifier,
                                                             task_executor&                        ul_exec) override
  {
    created_ul_teid_list.push_back(ul_teid);
    bearer.connect_f1u_rx_sdu_notifier(rx_notifier);
    up_transport_layer_info ul_up_tnl_info{transport_layer_address::create_from_string("127.0.0.2"), ul_teid};
    return std::make_unique<dummy_f1u_gateway_bearer>(bearer, *this, ul_up_tnl_info);
  }

  void attach_dl_teid(const up_transport_layer_info& ul_up_tnl_info,
                      const up_transport_layer_info& dl_up_tnl_info) override
  {
    attached_ul_teid_list.push_back(ul_up_tnl_info.gtp_teid);
  }

  void disconnect_cu_bearer(const up_transport_layer_info& ul_up_tnl_info) override
  {
    removed_ul_teid_list.push_back(ul_up_tnl_info.gtp_teid);
  }

  std::list<gtpu_teid_t> created_ul_teid_list  = {};
  std::list<gtpu_teid_t> attached_ul_teid_list = {};
  std::list<gtpu_teid_t> removed_ul_teid_list  = {};
};

class dummy_gtpu_gateway final : public gtpu_tnl_pdu_session
{
public:
  void set_bind_address(const std::string& ip_address) { ip_addr = ip_address; }

private:
  bool get_bind_address(std::string& ip_address) const override
  {
    ip_address = ip_addr;
    return true;
  }

  std::optional<uint16_t> get_bind_port() const override { return 2152; }

  void handle_pdu(byte_buffer pdu, const sockaddr_storage& dest_addr) override {}

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override {}

  std::string ip_addr = "127.0.0.2";
};

class dummy_ngu_session_manager final : public srs_cu_up::ngu_session_manager
{
public:
  gtpu_tnl_pdu_session& get_next_ngu_gateway() override { return ngu_gw; }

private:
  dummy_gtpu_gateway ngu_gw;
};

class dummy_e1ap final : public srs_cu_up::e1ap_interface
{
public:
  explicit dummy_e1ap()  = default;
  ~dummy_e1ap() override = default;
  void handle_bearer_context_inactivity_notification(
      const srs_cu_up::e1ap_bearer_context_inactivity_notification& msg) override
  {
  }
  void handle_connection_loss() override {}

  bool connect_to_cu_cp() override { return true; }

  async_task<cu_up_e1_setup_response> handle_cu_up_e1_setup_request(const cu_up_e1_setup_request& request) override
  {
    return {};
  }

  async_task<void> handle_cu_up_e1ap_release_request() override { return {}; }

  size_t get_nof_ues() const override { return 0; }

  void handle_message(const e1ap_message& msg) override {}

  void handle_pdcp_max_count_reached(srs_cu_up::ue_index_t ue_index) override {}
};

inline e1ap_message generate_bearer_context_setup_request(unsigned cu_cp_ue_e1ap_id)
{
  e1ap_message bearer_context_setup_request = {};

  bearer_context_setup_request.pdu.set_init_msg();
  bearer_context_setup_request.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

  auto& bearer_context_setup_req = bearer_context_setup_request.pdu.init_msg().value.bearer_context_setup_request();
  bearer_context_setup_req->gnb_cu_cp_ue_e1ap_id = cu_cp_ue_e1ap_id;
  bearer_context_setup_req->security_info.security_algorithm.ciphering_algorithm =
      asn1::e1ap::ciphering_algorithm_e::nea0;
  bearer_context_setup_req->security_info.up_securitykey.encryption_key.from_string("a6ae39efbe0d424cd85f4a9c3aee0414");
  bearer_context_setup_req->ue_dl_aggr_max_bit_rate = 1000000000U;
  bearer_context_setup_req->serving_plmn.from_string("02f899");
  bearer_context_setup_req->activity_notif_level.value  = asn1::e1ap::activity_notif_level_e::ue;
  bearer_context_setup_req->ue_inactivity_timer_present = true;
  bearer_context_setup_req->ue_inactivity_timer         = 60;

  auto& ng_ran_bearer_context_setup_req =
      bearer_context_setup_req->sys_bearer_context_setup_request.set_ng_ran_bearer_context_setup_request();

  ng_ran_bearer_context_setup_req.resize(1);
  auto& pdu_session_res_list = ng_ran_bearer_context_setup_req[0]->pdu_session_res_to_setup_list();

  asn1::e1ap::pdu_session_res_to_setup_item_s pdu_session_res_to_setup_item;
  pdu_session_res_to_setup_item.pdu_session_id   = 2;
  pdu_session_res_to_setup_item.pdu_session_type = asn1::e1ap::pdu_session_type_e::ipv4;
  pdu_session_res_to_setup_item.snssai.sst.from_number(1);
  pdu_session_res_to_setup_item.security_ind.integrity_protection_ind =
      asn1::e1ap::integrity_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.security_ind.confidentiality_protection_ind =
      asn1::e1ap::confidentiality_protection_ind_e::not_needed;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr_present = true;
  pdu_session_res_to_setup_item.pdu_session_res_dl_ambr         = 100000000U;

  auto& gtp_tunnel = pdu_session_res_to_setup_item.ng_ul_up_tnl_info.set_gtp_tunnel();
  gtp_tunnel.transport_layer_address.from_string("01111111000000000000000000000001");
  gtp_tunnel.gtp_teid.from_string("00000036");

  asn1::e1ap::drb_to_setup_item_ng_ran_s drb_to_setup_item_ng_ran = {};
  drb_to_setup_item_ng_ran.drb_id                                 = 4;

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

  asn1::e1ap::qos_flow_qos_param_item_s qos_flow_qos_param_item{};
  qos_flow_qos_param_item.qos_flow_id = 1;
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

  pdu_session_res_list.push_back(pdu_session_res_to_setup_item);

  return bearer_context_setup_request;
}

inline security::sec_as_config get_dummy_up_security_info()
{
  security::sec_as_config sec_info = {};
  sec_info.domain                  = security::sec_domain::up;
  return sec_info;
}

} // namespace srsran
