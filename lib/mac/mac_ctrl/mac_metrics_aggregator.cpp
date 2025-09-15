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

#include "mac_metrics_aggregator.h"
#include "spsc_metric_report_channel.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;

// Size of each cell queue. Should be large enough to account for the details in waking up the mac_dl_metric_handler
// to aggregate all cells separate reports.
static constexpr unsigned cell_report_queue_size = 32;

namespace {

/// Cell metrics report containing scheduler and MAC.
struct full_cell_report {
  scheduler_cell_metrics                   sched;
  std::optional<mac_dl_cell_metric_report> mac;
};

} // namespace

static full_cell_report report_preinit(unsigned max_ue_events = 64)
{
  full_cell_report report{};
  // Pre-reserve space for UE metrics.
  report.sched.ue_metrics.reserve(MAX_NOF_DU_UES);
  // Note: there can be more than one event per UE.
  const unsigned ue_event_capacity = std::min(max_ue_events, MAX_NOF_DU_UES * 3U);
  report.sched.events.reserve(ue_event_capacity);
  return report;
}

class mac_metrics_aggregator::cell_metric_handler final : public mac_cell_metric_notifier,
                                                          public scheduler_cell_metrics_notifier
{
public:
  cell_metric_handler(mac_metrics_aggregator&    parent_,
                      du_cell_index_t            cell_index_,
                      subcarrier_spacing         scs_common_,
                      mac_cell_clock_controller& time_source_,
                      srslog::basic_logger&      logger_) :
    parent(parent_),
    cell_index(cell_index_),
    scs_common(scs_common_),
    period_slots(get_nof_slots_per_subframe(scs_common) * parent.cfg.period.count()),
    time_source(time_source_),
    report_queue(cell_report_queue_size, logger_, [ue_events = parent.cfg.max_nof_ue_events]() {
      return report_preinit(ue_events);
    })
  {
  }

  bool is_report_required(slot_point slot_tx) override
  {
    // Note: Called from the cell execution context.
    if (SRSRAN_UNLIKELY(not last_sl_tx.valid())) {
      // Cell not yet active.
      return false;
    }
    slot_point_extended new_last_sl_tx{slot_tx, last_sl_tx.hyper_sfn()};
    if (SRSRAN_UNLIKELY(new_last_sl_tx < last_sl_tx)) {
      // SFN rollover detected.
      new_last_sl_tx += slot_tx.nof_slots_per_hyper_system_frame();
    }
    last_sl_tx = new_last_sl_tx;
    return mac_builder != nullptr and last_sl_tx >= next_report_slot_tx;
  }

  bool is_sched_report_required(slot_point slot_tx) const override
  {
    if (SRSRAN_UNLIKELY(not last_sl_tx.valid())) {
      return false;
    }
    slot_point_extended new_last_sl_tx{slot_tx, last_sl_tx.hyper_sfn()};
    if (SRSRAN_UNLIKELY(new_last_sl_tx < last_sl_tx)) {
      // SFN rollover detected.
      new_last_sl_tx += slot_tx.nof_slots_per_hyper_system_frame();
    }
    return new_last_sl_tx >= next_report_slot_tx;
  }

  void on_cell_activation() override
  {
    // Determine the next report slot.
    last_sl_tx          = time_source.now();
    unsigned slot_mod   = last_sl_tx.count() % period_slots;
    next_report_slot_tx = last_sl_tx + period_slots - slot_mod;

    // Notify the backend of a cell activation.
    defer_until_success(parent.ctrl_exec, parent.timers, [this, sl = next_report_slot_tx]() {
      parent.handle_cell_activation(cell_index, sl);
    });
  }

  void on_cell_deactivation(const mac_dl_cell_metric_report& report) override
  {
    next_report_slot_tx = {};
    last_sl_tx          = {};
    last_report         = report;
    defer_until_success(
        parent.ctrl_exec, parent.timers, [this]() { parent.handle_cell_deactivation(cell_index, last_report); });
  }

  void on_cell_metric_report(const mac_dl_cell_metric_report& report) override
  {
    // Note: Function called from the DU cell execution context.
    srsran_sanity_check(is_report_required(last_sl_tx.without_hyper_sfn()), "Report not required");

    // Save MAC report.
    mac_builder->mac = report;

    // Update next report slot.
    next_report_slot_tx += period_slots;

    // Commit report to MAC metric aggregator.
    mac_builder.reset();

    // If the token is acquired, it means that it is this thread's job to dispatch a job to handle pending reports.
    bool token_acquired = parent.report_count.fetch_add(1U, std::memory_order_acq_rel) == 0;
    if (not token_acquired) {
      // Another thread is already handling the reports.
      return;
    }

    // Defer the report handling to the control executor.
    parent.aggr_timer.run();
  }

private:
  friend class mac_metrics_aggregator;

  struct report_recycler {
    void operator()(full_cell_report& report)
    {
      // Keep the capacity.
      report.sched.ue_metrics.clear();
      report.sched.events.clear();
      report.mac.reset();
    }
  };

  /// Queue used to communicate MAC cell and SCHED cell reports to aggregator.
  using report_queue_type = spsc_metric_report_channel<full_cell_report, report_recycler>;

  /// \brief Called by the scheduler to start the preparation of a new report.
  scheduler_cell_metrics& get_next() final
  {
    if (sched_builder == nullptr) {
      sched_builder = report_queue.get_builder();
    }
    return sched_builder->sched;
  }

  /// \brief Called by the scheduler to commit the report.
  void commit(scheduler_cell_metrics& report) final
  {
    // Note: This function is called from the scheduler execution context.
    srsran_sanity_check(&report == &sched_builder->sched, "Invalid report being committed");

    // Scheduler prepared report, now it is the turn of the MAC cell to prepare its report.
    mac_builder = std::move(sched_builder);
  }

  mac_metrics_aggregator&    parent;
  const du_cell_index_t      cell_index;
  const subcarrier_spacing   scs_common;
  const unsigned             period_slots;
  mac_cell_clock_controller& time_source;

  // Reports from a given cell.
  report_queue_type          report_queue;
  report_queue_type::builder mac_builder;
  report_queue_type::builder sched_builder;
  mac_dl_cell_metric_report  last_report{};

  // Cached metric report being built.
  slot_point_extended last_sl_tx;
  slot_point_extended next_report_slot_tx;

  // Stateful flags access from the control executor.
  bool active_flag = false;
};

