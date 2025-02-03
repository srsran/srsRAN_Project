/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/services/app_resource_usage/app_resource_usage.h"
#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics_consumer.h"
#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics_producer.h"

using namespace srsran;
using namespace app_services;

app_resource_usage::app_resource_usage()
{
  last_snapshot = now(resource_usage_utils::rusage_measurement_type::PROCESS);
  if (!last_snapshot) {
    srslog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}", last_snapshot.error());
  }
}

resource_usage_metrics app_resource_usage::get_new_metrics()
{
  auto current_snapshot = now(resource_usage_utils::rusage_measurement_type::PROCESS);
  if (!current_snapshot) {
    srslog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}",
        current_snapshot.error());
    return {};
  }
  if (!last_snapshot) {
    last_snapshot = current_snapshot.value();
    return {};
  }

  resource_usage_utils::measurements measurements;
  measurements.duration = std::chrono::duration_cast<resource_usage_utils::rusage_meas_duration>(
      current_snapshot.value().tp - last_snapshot.value().tp);
  measurements.user_time   = (current_snapshot.value().user_time - last_snapshot.value().user_time);
  measurements.system_time = (current_snapshot.value().system_time - last_snapshot.value().system_time);
  measurements.max_rss     = current_snapshot.value().max_rss;

  last_snapshot = current_snapshot.value();
  return res_usage_measurements_to_metrics(measurements, measurements.duration);
}

app_resource_usage_service
app_services::build_app_resource_usage_service(app_services::metrics_notifier& metrics_notifier,
                                               srslog::basic_levels            metrics_level,
                                               srslog::sink&                   json_sink)
{
  app_resource_usage_service app_res_usage;
  if (metrics_level != srslog::basic_levels::info) {
    return app_res_usage;
  }
  app_res_usage.service = std::make_unique<app_resource_usage>();

  app_services::metrics_config& app_res_usage_metrics = app_res_usage.metrics.emplace_back();
  app_res_usage_metrics.metric_name                   = resource_usage_metrics_properties_impl().name();
  app_res_usage_metrics.callback                      = rusage_metrics_callback;
  app_res_usage_metrics.producers.emplace_back(
      std::make_unique<resource_usage_metrics_producer_impl>(metrics_notifier, *app_res_usage.service));
  app_res_usage_metrics.consumers.push_back(
      std::make_unique<resource_usage_metrics_consumer_log>(srslog::fetch_basic_logger("METRICS")));

  return app_res_usage;
}
