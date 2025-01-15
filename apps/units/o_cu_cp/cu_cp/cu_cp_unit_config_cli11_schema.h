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

struct cu_cp_unit_config;
struct cu_cp_unit_supported_ta_item;

/// Configures the given CLI11 application with the CU-CP application unit configuration schema.
void configure_cli11_with_cu_cp_unit_config_schema(CLI::App& app, cu_cp_unit_config& unit_cfg);

/// Auto derive CU-CP parameters after the parsing.
void autoderive_cu_cp_parameters_after_parsing(CLI::App& app, cu_cp_unit_config& unit_cfg);

} // namespace srsran