// class mac_metrics_aggregator

mac_metrics_aggregator::mac_metrics_aggregator(const mac_control_config::metrics_config& cfg_,
                                               task_executor&                            ctrl_exec_,
                                               timer_manager&                            timers_,
                                               srslog::basic_logger&                     logger_) :
  cfg(cfg_), ctrl_exec(ctrl_exec_), timers(timers_), logger(logger_)
{
  aggr_timer = timers_.create_unique_timer(ctrl_exec);
  aggr_timer.set(aggregation_timeout, [this](timer_id_t /* unused */) { handle_pending_reports(); });
}

mac_metrics_aggregator::~mac_metrics_aggregator() {}

cell_metric_report_config mac_metrics_aggregator::add_cell(du_cell_index_t            cell_index,
                                                           subcarrier_spacing         scs_common,
                                                           mac_cell_clock_controller& time_source)
{
  srsran_assert(not cells.contains(cell_index), "Duplicate cell creation");

  // Reserve space in reports for new cell.
  next_report.dl.cells.reserve(cells.size());
  next_report.sched.cells.reserve(cells.size());

  // Create a handler for the new cell.
  auto  cell_handler = std::make_unique<cell_metric_handler>(*this, cell_index, scs_common, time_source, logger);
  auto& cell_ref     = *cell_handler;
  cells.emplace(cell_index, std::move(cell_handler));

  // Return the cell report configuration.
  return cell_metric_report_config{cfg.period, &cell_ref, &cell_ref};
}

void mac_metrics_aggregator::rem_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell not found");
  cells.erase(cell_index);
}

void mac_metrics_aggregator::handle_pending_reports()
{
  uint32_t nof_reports = report_count.load(std::memory_order_acquire);
  while (nof_reports > 0) {
    unsigned pop_count = 0;
    for (auto& cell : cells) {
      for (bool report_popped = pop_report(*cell); report_popped; report_popped = pop_report(*cell)) {
        if (++pop_count == nof_reports) {
          // All reports have been processed.
          break;
        }
      }
    }
    if (pop_count != nof_reports) {
      // Not all reports have been processed. Wait for more reports.
      logger.warning("Not all reports have been processed");
      pop_count = nof_reports;
    }
    nof_reports = report_count.fetch_sub(pop_count, std::memory_order_acq_rel) - pop_count;

    // Send an aggregated report if ready.
    try_send_new_report();
  }
}

