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

#include "du_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/du_high/scheduler.h"
#include "apps/helpers/metrics/json_generators/o_du_high.h"
#include "du_metrics.h"

using namespace srsran;

void du_metrics_consumer_stdout::handle_metric(const app_services::metrics_set& metric)
{
  const srs_du::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!print_metrics.load(std::memory_order_relaxed)) {
    return;
  }

  if (!report.mac) {
    return;
  }

  sched_consumer.handle_metric(report.mac->sched);
}

void du_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const srs_du::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!report.mac) {
    return;
  }

  log_chan("{}", app_helpers::json_generators::generate_string(report, 2));
  if (report.mac) {
    log_chan("{}", app_helpers::json_generators::generate_string(report.mac->sched, 2));
  }
}

void du_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const srs_du::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!report.mac) {
    return;
  }
  mac_consumer.handle_metric(report.mac->dl);
  sched_consumer.handle_metric(report.mac->sched);
}

void du_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  const srs_du::du_metrics_report& report = static_cast<const du_metrics_impl&>(metric).get_report();

  if (!report.mac) {
    return;
  }

  sched_consumer.handle_metric(report.mac->sched);
}
