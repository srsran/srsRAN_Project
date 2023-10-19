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

#include "../../../../lib/ofh/serdes/ofh_uplane_message_decoder_static_compression_impl.h"
#include "srsran/ofh/compression/iq_decompressor.h"
#include "srsran/ran/cyclic_prefix.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

namespace {

/// Dummy IQ decompressor.
class iq_decompressor_dummy : public iq_decompressor
{
public:
  void decompress(span<cf_t>                   iq_data,
                  span<const compressed_prb>   compressed_prbs,
                  const ru_compression_params& params) override
  {
  }
};

} // namespace

TEST(ofh_uplane_packet_decoder_static_impl, valid_packet_should_decode_correctly)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01,
      0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2,
      0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0x01,
      0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12, 0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c,
      0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02,
      0x4e, 0x02, 0x58, 0x02, 0x58, 0x02, 0x62, 0x02, 0x62, 0x02, 0x6c, 0x02, 0x6c, 0x02, 0x76, 0x02, 0x76, 0x02, 0x80,
      0x02, 0x80, 0x02, 0x8a, 0x02, 0x8a, 0x02, 0x94, 0x02, 0x94, 0x02, 0x9e, 0x02, 0x9e, 0x02, 0xa8, 0x02, 0xa8, 0x02,
      0xb2, 0x02, 0xb2, 0x02, 0xbc, 0x02, 0xbc, 0x02, 0xc6, 0x02, 0xc6, 0x02, 0xd0, 0x02, 0xd0, 0x02, 0xda, 0x02, 0xda};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  slot_point slot(1, 2, 4, 1);
  ASSERT_TRUE(decode_result);
  ASSERT_EQ(results.params.direction, data_direction::uplink);
  ASSERT_EQ(results.params.slot, slot);
  ASSERT_EQ(results.params.filter_index, filter_index_type::standard_channel_filter);
  ASSERT_EQ(results.params.symbol_id, 2);

  const uplane_section_params& section = results.sections.front();
  ASSERT_EQ(section.section_id, 7);
  ASSERT_EQ(section.start_prb, 36);
  ASSERT_EQ(section.nof_prbs, 3);
  ASSERT_TRUE(section.is_every_rb_used);
  ASSERT_TRUE(section.use_current_symbol_number);
}

TEST(ofh_uplane_packet_decoder_static_impl, missing_one_iq_sample_must_fail)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01,
      0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2,
      0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea, 0x01,
      0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12, 0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c,
      0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02,
      0x4e, 0x02, 0x58, 0x02, 0x58, 0x02, 0x62, 0x02, 0x62, 0x02, 0x6c, 0x02, 0x6c, 0x02, 0x76, 0x02, 0x76, 0x02, 0x80,
      0x02, 0x80, 0x02, 0x8a, 0x02, 0x8a, 0x02, 0x94, 0x02, 0x94, 0x02, 0x9e, 0x02, 0x9e, 0x02, 0xa8, 0x02, 0xa8, 0x02,
      0xb2, 0x02, 0xb2, 0x02, 0xbc, 0x02, 0xbc, 0x02, 0xc6, 0x02, 0xc6, 0x02, 0xd0, 0x02, 0xd0};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, missing_one_prb_must_fail)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01,
      0xe0, 0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08,
      0x02, 0x12, 0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02,
      0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, static_compression_with_compression_header_fails)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x03, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86,
      0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0,
      0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08, 0x02, 0x12,
      0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02, 0x3a, 0x02, 0x3a,
      0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, decoding_one_section_and_failing_to_decode_another_passes)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x00, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2,
                                 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01,
                                 0xe0, 0x01, 0xea, 0x01, 0xea, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(results.sections.size(), 1);
}

