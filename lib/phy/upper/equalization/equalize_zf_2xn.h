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
/// \brief Zero Forcing equalizer algorithm for a MIMO 2 X N channel.

#pragma once

#include "../../../srsvec/simd.h"
#include "srsran/srsvec/zero.h"

namespace srsran {

/// \brief Implementation of a Zero Forcing equalizer algorithm for a MIMO 2 X N channel.
///
/// The number of receive ports must be equal to or greater than two.
///
/// \param[out] eq_symbols     Equalized channel symbols.
/// \param[out] noise_vars     Noise variances after equalization.
/// \param[in]  ch_symbols     Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates   Estimated channel coefficients.
/// \param[in]  noise_var_est  Estimated noise variance. It is assumed to be the same for all receive ports.
/// \param[in]  tx_scaling     Transmission gain scaling factor.
template <unsigned RX_PORTS>
void equalize_zf_2xn(span<cf_t>                            eq_symbols,
                     span<float>                           noise_vars,
                     const re_buffer_reader<cbf16_t>&      ch_symbols,
                     const channel_equalizer::ch_est_list& ch_estimates_,
                     float                                 noise_var_est,
                     float                                 tx_scaling)
{
  static constexpr unsigned nof_layers = 2;
  static constexpr unsigned nof_ports  = RX_PORTS;
  static_assert(nof_ports >= nof_layers, "The number of ports must be greater than or equal to the number of layers.");

  unsigned       i_re   = 0;
  const unsigned nof_re = ch_symbols.get_nof_re();

  // Skip processing if the noise variance is NaN, infinity or negative.
  if (!std::isnormal(noise_var_est) || (noise_var_est < 0.0F)) {
    srsvec::zero(eq_symbols);
    std::fill(noise_vars.begin(), noise_vars.end(), std::numeric_limits<float>::infinity());
    return;
  }

  // Views over input channel symbols, organized by receive port.
  std::array<span<const cbf16_t>, nof_ports> symbols_in;
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    symbols_in[i_port] = ch_symbols.get_slice(i_port);
  }

  // Views over channel estimates, organized by receive port and transmit layer.
  std::array<std::array<span<const cbf16_t>, nof_layers>, nof_ports> ch_estimates;
  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      ch_estimates[i_port][i_layer] = ch_estimates_.get_channel(i_port, i_layer);
    }
  }

