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

struct e2_config;

/// \brief Configures the given CLI11 application with the E2 application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config E2 configuration that stores the parameters.
/// \param[in] option_name Option name for the E2 unit property.
/// \param[out] option_description Option description for the E2 unit property.
void configure_cli11_with_e2_config_schema(CLI::App&          app,
                                           e2_config&         config,
                                           const std::string& option_name,
                                           const std::string& option_description);

} // namespace srsran
