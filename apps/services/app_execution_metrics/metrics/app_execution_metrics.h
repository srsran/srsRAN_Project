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
#include "srsran/support/executors/metrics/executor_metrics.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Executors metrics properties implementation.
class executor_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Executor metrics"; }
};

class executor_metrics_impl : public app_services::metrics_set
{
  executor_metrics_properties_impl properties;
  executor_metrics                 metrics;

public:
  explicit executor_metrics_impl(const executor_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const executor_metrics& get_metrics() const { return metrics; }
};

} // namespace srsran
