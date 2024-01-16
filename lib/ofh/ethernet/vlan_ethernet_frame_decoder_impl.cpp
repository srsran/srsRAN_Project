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

#include "vlan_ethernet_frame_decoder_impl.h"
#include "../support/network_order_binary_deserializer.h"
#include "ethernet_constants.h"

using namespace srsran;
using namespace ether;

static constexpr unsigned MIN_ETH_LEN = 64;

span<const uint8_t> vlan_frame_decoder_impl::decode(span<const uint8_t> frame, vlan_frame_params& eth_params)
{
  // Ethernet frames should include padding bytes up to the minimum length.
  if (frame.size() < MIN_ETH_LEN) {
    logger.debug(
        "Dropped received Ethernet frame of size '{}' bytes as it is below the minimum allowed size of '{}' bytes",
        frame.size(),
        MIN_ETH_LEN);
    return {};
  }

  ofh::network_order_binary_deserializer deserializer(frame);

  deserializer.read(eth_params.mac_dst_address);
  deserializer.read(eth_params.mac_src_address);

  // VLAN parameters are stripped by the NIC.

  eth_params.eth_type = deserializer.read<uint16_t>();

  return frame.last(frame.size() - deserializer.get_offset());
}
