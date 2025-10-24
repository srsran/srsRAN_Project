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
#include "srsran/srslog/srslog.h"

using namespace srsran;

mac_dl_cell_metric_report::latency_report
mac_dl_cell_metric_handler::non_persistent_data::latency_data::get_report() const
{
  mac_dl_cell_metric_report::latency_report result;
  result.min = min;
  result.max = max;
  if (count > 0) {
    result.average  = sum / count;
    result.max_slot = max_slot;
  } else {
    result.average  = std::chrono::nanoseconds(0);
    result.max_slot = slot_point{};
  }
  return result;
}

void mac_dl_cell_metric_handler::non_persistent_data::latency_data::save_sample(slot_point               sl_tx,
                                                                                std::chrono::nanoseconds tdiff)
{
  ++count;
  sum += tdiff;
  min = std::min(min, tdiff);
  if (max < tdiff) {
    max      = tdiff;
    max_slot = sl_tx;
  }
}

// mac_dl_cell_metric_handler

mac_dl_cell_metric_handler::mac_dl_cell_metric_handler(pci_t                     cell_pci_,
                                                       subcarrier_spacing        scs,
                                                       mac_cell_metric_notifier* notifier_) :
  cell_pci(cell_pci_),
  notifier(notifier_),
  slot_duration(std::chrono::nanoseconds(unsigned(1e6 / (get_nof_slots_per_subframe(scs)))))
{
}

void mac_dl_cell_metric_handler::on_cell_activation(slot_point sl_tx)
{
  if (not enabled() or active()) {
    return;
  }
  last_sl_tx = sl_tx;
  data       = {};
  // Notify cell creation and next slot on which the report will be generated.
  notifier->on_cell_activation();
}

void mac_dl_cell_metric_handler::on_cell_deactivation()
{
  if (not enabled() or not active()) {
    // Activation hasn't started or completed.
    return;
  }

  // Report the remainder metrics.
  data.last_report = true;
  send_new_report();
  last_sl_tx = {};
}

void mac_dl_cell_metric_handler::handle_slot_completion(const slot_measurement& meas)
{
  if (not enabled() or not active()) {
    return;
  }
  const unsigned nof_skipped_slots = last_sl_tx.valid() ? meas.sl_tx - last_sl_tx : 0;
  last_sl_tx                       = meas.sl_tx;

  // Time difference
  const auto                     stop_tp           = metric_clock::now();
  const std::chrono::nanoseconds enqueue_time_diff = meas.start_tp - meas.slot_ind_enqueue_tp;
  const std::chrono::nanoseconds time_diff         = stop_tp - meas.start_tp;

  // Compute resource usage in a lambda function.
  auto compute_diff = [&meas]() -> expected<resource_usage::diff, int> {
    // Get resource usage difference.
    if (meas.start_rusg.has_value()) {
      auto stop_rusg = resource_usage::now();
      if (stop_rusg.has_value()) {
        return stop_rusg.value() - *meas.start_rusg;
      }
    }
    return make_unexpected(meas.start_rusg.error());
  };

  // Get resource usage difference.
  expected<resource_usage::diff, int> rusg_diff = compute_diff();

  std::chrono::nanoseconds consecutive_slot_ind_time_diff{0};
  if (last_slot_ind_enqueue_tp != metric_clock::time_point{} and nof_skipped_slots == 1) {
    consecutive_slot_ind_time_diff = meas.slot_ind_enqueue_tp - last_slot_ind_enqueue_tp;
  }
  last_slot_ind_enqueue_tp = meas.slot_ind_enqueue_tp;

  // Update metrics.
  data.nof_slots++;
  if (not data.start_slot.valid()) {
    data.start_slot = meas.sl_tx;
  }
  data.wall.save_sample(meas.sl_tx, time_diff);
  data.slot_dequeue.save_sample(meas.sl_tx, enqueue_time_diff);
  data.sched.save_sample(meas.sl_tx, meas.sched_tp - meas.start_tp);
  auto last_tp = meas.sched_tp;
  if (meas.dl_tti_req_tp != metric_clock::time_point{}) {
    data.dl_tti_req.save_sample(meas.sl_tx, meas.dl_tti_req_tp - last_tp);
    last_tp = meas.dl_tti_req_tp;
    if (meas.tx_data_req_tp != metric_clock::time_point{}) {
      data.tx_data_req.save_sample(meas.sl_tx, meas.tx_data_req_tp - last_tp);
      last_tp = meas.tx_data_req_tp;
    }
  }
  if (meas.ul_tti_req_tp != metric_clock::time_point{}) {
    data.ul_tti_req.save_sample(meas.sl_tx, meas.ul_tti_req_tp - last_tp);
  }
  if (rusg_diff.has_value()) {
    auto& rusg_val = rusg_diff.value();
    data.count_vol_context_switches += rusg_val.vol_ctxt_switch_count;
    data.count_invol_context_switches += rusg_val.invol_ctxt_switch_count;
  }
  if (consecutive_slot_ind_time_diff != std::chrono::nanoseconds{0}) {
    data.slot_distance.save_sample(meas.sl_tx, consecutive_slot_ind_time_diff);
  }

  if (notifier->is_report_required(meas.sl_tx)) {
    send_new_report();
  }
}

void mac_dl_cell_metric_handler::send_new_report()
{
  // Prepare cell report.
  mac_dl_cell_metric_report report;
  report.pci                                = cell_pci;
  report.start_slot                         = data.start_slot;
  report.slot_duration                      = slot_duration;
  report.nof_slots                          = data.nof_slots;
  report.wall_clock_latency                 = data.wall.get_report();
  report.slot_ind_dequeue_latency           = data.slot_dequeue.get_report();
  report.sched_latency                      = data.sched.get_report();
  report.dl_tti_req_latency                 = data.dl_tti_req.get_report();
  report.tx_data_req_latency                = data.tx_data_req.get_report();
  report.ul_tti_req_latency                 = data.ul_tti_req.get_report();
  report.slot_ind_msg_time_diff             = data.slot_distance.get_report();
  report.count_voluntary_context_switches   = data.count_vol_context_switches;
  report.count_involuntary_context_switches = data.count_invol_context_switches;
  report.cell_deactivated                   = data.last_report;

  // Reset counters.
  data = {};

  if (not report.cell_deactivated) {
    // Forward normal cell report.
    notifier->on_cell_metric_report(report);
  } else {
    notifier->on_cell_deactivation(report);
  }
}
