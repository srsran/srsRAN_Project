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

class executor_metrics_notifier;

namespace app_helpers {
struct metrics_config;
}

namespace app_services {
class metrics_notifier;
}

///\brief Builds the application execution metrics configuration.
///
/// \param metrics[out]    - Application metrics configuration of the executor metrics service.
/// \param notifier[in]    - Application metrics notifier interface.
/// \param metrics_cfg[in] - Common metrics configuration.
///
/// \return reference to the executor metrics notifier.
executor_metrics_notifier& build_app_execution_metrics_config(app_services::metrics_config&      exec_metric_cfg,
                                                              app_services::metrics_notifier&    notifier,
                                                              const app_helpers::metrics_config& metrics_cfg);

} // namespace srsran
