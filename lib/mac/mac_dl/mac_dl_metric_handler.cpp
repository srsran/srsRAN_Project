/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_dl_metric_handler.h"
#include "srsran/adt/bounded_bitset.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;

mac_dl_cell_metric_report::latency_report
mac_dl_cell_metric_handler::non_persistent_data::latency_data::get_report(unsigned nof_slots_) const
{
  mac_dl_cell_metric_report::latency_report result;
  result.min      = min;
  result.max      = max;
  result.average  = sum / nof_slots_;
  result.max_slot = max_slot;
  return result;
}

void mac_dl_cell_metric_handler::non_persistent_data::latency_data::save_sample(slot_point               sl_tx,
                                                                                std::chrono::nanoseconds tdiff)
{
  sum += tdiff;
  min = std::min(min, tdiff);
  if (max < tdiff) {
    max      = tdiff;
    max_slot = sl_tx;
  }
}

mac_dl_cell_metric_handler::mac_dl_cell_metric_handler(mac_dl_metric_handler& parent_,
                                                       du_cell_index_t        cell_index_,
                                                       pci_t                  cell_pci_,
                                                       unsigned               period_slots_) :
  cell_index(cell_index_), cell_pci(cell_pci_), parent(parent_), period_slots(period_slots_)
{
}

void mac_dl_cell_metric_handler::on_cell_activation()
{
  data.deactivated = false;
}

void mac_dl_cell_metric_handler::on_cell_deactivation()
{
  data.deactivated = true;
}

void mac_dl_cell_metric_handler::handle_slot_completion(const slot_measurement& meas)
{
  // Time difference
  const auto                     stop_tp           = metric_clock::now();
  const std::chrono::nanoseconds enqueue_time_diff = meas.start_tp - meas.slot_ind_enqueue_tp;
  const std::chrono::nanoseconds time_diff         = stop_tp - meas.start_tp;

  // Get resource usage difference.
  expected<resource_usage::diff, int> rusg_diff;
  if (meas.start_rusg.has_value()) {
    auto stop_rusg = resource_usage::now();
    if (stop_rusg.has_value()) {
      rusg_diff = stop_rusg.value() - meas.start_rusg.value();
    } else {
      rusg_diff = make_unexpected(stop_rusg.error());
    }
  } else {
    rusg_diff = make_unexpected(meas.start_rusg.error());
  }

  // Update metrics.
  data.nof_slots++;
  if (not data.start_slot.valid()) {
    data.start_slot = meas.sl_tx;
  }
  data.wall.save_sample(meas.sl_tx, time_diff);
  data.slot_enqueue.save_sample(meas.sl_tx, enqueue_time_diff);
  if (meas.dl_tti_req_tp != metric_clock::time_point{}) {
    data.dl_tti_req.save_sample(meas.sl_tx, meas.dl_tti_req_tp - meas.start_tp);
    if (meas.tx_data_req_tp != metric_clock::time_point{}) {
      data.tx_data_req.save_sample(meas.sl_tx, meas.tx_data_req_tp - meas.dl_tti_req_tp);
    }
  }
  if (rusg_diff.has_value()) {
    auto& rusg_val = rusg_diff.value();
    data.count_vol_context_switches += rusg_val.vol_ctxt_switch_count;
    data.count_invol_context_switches += rusg_val.invol_ctxt_switch_count;
    data.user.save_sample(meas.sl_tx, std::chrono::nanoseconds{rusg_val.user_time});
    data.sys.save_sample(meas.sl_tx, std::chrono::nanoseconds{rusg_val.sys_time});
  }

  if (not next_report_slot.valid()) {
    // We enter here in the first call to this function.
    // We will make the \c next_report_slot aligned with the period.
    unsigned mod_val = meas.sl_tx.to_uint() % period_slots;
    next_report_slot = mod_val > 0 ? meas.sl_tx + period_slots - mod_val : meas.sl_tx;
    slot_duration    = std::chrono::nanoseconds(unsigned(1e6 / meas.sl_tx.nof_slots_per_subframe()));
  }

  if (trigger_report_slot.valid() and meas.sl_tx >= trigger_report_slot) {
    // It is time to combine all cells reports into an aggregated one and send it to notifier.
    trigger_report_slot.clear();
    parent.trigger_full_report();
  }

  if (meas.sl_tx >= next_report_slot) {
    // Prepare cell report.
    mac_dl_cell_metric_report report;
    report.pci                                = cell_pci;
    report.slot_duration                      = slot_duration;
    report.nof_slots                          = data.nof_slots;
    report.wall_clock_latency                 = data.wall.get_report(data.nof_slots);
    report.user_time                          = data.user.get_report(data.nof_slots);
    report.sys_time                           = data.sys.get_report(data.nof_slots);
    report.slot_ind_handle_latency            = data.slot_enqueue.get_report(data.nof_slots);
    report.dl_tti_req_latency                 = data.dl_tti_req.get_report(data.nof_slots);
    report.tx_data_req_latency                = data.tx_data_req.get_report(data.nof_slots);
    report.count_voluntary_context_switches   = data.count_vol_context_switches;
    report.count_involuntary_context_switches = data.count_invol_context_switches;
    report.cell_deactivated                   = data.deactivated;

    // Reset counters.
    data = {};

    // Set next report slot.
    next_report_slot += period_slots;

    // Forward cell report.
    if (parent.handle_cell_report(cell_index, report)) {
      // This cell is responsible for triggering the generation of the aggregated report once a timeout has elapsed.
      trigger_report_slot = meas.sl_tx + mac_dl_metric_handler::REPORT_GENERATION_TIMEOUT_SLOTS;
    }
  }
}

