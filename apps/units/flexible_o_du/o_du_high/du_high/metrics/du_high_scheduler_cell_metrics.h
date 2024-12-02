/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
