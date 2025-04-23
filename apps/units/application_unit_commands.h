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

#include "apps/services/cmdline/stdout_metrics_command.h"
#include "apps/services/remote_control/remote_command.h"
#include <memory>
#include <vector>

namespace srsran {

/// Application command-line commands.
struct cmdline_commands {
  std::vector<std::unique_ptr<app_services::cmdline_command>>                                       commands;
  std::vector<std::unique_ptr<app_services::toggle_stdout_metrics_app_command::metrics_subcommand>> metrics_subcommands;
};

/// \brief Application unit commands.
///
/// This container structure stores the commands (both command-line and remote) that the application unit provides.
struct application_unit_commands {
  cmdline_commands                                           cmdline;
  std::vector<std::unique_ptr<app_services::remote_command>> remote;
};

} // namespace srsran
