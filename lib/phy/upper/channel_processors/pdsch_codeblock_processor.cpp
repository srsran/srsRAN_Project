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

#include "pdsch_codeblock_processor.h"

using namespace srsran;

void pdsch_codeblock_processor::process(span<uint8_t> output, const described_segment& descr_seg)
{
  // Resize internal buffer to match data from the segmenter to the encoder (all segments have the same length).
  span<uint8_t> tmp_data = span<uint8_t>(temp_unpacked_cb).first(descr_seg.get_data().size());

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  span<uint8_t> tmp_encoded = span<uint8_t>(buffer_cb).first(descr_seg.get_metadata().cb_specific.full_length);

  // Unpack segment.
  srsvec::bit_unpack(tmp_data, descr_seg.get_data());

  // Set filler bits.
  span<uint8_t> filler_bits = tmp_data.last(descr_seg.get_metadata().cb_specific.nof_filler_bits);
  std::fill(filler_bits.begin(), filler_bits.end(), ldpc::FILLER_BIT);

  // Encode the segment into a codeblock.
  encoder->encode(tmp_encoded, tmp_data, descr_seg.get_metadata().tb_common);

  // Rate match the codeblock.
  rate_matcher->rate_match(output, tmp_encoded, descr_seg.get_metadata());
}
