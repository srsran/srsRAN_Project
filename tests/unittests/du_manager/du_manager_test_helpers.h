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

#include "lib/du/du_high/du_manager/ran_resource_management/du_ran_resource_manager.h"
#include "srsran/du/du_high/du_manager/du_manager_params.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_manager.h"
#include "srsran/mac/mac_paging_information_handler.h"
#include "srsran/mac/mac_positioning_measurement_handler.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <map>

namespace srsran {
namespace srs_du {

class dummy_teid_pool final : public gtpu_teid_pool
{
public:
  [[nodiscard]] expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> ret{int_to_gtpu_teid(next_gtpu_teid)};
    next_gtpu_teid++;
    return ret;
  }

  [[nodiscard]] bool release_teid(gtpu_teid_t teid) override { return true; }

  [[nodiscard]] bool full() const override { return false; }

  uint32_t get_max_nof_teids() override { return std::numeric_limits<uint32_t>::max(); }

private:
  uint32_t next_gtpu_teid = 0;
};

class dummy_ue_executor_mapper : public du_high_ue_executor_mapper
{
public:
  explicit dummy_ue_executor_mapper(task_executor& exec_) : exec(exec_) {}
  void           rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) override {}
  task_executor& ctrl_executor(du_ue_index_t ue_index) override { return exec; }
  task_executor& f1u_dl_pdu_executor(du_ue_index_t ue_index) override { return exec; }
  task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) override { return exec; }

  task_executor& exec;
};

class dummy_cell_executor_mapper : public du_high_cell_executor_mapper
{
public:
  explicit dummy_cell_executor_mapper(task_executor& exec_) : exec(exec_) {}
  task_executor& mac_cell_executor(du_cell_index_t cell_index) override { return exec; }
  task_executor& rlc_lower_executor(du_cell_index_t cell_index) override { return exec; }
  task_executor& slot_ind_executor(du_cell_index_t cell_index) override { return exec; }

  task_executor& exec;
};

class dummy_f1c_bearer : public f1c_bearer
{
public:
  byte_buffer       last_rx_pdu;
  byte_buffer_chain last_tx_sdu = byte_buffer_chain::create().value();

  void handle_pdu(byte_buffer pdu, bool rrc_delivery_status_request) override { last_rx_pdu = std::move(pdu); }
  async_task<bool> handle_pdu_and_await_delivery(byte_buffer               pdu,
                                                 bool                      report_rrc_delivery_status,
                                                 std::chrono::milliseconds timeout) override
  {
    last_rx_pdu = std::move(pdu);
    return launch_no_op_task(true);
  }
  async_task<bool> handle_pdu_and_await_transmission(byte_buffer               pdu,
                                                     bool                      rrc_delivery_status_request,
                                                     std::chrono::milliseconds timeout) override
  {
    last_rx_pdu = std::move(pdu);
    return launch_no_op_task(true);
  }
  void handle_sdu(byte_buffer_chain sdu) override { last_tx_sdu = std::move(sdu); }
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
};

