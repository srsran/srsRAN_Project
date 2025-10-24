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
