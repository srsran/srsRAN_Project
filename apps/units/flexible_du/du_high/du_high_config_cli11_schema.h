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

struct du_high_parsed_config;
struct du_high_unit_config;

/// Configures the given CLI11 application with the DU high configuration schema.
void configure_cli11_with_du_high_config_schema(CLI::App& app, du_high_parsed_config& parsed_cfg);

/// Auto derive DU high parameters after the parsing.
void autoderive_du_high_parameters_after_parsing(CLI::App& app, du_high_unit_config& unit_cfg);

} // namespace srsran
