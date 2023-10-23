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

pseudo_random_generator::state_s
pdsch_codeblock_processor::process(span<ci8_t> buffer, span<const uint8_t> data, const configuration& config)
{
  using namespace units::literals;

  // Initialize scrambling with the initial state.
  scrambler->init(config.c_init);

  // Prepare codeblock data.
  units::bits nof_used_bits = 0_bits;
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

  // Copy codeblock data.
  {
    bit_buffer message = cb_data.first(config.cb_info_size.value());
    srsvec::copy_offset(message, data, config.tb_offset.value());
    nof_used_bits += units::bits(config.cb_info_size);
  }

  // Append transport block CRC if applicable.
  if ((config.tb_offset + config.cb_info_size) == tbs) {
    constexpr units::bits MAX_BITS_CRC16{3824};
    crc_calculator&       tb_crc          = (tbs <= MAX_BITS_CRC16) ? *crc16 : *crc24a;
    units::bits           nof_tb_crc_bits = units::bits(get_crc_size(tb_crc.get_generator_poly()));

    crc_calculator_checksum_t tb_checksum = tb_crc.calculate_byte(data);
    for (unsigned i_checksum_byte = 0, i_checksum_byte_end = nof_tb_crc_bits.truncate_to_bytes().value();
         i_checksum_byte != i_checksum_byte_end;
         ++i_checksum_byte) {
      // Extract byte from the CRC.
      unsigned tb_crc_byte = (tb_checksum >> (nof_tb_crc_bits.value() - (i_checksum_byte + 1) * 8)) & 0xffUL;
      // Insert the byte at the end of the bit buffer.
      cb_data.insert(tb_crc_byte, nof_used_bits.value(), 8);
      // Increment the number of bits.
      nof_used_bits += 8_bits;
    }

    // Insert zero padding bits.
    for (units::bits nof_used_bits_end = nof_used_bits + config.zero_pad; nof_used_bits != nof_used_bits_end;) {
      // Calculate the number of zeros to pad, no more than a byte at a time.
      units::bits nof_zeros = std::min<units::bits>(8_bits, nof_used_bits_end - nof_used_bits);
      // Insert the zeros at the end of the bit buffer.
      cb_data.insert(0UL, nof_used_bits.value(), nof_zeros.value());
      // Increment the number of bits.
      nof_used_bits += nof_zeros;
    }
  }

  // Append codeblock CRC if applicable.
  if (config.has_cb_crc) {
    crc_calculator& cb_crc = *crc24b;

    crc_calculator_checksum_t cb_checksum = cb_crc.calculate(cb_data.first(nof_used_bits.value()));
    for (unsigned i_checksum_byte = 0, i_checksum_byte_end = 3; i_checksum_byte != i_checksum_byte_end;
         ++i_checksum_byte) {
      // Extract byte from the CRC.
      unsigned cb_crc_byte = (cb_checksum >> (24 - (i_checksum_byte + 1) * 8)) & 0xffUL;
      // Insert the byte at the end of the bit buffer.
      cb_data.insert(cb_crc_byte, nof_used_bits.value(), 8);
      // Increment the number of bits.
      nof_used_bits += 8_bits;
    }
  }

  // Append filler bits as zeros.
  while (nof_used_bits != config.cb_size) {
    // Calculate the number of zeros to pad, no more than a byte at a time.
    units::bits nof_zeros = std::min<units::bits>(8_bits, units::bits(config.cb_size) - nof_used_bits);
    // Insert the zeros at the end of the bit buffer.
    cb_data.insert(0UL, nof_used_bits.value(), nof_zeros.value());
    // Increment the number of bits.
    nof_used_bits += nof_zeros;
  }

  // Rate Matching output length.
  unsigned rm_length = config.metadata.cb_specific.rm_length;

  // CB bit position within the codeword.
  unsigned cw_offset_bit = config.metadata.cb_specific.cw_offset;

  // Extract modulation.
  modulation_scheme modulation = config.metadata.tb_common.mod;

  // Number of bits per symbol.
  unsigned bits_per_symbol = get_bits_per_symbol(modulation);
  srsran_assert(bits_per_symbol >= 1, "Number of bits per resource element must be greater than or equal to 1.");

  // CB symbol position within the codeword.
  unsigned cw_offset_symbol = cw_offset_bit / bits_per_symbol;

  // Number of modulated symbols.
  unsigned rm_length_symbol = rm_length / bits_per_symbol;

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  rm_buffer.resize(config.metadata.cb_specific.full_length);

  // Encode the segment into a codeblock.
  encoder->encode(rm_buffer, cb_data, config.metadata.tb_common);

  // Rate match the codeblock.
  temp_packed_bits.resize(rm_length);
  rate_matcher->rate_match(temp_packed_bits, rm_buffer, config.metadata);

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_packed_bits, temp_packed_bits);

  // Modulate.
  modulator->modulate(buffer.subspan(cw_offset_symbol, rm_length_symbol), temp_packed_bits, modulation);

  return scrambler->get_state();
}
