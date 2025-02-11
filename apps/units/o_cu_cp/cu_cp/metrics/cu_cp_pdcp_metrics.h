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
#include "srsran/pdcp/pdcp_entity.h"

namespace srsran {

/// CU-CP PDCP metrics properties implementation.
class cu_cp_pdcp_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "CU-CP PDCP metrics"; }
};

/// CU-CP PDCP metrics implementation.
class cu_cp_pdcp_metrics_impl : public app_services::metrics_set
{
  cu_cp_pdcp_metrics_properties_impl properties;
  pdcp_metrics_container             metrics;

public:
  explicit cu_cp_pdcp_metrics_impl(const pdcp_metrics_container& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const pdcp_metrics_container& get_metrics() const { return metrics; }
};

/// Callback for the CU-CP PDCP metrics.
inline auto cu_cp_pdcp_metrics_callback = [](const app_services::metrics_set&      report,
                                             span<app_services::metrics_consumer*> consumers,
                                             task_executor&                        executor,
                                             srslog::basic_logger&                 logger) {
  const auto& metric = static_cast<const cu_cp_pdcp_metrics_impl&>(report);

  if (!executor.defer([metric, consumers]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
