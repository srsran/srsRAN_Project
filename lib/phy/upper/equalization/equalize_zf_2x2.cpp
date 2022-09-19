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

static inline void equalize_zf_2x2_symbol(span<cf_t>       symbol_out_l0,
                                          span<cf_t>       symbol_out_l1,
                                          span<float>      noise_vars_l0,
                                          span<float>      noise_vars_l1,
                                          span<const cf_t> symbol_in_p0,
                                          span<const cf_t> symbol_in_p1,
                                          span<const cf_t> ch_estimates_p0_l0,
                                          span<const cf_t> ch_estimates_p0_l1,
                                          span<const cf_t> ch_estimates_p1_l0,
                                          span<const cf_t> ch_estimates_p1_l1,
                                          float            noise_var_p0,
                                          float            noise_var_p1,
                                          float            tx_scaling)
{
  const unsigned nof_subcs = symbol_in_p0.size();

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Channel estimates.
    const cf_t ch_p0_l0 = ch_estimates_p0_l0[i_subc];
    const cf_t ch_p0_l1 = ch_estimates_p0_l1[i_subc];
    const cf_t ch_p1_l0 = ch_estimates_p1_l0[i_subc];
    const cf_t ch_p1_l1 = ch_estimates_p1_l1[i_subc];

    // Conjugated channel estimates.
    const cf_t ch_p0_l0_conj = std::conj(ch_p0_l0);
    const cf_t ch_p0_l1_conj = std::conj(ch_p0_l1);
    const cf_t ch_p1_l0_conj = std::conj(ch_p1_l0);
    const cf_t ch_p1_l1_conj = std::conj(ch_p1_l1);

    // Absolute squares of the channel estimates.
    const float ch_p0_l0_mod_sq = std::real(ch_p0_l0 * ch_p0_l0_conj);
    const float ch_p0_l1_mod_sq = std::real(ch_p0_l1 * ch_p0_l1_conj);
    const float ch_p1_l0_mod_sq = std::real(ch_p1_l0 * ch_p1_l0_conj);
    const float ch_p1_l1_mod_sq = std::real(ch_p1_l1 * ch_p1_l1_conj);

    // Resource Elements coming from the Rx ports.
    const cf_t re_in_p0 = symbol_in_p0[i_subc];
    const cf_t re_in_p1 = symbol_in_p1[i_subc];

    // Calculate the product of the channel matrix and its hermitian transpose.
    const cf_t  xi        = (ch_p0_l0_conj * ch_p0_l1) + (ch_p1_l0_conj * ch_p1_l1);
    const cf_t  xi_conj   = std::conj(xi);
    const float xi_mod_sq = std::real(xi * xi_conj);

    const float norm_sq_ch_l0 = ch_p0_l0_mod_sq + ch_p1_l0_mod_sq;
    const float norm_sq_ch_l1 = ch_p0_l1_mod_sq + ch_p1_l1_mod_sq;

    // Calculate the Moore-Penrose pseudo-inverse of the channel matrix.
    const cf_t pinv_l0_p0 = norm_sq_ch_l1 * ch_p0_l0_conj - xi * ch_p0_l1_conj;
    const cf_t pinv_l0_p1 = norm_sq_ch_l1 * ch_p1_l0_conj - xi * ch_p1_l1_conj;
    const cf_t pinv_l1_p0 = norm_sq_ch_l0 * ch_p0_l1_conj - xi_conj * ch_p0_l0_conj;
    const cf_t pinv_l1_p1 = norm_sq_ch_l0 * ch_p1_l1_conj - xi_conj * ch_p1_l0_conj;

    // Calculate the reciprocal of the denominator. Set the symbols to zero in case of division by zero, NAN of INF.
    const float d_pinv        = tx_scaling * ((norm_sq_ch_l0 * norm_sq_ch_l1) - xi_mod_sq);
    float       d_pinv_rcp    = 0.0F;
    float       d_pinv_rcp_sq = 0.0F;
    if (std::isnormal(d_pinv)) {
      d_pinv_rcp    = 1.0F / d_pinv;
      d_pinv_rcp_sq = d_pinv_rcp * d_pinv_rcp;
    }

    // Apply Zero Forcing algorithm.
    symbol_out_l0[i_subc] = ((pinv_l0_p0 * re_in_p0) + (pinv_l0_p1 * re_in_p1)) * d_pinv_rcp;
    symbol_out_l1[i_subc] = ((pinv_l1_p0 * re_in_p0) + (pinv_l1_p1 * re_in_p1)) * d_pinv_rcp;

    // Calculate noise variances.
    noise_vars_l0[i_subc] = ((abs_sq(pinv_l0_p0) * noise_var_p0) + (abs_sq(pinv_l0_p1) * noise_var_p1)) * d_pinv_rcp_sq;
    noise_vars_l1[i_subc] = ((abs_sq(pinv_l1_p0) * noise_var_p0) + (abs_sq(pinv_l1_p1) * noise_var_p1)) * d_pinv_rcp_sq;
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
                           ch_estimates.get_noise_variance(1),
                           tx_scaling);
  }
}
