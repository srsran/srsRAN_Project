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
static constexpr unsigned cell_report_queue_size = 16;

namespace {

struct recycle_sched_report {
  void operator()(scheduler_cell_metrics& report)
  {
    report.events.clear();
    report.ue_metrics.clear();
  }
};

using sched_queue_report_channel = spsc_metric_report_channel<scheduler_cell_metrics, recycle_sched_report>;

} // namespace

class mac_metrics_aggregator::cell_metric_handler final : public mac_cell_metric_notifier,
                                                          public sched_queue_report_channel
{
public:
  cell_metric_handler(mac_metrics_aggregator&     parent_,
                      du_cell_index_t             cell_index_,
                      subcarrier_spacing          scs_common_,
                      scheduler_metrics_notifier* sched_notifier_,
                      srslog::basic_logger&       logger_) :
    sched_queue_report_channel(cell_report_queue_size,
                               logger_,
                               []() {
                                 scheduler_cell_metrics cell_metrics{};
                                 cell_metrics.ue_metrics.reserve(MAX_NOF_DU_UES);
                                 // Note: there can be more than one event per UE.
                                 constexpr unsigned prereserved_events_per_ue = 3;
                                 cell_metrics.events.reserve(MAX_NOF_DU_UES * prereserved_events_per_ue);
                                 return cell_metrics;
                               }),
    parent(parent_),
    cell_index(cell_index_),
    scs_common(scs_common_),
    period_slots(get_nof_slots_per_subframe(scs_common) * parent.period.count()),
    sched_notifier(sched_notifier_),
    logger(logger_),
    mac_report_queue(cell_report_queue_size)
  {
  }

  void on_cell_activation(slot_point first_report_slot) override
  {
    defer_until_success(parent.ctrl_exec, parent.timers, [this, first_report_slot]() {
      parent.handle_cell_activation(cell_index, first_report_slot);
    });
  }

  void on_cell_deactivation(const mac_dl_cell_metric_report& report) override
  {
    last_report = report;
    defer_until_success(
        parent.ctrl_exec, parent.timers, [this]() { parent.handle_cell_deactivation(cell_index, last_report); });
  }

  void on_cell_metric_report(const mac_dl_cell_metric_report& report) override
  {
    // Note: Function called from the cell execution context.
    if (not mac_report_queue.try_push(report)) {
      logger.error("Failed to enqueue MAC metric event. Cause: Task queue is full.");
      return;
    }

    // If the token is acquired, it means that it is this thread's job to dispatch a job to handle pending reports.
    bool token_acquired = parent.report_count.fetch_add(1U, std::memory_order_acq_rel) == 0;
    if (not token_acquired) {
      // Another thread is already handling the reports.
      return;
    }

    // Defer the report handling to the control executor.
    parent.aggr_timer.run();
  }

  /// \brief Called by the scheduler to commit the report.
  /// Note: This function is called from the scheduler execution context.
  void commit(scheduler_cell_metrics& report) override
  {
    if (sched_notifier != nullptr) {
      // TODO: Remove this and use only DU metrics interface.
      sched_notifier->report_metrics(report);
    }
    sched_queue_report_channel::commit(report);
  }

private:
  friend class mac_metrics_aggregator;

  /// Queue type used to push new MAC cell reports.
  using mac_report_queue_type = concurrent_queue<mac_dl_cell_metric_report,
                                                 concurrent_queue_policy::lockfree_spsc,
                                                 concurrent_queue_wait_policy::non_blocking>;

  sched_queue_report_channel& sched_queue() { return *static_cast<sched_queue_report_channel*>(this); }

  mac_metrics_aggregator&     parent;
  const du_cell_index_t       cell_index;
  const subcarrier_spacing    scs_common;
  const unsigned              period_slots;
  scheduler_metrics_notifier* sched_notifier;
  srslog::basic_logger&       logger;

  // Reports from a given MAC cell.
  mac_report_queue_type     mac_report_queue;
  mac_dl_cell_metric_report last_report{};

  // Stateful flags access from the control executor.
  bool       active_flag = false;
  slot_point next_report_slot;
  bool       report_ready = false;
};

