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

#include "srsran/adt/optional.h"
#include "srsran/gateways/network_gateway.h"
#include <sys/types.h>

namespace srsran {

constexpr uint16_t NGAP_PPID  = 60; // NGAP PPID, see TS 38.412, section 7.
constexpr uint16_t E2_CP_PPID = 70; // E2-CP PPID assigned by IANA
constexpr uint16_t E2_UP_PPID = 71; // E2-UP PPID assigned by IANA
constexpr uint16_t E2_DU_PPID = 72; // E2-DU PPID assigned by IANA

/// \brief Configuration for SCTP network gateway that is common to the server and client.
struct sctp_network_node_config : public common_network_gateway_config {
  /// Payload Protocol Identifier
  int ppid = 0;
  /// SCTP specific options
  optional<int32_t> rto_initial;
  optional<int32_t> rto_min;
  optional<int32_t> rto_max;
  optional<int32_t> init_max_attempts;
  optional<int32_t> max_init_timeo;
  optional<bool>    nodelay;
};

/// \brief Configuration for SCTP network client
struct sctp_network_connector_config : public sctp_network_node_config {
  std::string connection_name;
  std::string connect_address;
  int         connect_port = 0;
};

/// \brief Interface to inject PDUs into gateway entity.
class sctp_network_gateway_data_handler
{
public:
  virtual ~sctp_network_gateway_data_handler() = default;

  /// \brief Handle the incoming PDU.
  /// \param[in]  put Byte-buffer with new PDU.
  virtual void handle_pdu(const byte_buffer& pdu) = 0;
};

/// Interface to trigger bind/listen/connect operations on gateway socket.
class sctp_network_gateway_controller : public network_gateway_controller
{
public:
  virtual ~sctp_network_gateway_controller() = default;

  /// \brief Create and connect socket to given address.
  virtual bool create_and_connect() = 0;

  /// \brief Start listening on socket.
  virtual bool listen() = 0;

  /// \brief Return the port on which the socket is listening.
  ///
  /// In case the gateway was configured to listen on port 0, i.e. the operating system shall pick a random free port,
  /// this function can be used to get the actual port number.
  virtual optional<uint16_t> get_listen_port() = 0;
};

/// Interface to inform upper layers about connection establishment, drops, etc.
class sctp_network_gateway_control_notifier
{
public:
  virtual ~sctp_network_gateway_control_notifier() = default;

  /// \brief This callback is invoked when connect() succeeds (client) or a new client connected to a listening socket
  /// (server).
  virtual void on_connection_established() = 0;

  /// \brief This callback is invoked when the connection is dropped or cannot be established in the first place.
  virtual void on_connection_loss() = 0;
};

class sctp_network_gateway : public sctp_network_gateway_controller, public sctp_network_gateway_data_handler
{};

class sctp_network_association_factory
{
public:
  virtual ~sctp_network_association_factory() = default;

  virtual std::unique_ptr<network_gateway_data_notifier> create() = 0;
};

class sctp_network_server : public network_gateway_controller
{
public:
  /// \brief Start listening on socket.
  virtual bool listen() = 0;

  /// \brief Return the port on which the socket is listening.
  ///
  /// In case the gateway was configured to listen on port 0, i.e. the operating system shall pick a random free port,
  /// this function can be used to get the actual port number.
  virtual optional<uint16_t> get_listen_port() = 0;

  virtual void attach_association_handler(sctp_network_association_factory& factory) = 0;
};

} // namespace srsran
