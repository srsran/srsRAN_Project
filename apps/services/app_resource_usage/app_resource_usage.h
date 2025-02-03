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

#include "apps/services/metrics/metrics_config.h"
#include "srsran/support/resource_usage/resource_usage_utils.h"

namespace srsran {
namespace app_services {

class metrics_notifier;

/// This class can be periodically polled for the new app-level resource usage metrics.
class app_resource_usage
{
public:
  app_resource_usage();

  /// Returns new metrics measured from the last time this method has been called.
  resource_usage_metrics get_new_metrics();

private:
  expected<resource_usage_utils::snapshot, int> last_snapshot;
};

/// Aggregates application resource usage service with the related metrics.
struct app_resource_usage_service {
  std::unique_ptr<app_resource_usage>       service;
  std::vector<app_services::metrics_config> metrics;
};

/// Builds application resource usage service and related metrics.
app_resource_usage_service build_app_resource_usage_service(app_services::metrics_notifier& metrics_notifier,
                                                            srslog::basic_levels            metrics_level,
                                                            srslog::sink&                   json_sink);

} // namespace app_services
} // namespace srsran