bool mac_metrics_aggregator::pop_report(cell_metric_handler& cell)
{
  // Peek next report.
  const auto* next_ev = cell.report_queue.peek();

  if (next_ev == nullptr) {
    // No report to pop for this cell.
    return false;
  }

  if (not next_report_start_slot.valid()) {
    // All cells are deactivated.
    logger.warning("cell={}: Discarding report as all cells are deactivated", fmt::underlying(cell.cell_index));
    return false;
  }

  slot_point_extended start_slot{next_ev->sched.slot, next_report_start_slot.hyper_sfn()};
  if (std::abs(start_slot - next_report_start_slot) >= next_ev->sched.slot.nof_slots_per_hyper_system_frame() / 2) {
    if (start_slot < next_report_start_slot) {
      start_slot += next_ev->sched.slot.nof_slots_per_hyper_system_frame();
    } else {
      start_slot -= next_ev->sched.slot.nof_slots_per_hyper_system_frame();
    }
  }

  if (start_slot >= next_report_start_slot and start_slot < (next_report_start_slot + cell.period_slots)) {
    // Report is within expected window.
    if (next_ev->mac.has_value()) {
      next_report.dl.cells.push_back(*next_ev->mac);
    }
    next_report.sched.cells.push_back(next_ev->sched);
    cell.report_queue.pop();
    return true;
  }

  if (start_slot >= next_report_start_slot + cell.period_slots) {
    // Report is the one coming right after the expected one. Leave it in the queue to be dequeued later.
    return false;
  }

  // Report falls in invalid window. Discard it.
  cell.report_queue.pop();
  if (start_slot >= next_report_start_slot - cell.period_slots) {
    logger.info("cell={}: Discarding old metric report. Expected report for slot={} but got it for slot={}",
                fmt::underlying(cell.cell_index),
                next_report_start_slot.without_hyper_sfn(),
                start_slot.without_hyper_sfn());
  } else {
    logger.warning("cell={}: Discarding metric report falling in invalid report window. Expected report for slot={}, "
                   "but got for slot={}",
                   fmt::underlying(cell.cell_index),
                   next_report_start_slot.without_hyper_sfn(),
                   start_slot.without_hyper_sfn());
  }
  return true;
}

void mac_metrics_aggregator::try_send_new_report()
{
  if (next_report.dl.cells.empty() and next_report.sched.cells.empty()) {
    // No cells -> no report.
    return;
  }

  // Forward the full report.
  cfg.mac_notifier.on_new_metrics_report(next_report);

  const unsigned period_slots = next_report.dl.cells[0].start_slot.nof_slots_per_subframe() * cfg.period.count();
  logger.debug("Metric report of {} cells completed for slots=[{}, {})",
               next_report.dl.cells.size(),
               next_report_start_slot.without_hyper_sfn(),
               (next_report_start_slot + period_slots).without_hyper_sfn());

  // Update next report window start slot.
  next_report_start_slot += period_slots;

  // Reset report.
  next_report.dl.cells.clear();
  next_report.sched.cells.clear();
}

void mac_metrics_aggregator::handle_cell_activation(du_cell_index_t cell_index, slot_point_extended report_sl_tx)
{
  srsran_assert(cells.contains(cell_index), "MAC cell activated but not configured");
  auto& cell = *cells[cell_index];
  srsran_assert(not cell.active_flag, "Reactivation of cell not supported");

  cell.active_flag = true;
  if (not next_report_start_slot.valid()) {
    next_report_start_slot = report_sl_tx - cell.period_slots;
  }
  ++nof_active_cells;
}

void mac_metrics_aggregator::handle_cell_deactivation(du_cell_index_t                  cell_index,
                                                      const mac_dl_cell_metric_report& last_report)
{
  srsran_assert(cells.contains(cell_index), "MAC cell activated but not configured");
  auto& cell = *cells[cell_index];
  srsran_assert(cell.active_flag, "Deactivation of already deactivated cell not supported");
  srsran_assert(last_report.cell_deactivated, "Expected cell deactivated flag to be set");

  auto push_last_report = [&]() {
    if (last_report.start_slot.valid()) {
      slot_point next_start_sl_tx = next_report_start_slot.without_hyper_sfn();
      if (last_report.start_slot >= next_start_sl_tx and
          last_report.start_slot < next_start_sl_tx + cell.period_slots) {
        next_report.dl.cells.push_back(last_report);
        return true;
      }
    }
    return false;
  };

  // Save last report before deactivating cell.
  bool report_pushed = push_last_report();
  cell.active_flag   = false;
  if (--nof_active_cells == 0) {
    // No more cells are active.

    // Stop aggregation timer.
    aggr_timer.stop();

    // We don't expect more reports. Verify if there is nothing pending to be processed.
    handle_pending_reports();

    if (not report_pushed) {
      // It can happen that the next_report_start_slot was updated in handle_pending_reports() and therefore,
      // the last report can be pushed.
      push_last_report();
    }
    try_send_new_report();

    // Reset the next aggregated report start slot.
    next_report_start_slot = {};
  }
}
