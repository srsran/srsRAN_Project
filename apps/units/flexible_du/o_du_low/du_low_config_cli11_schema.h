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

#include "srsran/ran/duplex_mode.h"
#include "CLI/CLI11.hpp"

namespace srsran {

struct du_low_unit_config;

/// Configures the given CLI11 application with the DU low configuration schema.
void configure_cli11_with_du_low_config_schema(CLI::App& app, du_low_unit_config& parsed_cfg);

/// Auto derive DU low parameters after the parsing.
void autoderive_du_low_parameters_after_parsing(CLI::App&           app,
                                                du_low_unit_config& parsed_cfg,
                                                duplex_mode         mode,
                                                bool                is_blocking_mode_enabled,
                                                unsigned            nof_cells);

} // namespace srsran
