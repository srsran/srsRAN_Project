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

#include "ldpc_segmenter_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

/// Length of the CRC checksum added to the segments.
static constexpr units::bits SEG_CRC_LENGTH{24};
/// Maximum accepted transport block size.
/// Note: This value has to be multiple of 8.
static constexpr units::bits MAX_TBS{1277992};
static_assert(MAX_TBS.is_byte_exact(), "Value is not a multiple of 8");

std::unique_ptr<ldpc_segmenter_tx> ldpc_segmenter_impl::create_ldpc_segmenter_impl_tx(ldpc_segmenter_impl::sch_crc& c)
{
  srsran_assert(c.crc16, "Invalid CRC16 calculator.");
  srsran_assert(c.crc24A, "Invalid CRC24A calculator.");
  srsran_assert(c.crc24B, "Invalid CRC24B calculator.");
  srsran_assert(c.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Not a CRC generator of type CRC16.");
  srsran_assert(c.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Not a CRC generator of type CRC24A.");
  srsran_assert(c.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Not a CRC generator of type CRC24B.");

  return std::unique_ptr<ldpc_segmenter_impl>(new ldpc_segmenter_impl(std::move(c)));
}

std::unique_ptr<ldpc_segmenter_rx> ldpc_segmenter_impl::create_ldpc_segmenter_impl_rx()
{
  return std::unique_ptr<ldpc_segmenter_impl>(new ldpc_segmenter_impl());
}

unsigned ldpc_segmenter_impl::compute_rm_length(unsigned i_seg, modulation_scheme mod, unsigned nof_layers) const
{
  unsigned tmp = 0;
  if (i_seg < nof_short_segments) {
    // For unsigned, division then floor is the same as integer division.
    tmp = nof_symbols_per_layer / nof_segments;
  } else {
    tmp = divide_ceil(nof_symbols_per_layer, nof_segments);
  }
  return tmp * nof_layers * get_bits_per_symbol(mod);
}

static void check_inputs_tx(const static_vector<described_segment, MAX_NOF_SEGMENTS>& segments,
                            span<const uint8_t>                                       transport_block,
                            const segmenter_config&                                   cfg)
{
  using namespace units::literals;
  srsran_assert(segments.empty(), "Argument segments should be empty.");
  srsran_assert(!transport_block.empty(), "Argument transport_block should not be empty.");
  srsran_assert(units::bytes(transport_block.size()).to_bits() + 24_bits <= MAX_TBS,
                "Transport block too long. The admissible size, including CRC, is {}.",
                MAX_TBS.truncate_to_bytes());

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

// For the Tx-chain segmenter.
void ldpc_segmenter_impl::segment(static_vector<described_segment, MAX_NOF_SEGMENTS>& described_segments,
                                  span<const uint8_t>                                 transport_block,
                                  const segmenter_config&                             cfg)
{
  check_inputs_tx(described_segments, transport_block, cfg);

  using namespace units::literals;

  base_graph = cfg.base_graph;
  // Each transport_block entry is a byte, and TBS can always be expressed as an integer number of bytes (see, e.g.,
  // TS38.214 Section 5.1.3.2).
  units::bits nof_tb_bits_tmp = units::bytes(transport_block.size()).to_bits();

  constexpr units::bits MAX_BITS_CRC16{3824};
  crc_calculator&       tb_crc          = (nof_tb_bits_tmp <= MAX_BITS_CRC16) ? *crc_set.crc16 : *crc_set.crc24A;
  units::bits           nof_tb_crc_bits = compute_tb_crc_size(nof_tb_bits_tmp);

  nof_tb_bits_in = nof_tb_bits_tmp + nof_tb_crc_bits;

  nof_segments    = ldpc::compute_nof_codeblocks(nof_tb_bits_tmp, base_graph);
  nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += units::bits(nof_segments * SEG_CRC_LENGTH.value());
  }
  lifting_size   = compute_lifting_size(nof_tb_bits_tmp, base_graph, nof_segments);
  segment_length = compute_codeblock_size(base_graph, lifting_size);

  units::bits nof_crc_bits = (nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Compute the number of information bits that is assigned to a segment.
  units::bits cb_info_bits = units::bits(divide_ceil(nof_tb_bits_out.value(), nof_segments)) - nof_crc_bits;

  // Zero-padding if necessary.
  units::bits zero_pad = units::bits((cb_info_bits + nof_crc_bits).value() * nof_segments) - nof_tb_bits_out;

  // Append TB CRC.
  crc_calculator_checksum_t tb_checksum = tb_crc.calculate_byte(transport_block);

  // Number of channel symbols assigned to a transmission layer.
  nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  nof_short_segments = nof_segments - (nof_symbols_per_layer % nof_segments);

  // Codeword length (after concatenation of codeblocks).
  units::bits cw_length(cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod));

  // Number of filler bits in this segment.
  units::bits nof_filler_bits = segment_length - cb_info_bits - nof_crc_bits;

  unsigned cw_offset = 0;
  unsigned tb_offset = 0;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    bool last_cb = (i_segment == nof_segments - 1);

    // Generate metadata for this segment.
    codeblock_metadata cb_metadata =
        generate_cb_metadata({i_segment, cw_length, cw_offset, nof_filler_bits, nof_crc_bits, nof_tb_crc_bits}, cfg);

    // Prepare segment data.
    described_segments.emplace_back(cb_metadata, segment_length);
    described_segment& segment = described_segments.back();

    // Segment bit buffer.
    bit_buffer& cb_bit_buffer = segment.get_data();

    // Number of information bits to get from the transport block. Remove transport block CRC bits if it is the last CB.
    units::bits nof_used_bits = cb_info_bits;
    if (last_cb) {
      nof_used_bits -= nof_tb_crc_bits + zero_pad;
    }

    // Copy information bits from the transport block.
    bit_buffer cb_info_bit_buffer = cb_bit_buffer.first(nof_used_bits.value());
    srsvec::copy_offset(cb_info_bit_buffer, transport_block, tb_offset);
    tb_offset += nof_used_bits.value();

    // If it is the last CB, append the transport block CRC and padding bits.
    if (last_cb) {
      // For each byte of the transport block CRC...
      for (unsigned i_checksum_byte = 0, i_checksum_byte_end = nof_tb_crc_bits.truncate_to_bytes().value();
           i_checksum_byte != i_checksum_byte_end;
           ++i_checksum_byte) {
        // Extract byte from the CRC.
        unsigned tb_crc_byte = (tb_checksum >> (nof_tb_crc_bits.value() - (i_checksum_byte + 1) * 8)) & 0xffUL;
        // Insert the byte at the end of the bit buffer.
        cb_bit_buffer.insert(tb_crc_byte, nof_used_bits.value(), 8);
        // Increment the number of bits.
        nof_used_bits += 8_bits;
      }
      tb_offset += nof_tb_crc_bits.value();

      // Insert zero padding bits.
      for (units::bits nof_used_bits_end = nof_used_bits + zero_pad; nof_used_bits != nof_used_bits_end;) {
        // Calculate the number of zeros to pad, no more than a byte at a time.
        units::bits nof_zeros = std::min<units::bits>(8_bits, nof_used_bits_end - nof_used_bits);
        // Insert the zeros at the end of the bit buffer.
        cb_bit_buffer.insert(0UL, nof_used_bits.value(), nof_zeros.value());
        // Increment the number of bits.
        nof_used_bits += nof_zeros;
      }
    }

    // Append codeblock CRC if required.
    if (nof_crc_bits != 0_bits) {
      crc_calculator_checksum_t cb_checksum = crc_set.crc24B->calculate(cb_bit_buffer.first(nof_used_bits.value()));

      // For each byte of the transport block CRC...
      for (unsigned i_checksum_byte = 0, i_checksum_byte_end = nof_crc_bits.truncate_to_bytes().value();
           i_checksum_byte != i_checksum_byte_end;
           ++i_checksum_byte) {
        // Extract byte from the CRC.
        unsigned cb_crc_byte = (cb_checksum >> (nof_crc_bits.value() - (i_checksum_byte + 1) * 8)) & 0xffUL;
        // Insert the byte at the end of the bit buffer.
        cb_bit_buffer.insert(cb_crc_byte, nof_used_bits.value(), 8);
        // Increment the number of bits.
        nof_used_bits += 8_bits;
      }
    }

    // Append filler bits as zeros.
    while (nof_used_bits != segment_length) {
      // Calculate the number of zeros to pad, no more than a byte at a time.
      units::bits nof_zeros = std::min<units::bits>(8_bits, segment_length - nof_used_bits);
      // Insert the zeros at the end of the bit buffer.
      cb_bit_buffer.insert(0UL, nof_used_bits.value(), nof_zeros.value());
      // Increment the number of bits.
      nof_used_bits += nof_zeros;
    }

    // Advance offsets.
    cw_offset += cb_metadata.cb_specific.rm_length;
  }

  // After segmenting no bits should be left in the buffer.
  srsran_assert(nof_tb_bits_in.value() == tb_offset,
                "Transport block offset ({}) must be equal to the transport block size including CRC ({}).",
                tb_offset,
                nof_tb_bits_in);
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  srsran_assert(cw_length.value() == cw_offset,
                "Codeblock offset ({}) must be equal to the codeword size ({}).",
                cw_offset,
                cw_length.value());
}

static void check_inputs_rx(span<const log_likelihood_ratio> codeword_llrs, const segmenter_config& cfg)
{
  srsran_assert(!codeword_llrs.empty(), "Argument transport_block should not be empty.");
  srsran_assert(codeword_llrs.size() == cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod),
                "Wrong number of LLRs {} (expected {}).",
                codeword_llrs.size(),
                cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod));

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

// For the Rx-chain segmenter.
void ldpc_segmenter_impl::segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                                  span<const log_likelihood_ratio>                         codeword_llrs,
                                  unsigned                                                 tbs,
                                  const segmenter_config&                                  cfg)
{
  check_inputs_rx(codeword_llrs, cfg);

  using namespace units::literals;

  base_graph = cfg.base_graph;

  units::bits tbs_bits(tbs);
  units::bits nof_tb_crc_bits = compute_tb_crc_size(tbs_bits);

  nof_tb_bits_in = tbs_bits + nof_tb_crc_bits;

  nof_segments    = ldpc::compute_nof_codeblocks(tbs_bits, base_graph);
  nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += units::bits(nof_segments * SEG_CRC_LENGTH.value());
  }
  lifting_size   = compute_lifting_size(tbs_bits, base_graph, nof_segments);
  segment_length = compute_codeblock_size(base_graph, lifting_size);

  units::bits nof_crc_bits = (nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Compute the maximum number of information bits that can be assigned to a segment.
  units::bits max_info_bits = units::bits(divide_ceil(nof_tb_bits_out.value(), nof_segments)) - nof_crc_bits;

  // Number of channel symbols assigned to a transmission layer.
  nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  nof_short_segments = nof_segments - (nof_symbols_per_layer % nof_segments);

  // Codeword length (after concatenation of codeblocks).
  units::bits cw_length(codeword_llrs.size());

  unsigned cw_offset = 0;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    units::bits nof_filler_bits = segment_length - (max_info_bits + nof_crc_bits);

    codeblock_metadata tmp_description =
        generate_cb_metadata({i_segment, cw_length, cw_offset, nof_filler_bits, nof_crc_bits, nof_tb_crc_bits}, cfg);

    unsigned rm_length = tmp_description.cb_specific.rm_length;
    described_codeblocks.push_back({codeword_llrs.subspan(cw_offset, rm_length), tmp_description});
    cw_offset += rm_length;
  }
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  assert(cw_length.value() == cw_offset);
}

