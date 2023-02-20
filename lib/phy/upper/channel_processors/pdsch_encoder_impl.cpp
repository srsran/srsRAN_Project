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

  // Resize internal buffer to match data from the segmenter to the encoder (all segments have the same length).
  span<uint8_t> tmp_data = span<uint8_t>(temp_unpacked_cb).first(d_segments[0].get_data().size());

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  span<uint8_t> tmp_encoded = span<uint8_t>(buffer_cb).first(d_segments[0].get_metadata().cb_specific.full_length);

  unsigned offset = 0;
  for (const described_segment& descr_seg : d_segments) {
    // Unpack segment.
    srsvec::bit_unpack(tmp_data, descr_seg.get_data());

    // Set filler bits.
    span<uint8_t> filler_bits = tmp_data.last(descr_seg.get_metadata().cb_specific.nof_filler_bits);
    std::fill(filler_bits.begin(), filler_bits.end(), ldpc::FILLER_BIT);

    // Encode the segment into a codeblock.
    encoder->encode(tmp_encoded, tmp_data, descr_seg.get_metadata().tb_common);

    // Select the correct chunk of the output codeword.
    unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;
    srsgnb_assert(offset + rm_length <= codeword.size(), "Wrong codeword length.");

    span<uint8_t> codeblock = span<uint8_t>(codeword).subspan(offset, rm_length);
    offset += rm_length;

    // Rate match the codeblock.
    rate_matcher->rate_match(codeblock, tmp_encoded, descr_seg.get_metadata().tb_common);
  }
}
