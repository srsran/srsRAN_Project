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

#include "config_generators.h"
#include "dummy_test_components.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "result_test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/scheduler/scheduler_factory.h"

namespace srsran {

scheduler_expert_config make_custom_scheduler_expert_config(bool enable_csi_rs_pdsch_multiplexing);

/// Configuration of the scheduler test simulator.
struct scheduler_test_sim_config {
  scheduler_expert_config sched_cfg   = config_helpers::make_default_scheduler_expert_config();
  unsigned                tx_rx_delay = 4;
  subcarrier_spacing      max_scs     = subcarrier_spacing::kHz15;
  /// Whether to automatically respond to UCI grants with UCI indications.
  bool auto_uci = false;
  /// Whether to automatically respond to PUSCH grants with CRC indications.
  bool auto_crc = false;
};

/// Helper class to help setup a scheduler unit test.
class scheduler_test_simulator
{
public:
  explicit scheduler_test_simulator(unsigned           tx_rx_delay_                     = 4,
                                    subcarrier_spacing max_scs                          = subcarrier_spacing::kHz15,
                                    bool               enable_csi_rs_pdsch_multiplexing = true);
  explicit scheduler_test_simulator(const scheduler_expert_config& sched_cfg_,
                                    unsigned                       tx_rx_delay_ = 4,
                                    subcarrier_spacing             max_scs      = subcarrier_spacing::kHz15);
  explicit scheduler_test_simulator(const scheduler_test_sim_config& cfg);
  ~scheduler_test_simulator();

  slot_point next_slot_rx() const { return next_slot - tx_rx_delay; }

  slot_point last_result_slot() const { return next_slot - 1; }

  /// Configure new cell.
  void add_cell(const sched_cell_configuration_request_message& cell_cfg_req);

  /// Add new UE to scheduler.
  void add_ue(const sched_ue_creation_request_message& ue_request, bool wait_notification = false);

  /// Remove existing UE from scheduler.
  void rem_ue(du_ue_index_t ue_index);

  /// Push RLC DL buffer occupancy update.
  void push_dl_buffer_state(const dl_buffer_state_indication_message& upd);

  /// Push BSR indication.
  void push_bsr(const ul_bsr_indication_message& bsr) { sched->handle_ul_bsr_indication(bsr); }

  /// Push UCI indication with 1 positive ACK bit over PUCCH F1.
  void push_uci_indication(du_cell_index_t cell_idx, slot_point slot_uci);

  /// Advance scheduler by one slot.
  void run_slot(du_cell_index_t cell_idx = to_du_cell_index(0));

  bool run_slot_until(const std::function<bool()>& cond_func, unsigned slot_timeout = 1000);

  const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return srsran::find_ue_dl_pdcch(rnti, *last_sched_res_list[cell_idx]);
  }

  const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return srsran::find_ue_ul_pdcch(rnti, *last_sched_res_list[cell_idx]);
  }

  const unsigned                      tx_rx_delay;
  bool                                auto_uci = false;
  bool                                auto_crc = false;
  srslog::basic_logger&               logger;
  srslog::basic_logger&               test_logger;
  const scheduler_expert_config       sched_cfg;
  sched_cfg_dummy_notifier            notif;
  scheduler_ue_metrics_dummy_notifier metric_notif;
  std::unique_ptr<mac_scheduler>      sched;

  static_vector<cell_configuration, MAX_NOF_DU_CELLS> cell_cfg_list;

  slot_point                                           next_slot;
  static_vector<const sched_result*, MAX_NOF_DU_CELLS> last_sched_res_list;

private:
  /// Handle automatic feedback (UCI/CRC) if enabled.
  void handle_auto_feedback(du_cell_index_t cell_idx);

  std::unordered_map<rnti_t, du_ue_index_t> rnti_to_ue_index;
};

} // namespace srsran