#if SRSRAN_SIMD_CF_SIZE
  simd_f_t tx_scaling_simd    = srsran_simd_f_set1(tx_scaling);
  simd_f_t noise_var_est_simd = srsran_simd_f_set1(noise_var_est);
  simd_f_t zero_simd          = srsran_simd_f_zero();
  simd_f_t infinity_simd      = srsran_simd_f_set1(std::numeric_limits<float>::infinity());

  for (unsigned nof_re_simd = (nof_re / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE; i_re != nof_re_simd;
       i_re += SRSRAN_SIMD_CF_SIZE) {
    // Channel estimates, and their conjugate.
    std::array<std::array<simd_cf_t, nof_layers>, nof_ports> ch;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        ch[i_port][i_layer] = srsran_simd_cbf16_loadu(ch_estimates[i_port][i_layer].data() + i_re);
      }
    }

    // Input Resource Elements.
    std::array<simd_cf_t, nof_ports> re_in;
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      re_in[i_port] = srsran_simd_cbf16_loadu(symbols_in[i_port].data() + i_re);
    }

    // Calculate the product of the channel matrix (recall, it's an Nx2 matrix) and its hermitian transpose.
    // The diagonal coefficients are the squared norms of the channel matrix column vectors.
    simd_f_t norm_sq_ch[nof_layers];
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      simd_f_t sum = srsran_simd_f_zero();
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        sum = srsran_simd_f_add(sum, srsran_simd_cf_norm_sq(ch[i_port][i_layer]));
      }
      norm_sq_ch[i_layer] = sum;
    }

    // Calculate the anti-diagonal coefficients, which are xi and xi_conj.
    simd_cf_t xi = srsran_simd_cf_zero();
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      xi = srsran_simd_cf_add(xi, srsran_simd_cf_conjprod(ch[i_port][1], ch[i_port][0]));
    }
    simd_cf_t xi_conj   = srsran_simd_cf_conj(xi);
    simd_f_t  xi_mod_sq = srsran_simd_cf_norm_sq(xi);

    // Apply a matched filter for each transmit layer to the input signal.
    std::array<simd_cf_t, nof_layers> matched_input;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      simd_cf_t sum = srsran_simd_cf_zero();
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        sum = srsran_simd_cf_add(sum, srsran_simd_cf_conjprod(re_in[i_port], ch[i_port][i_layer]));
      }
      matched_input[i_layer] = sum;
    }

    // Calculate the denominators.
    simd_f_t d_pinv  = srsran_simd_f_mul(tx_scaling_simd,
                                        srsran_simd_f_sub(srsran_simd_f_mul(norm_sq_ch[0], norm_sq_ch[1]), xi_mod_sq));
    simd_f_t d_nvars = srsran_simd_f_mul(tx_scaling_simd, d_pinv);

    // Calculate the reciprocal of the denominators.
    simd_f_t d_pinv_rcp  = srsran_simd_f_rcp(d_pinv);
    simd_f_t d_nvars_rcp = srsran_simd_f_rcp(d_nvars);

    // Apply Zero Forcing algorithm. This is equivalent to multiplying the input signal with the pseudo-inverse of the
    // channel matrix.
    simd_cf_t symbols_out_l0 = srsran_simd_cf_sub(srsran_simd_cf_mul(matched_input[0], norm_sq_ch[1]),
                                                  srsran_simd_cf_prod(xi, matched_input[1]));
    symbols_out_l0           = srsran_simd_cf_mul(symbols_out_l0, d_pinv_rcp);
    simd_cf_t symbols_out_l1 = srsran_simd_cf_sub(srsran_simd_cf_mul(matched_input[1], norm_sq_ch[0]),
                                                  srsran_simd_cf_prod(xi_conj, matched_input[0]));
    symbols_out_l1           = srsran_simd_cf_mul(symbols_out_l1, d_pinv_rcp);

    // Calculate post-equalization noise variances.
    simd_f_t eq_noise_vars_l0 = srsran_simd_f_mul(srsran_simd_f_mul(norm_sq_ch[1], noise_var_est_simd), d_nvars_rcp);
    simd_f_t eq_noise_vars_l1 = srsran_simd_f_mul(srsran_simd_f_mul(norm_sq_ch[0], noise_var_est_simd), d_nvars_rcp);

    // Return values in case of abnormal computation parameters. These include negative, zero, NAN or INF noise
    // variances and zero, NAN or INF channel estimation coefficients.
    {
      // Detect abnormal computation parameters. This detects whenever the channel estimate is zero or NaN.
      simd_sel_t isnormal_mask = srsran_simd_f_max(d_pinv, zero_simd);

      // Detect abnormal computation parameters. This detects whenever the channel estimate is infinity.
      isnormal_mask = srsran_simd_sel_and(isnormal_mask, srsran_simd_f_max(infinity_simd, d_pinv));

      symbols_out_l0   = srsran_simd_cf_select(srsran_simd_cf_zero(), symbols_out_l0, isnormal_mask);
      symbols_out_l1   = srsran_simd_cf_select(srsran_simd_cf_zero(), symbols_out_l1, isnormal_mask);
      eq_noise_vars_l0 = srsran_simd_f_select(infinity_simd, eq_noise_vars_l0, isnormal_mask);
      eq_noise_vars_l1 = srsran_simd_f_select(infinity_simd, eq_noise_vars_l1, isnormal_mask);
    }

    // Revert layer mapping for the equalized symbols.
    simd_cf_t symbols_low  = srsran_simd_cf_interleave_low(symbols_out_l0, symbols_out_l1);
    simd_cf_t symbols_high = srsran_simd_cf_interleave_high(symbols_out_l0, symbols_out_l1);
    srsran_simd_cfi_storeu(eq_symbols.data() + nof_layers * i_re, symbols_low);
    srsran_simd_cfi_storeu(eq_symbols.data() + nof_layers * i_re + SRSRAN_SIMD_CF_SIZE, symbols_high);

    // Revert layer mapping for the estimated noise variance.
    simd_f_t eq_noise_vars_low  = srsran_simd_f_interleave_low(eq_noise_vars_l0, eq_noise_vars_l1);
    simd_f_t eq_noise_vars_high = srsran_simd_f_interleave_high(eq_noise_vars_l0, eq_noise_vars_l1);
    srsran_simd_f_storeu(noise_vars.data() + nof_layers * i_re, eq_noise_vars_low);
    srsran_simd_f_storeu(noise_vars.data() + nof_layers * i_re + SRSRAN_SIMD_CF_SIZE, eq_noise_vars_high);
  }
