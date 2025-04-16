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

using namespace srsran;

// Size of each cell queue. Should be large enough to account for the details in waking up the mac_dl_metric_handler
// to aggregate all cells separate reports.
static constexpr unsigned cell_report_queue_size = 16;

class mac_metrics_aggregator::cell_metric_handler : public mac_cell_metric_notifier
{
public:
  cell_metric_handler(mac_metrics_aggregator& parent_,
                      du_cell_index_t         cell_index_,
                      subcarrier_spacing      scs_common_,
                      srslog::basic_logger&   logger_) :
    parent(parent_),
    cell_index(cell_index_),
    scs_common(scs_common_),
    period_slots(get_nof_slots_per_subframe(scs_common) * parent.period.count()),
    logger(logger_),
    report_queue(cell_report_queue_size)
  {
  }

  void on_cell_metric_report(const mac_dl_cell_metric_report& report) override
  {
    // Note: Function called from the cell execution context.
    if (not report_queue.try_push(report)) {
      logger.error("Failed to enqueue metric cell stop event. Cause: Task queue is full.");
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

private:
  friend class mac_metrics_aggregator;

  using report_queue_type = concurrent_queue<mac_dl_cell_metric_report,
                                             concurrent_queue_policy::lockfree_spsc,
                                             concurrent_queue_wait_policy::non_blocking>;

  mac_metrics_aggregator&  parent;
  const du_cell_index_t    cell_index;
  const subcarrier_spacing scs_common;
  const unsigned           period_slots;
  srslog::basic_logger&    logger;

  // Reports from a given MAC cell.
  report_queue_type report_queue;
};

// class mac_metrics_aggregator

mac_metrics_aggregator::mac_metrics_aggregator(std::chrono::milliseconds period_,
                                               mac_metrics_notifier&     notifier_,
                                               task_executor&            ctrl_exec_,
                                               timer_manager&            timers_,
                                               srslog::basic_logger&     logger_) :
  period(period_), notifier(notifier_), ctrl_exec(ctrl_exec_), logger(logger_)
{
  aggr_timer = timers_.create_unique_timer(ctrl_exec);
  aggr_timer.set(aggregation_timeout, [this](timer_id_t /* unused */) { trigger_report_creation(); });
}

mac_metrics_aggregator::~mac_metrics_aggregator() {}

mac_cell_metric_report_config mac_metrics_aggregator::add_cell(du_cell_index_t    cell_index,
                                                               subcarrier_spacing scs_common)
{
  srsran_assert(not cells.contains(cell_index), "Duplicate cell creation");

  // Reserve space in reports for new cell.
  next_report.dl.cells.reserve(cells.size());

  // Create a handler for the new cell.
  auto  cell_handler = std::make_unique<cell_metric_handler>(*this, cell_index, scs_common, logger);
  auto& cell_ref     = *cell_handler;
  cells.emplace(cell_index, std::move(cell_handler));

  // Return the cell report configuration.
  return mac_cell_metric_report_config{static_cast<unsigned>(period.count() * get_nof_slots_per_subframe(scs_common)),
                                       &cell_ref};
}

void mac_metrics_aggregator::rem_cell(du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "Cell not found");
  cells.erase(cell_index);
}

void mac_metrics_aggregator::handle_cell_metrics_report(du_cell_index_t                  cell_index,
                                                        const mac_dl_cell_metric_report& report)
{
}

void mac_metrics_aggregator::trigger_report_creation()
{
  // Aggregate all cell reports.
  mac_dl_cell_metric_report cell_report;
  uint32_t                  nof_reports = report_count.load(std::memory_order_acquire);
  while (nof_reports > 0) {
    unsigned pop_count = 0;
    for (auto& cell : cells) {
      while (pop_report(*cell, cell_report)) {
        ++pop_count;

        // Consume the report.
        next_report.dl.cells.push_back(cell_report);

        if (pop_count == nof_reports) {
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

    // Send full report to notifier.
    send_new_report();
  }
}

bool mac_metrics_aggregator::pop_report(cell_metric_handler& cell, mac_dl_cell_metric_report& report)
{
  // Peek next report.
  const auto* next_ev = cell.report_queue.front();

  if (next_ev == nullptr) {
    // No report to pop.
    return false;
  }

  const unsigned& period_slots = cell.period_slots;

  if (SRSRAN_UNLIKELY(not next_report_start_slot.valid())) {
    // This is the first report ever. Compute window start slot.
    next_report_start_slot = next_ev->start_slot - (next_ev->start_slot.to_uint() % period_slots);
    return cell.report_queue.try_pop(report);
  }

  if (next_ev->start_slot >= next_report_start_slot and next_ev->start_slot < next_report_start_slot + period_slots) {
    // Report is within expected window.
    return cell.report_queue.try_pop(report);
  }

  if (next_ev->start_slot >= next_report_start_slot + period_slots and
      next_ev->start_slot < next_report_start_slot + 2 * period_slots) {
    // This is the report for the next window. Leave it in the queue to be processed later.
    return false;
  }

  // Report falls in invalid window. Discard it.
  cell.report_queue.try_pop(report);
  logger.info(
      "cell={}: Discarding old metric report for slot {}", fmt::underlying(cell.cell_index), next_ev->start_slot);
  return false;
}

void mac_metrics_aggregator::send_new_report()
{
  // Forward the full report.
  notifier.on_new_metrics_report(next_report);

  // Update next report window start slot.
  if (not next_report.dl.cells.empty()) {
    const unsigned period_slots = next_report.dl.cells[0].start_slot.nof_slots_per_subframe() * period.count();
    const unsigned start_mod    = next_report.dl.cells[0].start_slot.to_uint() % period_slots;
    next_report_start_slot      = (next_report.dl.cells[0].start_slot - start_mod) + period_slots;
  }

  // Reset report.
  next_report.dl.cells.clear();
}
