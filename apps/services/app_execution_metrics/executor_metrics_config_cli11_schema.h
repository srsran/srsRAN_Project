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
namespace app_services {

struct executor_metrics_config;

/// Configures the given CLI11 application with the metrics logger application configuration schema.
void configure_cli11_with_executor_metrics_appconfig_schema(CLI::App& app, executor_metrics_config& config);

} // namespace app_services
} // namespace srsran
