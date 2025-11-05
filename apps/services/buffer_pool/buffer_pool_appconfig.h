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
#include "srsran/adt/byte_buffer.h"

namespace srsran {
namespace app_services {

/// Buffer pool metrics configuration.
struct buffer_pool_metrics_config {
  /// Common metrics config.
  app_helpers::metrics_config common_metrics_cfg;
  /// Defines whether to log buffer pool metrics.
  bool enable_metrics = false;
};

/// Buffer pool application configuration.
struct buffer_pool_appconfig {
  std::size_t nof_segments = 1048576;
  std::size_t segment_size = byte_buffer_segment_pool_default_segment_size();
  /// Metrics config.
  buffer_pool_metrics_config metrics_config;
};

} // namespace app_services
} // namespace srsran
