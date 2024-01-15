/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ldpc_rate_matcher_impl.h"
#include "ldpc_luts_impl.h"
#include "srsran/adt/interval.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

static const std::array<double, 4> shift_factor_bg1 = {0, 17, 33, 56};
static const std::array<double, 4> shift_factor_bg2 = {0, 13, 25, 43};

void ldpc_rate_matcher_impl::init(const codeblock_metadata& cfg, unsigned block_length, unsigned rm_length)
{
  srsran_assert((cfg.tb_common.rv >= 0) && (cfg.tb_common.rv <= 3), "RV should an integer between 0 and 3.");
  rv               = cfg.tb_common.rv;
  modulation_order = get_bits_per_symbol(cfg.tb_common.mod);
  base_graph       = cfg.tb_common.base_graph;
  nof_filler_bits  = cfg.cb_specific.nof_filler_bits;

  if (cfg.tb_common.Nref > 0) {
    buffer_length = std::min(cfg.tb_common.Nref, block_length);
  } else {
    buffer_length = block_length;
  }

  // The output size cannot be larger than the maximum rate-matched codeblock length.
  srsran_assert(rm_length <= MAX_CODEBLOCK_RM_SIZE,
                "The length of the rate-matched codeblock is {} but it shouldn't be more than {}.",
                rm_length,
                MAX_CODEBLOCK_RM_SIZE);

  // The output size must be a multiple of the modulation order.
  srsran_assert(rm_length % modulation_order == 0, "The output length should be a multiple of the modulation order.");

  // Compute shift_k0 according to TS38.212 Table 5.4.2.1-2.
  span<const double> shift_factor;
  unsigned           BG_N_short = 0;
  unsigned           BG_K       = 0;
  if ((block_length % BG1_N_SHORT) == 0) {
    // input is a BG1 codeblock
    shift_factor = shift_factor_bg1;
    BG_N_short   = BG1_N_SHORT;
    BG_K         = BG1_N_FULL - BG1_M;
  } else if ((block_length % BG2_N_SHORT) == 0) {
    // input is a BG2 codeblock
    shift_factor = shift_factor_bg2;
    BG_N_short   = BG2_N_SHORT;
    BG_K         = BG2_N_FULL - BG2_M;
  } else {
    srsran_assert(false, "LDPC rate matching: invalid input length.");
  }
  uint16_t lifting_size = block_length / BG_N_short;
  srsran_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
                "LDPC rate matching: invalid input length.");

  // Recall that 2 * lifting_size systematic bits are shortened out of the codeblock.
  nof_systematic_bits = (BG_K - 2) * lifting_size;
  srsran_assert(cfg.cb_specific.nof_filler_bits < nof_systematic_bits,
                "LDPC rate matching: invalid number of filler bits.");
  nof_filler_bits = cfg.cb_specific.nof_filler_bits;

  srsran_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
                "LDPC rate matching: invalid input length.");
  double tmp = (shift_factor[rv] * buffer_length) / block_length;
  shift_k0   = static_cast<uint16_t>(floor(tmp)) * lifting_size;
}

void ldpc_rate_matcher_impl::rate_match(bit_buffer& output, const bit_buffer& input, const codeblock_metadata& cfg)
{
  init(cfg, input.size(), output.size());

  span<uint8_t> aux = span<uint8_t>(auxiliary_buffer).first(output.size());
  select_bits(aux, input.first(buffer_length));
  interleave_bits(output, aux);
}

void ldpc_rate_matcher_impl::select_bits(span<uint8_t> out, const bit_buffer& in) const
{
  unsigned output_length = out.size();
  unsigned out_index     = 0;
  unsigned in_index      = shift_k0;

  // Equivalent code:
  // unsigned in_index = shift_k0 % buffer_length;
  // for (auto& this_out : out) {
  //   while (in[in_index] == FILLER_BIT) {
  //     in_index = (in_index + 1) % buffer_length;
  //   }
  //   this_out = in[in_index];
  //   in_index = (in_index + 1) % buffer_length;
  // }

  // Calculate the filler bits range.
  interval<unsigned> filler_bits_range(nof_systematic_bits - nof_filler_bits, nof_systematic_bits);

  // as long as the ouput index does not reach the output length...
  while (out_index < output_length) {
    // If the index is within the filler bits range, advance it.
    if (filler_bits_range.contains(in_index)) {
      in_index = filler_bits_range.stop();
    }

    // Select input chunk interval. Stop the chunk at the first filler bit.
    interval<unsigned> input_chunk_range(in_index, in.size());
    if (!filler_bits_range.empty() && input_chunk_range.contains(filler_bits_range.start())) {
      input_chunk_range = {in_index, filler_bits_range.start()};
    }

    // Trim the count to the remainder bits.
    unsigned count = std::min(input_chunk_range.length(), output_length - out_index);

    // Append the consecutive number of bits.
    srsvec::bit_unpack(out.subspan(out_index, count), in, in_index);
    out_index += count;

    // Advance in_index the amount of written bits.
    in_index = (in_index + count) % buffer_length;
  }
}

namespace {

template <unsigned Qm>
void interleave_bits_Qm(bit_buffer& out, span<const uint8_t> in)
{
  unsigned E = out.size();
  unsigned K = E / Qm;

  for (unsigned out_index = 0, i = 0; i != K; ++i) {
    uint8_t word = in[i];

    for (unsigned j = 1; j != Qm; ++j) {
      // Extract bit to pack.
      uint8_t bit = in[K * j + i];

      // Append bit to the word.
      word = (word << 1) | bit;
    }

    // Insert word in the packed buffer.
    out.insert(word, out_index, Qm);

    // Increment the output index.
    out_index += Qm;
  }
}

template <>
void interleave_bits_Qm<1>(bit_buffer& out, span<const uint8_t> in)
{
  srsvec::bit_pack(out, in);
}

template <>
void interleave_bits_Qm<8>(bit_buffer& out, span<const uint8_t> in)
{
  unsigned      E         = out.size();
  unsigned      K         = E / 8;
  span<uint8_t> out_bytes = out.get_buffer().first(K);

  srsvec::copy(out_bytes, in.first(K));
  for (unsigned q = 1; q != 8; ++q) {
    span<const uint8_t> in_q = in.subspan(K * q, K);
    for (unsigned k = 0; k != K; ++k) {
      out_bytes[k] = (out_bytes[k] << 1) | in_q[k];
    }
  }
}

} // namespace

void ldpc_rate_matcher_impl::interleave_bits(bit_buffer& out, span<const uint8_t> in) const
{
  switch (modulation_order) {
    case 1:
      interleave_bits_Qm<1>(out, in);
      break;
    case 2:
      interleave_bits_Qm<2>(out, in);
      break;
    case 4:
      interleave_bits_Qm<4>(out, in);
      break;
    case 6:
      interleave_bits_Qm<6>(out, in);
      break;
    case 8:
    default:
      interleave_bits_Qm<8>(out, in);
      break;
  }
}
