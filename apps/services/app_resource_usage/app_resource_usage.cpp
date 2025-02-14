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
using namespace resource_usage_utils;

app_resource_usage::app_resource_usage(std::unique_ptr<energy_consumption_reader> energy_reader_) :
  energy_reader(std::move(energy_reader_))
{
  srsran_assert(energy_reader, "Invalid energy consumption reader passed");

  auto cpu_snapshot = cpu_usage_now(rusage_measurement_type::PROCESS);
  if (!cpu_snapshot) {
    srslog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}", cpu_snapshot.error());
  } else {
    last_snapshot->cpu_usage    = cpu_snapshot.value();
    last_snapshot->energy_usage = energy_usage_now();
  }
}

resource_usage_metrics app_resource_usage::get_new_metrics()
{
  auto current_cpu_snapshot = cpu_usage_now(rusage_measurement_type::PROCESS);
  if (!current_cpu_snapshot) {
    srslog::fetch_basic_logger("METRICS").warning(
        "Application resource usage service failed to query current resource usage, errno={}",
        current_cpu_snapshot.error());
    return {};
  }

  if (!last_snapshot) {
    last_snapshot->cpu_usage    = current_cpu_snapshot.value();
    last_snapshot->energy_usage = energy_usage_now();

    return {};
  }

  resource_usage_metrics new_metrics;
  // Calculate CPU metrics.
  update_cpu_usage_metric(current_cpu_snapshot.value(), new_metrics);
  // Calculate CPU power consumption in Watts.
  update_power_consumption_metric(new_metrics);

  return new_metrics;
}

void app_resource_usage::update_cpu_usage_metric(const cpu_snapshot&     current_cpu_snapshot,
                                                 resource_usage_metrics& metrics)
{
  resource_usage_utils::measurements measurements;

  measurements.duration = std::chrono::duration_cast<resource_usage_utils::rusage_meas_duration>(
      current_cpu_snapshot.tp - last_snapshot->cpu_usage.tp);
  measurements.user_time   = (current_cpu_snapshot.user_time - last_snapshot->cpu_usage.user_time);
  measurements.system_time = (current_cpu_snapshot.system_time - last_snapshot->cpu_usage.system_time);
  measurements.max_rss     = current_cpu_snapshot.max_rss;

  // Save current snapshot.
  last_snapshot->cpu_usage = current_cpu_snapshot;
  // Update metrics.
  metrics = res_usage_measurements_to_metrics(measurements, measurements.duration);
}

void app_resource_usage::update_power_consumption_metric(resource_usage_metrics& metrics)
{
  auto current_energy_snapshot = energy_usage_now();

  // In the following calculations we use only package consumption.
  uint64_t consumed_energy = calculate_energy_diff(current_energy_snapshot.probe.package_consumed_uj,
                                                   last_snapshot->energy_usage.probe.package_consumed_uj);

  std::chrono::microseconds time_diff = std::chrono::duration_cast<std::chrono::microseconds>(
      current_energy_snapshot.probe_time - last_snapshot->energy_usage.probe_time);

  // Save current snapshot.
  last_snapshot->energy_usage = current_energy_snapshot;
  // Update metrics.
  metrics.power_usage_watts = static_cast<double>(consumed_energy) / static_cast<double>(time_diff.count());
}

energy_snapshot app_resource_usage::energy_usage_now()
{
  energy_snapshot current_snapshot;
  current_snapshot.probe_time = std::chrono::high_resolution_clock::now();
  current_snapshot.probe      = energy_reader->read_consumed_energy();

  return current_snapshot;
}

app_resource_usage_service
app_services::build_app_resource_usage_service(app_services::metrics_notifier& metrics_notifier,
                                               srslog::basic_levels            metrics_level,
                                               srslog::sink*                   json_sink)
{
  app_resource_usage_service app_res_usage;
  if (metrics_level != srslog::basic_levels::info && !json_sink) {
    return app_res_usage;
  }
  app_res_usage.service =
      std::make_unique<app_resource_usage>(build_energy_consumption_reader(srslog::fetch_basic_logger("METRICS")));

  app_services::metrics_config& app_res_usage_metrics = app_res_usage.metrics.emplace_back();
  app_res_usage_metrics.metric_name                   = resource_usage_metrics_properties_impl().name();
  app_res_usage_metrics.callback                      = rusage_metrics_callback;
  app_res_usage_metrics.producers.emplace_back(
      std::make_unique<resource_usage_metrics_producer_impl>(metrics_notifier, *app_res_usage.service));

  if (metrics_level == srslog::basic_levels::info) {
    app_res_usage_metrics.consumers.push_back(
        std::make_unique<resource_usage_metrics_consumer_log>(srslog::fetch_basic_logger("METRICS")));
  }

  if (json_sink) {
    srslog::log_channel& json_channel = srslog::fetch_log_channel("JSON_channel", *json_sink, {});
    json_channel.set_enabled(true);

    app_res_usage_metrics.consumers.push_back(std::make_unique<resource_usage_metrics_consumer_json>(json_channel));
  }

  return app_res_usage;
}
