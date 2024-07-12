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

#include <CLI/CLI11.hpp>

namespace srsran {

struct buffer_pool_appconfig;

/// Configures the given CLI11 application with the logger application configuration schema.
void configure_cli11_with_buffer_pool_appconfig_schema(CLI::App& app, buffer_pool_appconfig& config);

} // namespace srsran