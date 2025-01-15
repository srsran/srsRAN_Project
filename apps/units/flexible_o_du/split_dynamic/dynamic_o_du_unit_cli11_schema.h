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

struct dynamic_o_du_unit_config;

/// Configures the given CLI11 application with the dynamic O-RAN DU unit configuration schema.
void configure_cli11_with_dynamic_o_du_unit_config_schema(CLI::App& app, dynamic_o_du_unit_config& parsed_cfg);

/// Auto derive dynamic O-RAN DU parameters after the parsing.
void autoderive_dynamic_o_du_parameters_after_parsing(CLI::App& app, dynamic_o_du_unit_config& parsed_cfg);

} // namespace srsran
