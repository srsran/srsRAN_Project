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
