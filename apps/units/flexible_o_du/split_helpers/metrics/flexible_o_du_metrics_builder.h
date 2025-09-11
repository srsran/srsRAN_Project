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

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/services/cmdline/stdout_metrics_command.h"
#include "apps/services/metrics/metrics_config.h"
#include "srsran/ran/pci.h"

namespace srsran {

class flexible_o_du_metrics_notifier;

namespace app_helpers {
struct metrics_config;
}

namespace app_services {
class metrics_notifier;
}

/// Builds the flexible O-DU metrics configuration.
flexible_o_du_metrics_notifier* build_flexible_o_du_metrics_config(
    std::vector<app_services::metrics_config>& metrics,
    std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>>&
                                       metrics_subcommands,
    app_services::metrics_notifier&    notifier,
    const app_helpers::metrics_config& metrics_cfg,
    const std::vector<pci_t>&          pci_cell_map,
    std::chrono::nanoseconds           symbol_duration);

} // namespace srsran