// Size of each cell queue. Should be large enough to account for the details in waking up the mac_dl_metric_handler
// to aggregate all cells separate reports.
static constexpr unsigned cell_report_queue_size = 16;

mac_dl_metric_handler::cell_context::cell_context(mac_dl_metric_handler& parent,
                                                  du_cell_index_t        cell_index_,
                                                  pci_t                  pci,
                                                  unsigned               period_slots) :
  cell_index(cell_index_), queue(cell_report_queue_size), handler(parent, cell_index, pci, period_slots)
{
}

// class mac_dl_metric_handler

mac_dl_metric_handler::mac_dl_metric_handler(std::chrono::milliseconds period_,
                                             mac_metrics_notifier&     notifier_,
                                             timer_manager&            timers_,
                                             task_executor&            ctrl_exec_) :
  period(period_),
  notifier(notifier_),
  timers(timers_),
  ctrl_exec(ctrl_exec_),
  logger(srslog::fetch_basic_logger("MAC"))
{
}

mac_dl_cell_metric_handler&
mac_dl_metric_handler::add_cell(du_cell_index_t cell_index, pci_t pci, subcarrier_spacing scs)
{
  if (not cells.contains(cell_index)) {
    unsigned period_slots = period.count() * get_nof_slots_per_subframe(scs);
    cells.emplace(cell_index, std::make_unique<cell_context>(*this, cell_index, pci, period_slots));

    // Pre-reserve space in full report object.
    next_report.dl.cells.reserve(cells.size());
  }

  return cells[cell_index]->handler;
}

void mac_dl_metric_handler::remove_cell(du_cell_index_t cell_index)
{
  cells.erase(cell_index);
}

bool mac_dl_metric_handler::handle_cell_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& cell_report)
{
  // Note: Function called from the cell execution context. Use thread-safe queue to forward result.
  if (not cells[cell_index]->queue.try_push(cell_report)) {
    logger.error("Failed to enqueue metric cell stop event. Cause: Task queue is full.");
    return false;
  }

  // If the token is acquired, it means that it is this thread's job to dispatch a job to handle pending reports.
  bool token_acquired = report_count.fetch_add(1U, std::memory_order_acq_rel) == 0;
  if (not token_acquired) {
    // Another thread is already handling the reports.
    return false;
  }

  // This thread will be responsible for handling the trigger of the full report once timeout expires.
  return true;
}

void mac_dl_metric_handler::trigger_full_report()
{
  // Defer the report handling to the control executor.
  defer_until_success(ctrl_exec, timers, [this]() { handle_pending_cell_reports(); });
}

void mac_dl_metric_handler::handle_pending_cell_reports()
{
  mac_dl_cell_metric_report cell_report;
  uint32_t                  nof_reports = report_count.load(std::memory_order_acquire);
  while (nof_reports > 0) {
    unsigned pop_count = 0;
    for (auto& cell : cells) {
      while (pop_report(*cell, cell_report)) {
        ++pop_count;
        consume_cell_report(cell->cell_index, cell_report);
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
    send_full_report();
  }
}

bool mac_dl_metric_handler::pop_report(cell_context& cell, mac_dl_cell_metric_report& report)
{
  // Peek next report.
  const auto* next_ev = cell.queue.front();

  if (next_ev == nullptr) {
    // No report to pop.
    return false;
  }

  const unsigned period_slots = next_ev->start_slot.nof_slots_per_subframe() * period.count();

  if (SRSRAN_UNLIKELY(not next_report_start_slot.valid())) {
    // This is the first report ever. Compute window start slot.
    next_report_start_slot = next_ev->start_slot - (next_ev->start_slot.to_uint() % period_slots);
    return cell.queue.try_pop(report);
  }

  if (next_ev->start_slot >= next_report_start_slot and next_ev->start_slot < next_report_start_slot + period_slots) {
    // Report is within expected window.
    return cell.queue.try_pop(report);
  }

  if (next_ev->start_slot < next_report_start_slot) {
    // Report is too old. Discard it.
    cell.queue.try_pop(report);
    logger.info(
        "cell={}: Discarding old metric report for slot {}", fmt::underlying(cell.cell_index), next_ev->start_slot);
    return false;
  }

  // Report is too new yet. Leave it in the queue.
  return false;
}

void mac_dl_metric_handler::consume_cell_report(du_cell_index_t                  cell_index,
                                                const mac_dl_cell_metric_report& cell_report)
{
  cells[cell_index]->active = not cell_report.cell_deactivated;
  next_report.dl.cells.push_back(cell_report);
}

void mac_dl_metric_handler::send_full_report()
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
  next_report.dl.cells.reserve(cells.size());
}
