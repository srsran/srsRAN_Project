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

#include "../../../lib/ofh/ecpri/ecpri_packet_builder_impl.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ecpri;

TEST(ecpri_packet_builder_impl_test, build_valid_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x00, 0x14, 0xbe, 0xbe, 0xca, 0xfe, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  realtime_control_parameters params;
  params.rtc_id = 0xbebe;
  params.seq_id = 0xcafe;

  packet_builder_impl builder;
  std::copy(packet.begin() + builder.get_header_size(message_type::rt_control_data).value(),
            packet.end(),
            result_packet.begin() + builder.get_header_size(message_type::rt_control_data).value());

  builder.build_control_packet(result_packet, params);

  ASSERT_EQ(packet, result_packet);
}

TEST(ecpri_packet_builder_impl_test, build_valid_data_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  iq_data_parameters params;
  params.pc_id  = 0xcafe;
  params.seq_id = 0xdeba;

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_builder_impl builder;
  std::copy(packet.begin() + builder.get_header_size(message_type::iq_data).value(),
            packet.end(),
            result_packet.begin() + builder.get_header_size(message_type::iq_data).value());

  builder.build_data_packet(result_packet, params);

  ASSERT_EQ(packet, result_packet);
}
