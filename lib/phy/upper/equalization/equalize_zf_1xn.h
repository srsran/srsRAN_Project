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

namespace detail {

/// \brief Processes a single OFDM symbol with the Zero-Forcing equalization algorithm.
/// \tparam RX_PORTS          Number of receive antenna ports.
/// \param[out] symbol_out    Resultant equalized symbol.
/// \param[out] eq_noise_vars Noise variances after equalization.
/// \param[in]  symbols_in    Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates  Channel estimation coefficients.
/// \param[in]  noise_var_est Estimated noise variance. It is assumed to be the same for each receive port.
/// \param[in]  tx_scaling    Transmission gain scaling factor.
template <unsigned RX_PORTS>
static void equalize_zf_1xn_symbol(span<cf_t>                                    symbol_out,
                                   span<float>                                   eq_noise_vars,
                                   const std::array<span<const cf_t>, RX_PORTS>& symbols_in,
                                   const std::array<span<const cf_t>, RX_PORTS>& ch_estimates,
                                   float                                         noise_var_est,
                                   float                                         tx_scaling)
{
  unsigned nof_subcs = symbols_in[0].size();

  for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    float ch_mod_sq = 0;
    cf_t  re_out    = 0;

    // Channel estimates and input resource elements.
    for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
      // Sum the absolute value squared of each port channel estimate.
      ch_mod_sq += abs_sq(ch_estimates[i_port][i_subc]);

      // Multiply each resource element by its matched filter, according to the channel estimates.
      re_out += symbols_in[i_port][i_subc] * std::conj(ch_estimates[i_port][i_subc]);
    }

    // Calculate the reciprocal of the denominators. Set the symbols to zero in case of division by zero, NAN of INF.
    float d_pinv           = tx_scaling * ch_mod_sq;
    float d_nvars          = d_pinv * tx_scaling;
    float d_pinv_rcp       = 0.0F;
    float d_pinv_rcp_nvars = 0.0F;
    if (std::isnormal(d_pinv)) {
      d_pinv_rcp       = 1.0F / d_pinv;
      d_pinv_rcp_nvars = 1.0F / d_nvars;
    }
    // Normalize the gain of the channel combined with the equalization to unity.
    re_out *= d_pinv_rcp;
    symbol_out[i_subc] = re_out;

    // Calculate noise variances.
    eq_noise_vars[i_subc] = noise_var_est * d_pinv_rcp_nvars;
  }
}

/// \brief Processes a single OFDM symbol with the Zero-Forcing equalization algorithm. specialization for SISO case.
/// \param[out] symbol_out    Resultant equalized symbol.
/// \param[out] eq_noise_vars Noise variances after equalization.
/// \param[in]  symbol_in     Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates  Channel estimation coefficients.
/// \param[in]  noise_var_est Estimated noise variance. It is assumed to be the same for each receive port.
/// \param[in]  tx_scaling    Transmission gain scaling factor.
void equalize_zf_1x1_symbol(span<cf_t>       symbol_out,
                            span<float>      eq_noise_vars,
                            span<const cf_t> symbol_in,
                            span<const cf_t> ch_estimates,
                            float            noise_var_est,
                            float            tx_scaling);

} // namespace detail

/// \brief Implementation of a Zero Forcing equalizer for a SIMO 1 X \c RX_PORTS channel.
/// \tparam RX_PORTS         Number of receive antenna ports.
/// \param[out] eq_symbols   Resultant equalized symbols.
/// \param[out] noise_vars   Noise variances after equalization.
/// \param[in]  ch_symbols   Channel symbols, i.e., complex samples from the receive ports.
/// \param[in]  ch_estimates Channel estimation coefficients.
/// \param[in]  tx_scaling   Transmission gain scaling factor.
template <unsigned RX_PORTS>
void equalize_zf_1xn(channel_equalizer::re_list&        eq_symbols,
                     channel_equalizer::noise_var_list& noise_vars,
                     const channel_equalizer::re_list&  ch_symbols,
                     const channel_estimate&            ch_estimates,
                     float                              tx_scaling)
{
  const unsigned nof_symbols = ch_symbols.get_dimensions_size()[channel_equalizer::re_dims::symbol];

  // Structures to hold the Rx resource elements and estimates for all ports and a single OFDM symbol.
  std::array<span<const cf_t>, RX_PORTS> port_symbols;
  std::array<span<const cf_t>, RX_PORTS> port_estimates;

  // Equalize symbol by symbol.
  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    for (unsigned i_port = 0; i_port != RX_PORTS; ++i_port) {
      port_symbols[i_port]   = ch_symbols.get_view<channel_equalizer::re_dims::symbol>({i_symb, i_port});
      port_estimates[i_port] = ch_estimates.get_symbol_ch_estimate(i_symb, i_port, 0);
    }

    detail::equalize_zf_1xn_symbol<RX_PORTS>(eq_symbols.get_view<channel_equalizer::re_dims::symbol>({i_symb, 0}),
                                             noise_vars.get_view<channel_equalizer::re_dims::symbol>({i_symb, 0}),
                                             port_symbols,
                                             port_estimates,
                                             ch_estimates.get_noise_variance(0),
                                             tx_scaling);
  }
}

/// Specialization for the SISO case.
template <>
inline void equalize_zf_1xn<1>(channel_equalizer::re_list&        eq_symbols,
                               channel_equalizer::noise_var_list& noise_vars,
                               const channel_equalizer::re_list&  ch_symbols,
                               const channel_estimate&            ch_estimates,
                               float                              tx_scaling)
{
  const unsigned nof_symbols        = ch_symbols.get_dimensions_size()[channel_equalizer::re_dims::symbol];
  const float    noise_var_estimate = ch_estimates.get_noise_variance(0);

  // Equalize symbol by symbol.
  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    detail::equalize_zf_1x1_symbol(eq_symbols.get_view<channel_equalizer::re_dims::symbol>({i_symb, 0}),
                                   noise_vars.get_view<channel_equalizer::re_dims::symbol>({i_symb, 0}),
                                   ch_symbols.get_view<channel_equalizer::re_dims::symbol>({i_symb, 0}),
                                   ch_estimates.get_symbol_ch_estimate(i_symb),
                                   noise_var_estimate,
                                   tx_scaling);
  }
}

} // namespace srsgnb