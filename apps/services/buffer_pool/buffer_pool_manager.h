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

#include "apps/services/buffer_pool/buffer_pool_appconfig.h"
#include "apps/services/buffer_pool/metrics/buffer_pool_metrics_builder.h"
#include "srsran/adt/detail/byte_buffer_segment_pool.h"

namespace srsran {
namespace app_services {

/// Buffer pool manager for the applications.
class buffer_pool_manager
{
public:
  explicit buffer_pool_manager(const buffer_pool_appconfig& config)
  {
    init_byte_buffer_segment_pool(config.nof_segments, config.segment_size);
  }

  /// Returns current size of the segment pool central cache.
  static size_t get_central_cache_size()
  {
    auto& pool = detail::get_default_byte_buffer_segment_pool();
    return pool.get_central_cache_approx_size();
  }

  ///\brief Adds the buffer pool metrics configuration to the metrics service.
  ///
  /// \param metrics[in,out] - Vector of application metrics configs. The service can extend it with its own config.
  /// \param notifier[in]    - Application metrics notifier interface.
  /// \param metrics_cfg[in] - Buffer pool metrics configuration.
  void add_metrics_to_metrics_service(std::vector<app_services::metrics_config>& app_metrics,
                                      const buffer_pool_metrics_config&          metrics_cfg,
                                      app_services::metrics_notifier&            notifier)
  {
    if (!metrics_cfg.enable_metrics) {
      return;
    }

    app_metrics.emplace_back(build_buffer_pool_metrics_config(notifier, metrics_cfg.common_metrics_cfg));
  }
};

} // namespace app_services
} // namespace srsran
