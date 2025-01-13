/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

pdsch_codeblock_processor::result pdsch_codeblock_processor::process(span<const uint8_t>          data,
                                                                     const ldpc_segmenter_buffer& segment_buffer,
                                                                     const configuration&         config)
{
  using namespace units::literals;

  // Initialize scrambling with the initial state.
  scrambler->init(config.c_init);

  // Advance scrambling sequence to the specific codeword offset.
  scrambler->advance(config.metadata.cb_specific.cw_offset);

  // Prepare codeblock data.
  cb_data.resize(config.cb_size.value());

  // Calculate transport block size.
  units::bits tbs = units::bytes(data.size()).to_bits();

  // Verify range of the CB information bits.
  srsran_assert(
      (config.tb_offset + config.cb_info_size) <= tbs,
      "TB offset (i.e., {}) and number of information bits (i.e., {}) exceeds the transport block size (i.e., {})",
      config.tb_offset,
      config.cb_info_size,
      tbs);

  // Rate Matching output length.
  unsigned rm_length = segment_buffer.get_rm_length(config.cb_index);

  // Extract modulation.
  modulation_scheme modulation = config.metadata.tb_common.mod;

  // Number of bits per symbol.
  unsigned bits_per_symbol = get_bits_per_symbol(modulation);
  srsran_assert(bits_per_symbol >= 1, "Number of bits per resource element must be greater than or equal to 1.");

  // Copy codeblock data, including TB and/or CB CRC if applicable, as well as filler and zero padding bits.
  segment_buffer.read_codeblock(cb_data, data, config.cb_index);

  // Encode the segment into a codeblock.
  const ldpc_encoder_buffer& rm_buffer = encoder->encode(cb_data, config.metadata.tb_common);

  // Rate match the codeblock.
  temp_packed_bits.resize(rm_length);
  rate_matcher->rate_match(temp_packed_bits, rm_buffer, config.metadata);

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_packed_bits, temp_packed_bits);

  span<ci8_t> cb_symbols = span<ci8_t>(temp_cb_symbols).first(rm_length / bits_per_symbol);

  // Modulate.
  modulator->modulate(cb_symbols, temp_packed_bits, modulation);

  return {scrambler->get_state(), cb_symbols};
}
