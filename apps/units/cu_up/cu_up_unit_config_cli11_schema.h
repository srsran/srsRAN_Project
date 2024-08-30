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

struct cu_up_unit_config;

/// Configures the given CLI11 application with the CU-UP application unit configuration schema.
void configure_cli11_with_cu_up_unit_config_schema(CLI::App& app, cu_up_unit_config& unit_cfg);

/// Auto derive CU-UP parameters after the parsing.
void autoderive_cu_up_parameters_after_parsing(const std::string& bind_addr, bool no_core, cu_up_unit_config& unit_cfg);

} // namespace srsran
