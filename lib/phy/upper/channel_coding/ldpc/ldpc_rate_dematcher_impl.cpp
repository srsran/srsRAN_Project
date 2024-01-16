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

#include "ldpc_rate_dematcher_impl.h"
#include "ldpc_graph_impl.h"
#include "ldpc_luts_impl.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::ldpc;

static const std::array<double, 4> shift_factor_bg1 = {0, 17, 33, 56};
static const std::array<double, 4> shift_factor_bg2 = {0, 13, 25, 43};

void ldpc_rate_dematcher_impl::init(bool new_data, const codeblock_metadata::tb_common_metadata& cfg)
{
  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
  rv = cfg.rv;

  modulation_order = get_bits_per_symbol(cfg.mod);

  is_new_data = new_data;
}

void ldpc_rate_dematcher_impl::rate_dematch(span<log_likelihood_ratio>       output,
                                            span<const log_likelihood_ratio> input,
                                            const bool                       new_data,
                                            const codeblock_metadata&        cfg)
{
  init(new_data, cfg.tb_common);

  unsigned block_length = output.size();

  // Make sure N_ref is valid.
  srsran_assert(cfg.tb_common.Nref <= MAX_CODEBLOCK_SIZE,
                "N_ref {} must be smaller or equal to {}.",
                cfg.tb_common.Nref,
                MAX_CODEBLOCK_SIZE);

  // If N_ref is given, limit the buffer size. Otherwise, use default.
  if (cfg.tb_common.Nref > 0) {
    buffer_length = std::min(cfg.tb_common.Nref, block_length);
  } else {
    buffer_length = block_length;
  }

  // The input size cannot be larger than the maximum rate-matched codeblock length.
  srsran_assert(input.size() <= MAX_CODEBLOCK_RM_SIZE,
                "The length of the rate-matched codeblock is {} but it shouldn't be more than {}.",
                input.size(),
                MAX_CODEBLOCK_RM_SIZE);

  // The input size must be a multiple of the modulation order.
  srsran_assert(input.size() % modulation_order == 0, "The input length should be a multiple of the modulation order.");

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
    srsran_assert(false, "LDPC rate dematching: invalid input length.");
  }
  uint16_t lifting_size = block_length / BG_N_short;
  srsran_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
                "LDPC rate dematching: invalid input length.");

  // Recall that 2 * lifting_size systematic bits are shortened out of the codeblock.
  nof_systematic_bits = (BG_K - 2) * lifting_size;
  srsran_assert(cfg.cb_specific.nof_filler_bits < nof_systematic_bits,
                "LDPC rate dematching: invalid number of filler bits.");
  nof_filler_bits = cfg.cb_specific.nof_filler_bits;

  double tmp = (shift_factor[rv] * buffer_length) / block_length;
  shift_k0   = static_cast<uint16_t>(floor(tmp)) * lifting_size;

  if (modulation_order == 1) {
    allot_llrs(output, input);
  } else {
    span<log_likelihood_ratio> aux = span<log_likelihood_ratio>(auxiliary_buffer).first(input.size());
    deinterleave_llrs(aux, input);
    allot_llrs(output, aux);
  }
};

void ldpc_rate_dematcher_impl::combine_softbits(span<log_likelihood_ratio>       out,
                                                span<const log_likelihood_ratio> in0,
                                                span<const log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  for (unsigned index = 0, index_end = out.size(); index != index_end; ++index) {
    out[index] = in0[index] + in1[index];
  }
}

