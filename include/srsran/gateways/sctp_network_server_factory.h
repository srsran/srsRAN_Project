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

#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/gateways/sctp_network_server.h"

namespace srsran {

/// Configuration of an SCTP server.
struct sctp_network_server_config {
  sctp_network_gateway_config       sctp;
  io_broker&                        broker;
  sctp_network_association_factory& association_handler_factory;
};

/// Creates and starts an SCTP network node that can operate both as a server and client and create new SCTP
/// associations.
std::unique_ptr<sctp_network_server> create_sctp_network_server(const sctp_network_server_config& config);

} // namespace srsran