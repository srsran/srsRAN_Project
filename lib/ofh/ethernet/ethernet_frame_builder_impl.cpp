/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