#endif // SRSRAN_SIMD_CF_SIZE

  // Iterate each RE.
  for (; i_re != nof_re; ++i_re) {
    // Channel estimates, and their conjugate.
    std::array<std::array<cf_t, nof_layers>, nof_ports> ch;
    std::array<std::array<cf_t, nof_layers>, nof_ports> ch_conj;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        ch[i_port][i_layer]      = to_cf(ch_estimates[i_port][i_layer][i_re]);
        ch_conj[i_port][i_layer] = std::conj(ch[i_port][i_layer]);
      }
    }

    // Input Resource Elements.
    std::array<cf_t, nof_ports> re_in;
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      re_in[i_port] = to_cf(symbols_in[i_port][i_re]);
    }

    // Calculate the product of the channel matrix (recall, it's an Nx2 matrix) and its hermitian transpose.
    // The diagonal coefficients are the squared norms of the channel matrix column vectors.
    std::array<float, nof_layers> norm_sq_ch;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      float sum = 0;
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        sum += std::norm(ch[i_port][i_layer]);
      }
      norm_sq_ch[i_layer] = sum;
    }

    // Calculate the anti-diagonal coefficients, which are xi and xi_conj.
    cf_t xi = cf_t();
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      xi += ch_conj[i_port][0] * ch[i_port][1];
    }
    cf_t  xi_conj   = std::conj(xi);
    float xi_mod_sq = std::norm(xi);

    // Apply a matched filter for each transmit layer to the input signal.
    std::array<cf_t, nof_layers> matched_input;
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      matched_input[i_layer] = 0;
      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        matched_input[i_layer] += ch_conj[i_port][i_layer] * re_in[i_port];
      }
    }

    // Calculate the denominators.
    float d_pinv  = tx_scaling * ((norm_sq_ch[0] * norm_sq_ch[1]) - xi_mod_sq);
    float d_nvars = tx_scaling * d_pinv;

    // Return values in case of abnormal computation parameters. These include negative, zero, NAN or INF noise
    // variances and zero, NAN or INF channel estimation coefficients.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      eq_symbols[nof_layers * i_re + i_layer] = 0;
      noise_vars[nof_layers * i_re + i_layer] = std::numeric_limits<float>::infinity();
    }

    if (std::isnormal(d_pinv)) {
      float d_pinv_rcp  = 1.0F / d_pinv;
      float d_nvars_rcp = 1.0F / d_nvars;

      // Apply Zero Forcing algorithm. This is equivalent to multiplying the input signal with the pseudo-inverse of the
      // channel matrix.
      eq_symbols[nof_layers * i_re + 0] = ((norm_sq_ch[1] * matched_input[0]) - (xi * matched_input[1])) * d_pinv_rcp;
      eq_symbols[nof_layers * i_re + 1] =
          ((norm_sq_ch[0] * matched_input[1]) - (xi_conj * matched_input[0])) * d_pinv_rcp;

      // Calculate post-equalization noise variances.
      noise_vars[nof_layers * i_re + 0] = noise_var_est * norm_sq_ch[1] * d_nvars_rcp;
      noise_vars[nof_layers * i_re + 1] = noise_var_est * norm_sq_ch[0] * d_nvars_rcp;
    }
  }
}

} // namespace srsran
