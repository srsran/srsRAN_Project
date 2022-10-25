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
/// \brief Zero Forcing equalizer algorithm for a SIMO 1 X N channel.

#pragma once

#include "channel_equalizer_zf_impl.h"

namespace srsgnb {

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
  span<float> nvars_out   = noise_vars.template get_view({});
  span<cf_t>  symbols_out = eq_symbols.template get_view({});

  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
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

    // Calculate the reciprocal of the denominator. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_pinv           = tx_scaling * ch_mod_sq;
    float d_nvars          = d_pinv * tx_scaling;
    float d_pinv_rcp       = 0.0F;
    float d_pinv_rcp_nvars = 0.0F;
    if (std::isnormal(d_pinv)) {
      d_pinv_rcp       = 1.0F / d_pinv;
      d_pinv_rcp_nvars = 1.0F / d_nvars;
    }

    // Normalize the gain of the channel combined with the equalization to unity.
    symbols_out[i_re] = re_out * d_pinv_rcp;

    // Calculate noise variances.
    nvars_out[i_re] = d_pinv_rcp_nvars * noise_var_est;
  }
}

} // namespace srsgnb
