/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Zero Forcing equalization function implementation for a SISO channel.

#include "equalize_zf_1x1.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

namespace {

inline void equalize_zf_1x1_symbol(span<cf_t>       symbol_out,
                                   span<float>      noise_vars,
                                   span<const cf_t> symbol_in,
                                   span<const cf_t> ch_estimates,
                                   float            noise_var_estimate,
                                   float            tx_scaling)
{
  const std::size_t nof_subcs     = symbol_in.size();
  const float       tx_scaling_sq = tx_scaling * tx_scaling;

  // Revert tx_scaling factor.
  srsvec::sc_prod(symbol_in, 1 / tx_scaling, symbol_out);

  for (std::size_t i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    const cf_t ch_est = ch_estimates[i_subc];

    // Apply Zero Forcing algorithm.
    symbol_out[i_subc] = symbol_out[i_subc] / ch_est;

    // Calculate noise variances.
    float ch_mod_sq = abs_sq(ch_est);
    if (std::isnormal(ch_mod_sq)) {
      noise_vars[i_subc] = noise_var_estimate / (ch_mod_sq * tx_scaling_sq);
    }
  }
}

} // namespace

void srsgnb::equalize_zf_1x1(equalizer_symbol_list&          eq_symbols,
                             equalizer_noise_var_list&       noise_vars,
                             const equalizer_ch_symbol_list& ch_symbols,
                             const channel_estimate&         ch_estimates,
                             float                           tx_scaling)
{
  const std::size_t nof_symbols = ch_symbols.size().nof_symbols;
  const std::size_t nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> channel_estimates  = ch_estimates.get_path_ch_estimate(0, 0);
  const float      noise_var_estimate = ch_estimates.get_noise_variance(0, 0);

  std::size_t re_index = 0;

  // Equalize symbol by symbol.
  for (std::size_t i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_1x1_symbol(eq_symbols.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 0),
                           channel_estimates.subspan(re_index, nof_subcs),
                           noise_var_estimate,
                           tx_scaling);
    re_index += nof_subcs;
  }
}
