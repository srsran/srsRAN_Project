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
/// \brief Zero Forcing equalizer algorithm for a SIMO 1 X N channel.

#pragma once

#include "channel_equalizer_zf_impl.h"

#if defined(__AVX2__) || defined(HAVE_NEON)
#include "../../../srsvec/simd.h"
#endif // __AVX2__ || HAVE_NEON

namespace srsran {

/// \brief Implementation of a Zero Forcing equalizer for a SIMO 1 X \c RX_PORTS channel.
/// \tparam RX_PORTS         Number of receive antenna ports.
/// \param[out] eq_symbols   Resultant equalized symbols.
/// \param[out] noise_vars   Noise variances after equalization.
/// \param[in]  ch_symbols   Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates Channel estimation coefficients.
/// \param[in]  noise_var_est Estimated noise variance. It is assumed to be the same for each receive port.
/// \param[in]  tx_scaling   Transmission gain scaling factor.
template <unsigned RX_PORTS>
void equalize_zf_1xn(channel_equalizer::re_list&           eq_symbols,
                     channel_equalizer::noise_var_list&    noise_vars,
                     const channel_equalizer::re_list&     ch_symbols,
                     const channel_equalizer::ch_est_list& ch_estimates,
                     float                                 noise_var_est,
                     float                                 tx_scaling)
{
  // Number of RE to process.
  unsigned nof_re = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);

  // Views over the output data.
  span<float> nvars_out   = noise_vars.get_view({});
  span<cf_t>  symbols_out = eq_symbols.get_view({});

  unsigned i_re = 0;

#if defined(__AVX2__) || defined(HAVE_NEON)
  // Views over the input data.
  std::array<span<const cf_t>, MAX_PORTS> port_symbols;
  std::array<span<const cf_t>, MAX_PORTS> port_ests;

  for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
    port_symbols[i_port] = ch_symbols.get_view({i_port});
    port_ests[i_port]    = ch_estimates.get_view({i_port, 0});
  }

  // Create registers with zero and infinity values.
  simd_f_t zero     = srsran_simd_f_zero();
  simd_f_t infinity = srsran_simd_f_set1(std::numeric_limits<float>::infinity());

  for (unsigned i_re_end = (nof_re / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i_re != i_re_end;
       i_re += SRSRAN_SIMD_CF_SIZE) {
    simd_f_t  ch_mod_sq = srsran_simd_f_zero();
    simd_cf_t re_out    = srsran_simd_cf_zero();

    for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
      // Get the input RE and channel estimate coefficients.
      simd_cf_t re_in  = srsran_simd_cfi_loadu(&(port_symbols[i_port][i_re]));
      simd_cf_t ch_est = srsran_simd_cfi_loadu(&(port_ests[i_port][i_re]));

      // Compute the channel square norm, by accumulating the channel square absolute values.
      ch_mod_sq = srsran_simd_f_add(ch_mod_sq, srsran_simd_cf_norm_sq(ch_est));

      // Apply the matched channel filter to each received RE and accumulate the results.
      re_out = srsran_simd_cf_add(re_out, srsran_simd_cf_conjprod(re_in, ch_est));
    }

    // Calculate the denominator of the pseudo-inverse.
    simd_f_t d_pinv = srsran_simd_f_mul(srsran_simd_f_set1(tx_scaling), ch_mod_sq);

    // Compute reciprocal of the denominator.
    simd_f_t d_pinv_rcp = srsran_simd_f_rcp(d_pinv);

    // Detect abnormal computation parameters. This detects whenever the channel estimate is zero or NaN.
    simd_sel_t isnormal_mask = srsran_simd_f_max(d_pinv, zero);

    // Detect abnormal computation parameters. This detects whenever the channel estimate is infinity.
    isnormal_mask = srsran_simd_sel_and(isnormal_mask, srsran_simd_f_max(infinity, d_pinv));

    // Calculate noise variances.
    simd_f_t vars_out = srsran_simd_f_mul(d_pinv_rcp, srsran_simd_f_set1(noise_var_est / tx_scaling));

    // Detect whenever the post-equalization noise variances are zero, negative or NaN.
    isnormal_mask = srsran_simd_sel_and(isnormal_mask, srsran_simd_f_max(vars_out, zero));

    // Detect whenever the post-equalizatino noise variances are set to infinity.
    isnormal_mask = srsran_simd_sel_and(isnormal_mask, srsran_simd_f_max(infinity, vars_out));

    // If abnormal calculation parameters are detected, the noise variances are set to infinity.
    srsran_simd_f_storeu(&nvars_out[i_re], srsran_simd_f_select(infinity, vars_out, isnormal_mask));

    // Normalize the gain of the channel combined with the equalization to unity.
    re_out = srsran_simd_cf_mul(re_out, d_pinv_rcp);

    // If abnormal calculation parameters are detected, the equalized symbols are set to zero.
    srsran_simd_cfi_storeu(&symbols_out[i_re],
                           srsran_simd_cf_select(srsran_simd_cf_set1({0, 0}), re_out, isnormal_mask));
  }
#endif // __AVX2__ || HAVE_NEON

  for (; i_re != nof_re; ++i_re) {
    float ch_mod_sq = 0.0F;
    cf_t  re_out    = 0.0F;

    for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
      // Get the input RE and channel estimate coefficient.
      cf_t re_in  = ch_symbols[{i_re, i_port}];
      cf_t ch_est = ch_estimates[{i_re, i_port}];

      // Compute the channel square norm, by accumulating the channel square absolute values.
      ch_mod_sq += abs_sq(ch_est);

      // Apply the matched channel filter to each received RE and accumulate the results.
      re_out += re_in * std::conj(ch_est);
    }

    // Return values in case of abnormal computation parameters. These include negative, zero, NAN or INF noise
    // variances and zero, NAN or INF channel estimation coefficients.
    symbols_out[i_re] = 0;
    nvars_out[i_re]   = std::numeric_limits<float>::infinity();

    float d_pinv = tx_scaling * ch_mod_sq;

    if (std::isnormal(d_pinv) && std::isnormal(noise_var_est) && (noise_var_est > 0.0F)) {
      // Calculate the reciprocal of the denominator.
      float d_pinv_rcp = 1.0F / d_pinv;

      // Normalize the gain of the channel combined with the equalization to unity.
      symbols_out[i_re] = re_out * d_pinv_rcp;

      // Calculate noise variances.
      nvars_out[i_re] = d_pinv_rcp * (noise_var_est / tx_scaling);
    }
  }
}

} // namespace srsran
