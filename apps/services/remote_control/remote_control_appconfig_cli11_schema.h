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

#include "CLI/CLI11.hpp"

namespace srsran {

struct remote_control_appconfig;

/// Configures the given CLI11 application with the remote control application configuration schema.
void configure_cli11_with_remote_control_appconfig_schema(CLI::App& app, remote_control_appconfig& config);

} // namespace srsran
