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

namespace srsran {
namespace app_services {

/// Application resource usage configuration.
struct app_resource_usage_config {
  app_helpers::metrics_config metrics_consumers_cfg;
  bool                        enable_app_usage = false;
};

} // namespace app_services
} // namespace srsran
