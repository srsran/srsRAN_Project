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

#include "pdsch_encoder_impl.h"
#include "srsran/srsvec/bit.h"

using namespace srsran;

void pdsch_encoder_impl::encode(span<uint8_t>           codeword,
                                span<const uint8_t>     transport_block,
                                const segmenter_config& cfg)
{
  // Clear the buffer.
  d_segments.clear();
  // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
  segmenter->segment(d_segments, transport_block, cfg);

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  rm_buffer.resize(d_segments[0].get_metadata().cb_specific.full_length);

  unsigned offset = 0;
  for (const described_segment& descr_seg : d_segments) {
    // Encode the segment into a codeblock.
    encoder->encode(rm_buffer, descr_seg.get_data(), descr_seg.get_metadata().tb_common);

    // Select the correct chunk of the output codeword.
    unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;
    srsran_assert(offset + rm_length <= codeword.size(), "Wrong codeword length.");
    span<uint8_t> codeblock = span<uint8_t>(codeword).subspan(offset, rm_length);

    // Rate match the codeblock.
    codeblock_packed.resize(rm_length);
    rate_matcher->rate_match(codeblock_packed, rm_buffer, descr_seg.get_metadata());

    // Unpack code block.
    srsvec::bit_unpack(codeblock, codeblock_packed);

    // Advance write code block.
    offset += rm_length;
  }
}
