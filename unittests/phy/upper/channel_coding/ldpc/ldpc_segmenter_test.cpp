/*
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
#include <numeric>

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
  std::unique_ptr<ldpc_segmenter_tx> segmenter_tx = create_ldpc_segmenter_tx();
  std::unique_ptr<ldpc_segmenter_rx> segmenter_rx = create_ldpc_segmenter_rx();

  segmenter_config seg_cfg{};

  for (const auto& test_data : ldpc_segmenter_test_data) {
    seg_cfg.base_graph     = static_cast<base_graph_t>(test_data.bg - 1);
    seg_cfg.mod            = modulation_scheme::QPSK;
    seg_cfg.rv             = 0;
    seg_cfg.nof_layers     = 1;
    seg_cfg.nof_ch_symbols = 150;

    const std::vector<uint8_t> trans_block    = test_data.trans_block.read();
    const std::vector<uint8_t> segments_check = test_data.segments.read();

    static_vector<described_segment, MAX_NOF_SEGMENTS> segments;
    segmenter_tx->segment(segments, trans_block, seg_cfg);

    TESTASSERT_EQ(segments.size(), test_data.nof_segments, "Wrong number of segments.");

    unsigned seg_offset = 0;
    for (const auto& seg : segments) {
      TESTASSERT_EQ(seg.first.size(), test_data.segment_length, "Wrong segment length.");
      TESTASSERT(std::equal(seg.first.begin(), seg.first.end(), segments_check.cbegin() + seg_offset),
                 "Wrong segment content.");
      seg_offset += test_data.segment_length;
    }

    std::vector<int8_t> cw_llrs(segments[0].second.tb_common.cw_length);
    std::iota(cw_llrs.begin(), cw_llrs.end(), -127);
    static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS> codeblocks;
    segmenter_rx->segment(codeblocks, cw_llrs, test_data.tbs, seg_cfg);

    for (const auto& cb : codeblocks) {
      TESTASSERT_EQ(cb.first.size(), cb.second.cb_specific.rm_length, "Wrong codeblock length.");
      TESTASSERT(std::equal(cb.first.begin(), cb.first.end(), cw_llrs.begin() + cb.second.cb_specific.cw_offset),
                 "Wrong codeblock content.");
    }
  }
}
