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

namespace srsran {

namespace app_helpers {
struct metrics_config;
} // namespace app_helpers

namespace app_services {

class buffer_pool_manager;
class metrics_notifier;

///\brief Builds the buffer pool metrics configuration.
///
/// \param notifier[in]    - Application metrics notifier interface.
/// \param metrics_cfg[in] - Common metrics configuration.
///
/// \return buffer pool metrics configuration.
metrics_config build_buffer_pool_metrics_config(metrics_notifier&                  notifier,
                                                const app_helpers::metrics_config& metrics_cfg);
} // namespace app_services
} // namespace srsran
