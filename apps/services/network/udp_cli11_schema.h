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

struct udp_appconfig {
  unsigned rx_max_mmsg              = 256;
  float    pool_occupancy_threshold = 0.9;
};

/// \brief Configures the given CLI11 application with the UDP application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config UDP configuration that stores the parameters.
void configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config);

} // namespace srsran
