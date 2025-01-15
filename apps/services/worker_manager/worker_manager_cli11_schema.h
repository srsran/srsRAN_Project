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

struct expert_execution_appconfig;

/// Configures the given CLI11 application with the worker manager application configuration schema.
void configure_cli11_with_worker_manager_appconfig_schema(CLI::App& app, expert_execution_appconfig& config);

} // namespace srsran
