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

#include "../../../lib/ofh/ecpri/ecpri_packet_decoder_impl.h"
#include "srsran/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ecpri;

TEST(ecpri_packet_decoder_impl_test, decode_valid_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x00, 0x14, 0xbe, 0xbe, 0xca, 0xfe, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_FALSE(payload.empty());
  ASSERT_EQ(params.header.payload_size.value() - 2 * sizeof(uint16_t), payload.size());
  ASSERT_TRUE(params.header.is_last_packet);
  ASSERT_EQ(params.header.revision, 1);
  ASSERT_EQ(params.header.msg_type, message_type::rt_control_data);
}

TEST(ecpri_packet_decoder_impl_test, decode_valid_data_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_FALSE(payload.empty());
  ASSERT_EQ(params.header.payload_size.value() - 2 * sizeof(uint16_t), payload.size());
  ASSERT_TRUE(params.header.is_last_packet);
  ASSERT_EQ(params.header.revision, 1);
  ASSERT_EQ(params.header.msg_type, message_type::iq_data);
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_packet_size_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_payload_size_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00,
                                 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_ecpri_protocol_revision_should_fail)
{
  std::vector<uint8_t> packet = {0x20, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}

TEST(ecpri_packet_decoder_impl_test, decode_invalid_ecpri_last_packet_should_fail)
{
  std::vector<uint8_t> packet = {0x11, 0x00, 0x00, 0x14, 0xca, 0xfe, 0xde, 0xba, 0x90, 0x00, 0x00, 0x00,
                                 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  packet_parameters params;

  packet_decoder_use_header_payload_size decoder(srslog::fetch_basic_logger("TEST"));

  span<const uint8_t> payload = decoder.decode(packet, params);

  ASSERT_TRUE(payload.empty());
}
