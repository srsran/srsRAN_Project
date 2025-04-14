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
  parent.handle_cell_activation(cell_index);
}

void mac_dl_cell_metric_handler::on_cell_deactivation()
{
  parent.handle_cell_deactivation(cell_index);
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

    // Reset counters.
    data = {};

    // Set next report slot.
    next_report_slot += period_slots;

    // Forward cell report.
    parent.handle_cell_report(cell_index, report);
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
  logger(srslog::fetch_basic_logger("MAC")),
  active_cells_bitmap(MAX_NOF_DU_CELLS)
{
}

mac_dl_cell_metric_handler&
mac_dl_metric_handler::add_cell(du_cell_index_t cell_index, pci_t pci, subcarrier_spacing scs)
{
  if (not cells.contains(cell_index)) {
    unsigned period_slots = period.count() * get_nof_slots_per_subframe(scs);
    cells.emplace(cell_index, std::make_unique<cell_context>(*this, cell_index, pci, period_slots));

    cell_left_bitmap.fetch_add((1U << static_cast<unsigned>(cell_index)), std::memory_order_acq_rel);
  }
  return cells[cell_index]->handler;
}

void mac_dl_metric_handler::remove_cell(du_cell_index_t cell_index)
{
  cells.erase(cell_index);
}

void mac_dl_metric_handler::handle_cell_activation(du_cell_index_t cell_index)
{
  // Enqueue cell start event.
  if (not cells[cell_index]->queue.try_push(cell_start_event_type{})) {
    logger.error("Failed to enqueue metric cell start event. Cause: Task queue is full.");
  }
}

void mac_dl_metric_handler::handle_cell_deactivation(du_cell_index_t cell_index)
{
  if (not cells[cell_index]->queue.try_push(cell_stop_event_type{})) {
    logger.error("Failed to enqueue metric cell stop event. Cause: Task queue is full.");
  }
}

void mac_dl_metric_handler::handle_cell_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& cell_report)
{
  // Note: Function called from the cell execution context. Use thread-safe queue to forward result.
  if (not cells[cell_index]->queue.try_push(cell_report)) {
    logger.error("Failed to enqueue metric cell stop event. Cause: Task queue is full.");
  }

  unsigned cell_bit = 1U << static_cast<unsigned>(cell_index);
  if (cell_left_bitmap.fetch_and(~cell_bit, std::memory_order_acq_rel) == cell_bit) {
    // All cells have reported. Prepare and forward the full report.

    defer_until_success(ctrl_exec, timers, [this]() { prepare_full_report(); });
  }
}

void mac_dl_metric_handler::prepare_full_report()
{
  // Traverse all configured cells and pop pending events.
  next_report.dl.cells.reserve(cells.size());
  event_type ev;
  for (const auto& cell : cells) {
    const du_cell_index_t cell_index = cell->cell_index;

    while (cell->queue.try_pop(ev)) {
      // Handle popped event.

      if (std::get_if<cell_start_event_type>(&ev) != nullptr) {
        // Cell start event.
        active_cells_bitmap.set(cell_index);
      } else if (std::get_if<cell_stop_event_type>(&ev) != nullptr) {
        // Cell stop event.
        active_cells_bitmap.reset(cell_index);
      } else if (const mac_dl_cell_metric_report* cell_report = std::get_if<mac_dl_cell_metric_report>(&ev)) {
        // Cell report event.
        if (active_cells_bitmap.test(cell_index)) {
          next_report.dl.cells.push_back(*cell_report);
        } else {
          logger.error("Received report for inactive cell. Cell index: {}", fmt::underlying(cell_index));
        }
      } else {
        report_fatal_error("Invalid event type in cell report queue.");
      }
    }
  }

  // Set bitmap to signal to cell executors to prepare the next cell report.
  cell_left_bitmap.store(active_cells_bitmap.to_uint64(), std::memory_order_release);

  // Forward the full report.
  notifier.on_new_metrics_report(next_report);
  next_report.dl.cells.clear();
}
