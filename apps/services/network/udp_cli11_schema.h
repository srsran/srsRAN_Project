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
#include <optional>

namespace srsran {

/// UDP specific configuration of an UDP gateway.
struct udp_appconfig {
  /// Maximum amount of messages RX in a single syscall.
  unsigned rx_max_msgs = 256;
  /// Pool accupancy threshold after which packets are dropped.
  float pool_threshold = 0.9;
  /// Differentiated Services Code Point value.
  std::optional<unsigned> dscp;
  // External address advertised by the UDP-GW.
  std::string ext_addr = "auto";
};

/// \brief Configures the given CLI11 application with the UDP application configuration schema.
///
/// \param[out] app CLI11 application to configure.
/// \param[out] config UDP configuration that stores the parameters.
void configure_cli11_with_udp_config_schema(CLI::App& app, udp_appconfig& config);

} // namespace srsran
