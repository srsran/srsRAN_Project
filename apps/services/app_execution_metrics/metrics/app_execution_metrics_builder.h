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
