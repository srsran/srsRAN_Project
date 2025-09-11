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

#include "du_high_worker_manager.h"
#include "tests/test_doubles/f1u/dummy_f1u_du_gateway.h"
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "srsran/du/du_high/du_high.h"
#include "srsran/du/du_high/du_high_configuration.h"
#include "srsran/du/du_high/du_metrics_notifier.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include "srsran/scheduler/config/cell_config_builder_params.h"

namespace srsran {

class io_broker;

namespace srs_du {

class dummy_f1c_test_client : public f1c_connection_client
{
public:
  bool cell_start_on_f1_setup = true;

  /// Last messages sent to the CU.
  std::vector<f1ap_message> last_f1ap_msgs;

  dummy_f1c_test_client(task_executor& test_exec_, bool cell_start_on_f1_setup_ = true);

  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  task_executor& test_exec;
};

class dummy_du_metrics_notifier : public du_metrics_notifier
{
public:
  std::optional<du_metrics_report> last_report;

  dummy_du_metrics_notifier(task_executor& exec_) : exec(exec_) {}

  void on_new_metric_report(const du_metrics_report& report) override
  {
    bool result = exec.defer([this, report]() { last_report = report; });
    report_fatal_error_if_not(result, "Failed to defer metric report to test thread");
  }

private:
  task_executor& exec;
};

bool is_ue_context_release_complete_valid(const f1ap_message& msg,
                                          gnb_du_ue_f1ap_id_t du_ue_id,
                                          gnb_cu_ue_f1ap_id_t cu_ue_id);

/// Parameters to set the DU-high environment simulator.
struct du_high_env_sim_params {
  unsigned                                  nof_cells = 1;
  std::optional<cell_config_builder_params> builder_params;
  std::optional<pucch_builder_params>       pucch_cfg;
  std::optional<unsigned>                   prach_frequency_start;
  std::optional<srs_periodicity>            srs_period;
  bool                                      active_cells_on_start = true;
};

du_high_configuration create_du_high_configuration(const du_high_env_sim_params& params = {});

class du_high_env_simulator
{
public:
  du_high_env_simulator(du_high_env_sim_params params = du_high_env_sim_params{});
  du_high_env_simulator(const du_high_configuration& du_hi_cfg, bool active_cells_on_start = true);
  virtual ~du_high_env_simulator();

  bool add_ue(rnti_t rnti, du_cell_index_t cell_index = to_du_cell_index(0));

  /// Run the RRC setup procedure for the given RNTI from the moment the CU-CP sends an RRC Setup (via DL RRC Message
  /// Transfer) until the CU receives the RRC Setup Complete (via UL RRC Message Transfer).
  bool run_rrc_setup(rnti_t rnti);

  /// Run the RRC reject procedure for a given RNTI.
  bool run_rrc_reject(rnti_t rnti);

  /// Run the RRC Reestablishment procedure for the given RNTI from the moment the CU-CP sends an RRC Reestablishment
  /// (via DL RRC Message Transfer) until the CU receives the RRC Reestablishment Complete (via UL RRC Message
  /// Transfer).
  enum class reestablishment_stage { reest_complete, reconf_complete };
  [[nodiscard]] bool run_rrc_reestablishment(rnti_t                rnti,
                                             rnti_t                old_rnti,
                                             reestablishment_stage stop_at = reestablishment_stage::reconf_complete);

  bool run_ue_context_setup(rnti_t rnti);

  bool run_ue_context_release(rnti_t rnti, srb_id_t srb_id = srb_id_t::srb1);

  void run_slot();

  bool run_until(unique_function<bool()> condition, std::optional<unsigned> max_slot_count = std::nullopt);

  virtual void handle_slot_results(du_cell_index_t cell_index);

  du_high_worker_manager                workers;
  timer_manager                         timers;
  std::unique_ptr<io_broker>            broker;
  std::unique_ptr<mac_clock_controller> timer_ctrl;
  dummy_f1c_test_client                 cu_notifier;
  cu_up_simulator                       cu_up_sim;
  dummy_du_metrics_notifier             du_metrics;

  du_high_configuration    du_high_cfg;
  du_high_dependencies     du_hi_dependencies;
  std::unique_ptr<du_high> du_hi;
  phy_test_dummy           phy;
  null_mac_pcap            mac_pcap;
  null_rlc_pcap            rlc_pcap;

  slot_point next_slot;

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

protected:
  struct ue_sim_context {
    struct srb_context {
      uint32_t next_pdcp_sn = 0;
    };

    rnti_t                                rnti = rnti_t::INVALID_RNTI;
    std::optional<gnb_du_ue_f1ap_id_t>    du_ue_id;
    std::optional<gnb_cu_ue_f1ap_id_t>    cu_ue_id;
    du_cell_index_t                       pcell_index;
    std::array<srb_context, MAX_NOF_SRBS> srbs;
  };

  [[nodiscard]] bool
  await_dl_msg_sched(const ue_sim_context& u, lcid_t lcid, std::optional<unsigned> max_slot_count = std::nullopt);

  [[nodiscard]] bool
  send_dl_rrc_msg_and_await_ul_rrc_msg(const ue_sim_context& u, const f1ap_message& dl_msg, uint32_t rlc_ul_sn);

  std::unordered_map<rnti_t, ue_sim_context> ues;

  unsigned next_cu_ue_id = 0;
};

} // namespace srs_du
} // namespace srsran
