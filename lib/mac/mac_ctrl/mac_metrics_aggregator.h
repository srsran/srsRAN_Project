/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../mac_dl/mac_dl_configurator.h"
#include "mac_config.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/support/timers.h"
#include <atomic>

namespace srsran {

/// \brief Aggregator of the metrics of all the different MAC components (UL, DL, scheduler)
class mac_metrics_aggregator
{
public:
  /// \brief Maximum delay between the first and last report in the aggregation period.
  constexpr static std::chrono::milliseconds aggregation_timeout{8};

  mac_metrics_aggregator(std::chrono::milliseconds period_,
                         mac_metrics_notifier&     notifier_,
                         task_executor&            ctrl_exec_,
                         timer_manager&            timers_,
                         srslog::basic_logger&     logger_);
  ~mac_metrics_aggregator();

  mac_cell_metric_report_config add_cell(du_cell_index_t cell_index, subcarrier_spacing scs_common);
  void                          rem_cell(du_cell_index_t cell_index);

private:
  class cell_metric_handler;

  void handle_cell_metrics_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& report);

  // Called when a new report should be forwarded.
  void trigger_report_creation();

  bool pop_report(cell_metric_handler& cell, mac_dl_cell_metric_report& report);

  void send_new_report();

  std::chrono::milliseconds period;
  mac_metrics_notifier&     notifier;
  task_executor&            ctrl_exec;
  srslog::basic_logger&     logger;

  // Metric handles for configured cells.
  slotted_id_table<du_cell_index_t, std::unique_ptr<cell_metric_handler>, MAX_CELLS_PER_DU> cells;

  // Expected start slot for the next report.
  slot_point next_report_start_slot;

  // Next report to be sent.
  mac_metric_report next_report;

  // Number of cell reports pending to be processed.
  std::atomic<unsigned> report_count{0};

  // Timer that when triggered aggregates all existing cell reports.
  unique_timer aggr_timer;
};

} // namespace srsran