codeblock_metadata ldpc_segmenter_impl::generate_cb_metadata(const segment_internal& seg_extra,
                                                             const segmenter_config& cfg) const
{
  codeblock_metadata tmp_description = {};

  tmp_description.tb_common.base_graph   = base_graph;
  tmp_description.tb_common.lifting_size = static_cast<lifting_size_t>(lifting_size);
  tmp_description.tb_common.rv           = cfg.rv;
  tmp_description.tb_common.mod          = cfg.mod;
  tmp_description.tb_common.Nref         = cfg.Nref;
  tmp_description.tb_common.cw_length    = seg_extra.cw_length.value();

  unsigned rm_length = compute_rm_length(seg_extra.i_segment, cfg.mod, cfg.nof_layers);

  tmp_description.cb_specific.full_length     = compute_full_codeblock_size(base_graph, segment_length).value();
  tmp_description.cb_specific.nof_filler_bits = seg_extra.nof_filler_bits.value();
  tmp_description.cb_specific.rm_length       = rm_length;
  tmp_description.cb_specific.cw_offset       = seg_extra.cw_offset;
  // nof_crc_bits == 0 indicates that we are using the TB CRC with length 16.
  tmp_description.cb_specific.nof_crc_bits =
      (nof_segments == 1) ? seg_extra.nof_tb_crc_bits.value() : seg_extra.nof_crc_bits.value();

  return tmp_description;
}