class f1ap_test_dummy : public f1ap_connection_manager,
                        public f1ap_ue_context_manager,
                        public f1ap_message_handler,
                        public f1ap_rrc_message_transfer_procedure_handler,
                        public f1ap_metrics_collector
{
  struct drb_to_idx {
    size_t   get_index(drb_id_t i) const { return static_cast<size_t>(i) - 1; }
    drb_id_t get_id(size_t i) const { return static_cast<drb_id_t>(i + 1); }
  };

  void collect_metrics_report(f1ap_metrics_report& report) override { report = {}; }

public:
  struct f1ap_ue_context {
    slotted_id_table<srb_id_t, dummy_f1c_bearer, MAX_NOF_SRBS> f1c_bearers;
  };

  slotted_id_table<du_ue_index_t, f1ap_ue_context, MAX_NOF_DU_UES> f1ap_ues;

  wait_manual_event_tester<f1_setup_result>                      wait_f1_setup;
  wait_manual_event_tester<void>                                 wait_f1_removal;
  std::optional<f1ap_ue_creation_request>                        last_ue_create;
  f1ap_ue_creation_response                                      next_ue_create_response;
  std::optional<f1ap_ue_configuration_request>                   last_ue_config;
  f1ap_ue_configuration_response                                 next_ue_config_response;
  std::optional<du_ue_index_t>                                   last_ue_deleted;
  std::optional<f1ap_ue_context_release_request>                 last_ue_release_req;
  wait_manual_event_tester<f1ap_ue_context_modification_confirm> wait_ue_mod;
  std::optional<gnbdu_config_update_request>                     last_du_cfg_req;

  bool connect_to_cu_cp() override { return true; }

  async_task<f1_setup_result> handle_f1_setup_request(const f1_setup_request_message& request) override
  {
    return wait_f1_setup.launch();
  }

  async_task<void> handle_f1_removal_request() override { return wait_f1_removal.launch(); }

  async_task<f1_reset_acknowledgement> handle_f1_reset_request(const f1_reset_request& req) override
  {
    return launch_no_op_task(f1_reset_acknowledgement{true});
  }

  async_task<gnbdu_config_update_response> handle_du_config_update(const gnbdu_config_update_request& request) override
  {
    last_du_cfg_req = gnbdu_config_update_request{request};
    return launch_no_op_task(gnbdu_config_update_response{true});
  }

  bool is_f1_setup() const override { return true; }

  /// Initiates creation of UE context in F1.
  f1ap_ue_creation_response handle_ue_creation_request(const f1ap_ue_creation_request& msg) override
  {
    last_ue_create = msg;
    return next_ue_create_response;
  }

  f1ap_ue_configuration_response handle_ue_configuration_request(const f1ap_ue_configuration_request& msg) override
  {
    last_ue_config = msg;
    return next_ue_config_response;
  }

  void handle_ue_deletion_request(du_ue_index_t ue_index) override { last_ue_deleted = ue_index; }

  void handle_ue_context_release_request(const f1ap_ue_context_release_request& msg) override
  {
    last_ue_release_req = msg;
  }

  async_task<f1ap_ue_context_modification_confirm>
  handle_ue_context_modification_required(const f1ap_ue_context_modification_required& msg) override
  {
    return wait_ue_mod.launch();
  }

  void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) override {}

  void handle_notify(const f1ap_notify_message& msg) override {}

  void handle_message(const f1ap_message& msg) override {}

  void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) override {}
};

class f1u_gw_bearer_dummy : public f1u_du_gateway_bearer
{
public:
  srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx;

  std::optional<nru_ul_message> last_sdu;

  f1u_gw_bearer_dummy(srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx_) : du_rx(du_rx_) {}

  void stop() override {}

  expected<std::string> get_bind_address() const override { return "127.0.0.1"; }

  void on_new_pdu(nru_ul_message msg) override { last_sdu = std::move(msg); }

  // helper function to push DL PDUs to the RX path.
  void on_new_sdu(nru_dl_message msg) { du_rx.on_new_pdu(std::move(msg)); }
};

class f1u_gateway_dummy : public f1u_du_gateway
{
public:
  bool next_bearer_is_created = true;

  std::unique_ptr<f1u_du_gateway_bearer> create_du_bearer(uint32_t                                   ue_index,
                                                          drb_id_t                                   drb_id,
                                                          five_qi_t                                  five_qi,
                                                          srs_du::f1u_config                         config,
                                                          const gtpu_teid_t&                         dl_teid,
                                                          const up_transport_layer_info&             ul_up_tnl_info,
                                                          srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                          timer_factory                              timers,
                                                          task_executor& ue_executor) override
  {
    if (next_bearer_is_created and f1u_bearers.count(dl_teid) == 0) {
      auto f1u_bearer = std::make_unique<f1u_gw_bearer_dummy>(du_rx);
      f1u_bearers.insert(std::make_pair(dl_teid, std::map<up_transport_layer_info, f1u_gw_bearer_dummy*>{}));
      f1u_bearers[dl_teid].emplace(ul_up_tnl_info, f1u_bearer.get());
      return f1u_bearer;
    }
    return nullptr;
  }

