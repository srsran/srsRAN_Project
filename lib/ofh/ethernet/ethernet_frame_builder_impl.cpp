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

#include "ethernet_frame_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "ethernet_constants.h"

using namespace srsran;
using namespace ether;

frame_builder_impl::frame_builder_impl(const srsran::ether::vlan_frame_params& eth_params_) : eth_params(eth_params_) {}

units::bytes frame_builder_impl::get_header_size() const
{
  return ETH_HEADER_SIZE;
}

void frame_builder_impl::build_frame(span<uint8_t> buffer)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Write destination MAC address (6 Bytes).
  serializer.write(eth_params.mac_dst_address);

  // Write source MAC address (6 Bytes).
  serializer.write(eth_params.mac_src_address);

  // Write Ethernet Type (2 Bytes).
  serializer.write(eth_params.eth_type);
}
