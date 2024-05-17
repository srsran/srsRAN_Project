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

#include "network_gateway.h"
#include "srsran/support/io/transport_layer_address.h"

namespace srsran {

/// Unidirectional notifier of PDUs for a given SCTP association.
class sctp_association_pdu_notifier
{
public:
  virtual ~sctp_association_pdu_notifier() = default;

  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

class sctp_network_association_factory
{
public:
  virtual ~sctp_network_association_factory() = default;

  virtual std::unique_ptr<sctp_association_pdu_notifier> create() = 0;
};

class sctp_network_gateway_info
{
public:
  virtual ~sctp_network_gateway_info() = default;

  /// \brief Retrieve the SCTP network node opened socket file descriptor.
  virtual int get_socket_fd() const = 0;
};

/// SCTP network client interface, which can be used to initiate new SCTP associations to a remote SCTP server.
class sctp_network_client : public sctp_network_gateway_info
{
public:
  virtual ~sctp_network_client() = default;

  /// \brief Connect to remote SCTP node.
  virtual std::unique_ptr<sctp_association_pdu_notifier>
  connect_to(const std::string& connection_name, const std::string& dest_addr, int dest_port) = 0;
};

/// SCTP network server interface, which will handle requests to start new SCTP associations.
class sctp_network_server : public sctp_network_gateway_info
{
public:
  virtual ~sctp_network_server() = default;

  /// \brief Start listening for new SCTP associations.
  virtual bool listen() = 0;
};

/// SCTP network node that can operate both as server and client.
///
/// The SCTP network node acts as a server after calling listen().
class sctp_network_node : public sctp_network_server, public sctp_network_client
{
public:
  virtual ~sctp_network_node() = default;
};

} // namespace srsran