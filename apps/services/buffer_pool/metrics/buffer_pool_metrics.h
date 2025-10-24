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

#include "apps/services/metrics/metrics_properties.h"
#include "apps/services/metrics/metrics_set.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Buffer pool metrics properties.
class buffer_pool_metrics_properties_impl : public app_services::metrics_properties
{
public:
  std::string_view name() const override { return "Buffer pool metrics"; }
};

/// Buffer pool metrics implementation.
class buffer_pool_metrics_impl : public app_services::metrics_set
{
  buffer_pool_metrics_properties_impl properties;
  size_t                              central_cache_size;

public:
  explicit buffer_pool_metrics_impl(size_t central_cache_size_) : central_cache_size(central_cache_size_) {}

  // See interface for documentation.
  const app_services::metrics_properties& get_properties() const override { return properties; }

  /// Returns current central cache size of the segment pool.
  unsigned get_central_cache_size() const { return central_cache_size; }
};

/// Callback for the buffer pool metrics.
inline auto buffer_pool_metrics_callback = [](const app_services::metrics_set&      report,
                                              span<app_services::metrics_consumer*> consumers,
                                              task_executor&                        executor,
                                              srslog::basic_logger&                 logger) {
  const auto& metric = static_cast<const buffer_pool_metrics_impl&>(report);

  if (!executor.defer([metric, consumers]() {
        for (auto& consumer : consumers) {
          consumer->handle_metric(metric);
        }
      })) {
    logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
  }
};

} // namespace srsran
