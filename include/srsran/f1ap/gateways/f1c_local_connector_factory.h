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

#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/f1ap/gateways/f1c_connection_server.h"

namespace srsran {

class dlt_pcap;
class io_broker;

class f1c_local_connector : public srs_du::f1c_connection_client, public srs_cu_cp::f1c_connection_server
{};

struct f1c_local_connector_config {
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// Creates a local connector between CU-CP and DU F1-C interfaces, avoiding the need to pack/unpack the exchanged
/// F1AP PDUs or any socket send/recv.
std::unique_ptr<f1c_local_connector> create_f1c_local_connector(const f1c_local_connector_config& cfg);

struct f1c_local_sctp_connector_config {
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
  /// IO broker to handle the SCTP Rx data and notifications.
  io_broker& broker;
};

/// Creates an F1-C local connector using an SCTP socket as channel.
///
/// Note: This class is useful for testing.
std::unique_ptr<f1c_local_connector> create_f1c_local_connector(const f1c_local_sctp_connector_config& cfg);

} // namespace srsran