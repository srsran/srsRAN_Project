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
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_rate_dematcher_impl::init(bool new_data, const codeblock_metadata::tb_common_metadata& cfg)
{
  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
  rv = cfg.rv;

  modulation_order = get_bits_per_symbol(cfg.mod);

  is_new_data = new_data;
}

void ldpc_rate_dematcher_impl::rate_dematch(span<int8_t>              output,
                                            span<const int8_t>        input,
                                            const bool                new_data,
                                            const codeblock_metadata& cfg)
{
  init(new_data, cfg.tb_common);

  unsigned block_length = output.size();

  srsran_assert(block_length >= cfg.tb_common.Nref, "N_ref should be smaller than the output length.");
  if (cfg.tb_common.Nref > 0) {
    buffer_length = cfg.tb_common.Nref;
  } else {
    buffer_length = block_length;
  }

  // The input size must be a multiple of the modulation order.
  srsran_assert(input.size() % modulation_order == 0, "The input length should be a multiple of the modulation order.");

  // Compute shift_k0 according to TS38.212 Table 5.4.2.1-2.
  std::array<double, 4> shift_factor{};
  uint16_t              lifting_size{};
  unsigned              BG_N_short{0};
  unsigned              BG_K{0};
  if (block_length % BG1_N_SHORT == 0) {
    // input is a BG1 codeblock
    shift_factor = {0, 17, 33, 56};
    BG_N_short   = BG1_N_SHORT;
    BG_K         = BG1_N_FULL - BG1_M;
  } else if (block_length % BG2_N_SHORT == 0) {
    // input is a BG2 codeblock
    shift_factor = {0, 13, 25, 43};
    BG_N_short   = BG2_N_SHORT;
    BG_K         = BG2_N_FULL - BG2_M;
  } else {
    srsran_assert(false, "LDPC rate dematching: invalid input length.");
  }
  lifting_size = block_length / BG_N_short;
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
    span<int8_t> aux = span<int8_t>(auxiliary_buffer).first(input.size());
    deinterleave_llrs(aux, input);
    allot_llrs(output, aux);
  }
};

static int8_t combine_llrs(int8_t llrs1, int8_t llrs2)
{
  // LLRs are quantized over 7 bits and take values from -63 to 63, with INT8_MAX corresponding to infinity.
  constexpr int MAX_RANGE = 63;

  int tmp = llrs1 + llrs2;

  if (std::abs(tmp) > MAX_RANGE) {
    tmp = (tmp > 0) ? MAX_RANGE : -MAX_RANGE;
  }
  return static_cast<int8_t>(tmp);
};

void ldpc_rate_dematcher_impl::allot_llrs(span<int8_t> out, span<const int8_t> in) const
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
      // Not a filler bit, copy value.
      out[tmp_idx] = combine_llrs(out[tmp_idx], in[k]);
      ++k;
    } else {
      // This is a filler bit: the corresponding LLR should be either 0 or INT8_MAX.
      assert((is_new_data && (out[tmp_idx] == 0)) || (!is_new_data && (out[tmp_idx] == INT8_MAX)));

      // Filler bits are counted as fixed, logical zeros by the decoder. Set the corresponding
      // LLR to +inf (i.e., INT8_MAX in our fixed-point representation).
      out[tmp_idx] = INT8_MAX;
    }
  }
}

void ldpc_rate_dematcher_impl::deinterleave_llrs(span<int8_t> out, span<const int8_t> in) const
{
  unsigned E = in.size();

  for (unsigned in_index = 0; in_index != E; ++in_index) {
    unsigned help_index = in_index % modulation_order;
    unsigned out_index  = (in_index + help_index * (E - 1)) / modulation_order;
    out[out_index]      = in[in_index];
  }
}

std::unique_ptr<ldpc_rate_dematcher> srsgnb::create_ldpc_rate_dematcher()
{
  return std::make_unique<ldpc_rate_dematcher_impl>();
}
