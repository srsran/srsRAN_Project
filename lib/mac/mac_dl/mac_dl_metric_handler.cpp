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

#include "mac_dl_metric_handler.h"
#include "srsran/mac/mac_metrics_notifier.h"
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

mac_dl_cell_metric_handler::mac_dl_cell_metric_handler(
    du_cell_index_t                                                        cell_index_,
    pci_t                                                                  cell_pci_,
    unsigned                                                               period_slots_,
    std::function<void(du_cell_index_t, const mac_dl_cell_metric_report&)> on_new_cell_report_) :
  cell_index(cell_index_),
  cell_pci(cell_pci_),
  on_new_cell_report(std::move(on_new_cell_report_)),
  period_slots(period_slots_)
{
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

mac_dl_cell_metric_handler&
mac_dl_metric_handler::add_cell(du_cell_index_t cell_index, pci_t pci, subcarrier_spacing scs)
{
  if (not cells.contains(cell_index)) {
    unsigned period_slots = period.count() * get_nof_slots_per_subframe(scs);
    cells.emplace(
        cell_index,
        std::make_unique<cell_context>(
            cell_index, pci, period_slots, [this](du_cell_index_t cidx, const mac_dl_cell_metric_report& rep) {
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
