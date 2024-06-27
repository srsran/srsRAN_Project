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
#include <netdb.h>
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

      // First treat remote_ip as an ip address.
      if (::inet_pton(AF_INET, remote_ip.c_str(), &remote_address.sin_addr) == 1) {
        // IP address found, do nothing.
      } else {
        // Treat remote_ip as a hostname.
        if (auto err = try_to_resolve_hostname(); !err.get_error().empty()) {
          return err;
        }
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
  /// Tries to resolve remote_ip as a hostname. Returns an error string on failure.
  detail::error_string try_to_resolve_hostname()
  {
    // IP address not found, try resolving ip address using DNS.
    ::addrinfo hints = {};
    // Accept all flags.
    hints.ai_flags = 0;
    // Use IPv4
    hints.ai_family = AF_INET;
    // Accept all socket types.
    hints.ai_socktype = SOCK_DGRAM;
    // As this is a UDP sink, use UDP for protocol.
    hints.ai_protocol  = IPPROTO_UDP;
    ::addrinfo* result = nullptr;
    if (::getaddrinfo(remote_ip.c_str(), nullptr, &hints, &result) != 0) {
      return fmt::format("Could not resolve '{}' as IP address or hostname", remote_ip);
    }
    unsigned nof_ip_addresses = 0;
    for (auto addr = result; addr != nullptr; addr = addr->ai_next) {
      ::sockaddr_in* ipv4     = reinterpret_cast<::sockaddr_in*>(addr->ai_addr);
      remote_address.sin_addr = ipv4->sin_addr;
      ++nof_ip_addresses;
    }

    // Check that only one valid IP was found, otherwise fail.
    if (nof_ip_addresses != 1) {
      return fmt::format("More than one hostname resolution for '{}'. Using IP address '{}'",
                         remote_ip,
                         ::inet_ntoa(remote_address.sin_addr));
    }

    return {};
  }

private:
  std::string   remote_ip;
  unsigned      port;
  int           socket_fd = -1;
  ::sockaddr_in remote_address;
};

} // namespace srslog
