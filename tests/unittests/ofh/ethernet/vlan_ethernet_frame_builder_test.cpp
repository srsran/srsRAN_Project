/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  std::unique_ptr<vlan_frame_builder> builder = create_vlan_frame_builder();
  std::copy(packet.begin() + builder->get_header_size().value(),
            packet.end(),
            result_packet.begin() + builder->get_header_size().value());

  builder->build_vlan_frame(result_packet, params);

  ASSERT_EQ(packet, result_packet);
}
