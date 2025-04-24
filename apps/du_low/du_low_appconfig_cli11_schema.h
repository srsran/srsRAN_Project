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

struct du_low_appconfig;

/// Configures the given CLI11 application with the DU application configuration schema.
void configure_cli11_with_du_low_appconfig_schema(CLI::App& app, du_low_appconfig& config);

/// Auto derive DU parameters after the parsing.
void autoderive_du_low_parameters_after_parsing(CLI::App& app, du_low_appconfig& config);

} // namespace srsran
