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

#include "srsran/gateways/sctp_network_gateway_factory.h"
#include "sctp_network_gateway_impl.h"
#include "sctp_network_server_impl.h"

using namespace srsran;

std::unique_ptr<sctp_network_gateway> srsran::create_sctp_network_gateway(sctp_network_gateway_creation_message msg)
{
  return std::make_unique<sctp_network_gateway_impl>(
      msg.config, msg.ctrl_notifier, msg.data_notifier, msg.io_rx_executor);
}
