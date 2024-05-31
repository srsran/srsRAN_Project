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

#include "f1c_connection_server.h"
#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

/// Configuration of an SCTP-based F1-C Gateway.
struct f1c_cu_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the F1AP messages.
  dlt_pcap& pcap;
};

/// Creates an F1-C Gateway server that listens for incoming SCTP connections, packs/unpacks F1AP PDUs and forwards
/// them to the GW/CU-CP F1AP handler.
std::unique_ptr<srs_cu_cp::f1c_connection_server> create_f1c_gateway_server(const f1c_cu_sctp_gateway_config& params);

} // namespace srsran
