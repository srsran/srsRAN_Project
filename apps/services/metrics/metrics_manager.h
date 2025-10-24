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

#include "apps/services/metrics/metrics_config.h"
#include "apps/services/metrics/metrics_notifier.h"
#include "apps/services/metrics/metrics_set.h"
#include "apps/services/metrics/periodic_metrics_report_controller.h"
#include "metrics_properties.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace app_services {

/// \brief Metrics manager application service.
///
/// The manager allows to register metrics, metrics producers and metrics consumers in the service.
class metrics_manager : public metrics_notifier
{
  /// Helper struct to store the information related to a metric.
  struct metrics_entry {
    /// Metric name.
    std::string metric_name;
    /// Callback to be executed when received this type of metrics.
    metrics_callback callback;
    /// Metric producer.
    std::vector<std::unique_ptr<metrics_producer>> producers;
    /// Metric consumer.
    std::vector<std::unique_ptr<metrics_consumer>> consumers;
    /// Helper vector to store the pointers to the consumers.
    std::vector<metrics_consumer*> consumers_helper;
  };

public:
  metrics_manager(srslog::basic_logger&     logger_,
                  task_executor&            executor_,
                  span<metrics_config>      metrics_info,
                  timer_manager&            timers,
                  std::chrono::milliseconds report_period) :
    logger(logger_),
    executor(executor_),
    periodic_controller(([](span<metrics_config> metrics_info_cfg) {
                          std::vector<metrics_producer*> producers;
                          for (auto& metric : metrics_info_cfg) {
                            for (auto& producer : metric.producers) {
                              srsran_assert(producer, "Invalid metrics producer");
                              producers.push_back(producer.get());
                            }
                          }
                          return producers;
                        }(metrics_info)),
                        timers,
                        executor_,
                        report_period)
  {
    for (auto& info : metrics_info) {
      metrics_entry entry;
      entry.metric_name = info.metric_name;
      entry.callback    = std::move(info.callback);
      entry.producers   = std::move(info.producers);
      entry.consumers   = std::move(info.consumers);
      for (const auto& con : entry.consumers) {
        entry.consumers_helper.push_back(con.get());
      }
      supported_metrics.push_back(std::move(entry));
    }
  }

  /// Forwards the given metric to the subscribers.
  void on_new_metric(const metrics_set& metric) override
  {
    std::string_view metrics_name = metric.get_properties().name();
    auto             iter         = std::find_if(
        supported_metrics.begin(), supported_metrics.end(), [&metrics_name](const metrics_entry& supported_metric) {
          return supported_metric.metric_name == metrics_name;
        });

    srsran_assert(iter != supported_metrics.end(), "Received unregistered metrics '{}'", metrics_name);

    if (!iter->consumers_helper.empty()) {
      iter->callback(metric, iter->consumers_helper, executor, logger);
    }
  }

  /// Starts the periodic metrics report controller.
  void start() { periodic_controller.start(); }

  /// Stops the periodic metrics report controller.
  void stop() { periodic_controller.stop(); }

private:
  srslog::basic_logger&              logger;
  task_executor&                     executor;
  std::vector<metrics_entry>         supported_metrics;
  periodic_metrics_report_controller periodic_controller;
};

} // namespace app_services
} // namespace srsran
