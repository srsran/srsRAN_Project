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
#include "srsran/du/du_high/du_metrics_report.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/synchronization/stop_event.h"
#include <utility>

namespace srsran {

/// DU metrics properties implementation.
class du_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "DU metrics"; }
};

/// MAC metrics implementation.
class du_metrics_impl : public app_services::metrics_set
{
  du_metrics_properties_impl properties;
  srs_du::du_metrics_report  report;

public:
  explicit du_metrics_impl(srs_du::du_metrics_report report_) : report(std::move(report_)) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  // Returns the DU metrics report.
  const srs_du::du_metrics_report& get_report() const { return report; }
};

/// Callback for the DU metrics.
inline auto du_metrics_callback = [](const app_services::metrics_set&      report,
                                     span<app_services::metrics_consumer*> consumers,
                                     task_executor&                        executor,
                                     srslog::basic_logger&                 logger,
                                     stop_event_token                      token) {
  const auto& metric = static_cast<const du_metrics_impl&>(report);

  if (!executor.defer([metric, consumers, stop_token = std::move(token)]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
