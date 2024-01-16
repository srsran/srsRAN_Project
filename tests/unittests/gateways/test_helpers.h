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

#include <arpa/inet.h>
#include <functional> // for std::function/std::bind
#include <gtest/gtest.h>
#include <netdb.h>
#include <netinet/in.h>
#include <queue>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

#include "srsran/adt/byte_buffer.h"
#include "srsran/gateways/addr_info.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/sockets.h"

using namespace srsran;

byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    if (not pdu.append((uint8_t)i)) {
      pdu.clear();
      break;
    }
  }
  return pdu;
}

byte_buffer make_small_tx_byte_buffer()
{
  return make_tx_byte_buffer(15);
}

byte_buffer make_large_tx_byte_buffer()
{
  return make_tx_byte_buffer(9000);
}

byte_buffer make_oversized_tx_byte_buffer()
{
  return make_tx_byte_buffer(9500);
}

void fill_sctp_hints(struct addrinfo& hints)
{
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_SEQPACKET;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_SCTP;
  hints.ai_canonname = nullptr;
  hints.ai_addr      = nullptr;
  hints.ai_next      = nullptr;
}

void fill_udp_hints(struct addrinfo& hints)
{
  // support ipv4, ipv6 and hostnames
  hints.ai_family    = AF_UNSPEC;
  hints.ai_socktype  = SOCK_DGRAM;
  hints.ai_flags     = 0;
  hints.ai_protocol  = IPPROTO_UDP;
  hints.ai_canonname = nullptr;
  hints.ai_addr      = nullptr;
  hints.ai_next      = nullptr;
}

int get_unused_port(const std::string& bind_addr, struct addrinfo& hints)
{
  int result_port = 0;
  int ret         = 0;

  sockaddr_storage tmp_addr;
  socklen_t        tmp_addr_len = 0;
  struct addrinfo* addrinfo_results;
  std::string      ip_addr;
  auto&            logger = srslog::fetch_basic_logger("TEST");

  // Obtain addrinfo that is compatible with IPv4 and IPv6
  ret = getaddrinfo(bind_addr.c_str(), nullptr, &hints, &addrinfo_results);
  if (ret != 0) {
    logger.error("Failure in `getaddrinfo` for address `{}`: {}", bind_addr.c_str(), strerror(errno));
    return result_port;
  }

  // Create socket of proper type
  int sock_fd = socket(addrinfo_results->ai_family, addrinfo_results->ai_socktype, addrinfo_results->ai_protocol);
  if (sock_fd < 0) {
    logger.error("Failed to create {} socket: {}", ipproto_to_string(addrinfo_results->ai_protocol), strerror(errno));
    goto free_addrinfo_results;
  }

  // Bind socket to a free port.
  ret = bind(sock_fd, addrinfo_results->ai_addr, addrinfo_results->ai_addrlen);
  if (ret != 0) {
    logger.error("Failed to bind socket to `{}`: {}", bind_addr.c_str(), strerror(errno));
    goto close_socket;
  }

  // Find out the port that was assigned by the OS.
  tmp_addr_len = sizeof(tmp_addr);
  ret          = getsockname(sock_fd, (struct sockaddr*)&tmp_addr, &tmp_addr_len);
  if (ret != 0) {
    logger.error("Failed to read port from socket bound to `{}`: {}", bind_addr.c_str(), strerror(errno));
    goto close_socket;
  }
  if (tmp_addr_len > sizeof(tmp_addr)) {
    logger.error("Insufficient tmp_addr_len for getsockname()");
    goto close_socket;
  }

  switch (addrinfo_results->ai_family) {
    case AF_INET:
      result_port = ntohs(((struct sockaddr_in*)&tmp_addr)->sin_port);
      break;
    case AF_INET6:
      result_port = ntohs(((struct sockaddr_in6*)&tmp_addr)->sin6_port);
      break;
    default:
      srslog::fetch_basic_logger("TEST").error("Protocol family is neither IPv4 nor IPv6");
  }
  sockaddr_to_ip_str(addrinfo_results->ai_addr, ip_addr, logger);
  logger.debug("Got unused port. host=\"{}\", family={} proto={} addr=\"{}\" port={}",
               bind_addr,
               addrinfo_results->ai_family == AF_INET ? "AF_INET" : "AF_INET6",
               sock_type_to_str(addrinfo_results->ai_socktype),
               ip_addr,
               result_port);

close_socket:
  ret = close(sock_fd);
  if (ret != 0) {
    logger.error("Failed to close socket with sock_fd={}: {}", sock_fd, strerror(errno));
  }
free_addrinfo_results:
  freeaddrinfo(addrinfo_results);

  return result_port;
}

int get_unused_sctp_port(const std::string& bind_addr)
{
  struct addrinfo hints;
  fill_sctp_hints(hints);

  int port = get_unused_port(bind_addr, hints);
  EXPECT_NE(port, 0) << "Failed to get unused port for SCTP socket";
  return port;
}

int get_unused_udp_port(const std::string& bind_addr)
{
  struct addrinfo hints;
  fill_udp_hints(hints);

  int port = get_unused_port(bind_addr, hints);
  EXPECT_NE(port, 0) << "Failed to get unused port for UDP socket";
  return port;
}

class dummy_network_gateway_control_notifier : public sctp_network_gateway_control_notifier
{
public:
  dummy_network_gateway_control_notifier() = default;

  void on_connection_loss() override { conn_dropped = true; }
  void on_connection_established() override { conn_established = true; }
  bool get_connection_dropped() { return conn_dropped; }
  bool get_connection_established() { return conn_established; }

private:
  bool conn_established = false;
  bool conn_dropped     = false;
};

class dummy_network_gateway_data_notifier : public network_gateway_data_notifier
{
public:
  dummy_network_gateway_data_notifier() = default;
  void on_new_pdu(byte_buffer pdu) override
  {
    // printf("Received PDU\n");
    rx_bytes += pdu.length();
    pdu_queue.push(std::move(pdu));
  }

  unsigned get_rx_bytes() { return rx_bytes; }

  std::queue<byte_buffer> pdu_queue;

private:
  unsigned rx_bytes = 0;
};

class dummy_network_gateway_data_notifier_with_src_addr : public network_gateway_data_notifier_with_src_addr
{
public:
  dummy_network_gateway_data_notifier_with_src_addr() = default;
  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    // printf("Received PDU\n");
    rx_bytes += pdu.length();
    pdu_queue.push(std::move(pdu));
  }

  unsigned get_rx_bytes() { return rx_bytes; }

  std::queue<byte_buffer> pdu_queue;

private:
  unsigned rx_bytes = 0;
};

class dummy_network_gateway_notifier : public sctp_network_gateway_control_notifier,
                                       public network_gateway_data_notifier
{
public:
  dummy_network_gateway_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_connection_loss() override { logger.info("Connection loss"); }
  void on_connection_established() override { logger.info("Connection established"); }
  void on_new_pdu(byte_buffer pdu) override { logger.info("Received PDU"); }

private:
  srslog::basic_logger& logger;
};

/// Dummy PDU handler
class dummy_network_gateway_data_handler : public sctp_network_gateway_data_handler
{
public:
  dummy_network_gateway_data_handler(){};
  void handle_pdu(const byte_buffer& pdu) override { last_pdu = pdu.copy(); }

  byte_buffer last_pdu;
};
