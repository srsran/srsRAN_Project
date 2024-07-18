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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/gateways/e1_connection_server.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

/// Configuration of an SCTP-based E1 Gateway in the CU-CP.
struct e1_cu_cp_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// Creates an E1 Gateway server that listens for incoming SCTP connections, packs/unpacks E1AP PDUs and forwards
/// them to the GW/CU-CP E1AP handler.
std::unique_ptr<srs_cu_cp::e1_connection_server> create_e1_gateway_server(const e1_cu_cp_sctp_gateway_config& params);

} // namespace srsran
