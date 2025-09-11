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
