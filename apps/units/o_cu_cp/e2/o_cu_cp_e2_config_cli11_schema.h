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

#include "srsran/adt/span.h"
#include "CLI/CLI11.hpp"

namespace srsran {

struct o_cu_cp_e2_config;

/// Configures the given CLI11 application with the O-RAN CU-CP E2 configuration schema.
void configure_cli11_with_o_cu_cp_e2_config_schema(CLI::App& app, o_cu_cp_e2_config& unit_cfg);

/// Auto derive O-RAN CU-CP E2 parameters after the parsing.
void autoderive_o_cu_cp_e2_parameters_after_parsing(o_cu_cp_e2_config& unit_cfg);

} // namespace srsran
