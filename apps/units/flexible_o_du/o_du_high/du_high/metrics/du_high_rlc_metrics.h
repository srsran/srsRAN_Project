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
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// RLC metrics properties implementation.
class rlc_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "RLC metrics"; }
};

class rlc_metrics_impl : public app_services::metrics_set
{
  rlc_metrics_properties_impl properties;
  rlc_metrics                 metrics;

public:
  explicit rlc_metrics_impl(const rlc_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const rlc_metrics& get_metrics() const { return metrics; }
};

/// Callback for the RLC metrics.
inline auto rlc_metrics_callback = [](const app_services::metrics_set&      report,
                                      span<app_services::metrics_consumer*> consumers,
                                      task_executor&                        executor,
                                      srslog::basic_logger&                 logger) {
  const auto& metric = static_cast<const rlc_metrics_impl&>(report);

  if (!executor.defer([metric, consumers]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
