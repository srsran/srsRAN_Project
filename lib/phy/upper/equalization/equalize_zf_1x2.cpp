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
/// \brief Zero Forcing equalization function implementation for a MISO 1 X 2 channel.

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
  const unsigned nof_subcs = symbol_in_p0.size();

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    const cf_t ch_est_p0 = ch_estimates_p0[i_subc];
    const cf_t ch_est_p1 = ch_estimates_p1[i_subc];

    // Calculate the reciprocal of the denominator. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_zf     = tx_scaling * (abs_sq(ch_est_p0) + abs_sq(ch_est_p1));
    float d_zf_rcp = 0.0F;
    if (std::isnormal(d_zf)) {
      d_zf_rcp = 1.0F / d_zf;
    }
    // Apply Zero Forcing algorithm.
    symbol_out[i_subc] =
        (symbol_in_p0[i_subc] * std::conj(ch_est_p0) + symbol_in_p1[i_subc] * std::conj(ch_est_p1)) * d_zf_rcp;
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
  const unsigned nof_symbols = ch_symbols.size().nof_symbols;

  // Equalize symbol by symbol.
  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_1x2_symbol(eq_symbols.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 1),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 0),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 1),
                           tx_scaling);
  }
}
