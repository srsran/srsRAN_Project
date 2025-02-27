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

#include "apps/helpers/network/udp_appconfig.h"
#include "srsran/ran/qos/five_qi.h"
#include <vector>

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

} // namespace srsran
