/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "app_resource_usage_metrics_consumer.h"
#include "apps/helpers/metrics/json_generators/resource_usage.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

void resource_usage_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();

  log_chan("{}", app_helpers::json_generators::generate_string(sys_metrics, 2));
}

void resource_usage_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();

  /// Bytes in one Mega Byte.
  static constexpr double BYTES_IN_MB = (1 << 20);

  // Convert used memory to Megabytes.
  double mem_usage = static_cast<double>(sys_metrics.memory_stats.memory_usage.value()) / BYTES_IN_MB;

  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "App resource usage: cpu_usage={:.2f}%, memory_usage={:.2f} MB, power_consumption={:.2f} Watts",
                 sys_metrics.cpu_stats.cpu_usage_percentage * 100.0,
                 mem_usage,
                 sys_metrics.power_usage_watts);

  // Flush buffer to the logger.
  log_chan("{}", to_c_str(buffer));
}
