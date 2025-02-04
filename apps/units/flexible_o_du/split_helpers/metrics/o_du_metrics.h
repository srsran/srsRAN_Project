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
#include "srsran/du/o_du_metrics.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// O-RAN DU metrics properties implementation.
class o_du_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "O-DU metrics"; }
};

class o_du_metrics_impl : public app_services::metrics_set
{
  o_du_metrics_properties_impl properties;
  srs_du::o_du_metrics         metrics;

public:
  explicit o_du_metrics_impl(const srs_du::o_du_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const srs_du::o_du_metrics& get_metrics() const { return metrics; }
};

/// Callback for the O-DU metrics.
inline auto o_du_metrics_callback = [](const app_services::metrics_set&      report,
                                       span<app_services::metrics_consumer*> consumers,
                                       task_executor&                        executor,
                                       srslog::basic_logger&                 logger) {
  const auto& metric = static_cast<const o_du_metrics_impl&>(report);

  if (!executor.defer([metric, consumers]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
