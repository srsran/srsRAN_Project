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
#include "srsran/adt/scope_exit.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;

namespace {

// Size of each cell queue. Should be large enough to account for the details in waking up the mac_dl_metric_handler
// to aggregate all cells separate reports.
constexpr unsigned cell_report_queue_size = 8;

/// Cell metrics report containing scheduler and MAC.
struct full_cell_report {
  slot_point_extended                      start_slot;
  scheduler_cell_metrics                   sched;
  std::optional<mac_dl_cell_metric_report> mac;
};

full_cell_report report_preinit(unsigned max_ue_events = 64U, unsigned tdd_period_slots = 0U)
{
  full_cell_report report{};
  // Pre-reserve space for UE metrics.
  report.sched.ue_metrics.reserve(MAX_NOF_DU_UES);
  // Note: there can be more than one event per UE.
  const unsigned ue_event_capacity = std::min(max_ue_events, MAX_NOF_DU_UES * 3U);
  report.sched.events.reserve(ue_event_capacity);
  report.sched.pusch_prbs_used_per_tdd_slot_idx.reserve(tdd_period_slots);
  report.sched.pdsch_prbs_used_per_tdd_slot_idx.reserve(tdd_period_slots);
  return report;
}

} // namespace

class mac_metrics_aggregator::cell_metric_handler final : public mac_cell_metric_notifier,
                                                          public scheduler_cell_metrics_notifier
{
public:
  cell_metric_handler(mac_metrics_aggregator&    parent_,
                      du_cell_index_t            cell_index_,
                      subcarrier_spacing         scs_common_,
                      unsigned                   tdd_period_slots,
                      mac_cell_clock_controller& time_source_,
                      srslog::basic_logger&      logger_) :
    parent(parent_),
    cell_index(cell_index_),
    scs_common(scs_common_),
    period_slots(get_nof_slots_per_subframe(scs_common) * parent.cfg.period.count()),
    time_source(time_source_),
    report_queue(cell_report_queue_size, logger_, [ue_events = parent.cfg.max_nof_ue_events, tdd_period_slots]() {
      return report_preinit(ue_events, tdd_period_slots);
    })
  {
  }

  bool is_report_required(slot_point slot_tx) override
  {
    // Note: Called by MAC, from the cell execution context.

    if (SRSRAN_UNLIKELY(not last_sl_tx.valid())) {
      // Cell not yet active.
      return false;
    }

    // Add HFN.
    slot_point_extended new_last_sl_tx{slot_tx, last_sl_tx.hyper_sfn()};
    if (SRSRAN_UNLIKELY(new_last_sl_tx < last_sl_tx)) {
      // SFN rollover detected.
      new_last_sl_tx += slot_tx.nof_slots_per_hyper_system_frame();
    }

    last_sl_tx = new_last_sl_tx;

    // Check if this is the last slot of the report.
    // Note: We compare with end-1, because this function is called when the scheduler is done with its scheduling
    // decision. e.g. for a report period of 10, we want to produce report at slots 0.9, 1.9, etc.
    return mac_builder != nullptr and last_sl_tx >= next_report_end_slot_tx - 1;
  }

  bool is_sched_report_required(slot_point slot_tx) const override
  {
    // Note: Called by SCHED, from the cell execution context.

    if (SRSRAN_UNLIKELY(not last_sl_tx.valid())) {
      return false;
    }

    // Add HFN.
    slot_point_extended new_last_sl_tx{slot_tx, last_sl_tx.hyper_sfn()};
    if (SRSRAN_UNLIKELY(new_last_sl_tx < last_sl_tx)) {
      // SFN rollover detected.
      new_last_sl_tx += slot_tx.nof_slots_per_hyper_system_frame();
    }

    // Check if this is the last slot of the report.
    return new_last_sl_tx >= next_report_end_slot_tx - 1;
  }

  void on_cell_activation() override
  {
    // Determine the next report slot.
    last_sl_tx        = time_source.now();
    unsigned slot_mod = last_sl_tx.count() % period_slots;

    // We are "slotting" the time when reports are produced. E.g. for a period of 10 slots and SCS=15kHz, the reports
    // will be for slots 0.0-0.9, 1.0-1.9, 2.0-2.9, ...
    next_report_end_slot_tx = last_sl_tx + period_slots - slot_mod;

    // Notify the backend of a cell activation.
    defer_until_success(parent.ctrl_exec, parent.timers, [this, sl = next_report_end_slot_tx]() {
      parent.handle_cell_activation(cell_index, sl);
    });
  }

  void on_cell_deactivation(const mac_dl_cell_metric_report& report) override
  {
    auto start_slot         = next_report_end_slot_tx - period_slots;
    next_report_end_slot_tx = {};
    last_sl_tx              = {};
    // Save MAC report and commit it.
    if (mac_builder != nullptr) {
      mac_builder->mac        = report;
      mac_builder->start_slot = start_slot;
      mac_builder.reset();
    }
    defer_until_success(parent.ctrl_exec, parent.timers, [this]() { parent.handle_cell_deactivation(cell_index); });
  }

  void on_cell_metric_report(const mac_dl_cell_metric_report& report) override
  {
    // Note: Function called from the DU cell execution context.
    srsran_sanity_check(is_report_required(last_sl_tx.without_hyper_sfn()), "Report not required");

    // Save MAC report.
    mac_builder->mac = report;

    // Update next report slot.
    auto start_slot         = next_report_end_slot_tx - period_slots;
    mac_builder->start_slot = start_slot;
    next_report_end_slot_tx += period_slots;

    // Commit report to MAC metric aggregator.
    mac_builder.reset();

    // If the token is acquired, it means that it is this thread's job to dispatch a job to handle pending reports.
    unsigned ring_index     = start_slot.count() / period_slots;
    auto&    ring_slot      = parent.report_ring[ring_index];
    bool     token_acquired = not ring_slot.end_slot_flag.exchange(true, std::memory_order_acq_rel);
    if (not token_acquired) {
      // Another cell is already handling the reports.
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
      report.sched.pusch_prbs_used_per_tdd_slot_idx.clear();
      report.sched.pdsch_prbs_used_per_tdd_slot_idx.clear();
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

  // Cached metric report being built.
  slot_point_extended last_sl_tx;
  slot_point_extended next_report_end_slot_tx;

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
                                                           unsigned                   tdd_period_slots,
                                                           mac_cell_clock_controller& time_source)
{
  srsran_assert(not cells.contains(cell_index), "Duplicate cell creation");

  // Reserve space in reports for new cell.
  for (auto& rep : report_ring) {
    rep.report.dl.cells.reserve(cells.size() + 1);
    rep.report.sched.cells.reserve(cells.size() + 1);
  }

  // Create a handler for the new cell.
  auto cell_handler =
      std::make_unique<cell_metric_handler>(*this, cell_index, scs_common, tdd_period_slots, time_source, logger);
  auto& cell_ref = *cell_handler;
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
  // Pop all pending reports.
  for (auto& cell : cells) {
    for (bool report_popped = pop_report(*cell); report_popped; report_popped = pop_report(*cell)) {
    }
  }

  // Send an aggregated report if ready.
  try_send_new_report();
}

bool mac_metrics_aggregator::pop_report(cell_metric_handler& cell)
{
  // Peek next report.
  const auto* next_ev = cell.report_queue.peek();

  if (next_ev == nullptr) {
    // No report to pop for this cell.
    return false;
  }

  // Fetch position in the report ring where to save report.
  const auto      ring_key   = next_ev->start_slot.count() / cell.period_slots;
  report_context& report_ctx = report_ring[ring_key];

  const bool is_in_window =
      report_ctx.start_slot.valid() and
      next_ev->start_slot.is_in_interval(report_ctx.start_slot, report_ctx.start_slot + cell.period_slots);
  if (not is_in_window) {
    // Report is not within expected window for this ring slot.

    if (not report_ctx.report.sched.cells.empty()) {
      logger.warning("Discarding metric report for slot={}. Cause: Some cells failed to produce their report",
                     report_ctx.report.sched.cells[0].slot);
      report_ctx.report.sched.cells.clear();
      report_ctx.report.dl.cells.clear();
    }

    // Update start slot in the respective ring report context.
    report_ctx.start_slot = next_ev->start_slot;
  }

  // Add front to pending report in the report_ring.
  if (next_ev->mac.has_value()) {
    report_ctx.report.dl.cells.push_back(*next_ev->mac);
  }
  report_ctx.report.sched.cells.push_back(next_ev->sched);

  // Pop report from respective queue.
  cell.report_queue.pop();

  return true;
}

void mac_metrics_aggregator::try_send_new_report()
{
  auto& ring_elem   = report_ring[next_report_start_slot.count() / period_slots];
  auto& next_report = ring_elem.report;

  if (not ring_elem.end_slot_flag.exchange(false, std::memory_order_acquire)) {
    // There might be pending reports for this ring position, but that does not mean that all cells have provided their
    // report yet.
    return;
  }
  auto clear_report = make_scope_exit([&]() {
    // Clear processed report.
    ring_elem.report.dl.cells.clear();
    ring_elem.report.sched.cells.clear();
    ring_elem.start_slot = {};

    // Update next report window start slot.
    next_report_start_slot += period_slots;
  });

  if (next_report.dl.cells.empty() and next_report.sched.cells.empty()) {
    // No cells -> no report.
    return;
  }

  if (ring_elem.start_slot != next_report_start_slot) {
    // Invalid ring slot.
    logger.warning("Discarding metric report for slot={}. Cause: Expected report for slot={}",
                   ring_elem.start_slot,
                   next_report_start_slot);
    return;
  }

  // Forward the full report.
  cfg.mac_notifier.on_new_metrics_report(next_report);

  logger.debug("Metric report of {} cells completed for slots=[{}, {})",
               next_report.dl.cells.size(),
               next_report_start_slot,
               (next_report_start_slot + period_slots));
}

void mac_metrics_aggregator::handle_cell_activation(du_cell_index_t cell_index, slot_point_extended report_end_sl_tx)
{
  srsran_assert(cells.contains(cell_index), "MAC cell activated but not configured");
  auto& cell = *cells[cell_index];
  srsran_assert(not cell.active_flag, "Reactivation of cell not supported");

  cell.active_flag = true;
  if (not next_report_start_slot.valid()) {
    next_report_start_slot = report_end_sl_tx - cell.period_slots;
    period_slots           = cell.period_slots;
    srsran_sanity_check(next_report_start_slot.count() % period_slots == 0,
                        "Invalid report end slot provided during the first MAC cell activation");
  }
  ++nof_active_cells;
}

void mac_metrics_aggregator::handle_cell_deactivation(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "MAC cell activated but not configured");
  auto& cell = *cells[cell_index];
  srsran_assert(cell.active_flag, "Deactivation of already deactivated cell not supported");

  cell.active_flag = false;
  if (--nof_active_cells == 0) {
    // This is last active cell.

    // Stop aggregation timer.
    aggr_timer.stop();

    // No more cells are active. Flush whatever reports are pending.
    auto& ring_elem = report_ring[next_report_start_slot.count() / period_slots];
    ring_elem.end_slot_flag.store(true, std::memory_order_release);
    handle_pending_reports();

    // Reset the next aggregated report start slot.
    next_report_start_slot = {};
  }
}
