/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_rate_dematcher_impl.h"
#include "ldpc_graph_impl.h"
#include "ldpc_luts_impl.h"
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_rate_dematcher_impl::init(bool new_data, const codeblock_metadata::tb_common_metadata& cfg)
{
  srsgnb_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
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
  srsgnb_assert(cfg.tb_common.Nref <= MAX_CODEBLOCK_SIZE,
                "N_ref {} must be smaller or equal to {}.",
                cfg.tb_common.Nref,
                MAX_CODEBLOCK_SIZE);

  // If N_ref is given, limit the buffer size. Otherwise, use default.
  if (cfg.tb_common.Nref > 0) {
    buffer_length = std::min(cfg.tb_common.Nref, block_length);
  } else {
    buffer_length = block_length;
  }

  // The input size must be a multiple of the modulation order.
  srsgnb_assert(input.size() % modulation_order == 0, "The input length should be a multiple of the modulation order.");

  // Compute shift_k0 according to TS38.212 Table 5.4.2.1-2.
  std::array<double, 4> shift_factor = {};
  unsigned              BG_N_short   = 0;
  unsigned              BG_K         = 0;
  if ((block_length % BG1_N_SHORT) == 0) {
    // input is a BG1 codeblock
    shift_factor = {0, 17, 33, 56};
    BG_N_short   = BG1_N_SHORT;
    BG_K         = BG1_N_FULL - BG1_M;
  } else if ((block_length % BG2_N_SHORT) == 0) {
    // input is a BG2 codeblock
    shift_factor = {0, 13, 25, 43};
    BG_N_short   = BG2_N_SHORT;
    BG_K         = BG2_N_FULL - BG2_M;
  } else {
    srsgnb_assert(false, "LDPC rate dematching: invalid input length.");
  }
  uint16_t lifting_size = block_length / BG_N_short;
  srsgnb_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
                "LDPC rate dematching: invalid input length.");

  // Recall that 2 * lifting_size systematic bits are shortened out of the codeblock.
  nof_systematic_bits = (BG_K - 2) * lifting_size;
  srsgnb_assert(cfg.cb_specific.nof_filler_bits < nof_systematic_bits,
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

void ldpc_rate_dematcher_impl::allot_llrs(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) const
{
  // When we are not combining the current codeblock with previous ones, ensure we start from clean LLRs.
  if (is_new_data) {
    std::fill(out.begin(), out.end(), 0);
  }

  unsigned matched_length = in.size();
  unsigned nof_info_bits  = nof_systematic_bits - nof_filler_bits;
  for (unsigned j = 0, k = 0; (j < buffer_length) && (k < matched_length); ++j) {
    unsigned tmp_idx = (shift_k0 + j) % buffer_length;

    if ((tmp_idx < nof_info_bits) || (tmp_idx >= nof_systematic_bits)) {
      // Not a filler bit, combine value with the previous LLR value relative to the same bit (if any).
      // Reminder: this is a sum between LLRs, therefore it is saturated.
      out[tmp_idx] += in[k];
      ++k;
    } else {
      // This is a filler bit: the corresponding LLR should be either 0 or INT8_MAX.
      assert((is_new_data && (out[tmp_idx] == 0)) || (!is_new_data && (out[tmp_idx] == INT8_MAX)));

      // Filler bits are counted as fixed, logical zeros by the decoder. Set the corresponding
      // LLR to +inf.
      out[tmp_idx] = LLR_INFINITY;
    }
  }
}

void ldpc_rate_dematcher_impl::deinterleave_llrs(span<log_likelihood_ratio>       out,
                                                 span<const log_likelihood_ratio> in) const
{
  unsigned E = in.size();

  for (unsigned in_index = 0; in_index != E; ++in_index) {
    unsigned help_index = in_index % modulation_order;
    unsigned out_index  = (in_index + help_index * (E - 1)) / modulation_order;
    out[out_index]      = in[in_index];
  }
}
