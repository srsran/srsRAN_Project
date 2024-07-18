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

#include "srsran/e1ap/gateways/e1_connection_client.h"
#include "srsran/e1ap/gateways/e1_connection_server.h"

namespace srsran {

class dlt_pcap;
class io_broker;

class e1_local_connector : public srs_cu_up::e1_connection_client, public srs_cu_cp::e1_connection_server
{};

struct e1_local_connector_config {
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates a local connector between CU-CP and CU-UP E1 interfaces, avoiding the need to pack/unpack the exchanged
/// E1AP PDUs or any socket send/recv.
std::unique_ptr<e1_local_connector> create_e1_local_connector(const e1_local_connector_config& cfg);

struct e1_local_sctp_connector_config {
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
  /// IO broker to handle the SCTP Rx data and notifications.
  io_broker& broker;
  /// Port to bind the SCTP socket.
  int bind_port = 0;
};

/// Creates an E1 local connector using an SCTP socket as channel.
///
/// Note: This class is useful for testing.
std::unique_ptr<e1_local_connector> create_e1_local_connector(const e1_local_sctp_connector_config& cfg);

} // namespace srsran