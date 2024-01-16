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

#include "../../../../lib/ofh/compression/iq_compression_none_impl.h"
#include "../../../../lib/ofh/serdes/ofh_uplane_message_builder_static_compression_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/srsvec/zero.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

namespace {

/// Open Fronthaul User-Plane section header size in bytes.
constexpr unsigned OFH_UP_SECTION_HEADER_SIZE = 4;
/// Open Fronthaul User-Plane radio application header size in bytes.
constexpr unsigned OFH_UP_RADIO_APP_HEADER_SIZE = 4;

/// Returns offset in number of bytes to the IQ data from the start of the packet.
units::bytes get_iq_data_offset_static_config()
{
  unsigned header_size = OFH_UP_SECTION_HEADER_SIZE + OFH_UP_RADIO_APP_HEADER_SIZE;
  return units::bytes(header_size);
}

/// Structure describing a test case.
struct test_case_t {
  /// Original IQ data.
  std::vector<cf_t> iq_data;
  /// OFH U-Plane packet builder parameters.
  uplane_message_params params;
  /// Expected output packet.
  std::vector<uint8_t> packet;
};

static const std::vector<test_case_t> ofh_uplane_builder_test_data = {
    {{// PRB 0
      {0.0116, 0.0116},
      {0.0119, 0.0119},
      {0.0122, 0.0122},
      {0.0125, 0.0125},
      {0.0128, 0.0128},
      {0.0131, 0.0131},
      {0.0134, 0.0134},
      {0.0137, 0.0137},
      {0.0140, 0.0140},
      {0.0143, 0.0143},
      {0.0146, 0.0146},
      {0.0150, 0.0150},
      // PRB 1
      {0.0153, 0.0153},
      {0.0156, 0.0156},
      {0.0159, 0.0159},
      {0.0162, 0.0162},
      {0.0165, 0.0165},
      {0.0168, 0.0168},
      {0.0171, 0.0171},
      {0.0174, 0.0174},
      {0.0177, 0.0177},
      {0.0180, 0.0180},
      {0.0183, 0.0183},
      {0.0186, 0.0186},
      // PRB 2
      {0.0189, 0.0189},
      {0.0192, 0.0192},
      {0.0195, 0.0195},
      {0.0198, 0.0198},
      {0.0201, 0.0201},
      {0.0204, 0.0204},
      {0.0208, 0.0208},
      {0.0211, 0.0211},
      {0.0214, 0.0214},
      {0.0217, 0.0217},
      {0.0220, 0.0220},
      {0.0223, 0.0223}},
     // Params: direction downlink, Numerology 1, slot 2, filter index 0, startPRB 0, 3 PRBs, Symbol
     // 9
     {data_direction::downlink,
      {1, 2},
      filter_index_type::standard_channel_filter,
      0,
      3,
      9,
      section_type::type_1,
      {compression_type::none, 16}},
     // Packet
     {0x90, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x03, 0x01, 0x7c, 0x01, 0x7c, 0x01, 0x86, 0x01, 0x86, 0x01,
      0x90, 0x01, 0x90, 0x01, 0x9a, 0x01, 0x9a, 0x01, 0xa4, 0x01, 0xa4, 0x01, 0xae, 0x01, 0xae, 0x01, 0xb8,
      0x01, 0xb8, 0x01, 0xc2, 0x01, 0xc2, 0x01, 0xcc, 0x01, 0xcc, 0x01, 0xd6, 0x01, 0xd6, 0x01, 0xe0, 0x01,
      0xe0, 0x01, 0xea, 0x01, 0xea, 0x01, 0xf4, 0x01, 0xf4, 0x01, 0xfe, 0x01, 0xfe, 0x02, 0x08, 0x02, 0x08,
      0x02, 0x12, 0x02, 0x12, 0x02, 0x1c, 0x02, 0x1c, 0x02, 0x26, 0x02, 0x26, 0x02, 0x30, 0x02, 0x30, 0x02,
      0x3a, 0x02, 0x3a, 0x02, 0x44, 0x02, 0x44, 0x02, 0x4e, 0x02, 0x4e, 0x02, 0x58, 0x02, 0x58, 0x02, 0x62,
      0x02, 0x62, 0x02, 0x6c, 0x02, 0x6c, 0x02, 0x76, 0x02, 0x76, 0x02, 0x80, 0x02, 0x80, 0x02, 0x8a, 0x02,
      0x8a, 0x02, 0x94, 0x02, 0x94, 0x02, 0x9e, 0x02, 0x9e, 0x02, 0xa8, 0x02, 0xa8, 0x02, 0xb2, 0x02, 0xb2,
      0x02, 0xbc, 0x02, 0xbc, 0x02, 0xc6, 0x02, 0xc6, 0x02, 0xd0, 0x02, 0xd0, 0x02, 0xda, 0x02, 0xda}}};

TEST(ofh_uplane_packet_builder_static_impl_test, non_compressed_packet_should_pass)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("TEST", false);

  for (const test_case_t& test_case : ofh_uplane_builder_test_data) {
    // Create a compressor.
    iq_compression_none_impl compressor(logger);

    ofh_uplane_message_builder_static_compression_impl builder(srslog::fetch_basic_logger("TEST"), compressor);

    // Prepare output buffer and build packet.
    std::vector<uint8_t> result_packet(test_case.packet.size(), 0);
    builder.build_message(result_packet, test_case.iq_data, test_case.params);

    // First make sure basic header matches the expected one.
    units::bytes header_size = get_iq_data_offset_static_config();
    ASSERT_TRUE(
        std::equal(result_packet.begin(), result_packet.begin() + header_size.value(), test_case.packet.begin()))
        << "Wrong header in the generated U-Plane packet";

    // Compare IQ data with an error tolerance in LSB bits, that can be introduced due to quantization.
    ASSERT_TRUE(std::equal(result_packet.begin() + header_size.value(),
                           result_packet.end(),
                           test_case.packet.begin() + header_size.value(),
                           [](uint8_t a, uint8_t b) { return std::abs(a - b) <= 2; }))
        << "Wrong IQ data in the generated U-Plane packet.";
  }
}

} // namespace
