/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "apps/helpers/network/udp_appconfig.h"
#include "srsran/gtpu/gtpu_config.h"
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
  /// Bind port used by the F1-U interface.
  uint16_t bind_port = GTPU_PORT;
  /// Peer port used by the F1-U interface.
  uint16_t                          peer_port = GTPU_PORT;
  std::vector<f1u_socket_appconfig> f1u_socket_cfg;
};

} // namespace srsran
