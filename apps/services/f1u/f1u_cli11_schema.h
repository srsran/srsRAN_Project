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

#include "apps/services/network/udp_cli11_schema.h"
#include "srsran/ran/qos/five_qi.h"
#include "CLI/CLI11.hpp"
#include <optional>

namespace srsran {

/// F1-U sockets configuration.
struct f1u_socket_appconfig {
  /// Bind address used by the F1-U interface.
  std::string bind_addr = "127.0.10.1";
  /// If the 5QI is not present, the socket will be used by default.
  std::optional<five_qi_t> five_qi;
  udp_appconfig            udp_config;
};

/// F1-U configuration.
struct f1u_sockets_appconfig {
  std::vector<f1u_socket_appconfig> f1u_socket_cfg;
};

/// Configures the given CLI11 application with the F1U sockets application configuration schema.
void configure_cli11_f1u_sockets_args(CLI::App& app, f1u_sockets_appconfig& f1u_params);

} // namespace srsran
