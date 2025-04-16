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

mac_dl_cell_metric_handler::mac_dl_cell_metric_handler(pci_t                                cell_pci_,
                                                       const mac_cell_metric_report_config& metrics_cfg) :
  cell_pci(cell_pci_), period_slots(metrics_cfg.period_slots), notifier(*metrics_cfg.notifier)
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
    notifier.on_cell_metric_report(report);
  }
}