  void remove_du_bearer(const up_transport_layer_info& dl_tnl_info) override
  {
    auto bearer_it = f1u_bearers.find(dl_tnl_info.gtp_teid);
    if (bearer_it == f1u_bearers.end()) {
      srslog::fetch_basic_logger("TEST").warning("Could not find DL-TEID at DU to remove. DL-TEID={}",
                                                 dl_tnl_info.gtp_teid);
      return;
    }
    f1u_bearers.erase(bearer_it);
  }

  expected<std::string> get_du_bind_address(gnb_du_id_t du_index) const override
  {
    if (f1u_ext_addr == "auto") {
      return std::string("127.0.0.1");
    }
    return f1u_ext_addr;
  }

  std::map<gtpu_teid_t, std::map<up_transport_layer_info, f1u_gw_bearer_dummy*>> f1u_bearers;

  void set_f1u_ext_addr(const std::string& addr) { f1u_ext_addr = addr; }

  std::string f1u_ext_addr = "auto";
};

class mac_test_dummy : public mac_manager,
                       public mac_cell_manager,
                       public mac_ue_configurator,
                       public mac_ue_control_information_handler,
                       public mac_paging_information_handler,
                       public mac_positioning_measurement_handler
{
public:
  class mac_cell_dummy : public mac_cell_controller
  {
  public:
    wait_manual_event_tester<void>           wait_start;
    wait_manual_event_tester<void>           wait_stop;
    std::optional<mac_cell_reconfig_request> last_cell_recfg_req;

    async_task<void>                       start() override { return wait_start.launch(); }
    async_task<void>                       stop() override { return wait_stop.launch(); }
    async_task<mac_cell_reconfig_response> reconfigure(const mac_cell_reconfig_request& request) override
    {
      last_cell_recfg_req = request;
      return launch_no_op_task(mac_cell_reconfig_response{true});
    }
  };

  class mac_cell_dummy_time_mapper final : public mac_cell_time_mapper
  {
  public:
    std::optional<mac_cell_slot_time_info> get_last_mapping() const override
    {
      return mac_cell_slot_time_info{slot_point(1, 1), std::chrono::system_clock::now()};
    }
    std::optional<time_point> get_time_point(slot_point slot) const override { return std::nullopt; }
    std::optional<slot_point> get_slot_point(time_point time) const override { return std::nullopt; }
  };

  mac_cell_dummy             mac_cell;
  mac_cell_dummy_time_mapper time_mapper;

  std::optional<mac_ue_create_request>                      last_ue_create_msg{};
  std::optional<mac_ue_reconfiguration_request>             last_ue_reconf_msg{};
  std::optional<mac_ue_delete_request>                      last_ue_delete_msg{};
  std::optional<mac_dl_buffer_state_indication_message>     last_dl_bs;
  byte_buffer                                               last_pushed_ul_ccch_msg;
  std::optional<du_ue_index_t>                              last_ue_config_applied;
  wait_manual_event_tester<mac_ue_create_response>          wait_ue_create;
  wait_manual_event_tester<mac_ue_reconfiguration_response> wait_ue_reconf;
  wait_manual_event_tester<mac_ue_delete_response>          wait_ue_delete;
  bool                                                      next_ul_ccch_msg_result = true;

  mac_cell_manager&                    get_cell_manager() override { return *this; }
  mac_ue_configurator&                 get_ue_configurator() override { return *this; }
  mac_positioning_measurement_handler& get_positioning_handler() override { return *this; }

  mac_cell_controller&  add_cell(const mac_cell_creation_request& cell_cfg) override { return mac_cell; }
  void                  remove_cell(du_cell_index_t cell_index) override {}
  mac_cell_controller&  get_cell_controller(du_cell_index_t cell_index) override { return mac_cell; }
  mac_cell_time_mapper& get_time_mapper(du_cell_index_t cell_index) override { return time_mapper; }

  async_task<mac_ue_create_response> handle_ue_create_request(const mac_ue_create_request& msg) override
  {
    last_ue_create_msg = msg;
    return wait_ue_create.launch();
  }
  async_task<mac_ue_reconfiguration_response>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg) override
  {
    last_ue_reconf_msg = msg;
    return wait_ue_reconf.launch();
  }
  async_task<mac_ue_delete_response> handle_ue_delete_request(const mac_ue_delete_request& msg) override
  {
    last_ue_delete_msg = msg;
    return wait_ue_delete.launch();
  }
  bool handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) override
  {
    last_pushed_ul_ccch_msg = std::move(pdu);
    return next_ul_ccch_msg_result;
  }
  void handle_ue_config_applied(du_ue_index_t ue_index) override { last_ue_config_applied = ue_index; }

  void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) override
  {
    last_dl_bs = dl_bs;
  }

  void handle_paging_information(const paging_information& msg) override {}

  async_task<mac_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_positioning_measurement_request& msg) override
  {
    return launch_no_op_task(mac_positioning_measurement_response{});
  }
};

