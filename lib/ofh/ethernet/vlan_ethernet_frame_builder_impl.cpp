/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "vlan_ethernet_frame_builder_impl.h"
#include "../support/network_order_binary_serializer.h"
#include "ethernet_constants.h"

using namespace srsran;
using namespace ether;

units::bytes vlan_frame_builder_impl::get_header_size() const
{
  return ETH_HEADER_SIZE + ETH_VLAN_TAG_SIZE;
}

void vlan_frame_builder_impl::build_vlan_frame(span<uint8_t> buffer, const vlan_frame_params& eth_params)
{
  ofh::network_order_binary_serializer serializer(buffer.data());

  // Write destination MAC address (6 Bytes).
  serializer.write(span<const uint8_t>(eth_params.mac_dst_address));

  // Write source MAC address (6 Bytes).
  serializer.write(span<const uint8_t>(eth_params.mac_src_address));

  // Write VLAN TPID (2 Bytes).
  serializer.write(VLAN_TPID);

  // Write VLAN TCI (2 Bytes).
  serializer.write(eth_params.tci);

  // Write Ethernet Type (2 Bytes).
  serializer.write(eth_params.eth_type);
}
