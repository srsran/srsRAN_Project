/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_metrics_consumers.h"
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

  mac_consumer.handle_metric(report.mac->dl);
  sched_handler.handle_metric(report.mac->sched);
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
