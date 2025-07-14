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

struct ntn_config;

/// Helper function to configure NTN capabilities.
void configure_cli11_ntn_args(CLI::App& app, ntn_config& config);

/// Helper function to configure advanced NTN capabilities.
void configure_cli11_advanced_ntn_args(CLI::App& app, ntn_config& config);

} // namespace srsran
