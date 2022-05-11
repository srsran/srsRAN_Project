/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Unit test for LDPC segmenter.
///
/// The test carries out the segmentation of a number of transport blocks of different lengths, for both possible base
/// graphs. Test vectors provide the transport blocks and the resulting segmented blocks for comparison.

#include "ldpc_segmenter_test_data.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter.h"
#include "srsgnb/support/srsgnb_test.h"
#include <iostream>

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
  std::unique_ptr<ldpc_segmenter> segmenter = create_ldpc_segmenter();

  segment_config seg_cfg{};

  for (const auto& test_data : ldpc_segmenter_test_data) {
    seg_cfg.base_graph     = static_cast<base_graph_t>(test_data.bg - 1);
    seg_cfg.mod            = modulation_scheme::QPSK;
    seg_cfg.rv             = 0;
    seg_cfg.nof_layers     = 1;
    seg_cfg.nof_ch_symbols = 150;

    const std::vector<uint8_t> trans_block    = test_data.trans_block.read();
    const std::vector<uint8_t> segments_check = test_data.segments.read();

    static_vector<described_segment, MAX_NOF_SEGMENTS> segments;
    segmenter->segment(segments, trans_block, seg_cfg);

    TESTASSERT_EQ(segments.size(), test_data.nof_segments, "Wrong number of segments.");

    unsigned seg_offset = 0;
    for (const auto& seg : segments) {
      TESTASSERT_EQ(seg.first.size(), test_data.segment_length, "Wrong segment length.");
      TESTASSERT(std::equal(seg.first.begin(), seg.first.end(), segments_check.cbegin() + seg_offset),
                 "Wrong segment content.");
      seg_offset += test_data.segment_length;
    }
  }
}