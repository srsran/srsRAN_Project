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

struct ru_ofh_unit_config;
struct ru_ofh_unit_parsed_config;

/// Configures the given CLI11 application with the Open Fronthaul Radio Unit configuration schema.
void configure_cli11_with_ru_ofh_config_schema(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg);

/// Auto derive OFH Radio Unit parameters after the parsing.
void autoderive_ru_ofh_parameters_after_parsing(CLI::App& app, ru_ofh_unit_parsed_config& parsed_cfg);

} // namespace srsran
