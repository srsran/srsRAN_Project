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

#include "ldpc_segmenter_tx_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

/// Length of the CRC checksum added to the segments.
static constexpr units::bits SEG_CRC_LENGTH{24};
/// Maximum accepted transport block size.
/// Note: This value has to be multiple of 8.
static constexpr units::bits MAX_TBS{1277992};
static_assert(MAX_TBS.is_byte_exact(), "Value is not a multiple of 8");

static void check_inputs_tx(span<const uint8_t> transport_block, const segmenter_config& cfg)
{
  using namespace units::literals;
  srsran_assert(!transport_block.empty(), "Argument transport_block should not be empty.");
  srsran_assert(units::bytes(transport_block.size()).to_bits() + 24_bits <= MAX_TBS,
                "Transport block too long. The admissible size, including CRC, is {}.",
                MAX_TBS.truncate_to_bytes());

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

// For the Tx-chain segmenter without using an intermediate buffer.
ldpc_segmenter_buffer& ldpc_segmenter_tx_impl::new_transmission(span<const uint8_t>     transport_block,
                                                                const segmenter_config& cfg)
{
  check_inputs_tx(transport_block, cfg);

  using namespace units::literals;

  params.base_graph = cfg.base_graph;
  // Each transport_block entry is a byte, and TBS can always be expressed as an integer number of bytes (see, e.g.,
  // TS38.214 Section 5.1.3.2).
  units::bits nof_tb_bits_tmp = units::bytes(transport_block.size()).to_bits();

  params.nof_tb_crc_bits = compute_tb_crc_size(nof_tb_bits_tmp);

  params.nof_tb_bits_in = nof_tb_bits_tmp + params.nof_tb_crc_bits;

  params.nof_segments    = ldpc::compute_nof_codeblocks(nof_tb_bits_tmp, params.base_graph);
  params.nof_tb_bits_out = params.nof_tb_bits_in;
  if (params.nof_segments > 1) {
    params.nof_tb_bits_out += units::bits(params.nof_segments * SEG_CRC_LENGTH.value());
  }
  params.lifting_size   = compute_lifting_size(nof_tb_bits_tmp, params.base_graph, params.nof_segments);
  params.segment_length = compute_codeblock_size(params.base_graph, params.lifting_size);

  params.nof_crc_bits = (params.nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Compute the number of information bits that is assigned to a segment.
  params.cb_info_bits =
      units::bits(divide_ceil(params.nof_tb_bits_out.value(), params.nof_segments)) - params.nof_crc_bits;

  // Zero-padding if necessary.
  params.zero_pad =
      units::bits((params.cb_info_bits + params.nof_crc_bits).value() * params.nof_segments) - params.nof_tb_bits_out;

  // Number of channel symbols assigned to a transmission layer.
  params.nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  params.nof_short_segments = params.nof_segments - (params.nof_symbols_per_layer % params.nof_segments);

  // Codeword length (after concatenation of codeblocks).
  params.cw_length = static_cast<units::bits>(cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod));

  // Number of filler bits in this segment.
  params.nof_filler_bits = params.segment_length - params.cb_info_bits - params.nof_crc_bits;

  unsigned cw_offset = 0;
  unsigned tb_offset = 0;
  for (unsigned i_segment = 0; i_segment != params.nof_segments; ++i_segment) {
    bool last_cb = (i_segment == params.nof_segments - 1);

    // Save codeblock offsets.
    params.cw_offset[i_segment] = cw_offset;
    params.tb_offset[i_segment] = tb_offset;

    // Generate metadata for this segment.
    params.cb_metadata[i_segment] = generate_cb_metadata(params, cfg, i_segment);

    // Number of information bits to get from the transport block. Remove transport block CRC bits if it is the last CB.
    units::bits nof_used_bits = params.cb_info_bits;
    if (last_cb) {
      nof_used_bits -= params.nof_tb_crc_bits + params.zero_pad;
      params.cb_info_bits_last = nof_used_bits;
      tb_offset += params.nof_tb_crc_bits.value();
    }

    // Advance codeblock offsets.
    tb_offset += nof_used_bits.value();
    cw_offset += this->get_rm_length(i_segment);
  }

  // After segmenting no bits should be left in the buffer.
  srsran_assert(params.nof_tb_bits_in.value() == tb_offset,
                "Transport block offset ({}) must be equal to the transport block size including CRC ({}).",
                tb_offset,
                params.nof_tb_bits_in);
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  srsran_assert(params.cw_length.value() == cw_offset,
                "Codeblock offset ({}) must be equal to the codeword size ({}).",
                cw_offset,
                params.cw_length.value());

  return *this;
}

void ldpc_segmenter_tx_impl::read_codeblock(bit_buffer          codeblock,
                                            span<const uint8_t> transport_block,
                                            unsigned            cb_index) const
{
  srsran_assert(codeblock.size() == params.segment_length.value(),
                "Invalid codeblock size (i.e., {}), expected {}.",
                codeblock.size(),
                params.segment_length.value());
  srsran_assert(cb_index < params.nof_segments,
                "Codeblock index ({}) must be lower than the number of segments ({}).",
                cb_index,
                params.nof_segments);

  using namespace units::literals;

  units::bits nof_used_bits    = 0_bits;
  unsigned    nof_segment_bits = this->get_cb_info_bits(cb_index).value();

  // Copy codeblock data.
  {
    bit_buffer message = codeblock.first(nof_segment_bits);
    srsvec::copy_offset(message, transport_block, params.tb_offset[cb_index]);
    nof_used_bits += units::bits(nof_segment_bits);
  }

  // Append transport block CRC if applicable.
  if ((params.tb_offset[cb_index] + nof_segment_bits) == units::bytes(transport_block.size()).to_bits().value()) {
    // Append TB CRC.
    constexpr units::bits MAX_BITS_CRC16{3824};
    crc_calculator&       tb_crc =
        (units::bytes(transport_block.size()).to_bits() <= MAX_BITS_CRC16) ? *crc_set.crc16 : *crc_set.crc24A;
    crc_calculator_checksum_t tb_checksum = tb_crc.calculate_byte(transport_block);

    for (unsigned i_checksum_byte = 0, i_checksum_byte_end = params.nof_tb_crc_bits.truncate_to_bytes().value();
         i_checksum_byte != i_checksum_byte_end;
         ++i_checksum_byte) {
      // Extract byte from the CRC.
      unsigned tb_crc_byte = (tb_checksum >> (params.nof_tb_crc_bits.value() - (i_checksum_byte + 1) * 8)) & 0xffUL;
      // Insert the byte at the end of the bit buffer.
      codeblock.insert(tb_crc_byte, nof_used_bits.value(), 8);
      // Increment the number of bits.
      nof_used_bits += 8_bits;
    }

    // Insert zero padding bits.
    for (units::bits nof_used_bits_end = nof_used_bits + params.zero_pad; nof_used_bits != nof_used_bits_end;) {
      // Calculate the number of zeros to pad, no more than a byte at a time.
      units::bits nof_zeros = std::min<units::bits>(8_bits, nof_used_bits_end - nof_used_bits);
      // Insert the zeros at the end of the bit buffer.
      codeblock.insert(0UL, nof_used_bits.value(), nof_zeros.value());
      // Increment the number of bits.
      nof_used_bits += nof_zeros;
    }
  }

  // Append codeblock CRC if applicable.
  if (params.nof_crc_bits != 0_bits) {
    crc_calculator_checksum_t cb_checksum = crc_set.crc24B->calculate(codeblock.first(nof_used_bits.value()));
    for (unsigned i_checksum_byte = 0, i_checksum_byte_end = 3; i_checksum_byte != i_checksum_byte_end;
         ++i_checksum_byte) {
      // Extract byte from the CRC.
      unsigned cb_crc_byte = (cb_checksum >> (24 - (i_checksum_byte + 1) * 8)) & 0xffUL;
      // Insert the byte at the end of the bit buffer.
      codeblock.insert(cb_crc_byte, nof_used_bits.value(), 8);
      // Increment the number of bits.
      nof_used_bits += 8_bits;
    }
  }

  // Append filler bits as zeros.
  while (nof_used_bits != params.segment_length) {
    // Calculate the number of zeros to pad, no more than a byte at a time.
    units::bits nof_zeros = std::min<units::bits>(8_bits, units::bits(params.segment_length) - nof_used_bits);
    // Insert the zeros at the end of the bit buffer.
    codeblock.insert(0UL, nof_used_bits.value(), nof_zeros.value());
    // Increment the number of bits.
    nof_used_bits += nof_zeros;
  }
}

codeblock_metadata ldpc_segmenter_tx_impl::get_cb_metadata(unsigned cb_index) const
{
  srsran_assert(cb_index < params.nof_segments,
                "Codeblock index ({}) must be lower than the number of segments ({}).",
                cb_index,
                params.nof_segments);

  return params.cb_metadata[cb_index];
}

units::bits ldpc_segmenter_tx_impl::get_cb_info_bits(unsigned cb_index) const
{
  srsran_assert(cb_index < params.nof_segments,
                "Codeblock index ({}) must be lower than the number of segments ({}).",
                cb_index,
                params.nof_segments);

  bool last_cb = (cb_index == params.nof_segments - 1);

  return last_cb ? params.cb_info_bits_last : params.cb_info_bits;
}

unsigned ldpc_segmenter_tx_impl::get_rm_length(unsigned cb_index) const
{
  srsran_assert(cb_index < params.nof_segments,
                "Codeblock index ({}) must be lower than the number of segments ({}).",
                cb_index,
                params.nof_segments);

  return params.cb_metadata[cb_index].cb_specific.rm_length;
}
