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

#pragma once

#include "app_resource_usage_config.h"
#include "apps/services/metrics/metrics_config.h"
#include "srsran/support/resource_usage/power_consumption.h"
#include "srsran/support/resource_usage/resource_usage_utils.h"

namespace srsran {

namespace app_helpers {
struct metrics_config;
}

namespace app_services {

class metrics_notifier;

/// This class can be periodically polled for the new app-level resource usage metrics.
class app_resource_usage
{
  /// Helper structure that aggregates CPU utilization and power consumption at a given point of time.
  struct snapshot {
    resource_usage_utils::cpu_snapshot    cpu_usage;
    resource_usage_utils::energy_snapshot energy_usage;
  };

public:
  app_resource_usage(std::unique_ptr<resource_usage_utils::energy_consumption_reader> energy_reader_);

  /// Returns new metrics measured from the last time this method has been called.
  resource_usage_metrics get_new_metrics();

private:
  /// Updates power consumption parameters in the given metrics.
  void update_power_consumption_metric(resource_usage_metrics& metrics);

  /// Updates CPU usage parameters in the given metrics.
  void update_cpu_usage_metric(const resource_usage_utils::cpu_snapshot& current_cpu_snapshot,
                               resource_usage_metrics&                   metrics);

  /// Returns a snapshot of the energy consumption.
  resource_usage_utils::energy_snapshot energy_usage_now();

  /// The last taken snapshot of resources usage.
  expected<snapshot, int> last_snapshot;
  /// Energy consumption reader.
  std::unique_ptr<resource_usage_utils::energy_consumption_reader> energy_reader;
};

/// Aggregates application resource usage service with the related metrics.
struct app_resource_usage_service {
  std::unique_ptr<app_resource_usage>       service;
  std::vector<app_services::metrics_config> metrics;
};

/// Builds application resource usage service and related metrics.
app_resource_usage_service build_app_resource_usage_service(app_services::metrics_notifier&  metrics_notifier,
                                                            const app_resource_usage_config& config,
                                                            srslog::basic_logger&            logger);

} // namespace app_services
} // namespace srsran
