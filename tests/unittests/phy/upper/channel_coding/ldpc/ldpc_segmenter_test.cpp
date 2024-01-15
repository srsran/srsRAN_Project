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

/// \file
/// \brief Unit test for LDPC segmenter.
///
/// The test carries out the segmentation of a number of transport blocks of different lengths, for both possible base
/// graphs. Test vectors provide the transport blocks and the resulting segmented blocks for comparison.

#include "ldpc_segmenter_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/srsvec/bit.h"

#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::ldpc;

namespace srsran {

std::ostream& operator<<(std::ostream& os, test_case_t tct)
{
  return os << fmt::format(
             "TBS {}, BG{}, N. Segments {}, Segment length {}", tct.tbs, tct.bg, tct.nof_segments, tct.segment_length);
}

bool operator==(span<const uint8_t> lhs, span<const uint8_t> rhs)
{
  return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

std::ostream& operator<<(std::ostream& os, span<const uint8_t> data)
{
  return os << fmt::format("{}", data);
}

std::ostream& operator<<(std::ostream& os, span<uint8_t> data)
{
  return os << fmt::format("{}", data);
}

} // namespace srsran

namespace {

using LDPCSegmenterParams = test_case_t;

class LDPCSegmenterFixture : public ::testing::TestWithParam<LDPCSegmenterParams>
{
protected:
  // Creates the factories just once.
  static void SetUpTestSuite()
  {
    if (!crc_factory) {
      crc_factory = create_crc_calculator_factory_sw("auto");
      ASSERT_NE(crc_factory, nullptr) << "Cannot create CRC factory";
    }

    if (!segmenter_tx_factory) {
      segmenter_tx_factory = create_ldpc_segmenter_tx_factory_sw(crc_factory);
      ASSERT_NE(segmenter_tx_factory, nullptr) << "Cannot create Tx segmenter factory.";
    }

    if (!segmenter_rx_factory) {
      segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
      ASSERT_NE(segmenter_rx_factory, nullptr) << "Cannot create Rx segmenter factory";
    }
  }

  // Asserts the creation of segmenters for the Tx and Rx side.
  void SetUp() override
  {
    segmenter_tx = segmenter_tx_factory->create();
    ASSERT_NE(segmenter_tx, nullptr);
    segmenter_rx = segmenter_rx_factory->create();
    ASSERT_NE(segmenter_rx, nullptr);
  }

  static std::shared_ptr<crc_calculator_factory>    crc_factory;
  static std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_tx_factory;
  static std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory;

  std::unique_ptr<ldpc_segmenter_tx> segmenter_tx;
  std::unique_ptr<ldpc_segmenter_rx> segmenter_rx;
};

std::shared_ptr<crc_calculator_factory>    LDPCSegmenterFixture::crc_factory          = nullptr;
std::shared_ptr<ldpc_segmenter_tx_factory> LDPCSegmenterFixture::segmenter_tx_factory = nullptr;
std::shared_ptr<ldpc_segmenter_rx_factory> LDPCSegmenterFixture::segmenter_rx_factory = nullptr;

TEST_P(LDPCSegmenterFixture, LDPCSegmenterTest)
{
  LDPCSegmenterParams test_data = GetParam();
  segmenter_config    seg_cfg{};
  seg_cfg.base_graph     = static_cast<ldpc_base_graph_type>(test_data.bg);
  seg_cfg.mod            = modulation_scheme::QPSK;
  seg_cfg.rv             = 0;
  seg_cfg.nof_layers     = 1;
  seg_cfg.nof_ch_symbols = 150;

  const std::vector<uint8_t> trans_block    = test_data.trans_block.read();
  const std::vector<uint8_t> segments_check = test_data.segments.read();

  static_vector<described_segment, MAX_NOF_SEGMENTS> segments;
  segmenter_tx->segment(segments, trans_block, seg_cfg);

  EXPECT_EQ(segments.size(), test_data.nof_segments) << "Wrong number of segments.";

  std::vector<uint8_t> segment_data(test_data.segment_length);

  unsigned seg_offset = 0;
  for (const auto& seg : segments) {
    srsvec::bit_unpack(segment_data, seg.get_data());
    span<uint8_t> filler_bits = span<uint8_t>(segment_data).last(seg.get_metadata().cb_specific.nof_filler_bits);
    std::fill(filler_bits.begin(), filler_bits.end(), FILLER_BIT);

    EXPECT_EQ(span<const uint8_t>(segment_data),
              span<const uint8_t>(segments_check).subspan(seg_offset, test_data.segment_length))
        << "Wrong codeblock content.";
    seg_offset += test_data.segment_length;
  }

  std::vector<log_likelihood_ratio> cw_llrs(segments[0].get_metadata().tb_common.cw_length);
  std::generate(cw_llrs.begin(), cw_llrs.end(), [n = static_cast<int8_t>(-127)]() mutable {
    int8_t r = clamp(n, LLR_MIN.to_value_type(), LLR_MAX.to_value_type());
    ++n;
    return log_likelihood_ratio(r);
  });
  static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblocks;
  segmenter_rx->segment(codeblocks, cw_llrs, test_data.tbs, seg_cfg);

  for (const auto& cb : codeblocks) {
    EXPECT_EQ(cb.first.size(), cb.second.cb_specific.rm_length) << "Wrong codeblock length.";
    EXPECT_TRUE(std::equal(cb.first.begin(), cb.first.end(), cw_llrs.begin() + cb.second.cb_specific.cw_offset))
        << "Wrong codeblock content.";
  }
}

INSTANTIATE_TEST_SUITE_P(LDPCSegmenterSuite, LDPCSegmenterFixture, ::testing::ValuesIn(ldpc_segmenter_test_data));

} // namespace
