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
