/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    logger.info("CPU package power usage: {:02f} Watts", sys_metrics.power_usage_watts);
  }

private:
  srslog::basic_logger& logger;
};

} // namespace srsran
