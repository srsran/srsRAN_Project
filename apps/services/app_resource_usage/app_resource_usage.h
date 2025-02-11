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
