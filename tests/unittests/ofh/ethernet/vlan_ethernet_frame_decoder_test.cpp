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

#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ether;

TEST(vlan_ethernet_frame_decoder_impl_test, decode_valid_vlan_ethernet_frame_should_pass)
{
  std::vector<uint8_t> packet = {
      0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80, 0x61, 0x5f, 0x0d, 0xdf, 0xaa, 0xaa, 0xbb, 0x66, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  std::unique_ptr<vlan_frame_decoder> decoder = create_vlan_frame_decoder(srslog::fetch_basic_logger("TEST"));

  vlan_frame_params params;
  auto              payload = decoder->decode(packet, params);
  ASSERT_TRUE((params.mac_src_address == mac_address{0x80, 0x61, 0x5f, 0x0d, 0xdf, 0xaa}));
  ASSERT_TRUE((params.mac_dst_address == mac_address{0x00, 0x11, 0x22, 0x33, 0x44, 0x55}));
  ASSERT_EQ(params.eth_type, 0xaabb);

  constexpr unsigned HEADER_SIZE = 14;
  ASSERT_EQ(payload.size(), packet.size() - HEADER_SIZE);
  ASSERT_EQ(payload[0], 0x66);
}

TEST(vlan_ethernet_frame_decoder_impl_test, decode_small_vlan_ethernet_frame_should_fail)
{
  std::vector<uint8_t> packet = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80, 0x61, 0x5f, 0x0d,
                                 0xdf, 0xaa, 0xaa, 0xbb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  std::unique_ptr<vlan_frame_decoder> decoder = create_vlan_frame_decoder(srslog::fetch_basic_logger("TEST"));

  vlan_frame_params params;
  auto              payload = decoder->decode(packet, params);

  ASSERT_TRUE(payload.empty());
}
