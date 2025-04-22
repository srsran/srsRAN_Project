/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_metrics_aggregator.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/adt/spsc_queue.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;

// Size of each cell queue. Should be large enough to account for the details in waking up the mac_dl_metric_handler
// to aggregate all cells separate reports.
static constexpr unsigned cell_report_queue_size = 16;

class mac_metrics_aggregator::cell_metric_handler final : public mac_cell_metric_notifier,
                                                          public scheduler_metrics_notifier
{
public:
  cell_metric_handler(mac_metrics_aggregator&     parent_,
                      du_cell_index_t             cell_index_,
                      subcarrier_spacing          scs_common_,
                      scheduler_metrics_notifier* sched_notifier_,
                      srslog::basic_logger&       logger_) :
    parent(parent_),
    cell_index(cell_index_),
    scs_common(scs_common_),
    period_slots(get_nof_slots_per_subframe(scs_common) * parent.period.count()),
    sched_notifier(sched_notifier_),
    logger(logger_),
    mac_report_queue(cell_report_queue_size),
    sched_report_queue(cell_report_queue_size)
  {
  }

  void on_cell_activation(slot_point first_report_slot) override
  {
    // We use a report with zero slots to represent a start cell event.
    mac_dl_cell_metric_report report{};
    report.start_slot       = first_report_slot;
    report.nof_slots        = 0;
    report.cell_deactivated = false;
    on_cell_metric_report(report);
  }

  void on_cell_deactivation(const mac_dl_cell_metric_report& report) override
  {
    srsran_assert(report.cell_deactivated, "Expected cell deactivated flag being set");
    on_cell_metric_report(report);
  }

  void on_cell_metric_report(const mac_dl_cell_metric_report& report) override
  {
    // Note: Function called from the cell execution context.
    if (not mac_report_queue.try_push(report)) {
      logger.error("Failed to enqueue MAC metric event. Cause: Task queue is full.");
      return;
    }

    // If the token is acquired, it means that it is this thread's job to dispatch a job to handle pending reports.
    bool token_acquired = parent.events_until_trigger.fetch_sub(1U, std::memory_order_acq_rel) - 1 <= 0;
    if (not token_acquired) {
      // Another thread is already handling the reports.
      return;
    }

    // Dispatch a job to handle the pending reports.
    defer_until_success(parent.ctrl_exec, parent.timers, [this]() { parent.handle_pending_reports(); });
  }

  void report_metrics(const scheduler_cell_metrics& report) override
  {
    // Note: Function called from the cell execution context.

    if (sched_notifier != nullptr) {
      sched_notifier->report_metrics(report);
    }

    if (not sched_report_queue.try_push(report)) {
      logger.error("Failed to enqueue scheduler metric event. Cause: Task queue is full.");
      return;
    }
  }

private:
  friend class mac_metrics_aggregator;

  /// Queue type used to push new MAC cell reports.
  using mac_report_queue_type = concurrent_queue<mac_dl_cell_metric_report,
                                                 concurrent_queue_policy::lockfree_spsc,
                                                 concurrent_queue_wait_policy::non_blocking>;

  /// Queue type used to push new SCHED cell reports.
  using sched_report_queue_type = concurrent_queue<scheduler_cell_metrics,
                                                   concurrent_queue_policy::lockfree_spsc,
                                                   concurrent_queue_wait_policy::non_blocking>;

  mac_metrics_aggregator&     parent;
  const du_cell_index_t       cell_index;
  const subcarrier_spacing    scs_common;
  const unsigned              period_slots;
  scheduler_metrics_notifier* sched_notifier;
  srslog::basic_logger&       logger;

  // Reports from a given MAC cell.
  mac_report_queue_type mac_report_queue;

  // Reports from the scheduler.
  sched_report_queue_type sched_report_queue;

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
  int                       current_count = events_until_trigger.load(std::memory_order_acquire);
  while (current_count <= 0) {
    int  pop_count    = 0;
    bool popped_event = false;
    for (auto& cell : cells) {
      while (pop_sched_report(*cell, sched_report)) {
        // Consume the SCHED report.
        next_report.sched.cells.push_back(sched_report);
      }

      for (auto result = pop_mac_report(*cell, cell_report); result != pop_result::no_op;
           result      = pop_mac_report(*cell, cell_report)) {
        pop_count++;
        popped_event = true;
        switch (result) {
          case pop_result::cell_activated:
            // Increase the number of events needed until the next trigger, as now we expect one more cell to report.
            pop_count++;
            break;
          case pop_result::cell_deactivated:
            // Decrease the number of events needed until the next trigger.
            pop_count--;
            next_report.dl.cells.push_back(cell_report);
            break;
          case pop_result::report:
            next_report.dl.cells.push_back(cell_report);
            break;
          case pop_result::pop_and_discarded:
            break;
          default:
            report_fatal_error("Unsupported case");
        }
      }
    }

    // Update number of events needed until next trigger.
    current_count = events_until_trigger.fetch_add(pop_count, std::memory_order_acq_rel) + pop_count;

    if (current_count <= 0 and not popped_event) {
      logger.error("Unable to dequeue any MAC cell metric report");
      break;
    }

    // Send an aggregated report if ready.
    try_send_new_report();
  }
}

bool mac_metrics_aggregator::pop_sched_report(cell_metric_handler& cell, scheduler_cell_metrics& report)
{
  // Peek next report.
  const auto* next_ev = cell.sched_report_queue.front();
  if (next_ev == nullptr) {
    // No report to pop.
    return false;
  }

  if (SRSRAN_UNLIKELY(not next_report_start_slot.valid())) {
    // This is the first report ever. Compute window start slot.
    next_report_start_slot = next_ev->slot - (next_ev->slot.to_uint() % cell.period_slots);
    return cell.sched_report_queue.try_pop(report);
  }

  if (next_ev->slot >= next_report_start_slot and next_ev->slot < next_report_start_slot + cell.period_slots) {
    // Report is within expected window.
    return cell.sched_report_queue.try_pop(report);
  }

  if (next_ev->slot >= next_report_start_slot + cell.period_slots and
      next_ev->slot < next_report_start_slot + 2 * cell.period_slots) {
    // This is the report for the next window. Leave it in the queue to be processed later.
    return false;
  }

  // Report falls in invalid window. Discard it.
  cell.sched_report_queue.try_pop(report);
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

  if (next_ev->nof_slots == 0 or next_ev->cell_deactivated) {
    // We are either starting or stopping a cell.
    if (not cell.mac_report_queue.try_pop(report)) {
      logger.warning("cell={}: Unable to pop metric report event", fmt::underlying(cell.cell_index));
      return pop_result::no_op;
    }
    if (report.cell_deactivated) {
      // Cell Stop signal.
      cell.active_flag      = false;
      cell.next_report_slot = {};
      nof_cell_active--;
      if (nof_cell_active == 0) {
        // No cells are active. Reset the next aggregated report start slot.
        next_report_start_slot = {};
      }
      return pop_result::cell_deactivated;
    }
    // Cell Start signal.
    cell.active_flag      = true;
    cell.next_report_slot = next_ev->start_slot;
    if (not next_report_start_slot.valid()) {
      // This is the first report ever. Compute window start slot.
      next_report_start_slot = next_ev->start_slot - cell.period_slots;
    }
    nof_cell_active++;
    return pop_result::cell_activated;
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
