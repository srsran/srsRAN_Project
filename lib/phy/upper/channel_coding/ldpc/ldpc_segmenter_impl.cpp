/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_segmenter_impl.h"
#include "../crc_calculator_impl.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/support/math_utils.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

// Length of the CRC checksum added to the segments.
static constexpr unsigned SEG_CRC_LENGTH = 24;
// Number of bits in one byte.
static constexpr unsigned BITS_PER_BYTE = 8;

ldpc_segmenter_impl::ldpc_segmenter_impl(ldpc_segmenter_impl::sch_crc& c)
{
  srsran_assert(c.crc16, "Invalid CRC16 calculator.");
  srsran_assert(c.crc24A, "Invalid CRC24A calculator.");
  srsran_assert(c.crc24B, "Invalid CRC24B calculator.");
  srsran_assert(c.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Not a CRC generator of type CRC16.");
  srsran_assert(c.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Not a CRC generator of type CRC24A.");
  srsran_assert(c.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Not a CRC generator of type CRC24B.");

  // Transfer CRC calculators' ownership.
  crc_set.crc16  = std::move(c.crc16);
  crc_set.crc24A = std::move(c.crc24A);
  crc_set.crc24B = std::move(c.crc24B);
}

void ldpc_segmenter_impl::compute_lifting_size()
{
  unsigned ref_length = 22;
  if (base_graph == base_graph_t::BG2) {
    if (nof_tb_bits_in > 640) {
      ref_length = 10;
    } else if (nof_tb_bits_in > 560) {
      ref_length = 9;
    } else if (nof_tb_bits_in > 192) {
      ref_length = 8;
    } else {
      ref_length = 6;
    }
  }

  unsigned total_ref_length = nof_segments * ref_length;

  lifting_size = 0;
  for (auto ls : all_lifting_sizes) {
    if (ls * total_ref_length >= nof_tb_bits_out) {
      lifting_size = ls;
      break;
    }
  }
  assert(lifting_size != 0);
}

void ldpc_segmenter_impl::compute_segment_length()
{
  constexpr unsigned base_length_BG1 = BG1_N_FULL - BG1_M;
  constexpr unsigned base_length_BG2 = BG2_N_FULL - BG2_M;
  unsigned           base_length     = (base_graph == base_graph_t::BG1) ? base_length_BG1 : base_length_BG2;

  segment_length = base_length * lifting_size;
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

static void fill_segment(span<uint8_t>                    segment,
                         span<const uint8_t>              tr_block,
                         std::unique_ptr<crc_calculator>& crc,
                         unsigned                         nof_crc_bits,
                         unsigned                         nof_filler)
{
  assert(segment.size() == tr_block.size() + nof_crc_bits + nof_filler);

  // First, copy information bits from the transport block.
  std::copy(tr_block.begin(), tr_block.end(), segment.begin());

  unsigned nof_used_bits = tr_block.size();
  // If needed, compute the CRC and append it to the information bits.
  if (nof_crc_bits > 0) {
    unsigned tmp_crc = crc->calculate_bit(tr_block);
    srsvec::bit_unpack(segment.subspan(nof_used_bits, nof_crc_bits), tmp_crc, nof_crc_bits);
    nof_used_bits += nof_crc_bits;
  }

  // If needed, fill the segment tail with filler bits.
  std::fill_n(segment.begin() + nof_used_bits, nof_filler, FILLER_BIT);
}

static void check_inputs_tx(const static_vector<described_segment, MAX_NOF_SEGMENTS>& segments,
                            span<const uint8_t>                                       transport_block,
                            const segmenter_config&                                   cfg)
{
  srsran_assert(segments.empty(), "Argument segments should be empty.");
  srsran_assert(!transport_block.empty(), "Argument transport_block should not be empty.");
  srsran_assert(transport_block.size() * 8 + 24 <= MAX_TBS,
                "Transport block too long. The admissible size, including CRC, is %d bytes.",
                MAX_TBS / BITS_PER_BYTE);

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

  base_graph = cfg.base_graph;
  // Each transport_block entry is a byte, and TBS can always be expressed as an integer number of bytes (see, e.g.,
  // TS38.214 Section 5.1.3.2).
  unsigned nof_tb_bits_tmp = transport_block.size() * BITS_PER_BYTE;

  constexpr unsigned MAX_BITS_CRC16  = 3824;
  crc_calculator&    tb_crc          = (nof_tb_bits_tmp <= MAX_BITS_CRC16) ? *crc_set.crc16 : *crc_set.crc24A;
  unsigned           nof_tb_crc_bits = (nof_tb_bits_tmp <= MAX_BITS_CRC16) ? 16 : 24;

  nof_tb_bits_in = nof_tb_bits_tmp + nof_tb_crc_bits;

  buffer.resize(nof_tb_bits_in);
  srsvec::bit_unpack(span<uint8_t>(buffer).first(nof_tb_bits_tmp), transport_block);
  unsigned tb_checksum = tb_crc.calculate_byte(transport_block);
  srsvec::bit_unpack(span<uint8_t>(buffer).last(nof_tb_crc_bits), tb_checksum, nof_tb_crc_bits);

  nof_segments    = ldpc::compute_nof_codeblocks(nof_tb_bits_tmp, base_graph);
  nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += nof_segments * SEG_CRC_LENGTH;
  }
  compute_lifting_size();
  compute_segment_length();

  unsigned nof_crc_bits = 0;
  if (nof_segments > 1) {
    nof_crc_bits = SEG_CRC_LENGTH;
  }
  // Compute the maximum number of information bits that can be assigned to a segment.
  unsigned max_info_bits = divide_ceil(nof_tb_bits_out, nof_segments) - nof_crc_bits;

  // Zero-padding if necessary.
  unsigned zero_pad = (max_info_bits + nof_crc_bits) * nof_segments - nof_tb_bits_out;
  if (zero_pad > 0) {
    buffer.resize(nof_tb_bits_in + zero_pad, 0);
  }

  // Number of channel symbols assigned to a transmission layer.
  nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  nof_short_segments = nof_segments - (nof_symbols_per_layer % nof_segments);

  // Codeword length (after concatenation of codeblocks).
  unsigned cw_length = cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod);

  unsigned input_idx = 0;
  unsigned cw_offset = 0;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    segment_data tmp_data(segment_length);
    // Number of filler bits in this segment.
    unsigned nof_filler_bits = segment_length - max_info_bits - nof_crc_bits;

    fill_segment(tmp_data,
                 span<uint8_t>(buffer).subspan(input_idx, max_info_bits),
                 crc_set.crc24B,
                 nof_crc_bits,
                 nof_filler_bits);
    input_idx += max_info_bits;

    codeblock_metadata tmp_description =
        generate_cb_metadata({i_segment, cw_length, cw_offset, nof_filler_bits, nof_crc_bits, nof_tb_crc_bits}, cfg);

    cw_offset += tmp_description.cb_specific.rm_length;

    described_segments.push_back({tmp_data, tmp_description});
  }
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  assert(cw_length == cw_offset);
}

static void check_inputs_rx(span<const int8_t> codeword_llrs, const segmenter_config& cfg)
{
  srsran_assert(!codeword_llrs.empty(), "Argument transport_block should not be empty.");
  srsran_assert(codeword_llrs.size() == cfg.nof_ch_symbols * get_bits_per_symbol(cfg.mod), "Wrong number of LLRs.");

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

// For the Rx-chain segmenter.
void ldpc_segmenter_impl::segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
                                  span<const int8_t>                                       codeword_llrs,
                                  unsigned                                                 tbs,
                                  const segmenter_config&                                  cfg)
{
  check_inputs_rx(codeword_llrs, cfg);

  base_graph = cfg.base_graph;

  constexpr unsigned MAX_BITS_CRC16  = 3824;
  unsigned           nof_tb_crc_bits = (tbs <= MAX_BITS_CRC16) ? 16 : 24;

  nof_tb_bits_in = tbs + nof_tb_crc_bits;

  nof_segments    = ldpc::compute_nof_codeblocks(tbs, base_graph);
  nof_tb_bits_out = nof_tb_bits_in;
  if (nof_segments > 1) {
    nof_tb_bits_out += nof_segments * SEG_CRC_LENGTH;
  }
  compute_lifting_size();
  compute_segment_length();

  unsigned nof_crc_bits = 0;
  if (nof_segments > 1) {
    nof_crc_bits = SEG_CRC_LENGTH;
  }
  // Compute the maximum number of information bits that can be assigned to a segment.
  unsigned max_info_bits = divide_ceil(nof_tb_bits_out, nof_segments) - nof_crc_bits;

  // Number of channel symbols assigned to a transmission layer.
  nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  nof_short_segments = nof_segments - (nof_symbols_per_layer % nof_segments);

  // Codeword length (after concatenation of codeblocks).
  unsigned cw_length = codeword_llrs.size();

  unsigned cw_offset = 0;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    unsigned nof_filler_bits = segment_length - max_info_bits - nof_crc_bits;

    codeblock_metadata tmp_description =
        generate_cb_metadata({i_segment, cw_length, cw_offset, nof_filler_bits, nof_crc_bits, nof_tb_crc_bits}, cfg);

    unsigned rm_length = tmp_description.cb_specific.rm_length;
    described_codeblocks.push_back({codeword_llrs.subspan(cw_offset, rm_length), tmp_description});
    cw_offset += rm_length;
  }
  // After accumulating all codeblock rate-matched lengths, cw_offset should be the same as cw_length.
  assert(cw_length == cw_offset);
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
  tmp_description.tb_common.cw_length    = seg_extra.cw_length;

  // BG1 has rate 1/3 and BG2 has rate 1/5.
  constexpr unsigned INVERSE_BG1_RATE = 3;
  constexpr unsigned INVERSE_BG2_RATE = 5;
  unsigned           inverse_rate     = (base_graph == base_graph_t::BG1) ? INVERSE_BG1_RATE : INVERSE_BG2_RATE;
  unsigned           rm_length        = compute_rm_length(seg_extra.i_segment, cfg.mod, cfg.nof_layers);

  tmp_description.cb_specific.full_length     = segment_length * inverse_rate;
  tmp_description.cb_specific.nof_filler_bits = seg_extra.nof_filler_bits;
  tmp_description.cb_specific.rm_length       = rm_length;
  tmp_description.cb_specific.cw_offset       = seg_extra.cw_offset;
  // nof_crc_bits == 0 indicates that we are using the TB CRC with length 16.
  tmp_description.cb_specific.nof_crc_bits = (nof_segments == 1) ? seg_extra.nof_tb_crc_bits : seg_extra.nof_crc_bits;

  return tmp_description;
}

std::unique_ptr<ldpc_segmenter_tx> srsgnb::create_ldpc_segmenter_tx()
{
  ldpc_segmenter_impl::sch_crc crcs = {create_crc_calculator(crc_generator_poly::CRC16),
                                       create_crc_calculator(crc_generator_poly::CRC24A),
                                       create_crc_calculator(crc_generator_poly::CRC24B)};
  return std::make_unique<ldpc_segmenter_impl>(crcs);
}

std::unique_ptr<ldpc_segmenter_rx> srsgnb::create_ldpc_segmenter_rx()
{
  ldpc_segmenter_impl::sch_crc crcs = {create_crc_calculator(crc_generator_poly::CRC16),
                                       create_crc_calculator(crc_generator_poly::CRC24A),
                                       create_crc_calculator(crc_generator_poly::CRC24B)};
  return std::make_unique<ldpc_segmenter_impl>(crcs);
}
