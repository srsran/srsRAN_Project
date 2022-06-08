/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_encoder_impl.h"

using namespace srsgnb;

void pdsch_encoder_impl::encode(span<uint8_t>           codeword,
                                span<const uint8_t>     transport_block,
                                const segmenter_config& cfg)
{
  // Clear the buffer.
  d_segments.clear();
  // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
  segmenter->segment(d_segments, transport_block, cfg);

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  span<uint8_t> tmp = span<uint8_t>(buffer_cb).first(d_segments[0].second.cb_specific.full_length);

  unsigned offset = 0;
  for (const auto& descr_seg : d_segments) {
    // Encode the segment into a codeblock.
    encoder->encode(tmp, descr_seg.first, descr_seg.second.tb_common);
    // Select the correct chunk of the output codeword.
    unsigned rm_length = descr_seg.second.cb_specific.rm_length;
    srsran_assert(offset + rm_length <= codeword.size(), "Wrong codeword length.");
    span<uint8_t> codeblock = span<uint8_t>(codeword).subspan(offset, rm_length);
    offset += rm_length;
    // Rate match the codeblock.
    rate_matcher->rate_match(codeblock, tmp, descr_seg.second.tb_common);
  }
}
