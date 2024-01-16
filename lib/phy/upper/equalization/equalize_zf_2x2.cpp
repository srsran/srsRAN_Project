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

/// \file
/// \brief Zero Forcing equalization function implementation for a MIMO 2 X 2 channel.

#include "equalize_zf_2x2.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

void srsran::equalize_zf_2x2(channel_equalizer::re_list&           eq_symbols,
                             channel_equalizer::noise_var_list&    noise_vars,
                             const channel_equalizer::re_list&     ch_symbols,
                             const channel_equalizer::ch_est_list& ch_estimates,
                             float                                 noise_var_est,
                             float                                 tx_scaling)
{
  // Views over equalized symbols, organized by transmit layer.
  span<cf_t> symbols_out_l0 = eq_symbols.get_view<>({0});
  span<cf_t> symbols_out_l1 = eq_symbols.get_view<>({1});

  // Views over post-equalization noise variances, organized by transmit layer.
  span<float> eq_noise_vars_l0 = noise_vars.get_view<>({0});
  span<float> eq_noise_vars_l1 = noise_vars.get_view<>({1});

  // Views over input channel symbols, organized by receive port.
  span<const cf_t> symbols_in_p0 = ch_symbols.get_view<>({0});
  span<const cf_t> symbols_in_p1 = ch_symbols.get_view<>({1});

  // Views over channel estimates, organized by receive port and transmit layer.
  span<const cf_t> ch_estimates_p0_l0 = ch_estimates.get_view<>({0, 0});
  span<const cf_t> ch_estimates_p0_l1 = ch_estimates.get_view<>({0, 1});
  span<const cf_t> ch_estimates_p1_l0 = ch_estimates.get_view<>({1, 0});
  span<const cf_t> ch_estimates_p1_l1 = ch_estimates.get_view<>({1, 1});

  const unsigned nof_re = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);

  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    // Channel estimates.
    cf_t ch_p0_l0 = ch_estimates_p0_l0[i_re];
    cf_t ch_p0_l1 = ch_estimates_p0_l1[i_re];
    cf_t ch_p1_l0 = ch_estimates_p1_l0[i_re];
    cf_t ch_p1_l1 = ch_estimates_p1_l1[i_re];

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

    // Input Resource Elements.
    cf_t re_in_p0 = symbols_in_p0[i_re];
    cf_t re_in_p1 = symbols_in_p1[i_re];

    // Calculate the product of the channel matrix and its hermitian transpose.
    // The diagonal coefficients are the squared norms of the channel matrix column vectors.
    float norm_sq_ch_l0 = ch_p0_l0_mod_sq + ch_p1_l0_mod_sq;
    float norm_sq_ch_l1 = ch_p0_l1_mod_sq + ch_p1_l1_mod_sq;

    // Calculate the anti-diagonal coefficients, which are -xi and -xi_conj.
    cf_t  xi        = (ch_p0_l0_conj * ch_p0_l1) + (ch_p1_l0_conj * ch_p1_l1);
    cf_t  xi_conj   = std::conj(xi);
    float xi_mod_sq = abs_sq(xi);

    // Apply a matched filter for each transmit layer to the input signal.
    cf_t matched_input_l0 = (ch_p0_l0_conj * re_in_p0) + (ch_p1_l0_conj * re_in_p1);
    cf_t matched_input_l1 = (ch_p0_l1_conj * re_in_p0) + (ch_p1_l1_conj * re_in_p1);

    // Calculate the reciprocal of the denominators. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_pinv  = tx_scaling * ((norm_sq_ch_l0 * norm_sq_ch_l1) - xi_mod_sq);
    float d_nvars = tx_scaling * d_pinv;

    // Return values in case of abnormal computation parameters. These include negative, zero, NAN or INF noise
    // variances and zero, NAN or INF channel estimation coefficients.
    symbols_out_l0[i_re]   = 0;
    symbols_out_l1[i_re]   = 0;
    eq_noise_vars_l0[i_re] = std::numeric_limits<float>::infinity();
    eq_noise_vars_l1[i_re] = std::numeric_limits<float>::infinity();

    if (std::isnormal(d_pinv) && std::isnormal(noise_var_est) && (noise_var_est > 0.0F)) {
      float d_pinv_rcp  = 1.0F / d_pinv;
      float d_nvars_rcp = 1.0F / d_nvars;

      // Apply Zero Forcing algorithm. This is equivalent to multiplying the input signal with the pseudo-inverse of the
      // channel matrix.
      symbols_out_l0[i_re] = ((norm_sq_ch_l1 * matched_input_l0) - (xi * matched_input_l1)) * d_pinv_rcp;
      symbols_out_l1[i_re] = ((norm_sq_ch_l0 * matched_input_l1) - (xi_conj * matched_input_l0)) * d_pinv_rcp;

      // Calculate post-equalization noise variances.
      eq_noise_vars_l0[i_re] = noise_var_est * norm_sq_ch_l1 * d_nvars_rcp;
      eq_noise_vars_l1[i_re] = noise_var_est * norm_sq_ch_l0 * d_nvars_rcp;
    }
  }
}
