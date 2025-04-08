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

namespace srsran {
namespace app_services {

/// Metrics service application configuration.
struct metrics_appconfig {
  /// Application usage report period in milliseconds.
  unsigned app_usage_report_period = 1000;
};

} // namespace app_services
} // namespace srsran
