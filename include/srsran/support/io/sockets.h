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

#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/support/io/unique_fd.h"
#include <sys/socket.h>

namespace srsran {

/// Get the IP address and port from sockaddr structure.
std::pair<std::string, int> get_nameinfo(struct sockaddr& ai_addr, const socklen_t& ai_addrlen);

bool set_reuse_addr(const unique_fd& fd, srslog::basic_logger& logger);

bool bind_to_interface(const unique_fd& fd, const std::string& interface, srslog::basic_logger& logger);

bool sockaddr_to_ip_str(const sockaddr* addr, std::string& ip_address, srslog::basic_logger& logger);

std::string sock_type_to_str(int type);

} // namespace srsran
