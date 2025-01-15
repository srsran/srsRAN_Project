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

struct ru_sdr_unit_config;

/// Configures the given CLI11 application with the SDR RU configuration schema.
void configure_cli11_with_ru_sdr_config_schema(CLI::App& app, ru_sdr_unit_config& parsed_cfg);

/// Auto derive SDR Radio Unit parameters after the parsing.
void autoderive_ru_sdr_parameters_after_parsing(CLI::App& app, ru_sdr_unit_config& parsed_cfg, unsigned nof_cells);

} // namespace srsran
