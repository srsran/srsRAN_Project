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

#include "app_resource_usage_metrics_consumer.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

namespace {

DECLARE_METRIC("cpu_usage_percent", metric_app_cpu_usage, double, "");
DECLARE_METRIC("memory_usage_MB", metric_app_memory_usage, double, "");
DECLARE_METRIC("power_consumption_Watts", metric_app_power_consumption, double, "");

/// Metrics root objects.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_SET("app_resource_usage",
                   mset_app_resource_usage,
                   metric_app_cpu_usage,
                   metric_app_memory_usage,
                   metric_app_power_consumption);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, mset_app_resource_usage>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

/// Bytes in one Mega Byte.
static constexpr double BYTES_IN_MB = (1 << 20);

void resource_usage_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();
  // Convert used memory to Megabytes.
  double mem_usage = static_cast<double>(sys_metrics.memory_stats.memory_usage.value()) / BYTES_IN_MB;

  metric_context_t ctx("JSON App Resource Usage Metrics");
  auto&            app_metrics = ctx.get<mset_app_resource_usage>();
  app_metrics.write<metric_app_cpu_usage>(sys_metrics.cpu_stats.cpu_usage_percentage * 100.0);
  app_metrics.write<metric_app_memory_usage>(mem_usage);
  app_metrics.write<metric_app_power_consumption>(sys_metrics.power_usage_watts);

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}

void resource_usage_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();
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
