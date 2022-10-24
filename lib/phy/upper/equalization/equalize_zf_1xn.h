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
#include "srsgnb/srsvec/add.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/srsvec/zero.h"

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
  unsigned nof_subcs = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);

  // Views over the output data.
  span<float> nvars_out = noise_vars.template get_view({});
  span<cf_t>  re_out    = eq_symbols.template get_view({});

  // Zero the output buffers.
  srsvec::zero(re_out);
  srsvec::zero(nvars_out);

  for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
    // Get the port resource elements.
    span<const cf_t> port_re = ch_symbols.template get_view({i_port});

    // Get the port channel estimates.
    span<const cf_t> port_ch_estimates = ch_estimates.template get_view({i_port});

    for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
      // Use the noise vars buffer as temp buffer to compute the channel square absolute value.
      nvars_out[i_subc] += abs_sq(port_ch_estimates[i_subc]);

      // Accumulate input RE matched with the channel response.
      re_out[i_subc] += port_re[i_subc] * std::conj(port_ch_estimates[i_subc]);
    }
  }

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Calculate the reciprocal of the denominator. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_pinv           = tx_scaling * nvars_out[i_subc];
    float d_nvars          = d_pinv * tx_scaling;
    float d_pinv_rcp       = 0.0F;
    float d_pinv_rcp_nvars = 0.0F;
    if (std::isnormal(d_pinv)) {
      d_pinv_rcp       = 1.0F / d_pinv;
      d_pinv_rcp_nvars = 1.0F / d_nvars;
    }

    // Normalize the gain of the channel combined with the equalization to unity.
    re_out[i_subc] *= d_pinv_rcp;

    // Calculate noise variances.
    nvars_out[i_subc] = d_pinv_rcp_nvars * noise_var_est;
  }
}

/// Specialization for the SISO case.
template <>
inline void equalize_zf_1xn<1>(channel_equalizer::re_list&           eq_symbols,
                               channel_equalizer::noise_var_list&    noise_vars,
                               const channel_equalizer::re_list&     ch_symbols,
                               const channel_equalizer::ch_est_list& ch_estimates,
                               float                                 noise_var_est,
                               float                                 tx_scaling)
{
  unsigned nof_subcs = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);

  // Views over the output data.
  span<float> nvars_out = noise_vars.template get_view({});
  span<cf_t>  re_out    = eq_symbols.template get_view({});

  // Views over the input data.
  span<const cf_t> ch_ests = ch_estimates.get_view({});
  span<const cf_t> re_in   = ch_symbols.get_view({});

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    cf_t  ch_est    = ch_ests[i_subc] * tx_scaling;
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
    re_out[i_subc] = re_in[i_subc] * ch_est_rcp;

    // Calculate noise variances.
    nvars_out[i_subc] = noise_var_est * ch_mod_sq_rcp;
  }
}

} // namespace srsgnb