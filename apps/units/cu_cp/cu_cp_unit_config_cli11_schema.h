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

struct cu_cp_unit_config;

/// Configures the given CLI11 application with the CU-CP application unit configuration schema.
void configure_cli11_with_cu_cp_unit_config_schema(CLI::App& app, cu_cp_unit_config& parsed_cfg);

} // namespace srsran
