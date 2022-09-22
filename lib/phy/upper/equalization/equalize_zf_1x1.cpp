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

static void equalize_zf_1x1_symbol(span<cf_t>       symbol_out,
                                   span<float>      eq_noise_vars,
                                   span<const cf_t> symbol_in,
                                   span<const cf_t> ch_estimates,
                                   float            noise_var_est,
                                   float            tx_scaling)
{
  const unsigned nof_subcs = symbol_in.size();

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    cf_t  ch_est    = ch_estimates[i_subc] * tx_scaling;
    float ch_mod_sq = abs_sq(ch_est);

    // Calculate the reciprocal of the channel estimate and its squared absolute value.
    // Set the symbols and noise variances to zero in case of division by zero, NAN of INF.
    cf_t  ch_est_rcp    = 0.0F;
    float ch_mod_sq_rcp = 0.0F;
    if (std::isnormal(ch_mod_sq)) {
      ch_mod_sq_rcp = 1.0F / ch_mod_sq;
      ch_est_rcp    = std::conj(ch_est) * ch_mod_sq_rcp;
    }

    // Apply Zero Forcing algorithm.
    symbol_out[i_subc] = symbol_in[i_subc] * ch_est_rcp;

    // Calculate noise variances.
    eq_noise_vars[i_subc] = noise_var_est * ch_mod_sq_rcp;
  }
}

void srsgnb::equalize_zf_1x1(equalizer_symbol_list&          eq_symbols,
                             equalizer_noise_var_list&       noise_vars,
                             const equalizer_ch_symbol_list& ch_symbols,
                             const channel_estimate&         ch_estimates,
                             float                           tx_scaling)
{
  const unsigned nof_symbols        = ch_symbols.size().nof_symbols;
  const float    noise_var_estimate = ch_estimates.get_noise_variance(0);

  // Equalize symbol by symbol.
  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_1x1_symbol(eq_symbols.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 0),
                           ch_estimates.get_symbol_ch_estimate(i_symb),
                           noise_var_estimate,
                           tx_scaling);
  }
}
