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

#include "apps/services/network/udp_cli11_schema.h"
#include "srsran/ran/qos/five_qi.h"
#include "CLI/CLI11.hpp"
#include <optional>

namespace srsran {

/// F1-U sockets configuration
struct f1u_socket_appconfig {
  std::string              bind_addr = "127.0.10.1"; /// Bind address used by the F1-U interface
  std::optional<five_qi_t> five_qi;                  /// If the 5QI is not present, the socket will be used by default.
  udp_appconfig            udp_config;
};

/// F1-U configuration
struct f1u_sockets_appconfig {
  std::vector<f1u_socket_appconfig> f1u_socket_cfg;
};

/// \brief TODO write docs.
void configure_cli11_f1u_sockets_args(CLI::App& app, f1u_sockets_appconfig& f1u_params);

} // namespace srsran
