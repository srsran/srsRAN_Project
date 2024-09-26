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

struct split_7_2_du_unit_config;

/// Configures the given CLI11 application with the split 7.2 DU unit configuration schema.
void configure_cli11_with_split_7_2_du_unit_config_schema(CLI::App& app, split_7_2_du_unit_config& parsed_cfg);

/// Auto derive split 7.2 DU parameters after the parsing.
void autoderive_split_7_2_du_parameters_after_parsing(CLI::App& app, split_7_2_du_unit_config& parsed_cfg);

} // namespace srsran