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
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;

mac_dl_cell_metric_report::latency_report
mac_dl_cell_metric_handler::non_persistent_data::latency_data::get_report(unsigned nof_slots_) const
{
  mac_dl_cell_metric_report::latency_report result;
  result.min     = min;
  result.max     = max;
  result.average = sum / nof_slots_;
  return result;
}

mac_dl_cell_metric_handler::mac_dl_cell_metric_handler(
    du_cell_index_t                                                        cell_index_,
    unsigned                                                               period_slots_,
    std::function<void(du_cell_index_t, const mac_dl_cell_metric_report&)> on_new_cell_report_) :
  cell_index(cell_index_), on_new_cell_report(std::move(on_new_cell_report_)), period_slots(period_slots_)
{
}

void mac_dl_cell_metric_handler::handle_slot_completion(slot_point                                     sl_tx,
                                                        std::chrono::high_resolution_clock::time_point start_tp,
                                                        const expected<resource_usage::snapshot, int>& start_rusg)
{
  // Time difference
  auto                     stop_tp   = std::chrono::high_resolution_clock::now();
  std::chrono::nanoseconds time_diff = stop_tp - start_tp;

  // Get resource usage difference.
  expected<resource_usage::diff, int> rusg_diff;
  if (start_rusg.has_value()) {
    auto stop_rusg = resource_usage::now();
    if (stop_rusg.has_value()) {
      rusg_diff = stop_rusg.value() - start_rusg.value();
    } else {
      rusg_diff = make_unexpected(stop_rusg.error());
    }
  } else {
    rusg_diff = make_unexpected(start_rusg.error());
  }

  // Update metrics.
  data.nof_slots++;
  data.wall.sum += time_diff;
  data.wall.max = std::max(data.wall.max, time_diff);
  data.wall.min = std::min(data.wall.min, time_diff);
  if (rusg_diff.has_value()) {
    auto& rusg_val = rusg_diff.value();
    data.count_vol_context_switches += rusg_val.vol_ctxt_switch_count;
    data.count_invol_context_switches += rusg_val.invol_ctxt_switch_count;
    data.user.sum += rusg_val.user_time;
    data.user.max = std::max(std::chrono::nanoseconds{rusg_val.user_time}, data.user.max);
    data.user.min = std::min(std::chrono::nanoseconds{rusg_val.user_time}, data.user.min);
    data.sys.sum += rusg_val.sys_time;
    data.sys.max = std::max(std::chrono::nanoseconds{rusg_val.sys_time}, data.sys.max);
    data.sys.min = std::min(std::chrono::nanoseconds{rusg_val.sys_time}, data.sys.min);
  }

  if (not next_report_slot.valid()) {
    // We enter here in the first call to this function.
    // We will make the \c next_report_slot aligned with the period.
    unsigned mod_val = sl_tx.to_uint() % period_slots;
    next_report_slot = mod_val > 0 ? sl_tx + period_slots - mod_val : sl_tx;
  }

  if (sl_tx >= next_report_slot) {
    // Prepare cell report.
    mac_dl_cell_metric_report report;
    report.slot_duration                    = std::chrono::nanoseconds(unsigned(1e9 / sl_tx.nof_slots_per_subframe()));
    report.nof_slots                        = data.nof_slots;
    report.wall_clock_latency               = data.wall.get_report(data.nof_slots);
    report.user_time                        = data.user.get_report(data.nof_slots);
    report.sys_time                         = data.sys.get_report(data.nof_slots);
    report.count_voluntary_context_switches = data.count_vol_context_switches;
    report.count_involuntary_context_switches = data.count_invol_context_switches;

    // Reset counters.
    data = {};

    // Set next report slot.
    next_report_slot += period_slots;

    // Forward cell report.
    on_new_cell_report(cell_index, report);
  }
}

mac_dl_metric_handler::mac_dl_metric_handler(std::chrono::milliseconds period_,
                                             mac_metrics_notifier&     notifier_,
                                             timer_manager&            timers_,
                                             task_executor&            ctrl_exec_) :
  period(period_), notifier(notifier_), timers(timers_), ctrl_exec(ctrl_exec_)
{
}

mac_dl_cell_metric_handler& mac_dl_metric_handler::add_cell(du_cell_index_t cell_index, subcarrier_spacing scs)
{
  if (not cells.contains(cell_index)) {
    unsigned period_slots = period.count() * get_nof_slots_per_subframe(scs);
    cells.emplace(cell_index,
                  std::make_unique<cell_context>(
                      cell_index, period_slots, [this](du_cell_index_t cidx, const mac_dl_cell_metric_report& rep) {
                        handle_cell_report(cidx, rep);
                      }));
    cell_left_bitmap.fetch_add((1U << static_cast<unsigned>(cell_index)), std::memory_order_acq_rel);
  }
  return cells[cell_index]->handler;
}

void mac_dl_metric_handler::remove_cell(du_cell_index_t cell_index)
{
  // TODO
}

void mac_dl_metric_handler::handle_cell_report(du_cell_index_t cell_index, const mac_dl_cell_metric_report& cell_report)
{
  // Note: Function called from the cell execution context. Use thread-safe queue to forward result.
  cells[cell_index]->queue.try_push(cell_report);

  unsigned cell_bit = 1U << static_cast<unsigned>(cell_index);
  if (cell_left_bitmap.fetch_and(~cell_bit, std::memory_order_acq_rel) == cell_bit) {
    // All cells have reported. Prepare and forward the full report.

    defer_until_success(ctrl_exec, timers, [this]() { prepare_full_report(); });
  }
}

void mac_dl_metric_handler::prepare_full_report()
{
  next_report.dl.cells.resize(cells.size());

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    auto& cell = cells[i];

    if (not cell->queue.try_pop(next_report.dl.cells[i])) {
      // Failed to pack result.
      next_report.dl.cells[i] = {};
    }
  }

  // Set bitmap to signal to cell executors to prepare the next cell report.
  cell_left_bitmap.store((1U << cells.size()) - 1U, std::memory_order_release);

  // Forward the full report.
  notifier.on_new_metrics_report(next_report);
}
