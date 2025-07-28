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

#include "du_metrics_aggregator_impl.h"
#include "srsran/mac/mac_metrics.h"
#include "srsran/mac/mac_metrics_notifier.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/executors/execute_until_success.h"

using namespace srsran;
using namespace srs_du;

// class du_manager_metrics_aggregator_impl

du_manager_metrics_aggregator_impl::du_manager_metrics_aggregator_impl(
    const du_manager_params::metrics_config_params& params_,
    task_executor&                                  du_mng_exec_,
    timer_manager&                                  timers_,
    f1ap_metrics_collector&                         f1ap_collector_,
    mac_metrics_notifier*                           mac_notifier_) :
  params(params_),
  du_mng_exec(du_mng_exec_),
  timers(timers_),
  mac_notifier(mac_notifier_),
  f1ap_collector(f1ap_collector_)
{
  (void)du_mng_exec;
  (void)timers;

  if (params.du_metrics != nullptr) {
    if (params.f1ap_enabled) {
      next_report.f1ap.emplace();
    }
    if (params.mac_enabled or params.sched_enabled) {
      next_report.mac.emplace();
    }
  }
}

du_manager_metrics_aggregator_impl::~du_manager_metrics_aggregator_impl() = default;

void du_manager_metrics_aggregator_impl::aggregate_mac_metrics_report(const mac_metric_report& report)
{
  // Forward the MAC report to notifier.
  if (mac_notifier != nullptr) {
    mac_notifier->on_new_metrics_report(report);
  }

  // In case the DU metrics notifier was specified, report the DU metrics.
  if (params.du_metrics != nullptr) {
    next_report.mac = report;
    trigger_report();
  }
}

void du_manager_metrics_aggregator_impl::trigger_report()
{
  next_report.start_time = std::chrono::steady_clock::now() - params.period;
  next_report.period     = params.period;
  next_report.version    = next_version++;

  if (next_report.f1ap.has_value()) {
    // Generate F1AP metrics report.
    f1ap_collector.collect_metrics_report(*next_report.f1ap);
  }

  // TODO: Generate RLC report.

  // Forward new report.
  params.du_metrics->on_new_metric_report(next_report);

  // Reset report.
  // Note: We use clear() to keep the vector capacity.
  if (params.f1ap_enabled) {
    next_report.f1ap->ues.clear();
  }
  if (params.mac_enabled) {
    next_report.mac->dl.cells.clear();
  }
  if (params.sched_enabled) {
    next_report.mac->sched.cells.clear();
  }
}

void du_manager_metrics_aggregator_impl::handle_cell_start(du_cell_index_t cell_index) {}

void du_manager_metrics_aggregator_impl::handle_cell_stop(du_cell_index_t cell_index) {}
