/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/srslog/logger.h"
#include "srsran/support/io/unique_fd.h"
#include <chrono>
#include <sys/socket.h>

namespace srsran {

/// Get the IP address and port from sockaddr structure.
struct socket_name_info {
  std::string address;
  int         port;
};
socket_name_info get_nameinfo(struct sockaddr& ai_addr, const socklen_t& ai_addrlen);

bool set_reuse_addr(const unique_fd& fd, srslog::basic_logger& logger);

bool bind_to_interface(const unique_fd& fd, const std::string& interface, srslog::basic_logger& logger);

bool sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, srslog::basic_logger& logger);

/// Set a receive timeout for a socket.
bool set_receive_timeout(const unique_fd& fd, std::chrono::seconds rx_timeout, srslog::basic_logger& logger);

/// Set a socket as non-blocking.
bool set_non_blocking(const unique_fd& fd, srslog::basic_logger& logger);

std::string sock_type_to_str(int type);

} // namespace srsran
