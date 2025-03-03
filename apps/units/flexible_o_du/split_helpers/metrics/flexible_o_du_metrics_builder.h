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
#include "apps/services/cmdline/cmdline_command.h"
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
flexible_o_du_metrics_notifier*
build_flexible_o_du_metrics_config(std::vector<app_services::metrics_config>&                   metrics,
                                   std::vector<std::unique_ptr<app_services::cmdline_command>>& unit_commands,
                                   app_services::metrics_notifier&                              notifier,
                                   const app_helpers::metrics_config&                           metrics_cfg,
                                   std::vector<pci_t>                                           pci_cell_map);

} // namespace srsran
