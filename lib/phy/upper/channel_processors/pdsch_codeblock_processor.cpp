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

pseudo_random_generator::state_s pdsch_codeblock_processor::process(re_buffer_writer&                buffer,
                                                                    const described_segment&         descr_seg,
                                                                    pseudo_random_generator::state_s c_init,
                                                                    unsigned                         nof_layers)
{
  // Initialize scrambling with the initial state.
  scrambler->init(c_init);

  // CB payload number of bits.
  unsigned cb_length = descr_seg.get_data().size();

  // Rate Matching output length.
  unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;

  // CB bit position within the codeword.
  unsigned cw_offset_bit = descr_seg.get_metadata().cb_specific.cw_offset;

  // Extract modulation.
  modulation_scheme modulation = descr_seg.get_metadata().tb_common.mod;

  // Number of bits per RE.
  unsigned nof_bits_per_re = nof_layers * get_bits_per_symbol(modulation);
  srsran_assert(nof_bits_per_re >= 1, "Number of bits per resource element must be greater than or equal to 1.");

  // CB RE position within the codeword.
  unsigned cw_offset_re = cw_offset_bit / nof_bits_per_re;

  // CB size in RE.
  unsigned cb_size_re = rm_length / nof_bits_per_re;

  // Resize internal buffer to match data from the segmenter to the encoder (all segments have the same length).
  span<uint8_t> tmp_data = span<uint8_t>(temp_unpacked_cb).first(cb_length);

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
  span<uint8_t> unpacked_bits = span<uint8_t>(temp_unpacked_bits).first(rm_length);
  rate_matcher->rate_match(unpacked_bits, tmp_encoded, descr_seg.get_metadata());

  // Pack bits.
  temp_packed_bits.resize(rm_length);
  srsvec::bit_pack(temp_packed_bits, unpacked_bits);

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_packed_bits, temp_packed_bits);

  // Modulate.
  span<cf_t> modulated = span<cf_t>(temp_modulated).first(cb_size_re * nof_layers);
  if (nof_layers == 1) {
    modulated = buffer.get_slice(0).subspan(cw_offset_re, cb_size_re * nof_layers);
  }
  modulator->modulate(modulated, temp_packed_bits, modulation);

  // Layer mapping.
  if (nof_layers > 1) {
    // Apply TS38.211 Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing.
    for (unsigned layer = 0; layer != nof_layers; ++layer) {
      span<cf_t> re_layer = buffer.get_slice(layer);
      for (unsigned i = 0, i_end = cb_size_re; i != i_end; ++i) {
        re_layer[i + cw_offset_re] = modulated[nof_layers * i + layer];
      }
    }
  }

  return scrambler->get_state();
}
