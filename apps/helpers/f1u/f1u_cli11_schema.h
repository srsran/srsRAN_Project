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

struct f1u_sockets_appconfig;

/// Configures the given CLI11 application with the F1U sockets application configuration schema.
void configure_cli11_f1u_sockets_args(CLI::App& app, f1u_sockets_appconfig& f1u_params);

} // namespace srsran
