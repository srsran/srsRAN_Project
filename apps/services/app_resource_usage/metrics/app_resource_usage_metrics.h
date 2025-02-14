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

#include "apps/services/metrics/metrics_consumer.h"
#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/resource_usage/resource_usage_metrics.h"

namespace srsran {

/// System resources usage metrics properties.
class resource_usage_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Resource usage metrics"; }
};

class resource_usage_metrics_impl : public app_services::metrics_set
{
  resource_usage_metrics_properties_impl properties;
  resource_usage_metrics                 metrics;

public:
  explicit resource_usage_metrics_impl(const resource_usage_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const resource_usage_metrics& get_metrics() const { return metrics; }
};

/// Callback for the Resource usage metrics.
// TODO: replace with better solution that doesn't allocate memory.
inline auto rusage_metrics_callback = [](const app_services::metrics_set&      report,
                                         span<app_services::metrics_consumer*> consumers,
                                         task_executor&                        executor,
                                         srslog::basic_logger&                 logger) {
  const auto& metric = static_cast<const resource_usage_metrics_impl&>(report);

  if (!executor.defer([metric, consumers]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
