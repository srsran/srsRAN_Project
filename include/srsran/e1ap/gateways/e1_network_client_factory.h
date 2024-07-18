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
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

/// Configuration of an SCTP-based E1 Gateway in the CU-UP.
struct e1_cu_up_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_connector_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// PCAP writer for the E1AP messages.
  dlt_pcap& pcap;
};

/// \brief Create an E1 gateway connector that the CU-UP can use to connect to the CU-CP.
std::unique_ptr<srs_cu_up::e1_connection_client> create_e1_gateway_client(const e1_cu_up_sctp_gateway_config& params);

} // namespace srsran