// class mac_metrics_aggregator

mac_metrics_aggregator::mac_metrics_aggregator(std::chrono::milliseconds   period_,
                                               mac_metrics_notifier&       mac_notifier_,
                                               scheduler_metrics_notifier* sched_notifier_,
                                               task_executor&              ctrl_exec_,
                                               timer_manager&              timers_,
                                               srslog::basic_logger&       logger_) :
  period(period_),
  notifier(mac_notifier_),
  sched_notifier(sched_notifier_),
  ctrl_exec(ctrl_exec_),
  timers(timers_),
  logger(logger_)
{
  aggr_timer = timers_.create_unique_timer(ctrl_exec);
  aggr_timer.set(aggregation_timeout, [this](timer_id_t /* unused */) { handle_pending_reports(); });
}

mac_metrics_aggregator::~mac_metrics_aggregator() {}

cell_metric_report_config mac_metrics_aggregator::add_cell(du_cell_index_t cell_index, subcarrier_spacing scs_common)
{
  srsran_assert(not cells.contains(cell_index), "Duplicate cell creation");

  // Reserve space in reports for new cell.
  next_report.dl.cells.reserve(cells.size());
  next_report.sched.cells.reserve(cells.size());

  // Create a handler for the new cell.
  auto  cell_handler = std::make_unique<cell_metric_handler>(*this, cell_index, scs_common, sched_notifier, logger);
  auto& cell_ref     = *cell_handler;
  cells.emplace(cell_index, std::move(cell_handler));

  // Return the cell report configuration.
  return cell_metric_report_config{period, &cell_ref, &cell_ref};
}

void mac_metrics_aggregator::rem_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell not found");
  cells.erase(cell_index);
}

