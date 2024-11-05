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

struct hal_appconfig;

/// Configures the given CLI11 application with the HAL application configuration schema.
void configure_cli11_with_hal_appconfig_schema(CLI::App& app, hal_appconfig& config);

/// Returns true if the HAL section is present in the given CLI11 application, otherwise false.
bool is_hal_section_present(CLI::App& app);

} // namespace srsran
