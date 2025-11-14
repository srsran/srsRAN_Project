/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_metrics_consumers.h"
#include "apps/helpers/metrics/json_generators/cu_cp/cu_cp_json_helper.h"
#include "cu_cp_metrics.h"

using namespace srsran;

void cu_cp_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const cu_cp_metrics_report& cp_metrics = static_cast<const cu_cp_metrics_impl&>(metric).get_metrics();

  // Only log if there is data.
  if (cp_metrics.dus.empty() && cp_metrics.ngaps.empty()) {
    return;
  }

  log_chan("{}", app_helpers::json_generators::generate_string(cp_metrics, 2));

  log_chan.enabled();
}

void cu_cp_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const cu_cp_metrics_report& cp_metrics = static_cast<const cu_cp_metrics_impl&>(metric).get_metrics();

  ngap_consumer.handle_metric(cp_metrics.ngaps, cp_metrics.mobility);
  rrc_consumer.handle_metric(cp_metrics.dus, cp_metrics.mobility);
}
