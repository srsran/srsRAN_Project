/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_client_impl.h"

using namespace srsran;

sctp_network_client_impl::sctp_network_client_impl(const sctp_network_node_config& sctp_cfg, io_broker& broker_) :
  sctp_network_gateway_common_impl(sctp_cfg), broker(broker_)
{
  (void)broker;
}

std::unique_ptr<sctp_network_client> sctp_network_client_impl::create(const sctp_network_node_config& sctp_cfg,
                                                                      io_broker&                      broker)
{
  return nullptr;
}

sctp_network_client_impl::~sctp_network_client_impl() {}

std::unique_ptr<sctp_association_pdu_notifier>
sctp_network_client_impl::connect_to(const std::string& connection_name, const std::string& dest_addr, int dest_port)
{
  if (not node_cfg.bind_address.empty()) {
    // If bind address was specified in the config, it means that we already created the socket and binded it. We can
    // reuse the socket.
  }

  logger.info("Connecting to {} on \"{}\":{}...", connection_name, dest_addr, dest_port);
  fmt::print("Connecting to {} on \"{}\":{}...\n", connection_name, dest_addr, dest_port);

  sockaddr_searcher searcher{dest_addr, dest_port, logger};
  auto              start = std::chrono::steady_clock::now();
  // Create SCTP socket only if not created earlier through bind (server case) or another connection (client with
  // multiple associations).
  bool             reuse_socket = socket.is_open();
  struct addrinfo* result       = nullptr;
  for (auto candidate = searcher.next(); candidate != nullptr and result == nullptr; candidate = searcher.next()) {
    if (not reuse_socket) {
      // Create SCTP socket only if not created earlier through bind or another connection.
      expected<sctp_socket> outcome = create_socket(candidate->ai_family, candidate->ai_socktype);
      if (outcome.is_error()) {
        if (errno == ESOCKTNOSUPPORT) {
          // Stop the search.
          break;
        }
        continue;
      }
      socket = std::move(outcome.value());
    }

    bool connection_success = socket.connect(*result->ai_addr, result->ai_addrlen);
    if (not connection_success and not reuse_socket) {
      // connection failed, but before trying the next address, make sure the just created socket is deleted.
      socket.close();
    }

    // Found a valid candidate.
    result = candidate;
  }

  if (result == nullptr) {
    auto end    = std::chrono::steady_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    fmt::print("Failed to connect SCTP socket to \"{}\":{}. error=\"{}\" timeout={}ms\n",
               dest_addr,
               dest_port,
               strerror(errno),
               now_ms.count());
    logger.error("Failed to connect SCTP socket to {}:{}. error=\"{}\" timeout={}ms",
                 dest_addr,
                 dest_port,
                 strerror(errno),
                 now_ms.count());

    return nullptr;
  }

  logger.info("SCTP socket connection to \"{}\":{} was successful", dest_addr, dest_port);

  return nullptr;
}