void mac_metrics_aggregator::handle_pending_reports()
{
  mac_dl_cell_metric_report cell_report;
  scheduler_cell_metrics    sched_report;
  uint32_t                  nof_reports = report_count.load(std::memory_order_acquire);
  while (nof_reports > 0) {
    unsigned pop_count = 0;
    for (auto& cell : cells) {
      while (pop_sched_report(*cell, sched_report)) {
        // Consume the SCHED report.
        next_report.sched.cells.push_back(sched_report);
      }
      for (auto result = pop_mac_report(*cell, cell_report); result != pop_result::no_op and pop_count != nof_reports;
           result      = pop_mac_report(*cell, cell_report)) {
        ++pop_count;

        // Consume the report.
        if (result == pop_result::report) {
          next_report.dl.cells.push_back(cell_report);
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

bool mac_metrics_aggregator::pop_sched_report(cell_metric_handler& cell, scheduler_cell_metrics& report)
{
  // Peek next report.
  scheduler_cell_metrics* next_rep = cell.sched_queue().peek();
  if (next_rep == nullptr) {
    // No report to pop.
    return false;
  }

  if (SRSRAN_UNLIKELY(not next_report_start_slot.valid())) {
    // This is the first report ever. Compute window start slot.
    next_report_start_slot = next_rep->slot - (next_rep->slot.to_uint() % cell.period_slots);
    report                 = *next_rep;
    cell.sched_queue().pop();
    return true;
  }

  if (next_rep->slot >= next_report_start_slot and next_rep->slot < next_report_start_slot + cell.period_slots) {
    // Report is within expected window.
    report = *next_rep;
    cell.sched_queue().pop();
    return true;
  }

  if (next_rep->slot >= next_report_start_slot + cell.period_slots and
      next_rep->slot < next_report_start_slot + 2 * cell.period_slots) {
    // This is the report for the next window. Leave it in the queue to be processed later.
    return false;
  }

  // Report falls in invalid window. Discard it.
  cell.sched_queue().pop();
  logger.info("cell={}: Discarding old metric report for slot {}", fmt::underlying(cell.cell_index), report.slot);
  return false;
}

mac_metrics_aggregator::pop_result mac_metrics_aggregator::pop_mac_report(cell_metric_handler&       cell,
                                                                          mac_dl_cell_metric_report& report)
{
  // Peek next report.
  const auto* next_ev = cell.mac_report_queue.front();

  if (next_ev == nullptr) {
    // No report to pop.
    return pop_result::no_op;
  }

  if (next_ev->start_slot >= next_report_start_slot + cell.period_slots and
      next_ev->start_slot < next_report_start_slot + 2 * cell.period_slots) {
    // This is the report for the next window. Leave it in the queue to be processed later.
    return pop_result::no_op;
  }

  if (not cell.mac_report_queue.try_pop(report)) {
    logger.warning("cell={}: Unable to pop metric report event", fmt::underlying(cell.cell_index));
    return pop_result::no_op;
  }

  if (report.start_slot >= next_report_start_slot and report.start_slot < next_report_start_slot + cell.period_slots) {
    // Report is within expected window.
    cell.report_ready = true;
    return pop_result::report;
  }

  // Report falls in invalid window. Discard it.
  logger.info("cell={}: Discarding old metric report for slot {}", fmt::underlying(cell.cell_index), report.start_slot);
  return pop_result::pop_and_discarded;
}

void mac_metrics_aggregator::try_send_new_report()
{
  if (next_report.dl.cells.empty()) {
    return;
  }
  for (const auto& c : cells) {
    if (c->active_flag and next_report_start_slot + c->period_slots == c->next_report_slot and not c->report_ready) {
      // Metric report missing for this cell.
      return;
    }
  }
  if (not next_report.sched.cells.empty() and next_report.sched.cells.size() != next_report.dl.cells.size()) {
    logger.warning("Mismatch in number of cell metrics reported by the MAC and scheduler ({} != {})",
                   next_report.dl.cells.size(),
                   next_report.sched.cells.size());
  }

  // Forward the full report.
  notifier.on_new_metrics_report(next_report);

  // Update next report window start slot.
  const unsigned period_slots = next_report.dl.cells[0].start_slot.nof_slots_per_subframe() * period.count();
  const unsigned start_mod    = next_report.dl.cells[0].start_slot.to_uint() % period_slots;
  next_report_start_slot      = (next_report.dl.cells[0].start_slot - start_mod) + period_slots;
  for (auto& c : cells) {
    if (c->active_flag) {
      c->next_report_slot = next_report_start_slot;
      c->report_ready     = false;
    }
  }

  // Reset report.
  next_report.dl.cells.clear();
  next_report.sched.cells.clear();
}

void mac_metrics_aggregator::handle_cell_activation(du_cell_index_t cell_index, slot_point first_report_slot)
{
  srsran_assert(cells.contains(cell_index), "MAC cell activated but not configured");
  auto& cell = *cells[cell_index];
  srsran_assert(not cell.active_flag, "Reactivation of cell not supported");

  cell.active_flag      = true;
  cell.report_ready     = false;
  cell.next_report_slot = first_report_slot;
  if (not next_report_start_slot.valid()) {
    // This is the first report ever. Compute window start slot.
    next_report_start_slot = first_report_slot - cell.period_slots;
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

  // Save last report before deactivating cell.
  if (last_report.start_slot >= next_report_start_slot and
      last_report.start_slot < next_report_start_slot + cell.period_slots) {
    next_report.dl.cells.push_back(last_report);
    cell.report_ready = true;
  }
  cell.active_flag      = false;
  cell.next_report_slot = {};
  if (--nof_active_cells == 0) {
    // No cells are active. Reset the next aggregated report start slot.
    next_report_start_slot = {};
  }
}
