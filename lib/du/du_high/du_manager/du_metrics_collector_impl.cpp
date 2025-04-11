/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_collector_impl.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;
using namespace srs_du;

du_manager_metrics_collector_impl::du_manager_metrics_collector_impl(
    const du_manager_params::metrics_config_params& params_,
    task_executor&                                  du_mng_exec_,
    timer_manager&                                  timers_,
    f1ap_metrics_collector&                         f1ap_collector_,
    mac_metrics_notifier*                           mac_notifier_,
    scheduler_metrics_notifier*                     sched_notifier_) :
  params(params_),
  du_mng_exec(du_mng_exec_),
  timers(timers_),
  mac_notifier(mac_notifier_),
  sched_notifier(sched_notifier_),
  f1ap_collector(f1ap_collector_)
{
}

void du_manager_metrics_collector_impl::handle_mac_metrics_report(const mac_metric_report& report)
{
  // Forward the MAC report to notifier.
  if (mac_notifier != nullptr) {
    mac_notifier->on_new_metrics_report(report);
  }

  // In case the DU metrics notifier was specified, report the DU metrics.
  if (params.du_metrics != nullptr) {
    execute_until_success(du_mng_exec, timers, [this]() { trigger_report(); });
  }
}

void du_manager_metrics_collector_impl::handle_scheduler_metrics_report(const scheduler_cell_metrics& report)
{
  if (sched_notifier != nullptr) {
    sched_notifier->report_metrics(report);
  }
}

void du_manager_metrics_collector_impl::trigger_report()
{
  next_report.start_time = std::chrono::steady_clock::now() - params.period;
  next_report.period     = params.period;
  next_report.version    = next_version++;

  if (params.f1ap_enabled) {
    // Generate F1AP metrics report.
    next_report.f1ap.emplace();
    f1ap_collector.handle_metrics_report_request(*next_report.f1ap);
  }

  // Forward new report.
  params.du_metrics->on_new_metric_report(next_report);

  // Reset report.
  next_report.f1ap.reset();
}
