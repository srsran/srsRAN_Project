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

#include "apps/services/app_resource_usage/metrics/app_resource_usage_metrics.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// Consumer for the log resource usage metrics.
class resource_usage_metrics_consumer_log : public app_services::metrics_consumer
{
public:
  explicit resource_usage_metrics_consumer_log(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  void handle_metric(const app_services::metrics_set& metric) override
  {
    const resource_usage_metrics& sys_metrics = static_cast<const resource_usage_metrics_impl&>(metric).get_metrics();

    logger.info("Application CPU usage: {}%, {} CPUs",
                sys_metrics.cpu_stats.cpu_usage_percentage,
                sys_metrics.cpu_stats.cpu_utilization_nof_cpus);
    logger.info("Application memory usage: {} Bytes", sys_metrics.memory_stats.memory_usage.value());
  }

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
