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

#include "../mac_dl/mac_dl_configurator.h"
#include "mac_config.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/support/timers.h"
#include <atomic>

namespace srsran {

struct cell_metric_report_config {
  /// \brief Period of the metric report.
  std::chrono::milliseconds report_period;
  /// \brief Pointer to the MAC cell metric notifier.
  mac_cell_metric_notifier* mac_notifier = nullptr;
  /// \brief Pointer to the SCHED cell metric notifier.
  scheduler_cell_metrics_notifier* sched_notifier = nullptr;
};

/// \brief Aggregator of the metrics of all the different MAC components (UL, DL, scheduler)
class mac_metrics_aggregator
{
public:
  /// \brief Maximum delay between the first and last report in the aggregation period.
  /// \remark The value is an heuristic to compensate for cells that get too delayed.
  static constexpr std::chrono::milliseconds aggregation_timeout{8};

  mac_metrics_aggregator(const mac_control_config::metrics_config& cfg,
                         task_executor&                            ctrl_exec_,
                         timer_manager&                            timers_,
                         srslog::basic_logger&                     logger_);
  ~mac_metrics_aggregator();

  cell_metric_report_config add_cell(du_cell_index_t            cell_index,
                                     subcarrier_spacing         scs_common,
                                     unsigned                   tdd_period_slots,
                                     mac_cell_clock_controller& time_source);

  void rem_cell(du_cell_index_t cell_index);

private:
  class cell_metric_handler;

  /// Called when pending reports should be handled.
  void handle_pending_reports();

  void handle_cell_activation(du_cell_index_t cell_index, slot_point_extended report_slot);

  void handle_cell_deactivation(du_cell_index_t cell_index, const mac_dl_cell_metric_report& last_report);

  bool pop_report(cell_metric_handler& cell);

  /// Creates a new aggregated metric report if the right conditions are met.
  void try_send_new_report();

  mac_control_config::metrics_config cfg;
  task_executor&                     ctrl_exec;
  timer_manager&                     timers;
  srslog::basic_logger&              logger;

  /// Metric handlers for configured cells.
  slotted_id_table<du_cell_index_t, std::unique_ptr<cell_metric_handler>, MAX_CELLS_PER_DU> cells;

  /// Expected start slot for the next report.
  slot_point_extended next_report_start_slot;

  /// Number of cells currently active.
  unsigned nof_active_cells = 0;

  /// Next report to be sent.
  mac_metric_report next_report;

  // Number of cell reports pending to be processed.
  std::atomic<unsigned> report_count{0};

  // Timer that when triggered aggregates all existing cell reports.
  unique_timer aggr_timer;
};

} // namespace srsran
