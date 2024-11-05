/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "CLI/CLI11.hpp"

namespace srsran {

struct du_appconfig;

/// Configures the given CLI11 application with the DU application configuration schema.
void configure_cli11_with_du_appconfig_schema(CLI::App& app, du_appconfig& parsed_cfg);

/// Auto derive DU parameters after the parsing.
void autoderive_du_parameters_after_parsing(CLI::App& app, du_appconfig& du_cfg);

} // namespace srsran
