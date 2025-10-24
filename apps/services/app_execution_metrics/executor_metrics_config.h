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

#include "apps/helpers/metrics/metrics_config.h"
#include <chrono>

namespace srsran {
namespace app_services {

/// Executor metrics configuration.
struct executor_metrics_config {
  /// Common metrics config.
  app_helpers::metrics_config common_metrics_cfg;
  /// Whether to log performace metrics of application executors.
  bool enable_executor_metrics = false;
  /// Periodicity of executors metrics in milliseconds, if enabled.
  std::chrono::milliseconds report_period_ms{1000};
};

} // namespace app_services
} // namespace srsran
