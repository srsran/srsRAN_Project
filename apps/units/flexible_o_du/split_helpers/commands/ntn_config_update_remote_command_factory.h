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

namespace srsran {

struct application_unit_commands;

namespace srs_ntn {
class ntn_configuration_manager;
} // namespace srs_ntn

/// \brief Adds NTN config update remote command.
/// \param commands Application unit commands.
/// \param ntn_manager NTN config manager.
void add_ntn_config_update_remote_command(application_unit_commands&          commands,
                                          srs_ntn::ntn_configuration_manager& ntn_manager);

} // namespace srsran
