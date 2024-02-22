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

#include "../../../../lib/ofh/serdes/ofh_cplane_message_builder_static_compression_impl.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {
      0x90, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(0, 0, 0);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 0;
  section.prb_start                             = 0;
  section.nof_prb                               = 0;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;
  packet_params.compr_params                    = {compression_type::none, 16};

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_invented_control_packet_should_pass)
{
  std::vector<uint8_t> packet = {
      0x90, 0x02, 0x30, 0x44, 0x01, 0x01, 0x00, 0x00, 0x00, 0x50, 0x06, 0x07, 0xff, 0xfe, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type1_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(1, 2, 7);
  header.start_symbol                     = 4;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 5;
  section.prb_start                             = 6;
  section.nof_prb                               = 7;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 14;

  packet_params.compr_params = {compression_type::none, 16};

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_dl_ul_radio_channel_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_invented_idle_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x90, 0x28, 0x20, 0x4a, 0x01, 0x00, 0x08, 0xa0, 0x01, 0x00,
                                 0x00, 0x00, 0x00, 0x50, 0x06, 0x07, 0xff, 0xf4, 0x00, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type0_parameters  packet_params;
  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::downlink;
  header.filter_index                     = filter_index_type::standard_channel_filter;
  header.slot                             = slot_point(1, 40, 5);
  header.start_symbol                     = 10;

  cplane_common_section_0_1_3_5_fields& section = packet_params.section_fields.common_fields;
  section.section_id                            = 5;
  section.prb_start                             = 6;
  section.nof_prb                               = 7;
  section.re_mask                               = 0xfff;
  section.nof_symbols                           = 4;

  packet_params.cp          = cyclic_prefix::NORMAL;
  packet_params.scs         = srsran::subcarrier_spacing::kHz30;
  packet_params.time_offset = 2208;

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_idle_guard_period_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}

TEST(ofh_control_plane_packet_builder_impl_test, build_valid_prach_mixed_num_packet_should_pass)
{
  std::vector<uint8_t> packet = {0x13, 0xf4, 0x90, 0x40, 0x01, 0x03, 0x01, 0xe4, 0xc1, 0x00, 0x00, 0x00,
                                 0x00, 0x10, 0x00, 0x0c, 0xff, 0xfc, 0x00, 0x00, 0xff, 0xf3, 0x34, 0x00};

  std::vector<uint8_t> result_packet(packet.size(), 0);

  cplane_section_type3_parameters packet_params;
  packet_params.compr_params                    = {compression_type::none, 16};
  packet_params.scs                             = cplane_scs::kHz30;
  packet_params.time_offset                     = 484;
  packet_params.cpLength                        = 0;
  packet_params.fft_size                        = cplane_fft_size::fft_4096;
  packet_params.section_fields.frequency_offset = -3276;

  cplane_radio_application_header& header = packet_params.radio_hdr;
  header.direction                        = data_direction::uplink;
  header.filter_index                     = filter_index_type::ul_prach_preamble_short;
  header.slot                             = slot_point(to_numerology_value(subcarrier_spacing::kHz30), 244, 19);
  header.start_symbol                     = 0;

  cplane_common_section_0_1_3_5_fields& common_section = packet_params.section_fields.common_fields;
  common_section.section_id                            = 1;
  common_section.prb_start                             = 0;
  common_section.nof_prb                               = 12;
  common_section.re_mask                               = 0xfff;
  common_section.nof_symbols                           = 12;

  cplane_message_builder_static_compression_impl builder;

  unsigned nof_bytes = builder.build_prach_mixed_numerology_message(result_packet, packet_params);

  ASSERT_EQ(packet, result_packet);
  ASSERT_EQ(nof_bytes, packet.size());
}
