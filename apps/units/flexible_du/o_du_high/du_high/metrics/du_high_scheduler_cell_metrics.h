/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "srsran/scheduler/scheduler_metrics.h"

namespace srsran {

/// Scheduler cell metrics property implementation.
class scheduler_cell_metrics_property_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Scheduler UE metrics"; }
};

/// Scheduler cell metrics implementation for the application service.
class scheduler_cell_metrics_impl : public app_services::metrics_set
{
  scheduler_cell_metrics_property_impl properties;
  const scheduler_cell_metrics&        metrics;

public:
  explicit scheduler_cell_metrics_impl(const scheduler_cell_metrics& metrics_) : metrics(metrics_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  const scheduler_cell_metrics& get_metrics() const { return metrics; }
};

} // namespace srsran