TEST(ofh_uplane_packet_decoder_static_impl, missing_section_header_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, missing_header_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, downlink_packet_should_fail)
{
  std::vector<uint8_t> packet = {0x90, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86,
                                 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4,
                                 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc,
                                 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, reserved_filter_index_should_fail)
{
  std::vector<uint8_t> packet = {0x18, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86,
                                 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4,
                                 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc,
                                 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, symbol_index_out_of_range_should_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x4e, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86,
                                 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4,
                                 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc,
                                 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01, 0xea, 0x01, 0xea};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 16},
                                                         {compression_type::none, 16});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, none_compression_with_15_bits_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86,
                                 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4,
                                 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc,
                                 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::none, 15},
                                                         {compression_type::none, 15});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, bfp_with_15_bits_should_pass)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01,
                                 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01,
                                 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01,
                                 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 15},
                                                         {compression_type::BFP, 15});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, bfp_with_15_bits_without_ud_comp_len_must_fail)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x01, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86,
                                 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4,
                                 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8, 0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc,
                                 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01, 0xe0, 0x01};

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         273,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 15},
                                                         {compression_type::BFP, 15});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_FALSE(decode_result);
}

TEST(ofh_uplane_packet_decoder_static_impl, if_message_num_prbs_equals_zero_decoder_uses_configured_ru_nof_prbs)
{
  std::vector<uint8_t> packet = {0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c,
                                 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01,
                                 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a,
                                 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
                                 0x9a, 0x01, 0xa4, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(ru_nof_prbs, results.sections.front().nof_prbs);
}

TEST(ofh_uplane_packet_decoder_static_impl, if_message_contains_one_valid_section_and_padding_passes)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90,
      0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
      0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  uplane_message_decoder_results results;
  bool                           decode_result = decoder.decode(results, packet);

  ASSERT_TRUE(decode_result);
  ASSERT_EQ(ru_nof_prbs, results.sections.front().nof_prbs);
  ASSERT_EQ(1, results.sections.size());
}

TEST(ofh_uplane_packet_decoder_static_impl, peek_filter_index)
{
  std::vector<uint8_t> packet = {
      0x10, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90,
      0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
      0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  ASSERT_EQ(filter_index_type::standard_channel_filter, decoder.peek_filter_index(packet));
}

TEST(ofh_uplane_packet_decoder_static_impl, peek_filter_index_returns_reserved_on_peek_failure)
{
  std::vector<uint8_t> packet;
  const unsigned       ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  ASSERT_EQ(filter_index_type::reserved, decoder.peek_filter_index(packet));
}

TEST(ofh_uplane_packet_decoder_static_impl, peek_prach_filter_index)
{
  std::vector<uint8_t> packet = {
      0x11, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90,
      0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
      0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  ASSERT_EQ(filter_index_type::ul_prach_preamble_1p25khz, decoder.peek_filter_index(packet));
}

TEST(ofh_uplane_packet_decoder_static_impl, peek_slot_symbol_point)
{
  slot_symbol_point slot_point({1, 2, 4, 1}, 2, 14);

  std::vector<uint8_t> packet = {
      0x11, 0x02, 0x40, 0x42, 0x00, 0x70, 0x24, 0x00, 0x00, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90,
      0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01,
      0x9a, 0x01, 0xa4, 0x01, 0x86, 0x01, 0x86, 0x01, 0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0x01,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  ASSERT_EQ(slot_point, decoder.peek_slot_symbol_point(packet));
}

TEST(ofh_uplane_packet_decoder_static_impl, return_invalid_slot_point_on_packet_size_smaller_than_required)
{
  std::vector<uint8_t> packet = {0x10, 0x30, 0x40};

  const unsigned ru_nof_prbs = 2;

  uplane_message_decoder_static_compression_impl decoder(srslog::fetch_basic_logger("TEST"),
                                                         subcarrier_spacing::kHz30,
                                                         get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                         ru_nof_prbs,
                                                         std::make_unique<iq_decompressor_dummy>(),
                                                         {compression_type::BFP, 9},
                                                         {compression_type::BFP, 9});

  slot_symbol_point slot = decoder.peek_slot_symbol_point(packet);

  ASSERT_FALSE(slot.get_slot().valid());
}
