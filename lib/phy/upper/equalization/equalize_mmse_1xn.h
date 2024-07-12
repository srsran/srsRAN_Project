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
/// \brief Minimum Mean Square Error equalizer algorithm for a SIMO 1 X N channel.

#pragma once

#include "../../../srsvec/simd.h"
#include "srsran/srsvec/zero.h"

namespace srsran {

/// \brief Implementation of an MMSE equalizer for a SIMO 1 X \c RX_PORTS channel.
/// \tparam RX_PORTS          Number of receive antenna ports.
/// \param[out] eq_symbols    Resultant equalized symbols.
/// \param[out] noise_vars    Noise variances after equalization.
/// \param[in]  ch_symbols    Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates  Channel estimation coefficients.
/// \param[in]  noise_var_est Estimated noise variance for each port.
/// \param[in]  tx_scaling    Transmission gain scaling factor.
template <unsigned RX_PORTS>
void equalize_mmse_1xn(span<cf_t>                            symbols_out,
                       span<float>                           nvars_out,
                       const re_buffer_reader<cbf16_t>&      ch_symbols,
                       const channel_equalizer::ch_est_list& ch_estimates,
                       span<const float>                     noise_var_est,
                       float                                 tx_scaling)
{
  // Number of RE to process.
  unsigned nof_re = ch_symbols.get_nof_re();

  unsigned i_re = 0;

  for (; i_re != nof_re; ++i_re) {
    float ch_mod_sq = 0.0F;
    float nvar_acc  = 0.0F;
    cf_t  re_out    = cf_t();

    for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
      // Get the input RE and channel estimate coefficient.
      cf_t re_in  = to_cf(ch_symbols.get_slice(i_port)[i_re]);
      cf_t ch_est = to_cf(ch_estimates.get_channel(i_port, 0)[i_re]) * tx_scaling;

      // Compute the channel square norm.
      float ch_est_norm = std::norm(ch_est);

      if (std::isnormal(ch_est_norm) && std::isnormal(noise_var_est[i_port]) && (noise_var_est[i_port] > 0)) {
        // Accumulate the channel square absolute values.
        ch_mod_sq += ch_est_norm;

        // Accumulate the noise variance weighted with the channel estimate norm.
        nvar_acc += ch_est_norm * noise_var_est[i_port];

        // Apply the matched channel filter to each received RE and accumulate the results.
        re_out += re_in * std::conj(ch_est);
      }
    }

    // Return values in case of abnormal computation parameters. These include negative, zero, NAN or INF noise
    // variances and zero, NAN or INF channel estimation coefficients.
    symbols_out[i_re] = 0;
    nvars_out[i_re]   = std::numeric_limits<float>::infinity();

    if (std::isnormal(ch_mod_sq) && std::isnormal(nvar_acc)) {
      // Calculate the reciprocal of the equalizer denominator.
      float d_pinv_rcp = 1.0F / (ch_mod_sq * ch_mod_sq + nvar_acc);

      // Normalize the gain of the channel combined with the equalization to unity.
      symbols_out[i_re] = re_out * ch_mod_sq * d_pinv_rcp;

      // Calculate noise variances.
      nvars_out[i_re] = nvar_acc * d_pinv_rcp;
    }
  }
}

} // namespace srsran
