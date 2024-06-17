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

#include "srsran/f1u/cu_up/split_connector/f1u_split_connector_factory.h"
#include "f1u_split_connector.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<f1u_cu_up_udp_gateway> srsran::srs_cu_up::create_split_f1u_gw(f1u_cu_up_split_gateway_creation_msg msg)
{
  return std::make_unique<f1u_split_connector>(msg.udp_gw, msg.demux, msg.gtpu_pcap, msg.peer_port);
}
