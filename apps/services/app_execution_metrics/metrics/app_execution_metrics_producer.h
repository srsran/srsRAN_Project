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

#include "app_execution_metrics.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_producer.h"
#include "srsran/support/executors/metrics/executor_metrics_notifier.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

class app_execution_metrics_producer_impl : public app_services::metrics_producer, public executor_metrics_notifier
{
  static constexpr size_t METRIC_POOL_CAPACITY = 1024;

public:
  explicit app_execution_metrics_producer_impl(app_services::metrics_notifier& notifier_) :
    notifier(notifier_), metrics_pool(METRIC_POOL_CAPACITY)
  {
  }

  // See interface for documentation.
  void on_new_report_period() override {}

  // See interface for documentation.
  void on_new_metrics(const executor_metrics& metrics) override
  {
    notifier.on_new_metric(executor_metrics_impl(metrics));
  }

  /// Returns the callback for the application execution metrics.
  app_services::metrics_callback get_callback()
  {
    return [&](const app_services::metrics_set&      report,
               span<app_services::metrics_consumer*> consumers,
               task_executor&                        executor,
               srslog::basic_logger&                 logger) {
      const auto& metric = static_cast<const executor_metrics_impl&>(report);

      // Fetch a metrics report object from the pool.
      auto metrics_report = metrics_pool.get();
      if (!metrics_report) {
        // Discard metrics.
        logger.error("Unable to reserve new object from the pool to save the executor metrics");
        return;
      }

      *metrics_report = metric.get_metrics();
      if (!executor.defer([new_report = std::move(metrics_report), consumers]() {
            for (auto& consumer : consumers) {
              consumer->handle_metric(executor_metrics_impl(*new_report));
            }
          })) {
        logger.error("Failed to dispatch the metric '{}'", metric.get_properties().name());
      }
    };
  }

private:
  app_services::metrics_notifier& notifier;
  /// Pool of metric reports used to avoid memory allocation while handling every new report.
  bounded_object_pool<executor_metrics> metrics_pool;
};

} // namespace srsran
