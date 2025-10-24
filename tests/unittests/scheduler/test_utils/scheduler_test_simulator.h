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
#include "tests/test_doubles/scheduler/dummy_scheduler_ue_metric_notifier.h"
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
  /// Cell-Specific K-offset for NTN cells.
  unsigned ntn_cs_koffset = 0;
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

  /// Advance scheduler by one slot for a given cell. If cell not provided, advance it for all cells.
  void run_slot(std::optional<du_cell_index_t> cell_idx = {});

  bool run_slot_until(const std::function<bool()>& cond_func, unsigned slot_timeout = 1000);

  const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return srsran::find_ue_dl_pdcch(rnti, *last_sched_result(cell_idx));
  }

  const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return srsran::find_ue_ul_pdcch(rnti, *last_sched_result(cell_idx));
  }

  const std::unordered_map<rnti_t, du_ue_index_t>& rnti_to_ue_index_mapping() const { return rnti_to_ue_index; }

  const unsigned                      tx_rx_delay;
  bool                                auto_uci       = false;
  bool                                auto_crc       = false;
  unsigned                            ntn_cs_koffset = 0;
  srslog::basic_logger&               logger;
  srslog::basic_logger&               test_logger;
  const scheduler_expert_config       sched_cfg;
  sched_cfg_dummy_notifier            notif;
  scheduler_ue_metrics_dummy_notifier metric_notif;
  std::unique_ptr<mac_scheduler>      sched;

  slot_point next_slot;

  bool contains(du_cell_index_t cell_idx) const
  {
    return cell_idx < sim_cells.size() and sim_cells[cell_idx] != nullptr;
  }
  const cell_configuration& cell_cfg(du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return sim_cells.at(cell_idx)->cfg;
  }
  const sched_result* last_sched_result(du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return sim_cells.at(cell_idx)->last_res;
  }
  void request_metrics_on_next_slot(du_cell_index_t cell_idx = to_du_cell_index(0))
  {
    sim_cells[cell_idx]->cell_metrics.request_metrics();
  }
  const std::optional<scheduler_cell_metrics>& last_metrics(du_cell_index_t cell_idx = to_du_cell_index(0)) const
  {
    return sim_cells.at(cell_idx)->cell_metrics.last();
  }

  /// Register UCI indication event adapter, which will be called before an UCI indication is forwarded to the
  /// scheduler.
  void register_uci_handler(std::function<void(uci_indication&)> func) { on_uci_indication = std::move(func); }

private:
  struct sim_cell_context {
    dummy_scheduler_cell_metrics_notifier cell_metrics;
    cell_configuration                    cfg;
    const sched_result*                   last_res = nullptr;

    sim_cell_context(const scheduler_expert_config&                  sched_cfg,
                     const sched_cell_configuration_request_message& cell_cfg_req) :
      cfg(sched_cfg, cell_cfg_req)
    {
    }
  };

  /// Handle automatic feedback (UCI/CRC) if enabled.
  void handle_auto_feedback(du_cell_index_t cell_idx);

  std::vector<std::unique_ptr<sim_cell_context>> sim_cells;

  std::unordered_map<rnti_t, du_ue_index_t> rnti_to_ue_index;

  std::function<void(uci_indication&)> on_uci_indication = [](uci_indication&) {};
};

} // namespace srsran
