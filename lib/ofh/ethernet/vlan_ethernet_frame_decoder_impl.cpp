/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    logger.debug("Sector #{}: Dropped received Ethernet frame of size '{}' bytes as it is below the minimum allowed "
                 "size of '{}' bytes",
                 sector_id,
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
