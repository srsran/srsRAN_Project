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
#include "apps/services/metrics/metrics_producer.h"
#include "srsran/adt/span.h"
#include "srsran/srslog/srslog.h"
#include <memory>
#include <string>
#include <vector>

namespace srsran {

class task_executor;

namespace app_services {

/// \brief Metrics callback.
///
/// This callback is used to notify metrics to the given metrics consumer running in a different execution context
/// defined by the given task executor. The implementation of the callback must make sure that the metrics object lives
/// longer than the handling of the metrics by the consumer.
using metrics_callback =
    std::function<void(const metrics_set&, span<metrics_consumer*>, task_executor&, srslog::basic_logger&)>;

/// Metrics configuration.
struct metrics_config {
  /// Metric name.
  std::string metric_name;
  /// Callback to be executed when received this type of metrics. This callback is used to notify the consumers of new
  /// metrics using a different execution context.
  metrics_callback callback;
  /// Metric producer.
  std::vector<std::unique_ptr<metrics_producer>> producers;
  /// Metric consumer.
  std::vector<std::unique_ptr<metrics_consumer>> consumers;
};

} // namespace app_services
} // namespace srsran