void ldpc_rate_dematcher_impl::allot_llrs(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const
{
  unsigned nof_info_bits = nof_systematic_bits - nof_filler_bits;

  // Set to true for copy, false to combine.
  bool is_copy_mode = is_new_data;

  unsigned tmp_idx = shift_k0;
  while (!in.empty()) {
    // Process consecutive bits before filler bits.
    if (tmp_idx < nof_info_bits) {
      // Calculate number of bits between tmp_idx to the first filler bit.
      unsigned nbits_systematic = std::min(nof_info_bits - tmp_idx, static_cast<unsigned>(in.size()));

      // Select output bits.
      span<log_likelihood_ratio> out_chunk = out.subspan(tmp_idx, nbits_systematic);

      // Not a filler bit, combine value with the previous LLR value relative to the same bit (if any).
      // Reminder: this is a sum between LLRs, therefore it is saturated.
      if (is_copy_mode) {
        srsvec::zero(out.first(tmp_idx));
        srsvec::copy(out_chunk, in.first(nbits_systematic));
      } else {
        combine_softbits(out_chunk, out_chunk, in.first(nbits_systematic));
      }

      // Advance buffers.
      tmp_idx += nbits_systematic;
      in = in.last(in.size() - nbits_systematic);
    } else if (is_copy_mode) {
      srsvec::zero(out.first(nof_info_bits));
    }

    // Set filler bits if it is copying data.
    if (is_copy_mode) {
      // Filler bits are counted as fixed, logical zeros by the decoder. Set the corresponding LLR to +inf.
      span<log_likelihood_ratio> filler_bits = out.subspan(nof_info_bits, nof_filler_bits);
      std::fill(filler_bits.begin(), filler_bits.end(), LLR_INFINITY);
    }

    // Skip filler bits.
    if (tmp_idx < nof_systematic_bits) {
      tmp_idx = nof_systematic_bits;
    }

    // Process parity bits. Calculate number of bits between tmp_idx to the end of the buffer.
    unsigned nbits_parity = std::min(buffer_length - tmp_idx, static_cast<unsigned>(in.size()));

    // Select output bits.
    span<log_likelihood_ratio> out_chunk = out.subspan(tmp_idx, nbits_parity);

    // Not a filler bit, combine value with the previous LLR value relative to the same bit (if any).
    // Reminder: this is a sum between LLRs, therefore it is saturated.
    if (is_copy_mode) {
      srsvec::copy(out_chunk, in.first(nbits_parity));
    } else {
      combine_softbits(out_chunk, out_chunk, in.first(nbits_parity));
    }

    // Advance buffers.
    tmp_idx = (tmp_idx + nbits_parity) % buffer_length;
    in      = in.last(in.size() - nbits_parity);

    // All iterations after the first one are in combine mode.
    if (!in.empty()) {
      is_copy_mode = false;
    }
  }

  // If the data is new and the buffer has not been completely filled, then set the remaining bits to zero.
  if ((is_copy_mode) && (tmp_idx != 0)) {
    srsvec::zero(out.last(buffer_length - tmp_idx));
  }
}

template <unsigned Qm>
static void deinterleave_bits_Qm(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in)
{
  unsigned E = out.size();
  unsigned K = E / Qm;
  for (unsigned in_index = 0, i = 0; i != K; ++i) {
    for (unsigned j = 0; j != Qm; ++j, ++in_index) {
      out[K * j + i] = in[in_index];
    }
  }
}

void ldpc_rate_dematcher_impl::deinterleave_qpsk(span<log_likelihood_ratio>       out,
                                                 span<const log_likelihood_ratio> in) const
{
  deinterleave_bits_Qm<2>(out, in);
}

void ldpc_rate_dematcher_impl::deinterleave_qam16(span<log_likelihood_ratio>       out,
                                                  span<const log_likelihood_ratio> in) const
{
  deinterleave_bits_Qm<4>(out, in);
}

void ldpc_rate_dematcher_impl::deinterleave_qam64(span<log_likelihood_ratio>       out,
                                                  span<const log_likelihood_ratio> in) const
{
  deinterleave_bits_Qm<6>(out, in);
}

void ldpc_rate_dematcher_impl::deinterleave_qam256(span<log_likelihood_ratio>       out,
                                                   span<const log_likelihood_ratio> in) const
{
  deinterleave_bits_Qm<8>(out, in);
}

void ldpc_rate_dematcher_impl::deinterleave_llrs(span<log_likelihood_ratio>       out,
                                                 span<const log_likelihood_ratio> in) const
{
  switch (modulation_order) {
    default:
    case 2:
      deinterleave_qpsk(out, in);
      break;
    case 4:
      deinterleave_qam16(out, in);
      break;
    case 6:
      deinterleave_qam64(out, in);
      break;
    case 8:
      deinterleave_qam256(out, in);
      break;
  }
}
