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
