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

#include "srsran/srslog/sink.h"
#include <arpa/inet.h>
#include <unistd.h>

namespace srslog {

/// This sink implementation writes to a UDP socket.
class udp_sink : public sink
{
public:
  udp_sink(std::string remote_ip_, unsigned port_, std::unique_ptr<log_formatter> f) :
    sink(std::move(f)), remote_ip(std::move(remote_ip_)), port(port_)
  {
  }

  ~udp_sink() { ::close(socket_fd); }

  udp_sink(const udp_sink& other)            = delete;
  udp_sink& operator=(const udp_sink& other) = delete;

  detail::error_string write(detail::memory_buffer buffer) override
  {
    if (socket_fd == -1) {
      socket_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
      if (socket_fd < 0) {
        return "Cannot create UDP socket";
      }

      remote_address            = {};
      remote_address.sin_family = AF_INET;
      remote_address.sin_port   = ::htons(port);
      if (::inet_pton(AF_INET, remote_ip.c_str(), &remote_address.sin_addr) < 1) {
        return "Invalid IP address format";
      }
    }

    ::sendto(socket_fd,
             buffer.data(),
             buffer.size(),
             0,
             reinterpret_cast<const struct sockaddr*>(&remote_address),
             sizeof(remote_address));

    return {};
  }

  detail::error_string flush() override { return {}; }

private:
  std::string   remote_ip;
  unsigned      port;
  int           socket_fd = -1;
  ::sockaddr_in remote_address;
};

} // namespace srslog
