/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "equalize_zf_1x2.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

namespace {

inline void equalize_zf_1x2_symbol(span<cf_t>       symbol_out,
                                   span<float>      noise_vars,
                                   span<const cf_t> symbol_in_p0,
                                   span<const cf_t> symbol_in_p1,
                                   span<const cf_t> ch_estimates_p0,
                                   span<const cf_t> ch_estimates_p1,
                                   float            tx_scaling)
{
  const std::size_t nof_subcs = symbol_in_p0.size();

  for (std::size_t i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    const cf_t ch_est_p0 = ch_estimates_p0[i_subc];
    const cf_t ch_est_p1 = ch_estimates_p1[i_subc];

    // Apply Zero Forcing algorithm.
    float d_zf = tx_scaling * (abs_sq(ch_est_p0) + abs_sq(ch_est_p1));
    if (std::isnormal(d_zf)) {
      symbol_out[i_subc] =
          (symbol_in_p0[i_subc] * std::conj(ch_est_p0) + symbol_in_p1[i_subc] * std::conj(ch_est_p1)) / d_zf;
    }
  }

  // For now, do not compute the output noise variances.
  srsvec::zero(noise_vars);
}

} // namespace

void srsgnb::equalize_zf_1x2(equalizer_symbol_list&          eq_symbols,
                             equalizer_noise_var_list&       noise_vars,
                             const equalizer_ch_symbol_list& ch_symbols,
                             const channel_estimate&         ch_estimates,
                             float                           tx_scaling)
{
  const std::size_t nof_symbols = ch_symbols.size().nof_symbols;
  const std::size_t nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> ch_estimates_p0 = ch_estimates.get_path_ch_estimate(0, 0);
  span<const cf_t> ch_estimates_p1 = ch_estimates.get_path_ch_estimate(1, 0);

  std::size_t re_index = 0;

  // Equalize symbol by symbol.
  for (std::size_t i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_1x2_symbol(eq_symbols.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 1),
                           ch_estimates_p0.subspan(re_index, nof_subcs),
                           ch_estimates_p1.subspan(re_index, nof_subcs),
                           tx_scaling);
    re_index += nof_subcs;
  }
}
