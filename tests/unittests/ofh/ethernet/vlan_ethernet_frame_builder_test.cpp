/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ofh/ethernet/ethernet_factories.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ether;

TEST(vlan_ethernet_frame_builder_impl_test, build_valid_vlan_ethernet_frame_should_pass)
{
  std::vector<uint8_t> packet = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80, 0x61, 0x5f, 0x0d, 0xdf,
                                 0xaa, 0x81, 0x00, 0x00, 0x02, 0xaa, 0xbb, 0x00, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  vlan_frame_params params;
  params.mac_dst_address = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  params.mac_src_address = {0x80, 0x61, 0x5f, 0x0d, 0xdf, 0xaa};
  params.tci             = 2;
  params.eth_type        = 0xaabb;

  std::unique_ptr<frame_builder> builder = create_vlan_frame_builder(params);
  std::copy(packet.begin() + builder->get_header_size().value(),
            packet.end(),
            result_packet.begin() + builder->get_header_size().value());

  builder->build_frame(result_packet);

  ASSERT_EQ(packet, result_packet);
}

TEST(ethernet_frame_builder_without_vlan_impl_test, build_valid_ethernet_frame_should_pass)
{
  std::vector<uint8_t> packet = {
      0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80, 0x61, 0x5f, 0x0d, 0xdf, 0xaa, 0xaa, 0xbb, 0x00, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  vlan_frame_params params;
  params.mac_dst_address = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
  params.mac_src_address = {0x80, 0x61, 0x5f, 0x0d, 0xdf, 0xaa};
  params.eth_type        = 0xaabb;

  std::unique_ptr<frame_builder> builder = create_frame_builder(params);
  std::copy(packet.begin() + builder->get_header_size().value(),
            packet.end(),
            result_packet.begin() + builder->get_header_size().value());

  builder->build_frame(result_packet);

  ASSERT_EQ(packet, result_packet);
}
