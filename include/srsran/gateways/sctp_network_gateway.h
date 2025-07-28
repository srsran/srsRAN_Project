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

#include "srsran/gateways/network_gateway.h"
#include <optional>
#include <sys/types.h>

namespace srsran {

constexpr uint16_t NGAP_PPID  = 60; ///< NGAP PPID, see TS 38.412, section 7.
constexpr uint16_t F1AP_PPID  = 62; ///< F1AP PPID, see TS 38.472, section 7.
constexpr uint16_t E1AP_PPID  = 64; ///< E1AP PPID, see TS 37.482, section 7.
constexpr uint16_t E2_CP_PPID = 70; ///< E2-CP PPID assigned by IANA
constexpr uint16_t E2_UP_PPID = 71; ///< E2-UP PPID assigned by IANA
constexpr uint16_t E2_DU_PPID = 72; ///< E2-DU PPID assigned by IANA

constexpr uint16_t NGAP_PORT = 38412; ///< NGAP port, see TS 38.412, section 7.
constexpr uint16_t E1AP_PORT = 38462; ///< E1AP port, see TS 38.462, section 7.
constexpr uint16_t F1AP_PORT = 38472; ///< F1AP port, see TS 38.472, section 7.

/// \brief Configuration for SCTP network gateway that is common to the server and client.
struct sctp_network_gateway_config : public common_network_gateway_config {
  /// Name provided to the interface node.
  std::string if_name;
  /// Payload Protocol Identifier
  int                                      ppid = 0;
  std::optional<std::chrono::milliseconds> rto_initial;
  std::optional<std::chrono::milliseconds> rto_min;
  std::optional<std::chrono::milliseconds> rto_max;
  std::optional<int32_t>                   init_max_attempts;
  std::optional<std::chrono::milliseconds> max_init_timeo;
  std::optional<std::chrono::milliseconds> hb_interval;
  std::optional<int32_t>                   assoc_max_rxt;
  std::optional<bool>                      nodelay;
};

/// \brief Configuration for SCTP network client
struct sctp_network_connector_config : public sctp_network_gateway_config {
  std::string dest_name;
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
  virtual std::optional<uint16_t> get_listen_port() = 0;
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

} // namespace srsran