class dummy_ue_resource_configurator_factory : public du_ran_resource_manager
{
public:
  class dummy_resource_updater : public ue_ran_resource_configurator::resource_updater
  {
  public:
    dummy_resource_updater(dummy_ue_resource_configurator_factory& parent_, du_ue_index_t ue_index_);
    ~dummy_resource_updater();
    du_ue_resource_update_response              update(du_cell_index_t                       pcell_index,
                                                       const f1ap_ue_context_update_request& upd_req,
                                                       const du_ue_resource_config*          reestablished_context,
                                                       const ue_capability_summary*          reestablished_ue_caps) override;
    void                                        config_applied() override {}
    const du_ue_resource_config&                get() override;
    const std::optional<ue_capability_summary>& ue_capabilities() const override;

    du_ue_index_t                           ue_index;
    dummy_ue_resource_configurator_factory& parent;
  };

  std::optional<du_ue_index_t>                   last_ue_index;
  std::optional<du_cell_index_t>                 last_ue_pcell;
  f1ap_ue_context_update_request                 last_ue_ctx_upd;
  std::optional<ue_capability_summary>           next_ue_caps;
  std::map<du_ue_index_t, du_ue_resource_config> ue_resource_pool;
  du_ue_resource_config                          next_context_update_result;
  du_ue_resource_update_response                 next_config_resp;

  dummy_ue_resource_configurator_factory();

  expected<ue_ran_resource_configurator, std::string>
  create_ue_resource_configurator(du_ue_index_t ue_index, du_cell_index_t pcell_index, bool has_tc_rnti) override;
};

f1ap_ue_context_update_request create_f1ap_ue_context_update_request(du_ue_index_t                   ue_idx,
                                                                     std::initializer_list<srb_id_t> srbs_to_addmod,
                                                                     std::initializer_list<drb_id_t> drbs_to_add,
                                                                     std::initializer_list<drb_id_t> drbs_to_mod = {});

class du_manager_test_bench
{
public:
  du_manager_test_bench(span<const du_cell_config> cells);

  std::vector<du_cell_config>            du_cells;
  timer_manager                          timers;
  manual_task_worker                     worker;
  task_executor&                         du_mng_exec;
  dummy_ue_executor_mapper               ue_exec_mapper;
  dummy_cell_executor_mapper             cell_exec_mapper;
  f1ap_test_dummy                        f1ap;
  f1u_gateway_dummy                      f1u_gw;
  mac_test_dummy                         mac;
  null_rlc_pcap                          rlc_pcap;
  du_manager_params                      params;
  dummy_ue_resource_configurator_factory cell_res_alloc;
  srslog::basic_logger&                  logger;
};

} // namespace srs_du
} // namespace srsran
