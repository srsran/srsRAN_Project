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

#include "srsran/gateways/sctp_network_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

/// Configuration of an SCTP client.
struct sctp_network_client_config {
  sctp_network_gateway_config sctp;
  io_broker&                  broker;
};

/// Creates an SCTP network client.
std::unique_ptr<sctp_network_client> create_sctp_network_client(const sctp_network_client_config& config);

} // namespace srsran