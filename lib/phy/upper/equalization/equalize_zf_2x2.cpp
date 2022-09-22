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
/// \brief Zero Forcing equalization function implementation for a MIMO 2 X 2 channel.

#include "equalize_zf_2x2.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

static void equalize_zf_2x2_symbol(span<cf_t>       symbol_out_l0,
                                   span<cf_t>       symbol_out_l1,
                                   span<float>      eq_noise_vars_l0,
                                   span<float>      eq_noise_vars_l1,
                                   span<const cf_t> symbol_in_p0,
                                   span<const cf_t> symbol_in_p1,
                                   span<const cf_t> ch_estimates_p0_l0,
                                   span<const cf_t> ch_estimates_p0_l1,
                                   span<const cf_t> ch_estimates_p1_l0,
                                   span<const cf_t> ch_estimates_p1_l1,
                                   float            noise_var_est,
                                   float            tx_scaling)
{
  const unsigned nof_subcs = symbol_in_p0.size();

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Channel estimates.
    cf_t ch_p0_l0 = ch_estimates_p0_l0[i_subc];
    cf_t ch_p0_l1 = ch_estimates_p0_l1[i_subc];
    cf_t ch_p1_l0 = ch_estimates_p1_l0[i_subc];
    cf_t ch_p1_l1 = ch_estimates_p1_l1[i_subc];

    // Conjugated channel estimates.
    cf_t ch_p0_l0_conj = std::conj(ch_p0_l0);
    cf_t ch_p0_l1_conj = std::conj(ch_p0_l1);
    cf_t ch_p1_l0_conj = std::conj(ch_p1_l0);
    cf_t ch_p1_l1_conj = std::conj(ch_p1_l1);

    // Absolute squares of the channel estimates.
    float ch_p0_l0_mod_sq = abs_sq(ch_p0_l0);
    float ch_p0_l1_mod_sq = abs_sq(ch_p0_l1);
    float ch_p1_l0_mod_sq = abs_sq(ch_p1_l0);
    float ch_p1_l1_mod_sq = abs_sq(ch_p1_l1);

    // Resource Elements coming from the Rx ports.
    cf_t re_in_p0 = symbol_in_p0[i_subc];
    cf_t re_in_p1 = symbol_in_p1[i_subc];

    // Calculate the product of the channel matrix and its hermitian transpose.
    // The diagonal coefficients are the squared norms of the channel matrix column vectors.
    float norm_sq_ch_l0 = ch_p0_l0_mod_sq + ch_p1_l0_mod_sq;
    float norm_sq_ch_l1 = ch_p0_l1_mod_sq + ch_p1_l1_mod_sq;

    // Calculate the anti-diagonal coefficients, which are -xi and -xi_conj.
    cf_t  xi        = (ch_p0_l0_conj * ch_p0_l1) + (ch_p1_l0_conj * ch_p1_l1);
    cf_t  xi_conj   = std::conj(xi);
    float xi_mod_sq = abs_sq(xi);

    // Apply a matched filter for each Tx layer to the input signal.
    cf_t matched_input_l0 = (ch_p0_l0_conj * re_in_p0) + (ch_p1_l0_conj * re_in_p1);
    cf_t matched_input_l1 = (ch_p0_l1_conj * re_in_p0) + (ch_p1_l1_conj * re_in_p1);

    // Calculate the reciprocal of the denominators. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_pinv      = tx_scaling * ((norm_sq_ch_l0 * norm_sq_ch_l1) - xi_mod_sq);
    float d_nvars     = tx_scaling * d_pinv;
    float d_pinv_rcp  = 0.0F;
    float d_nvars_rcp = 0.0F;
    if (std::isnormal(d_pinv)) {
      d_pinv_rcp  = 1.0F / d_pinv;
      d_nvars_rcp = 1.0F / d_nvars;
    }

    // Apply Zero Forcing algorithm. This is equivalent to multiplying the input signal with the
    // pseudo-inverse of the channel matrix.
    symbol_out_l0[i_subc] = ((norm_sq_ch_l1 * matched_input_l0) - (xi * matched_input_l1)) * d_pinv_rcp;
    symbol_out_l1[i_subc] = ((norm_sq_ch_l0 * matched_input_l1) - (xi_conj * matched_input_l0)) * d_pinv_rcp;

    // Calculate noise variances.
    eq_noise_vars_l0[i_subc] = noise_var_est * norm_sq_ch_l1 * d_nvars_rcp;
    eq_noise_vars_l1[i_subc] = noise_var_est * norm_sq_ch_l0 * d_nvars_rcp;
  }
}

void srsgnb::equalize_zf_2x2(equalizer_symbol_list&          eq_symbols,
                             equalizer_noise_var_list&       noise_vars,
                             const equalizer_ch_symbol_list& ch_symbols,
                             const channel_estimate&         ch_estimates,
                             float                           tx_scaling)
{
  const unsigned nof_symbols = ch_symbols.size().nof_symbols;

  // Equalize symbol by symbol.
  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_2x2_symbol(eq_symbols.get_symbol(i_symb, 0),
                           eq_symbols.get_symbol(i_symb, 1),
                           noise_vars.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 1),
                           ch_symbols.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 1),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 0, 0),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 0, 1),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 1, 0),
                           ch_estimates.get_symbol_ch_estimate(i_symb, 1, 1),
                           ch_estimates.get_noise_variance(0),
                           tx_scaling);
  }
}
