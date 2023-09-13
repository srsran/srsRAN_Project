/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_codeblock_processor.h"

using namespace srsran;

pseudo_random_generator::state_s pdsch_codeblock_processor::process(span<ci8_t>                      buffer,
                                                                    const described_segment&         descr_seg,
                                                                    pseudo_random_generator::state_s c_init)
{
  // Initialize scrambling with the initial state.
  scrambler->init(c_init);

  // Rate Matching output length.
  unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;

  // CB bit position within the codeword.
  unsigned cw_offset_bit = descr_seg.get_metadata().cb_specific.cw_offset;

  // Extract modulation.
  modulation_scheme modulation = descr_seg.get_metadata().tb_common.mod;

  // Number of bits per symbol.
  unsigned bits_per_symbol = get_bits_per_symbol(modulation);
  srsran_assert(bits_per_symbol >= 1, "Number of bits per resource element must be greater than or equal to 1.");

  // CB symbol position within the codeword.
  unsigned cw_offset_symbol = cw_offset_bit / bits_per_symbol;

  // Number of modulated symbols.
  unsigned rm_length_symbol = rm_length / bits_per_symbol;

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  rm_buffer.resize(descr_seg.get_metadata().cb_specific.full_length);

  // Encode the segment into a codeblock.
  encoder->encode(rm_buffer, descr_seg.get_data(), descr_seg.get_metadata().tb_common);

  // Rate match the codeblock.
  temp_packed_bits.resize(rm_length);
  rate_matcher->rate_match(temp_packed_bits, rm_buffer, descr_seg.get_metadata());

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_packed_bits, temp_packed_bits);

  // Modulate.
  modulator->modulate(buffer.subspan(cw_offset_symbol, rm_length_symbol), temp_packed_bits, modulation);

  return scrambler->get_state();
}
