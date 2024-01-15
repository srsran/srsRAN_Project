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
/// \brief Zero Forcing equalization function implementation.

#include "channel_equalizer_zf_impl.h"
#include "equalize_zf_1xn.h"
#include "equalize_zf_2x2.h"

using namespace srsran;

/// Assert that the dimensions of the equalizer input and output data structures match.
static inline void assert_sizes(channel_equalizer::re_list&           eq_symbols,
                                channel_equalizer::noise_var_list&    eq_noise_vars,
                                const channel_equalizer::re_list&     ch_symbols,
                                const channel_equalizer::ch_est_list& ch_estimates,
                                span<const float>                     noise_var_estimates)
{
  // Rx symbols dimensions.
  unsigned ch_symb_nof_re       = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);
  unsigned ch_symb_nof_rx_ports = ch_symbols.get_dimension_size(channel_equalizer::re_list::dims::slice);

  // Output symbols dimensions.
  unsigned eq_symb_nof_re        = eq_symbols.get_dimension_size(channel_equalizer::re_list::dims::re);
  unsigned eq_symb_nof_tx_layers = eq_symbols.get_dimension_size(channel_equalizer::re_list::dims::slice);

  // Noise var estimates dimensions.
  unsigned nvar_ests_nof_rx_ports = noise_var_estimates.size();

  // Output noise vars dimensions.
  unsigned eq_nvars_nof_re        = eq_noise_vars.get_dimension_size(channel_equalizer::re_list::dims::re);
  unsigned eq_nvars_nof_tx_layers = eq_noise_vars.get_dimension_size(channel_equalizer::re_list::dims::slice);

  // Channel estimates dimensions.
  unsigned ch_ests_nof_re        = ch_estimates.get_dimension_size(channel_equalizer::ch_est_list::dims::re);
  unsigned ch_ests_nof_rx_ports  = ch_estimates.get_dimension_size(channel_equalizer::ch_est_list::dims::rx_port);
  unsigned ch_ests_nof_tx_layers = ch_estimates.get_dimension_size(channel_equalizer::ch_est_list::dims::tx_layer);

  // Assert that the number of Resource Elements per port matches for all inputs and outputs.
  srsran_assert((ch_symb_nof_re == ch_ests_nof_re) && (ch_symb_nof_re == eq_symb_nof_re) &&
                    (ch_symb_nof_re == eq_nvars_nof_re),
                "Number of single port Resource Elements does not match: \n"
                "Received symbols RE:\t {}\n"
                "Output symbols RE:\t {}\n"
                "Output noise variance RE: \t {}\n"
                "Channel estimates RE:\t {}",
                ch_symb_nof_re,
                eq_symb_nof_re,
                eq_nvars_nof_re,
                ch_ests_nof_re);

  // Assert that the number of receive ports matches.
  srsran_assert((ch_ests_nof_rx_ports == ch_symb_nof_rx_ports) && (ch_ests_nof_rx_ports == nvar_ests_nof_rx_ports),
                "Number of Rx ports does not match: \n"
                "Received symbols Rx ports:\t {}\n"
                "Noise variance estimates Rx ports: \t {}\n"
                "Channel estimates Rx ports:\t {}",
                ch_symb_nof_rx_ports,
                nvar_ests_nof_rx_ports,
                ch_ests_nof_rx_ports);

  // Assert that the number of transmit layers matches.
  srsran_assert((ch_ests_nof_tx_layers == eq_symb_nof_tx_layers) && (ch_ests_nof_tx_layers == eq_nvars_nof_tx_layers),
                "Number of Tx layers does not match: \n"
                "Output symbols Tx layers:\t {}\n"
                "Output noise variances Tx layers: \t {}\n"
                "Channel estimates Tx layers:\t {}",
                eq_symb_nof_tx_layers,
                eq_nvars_nof_tx_layers,
                ch_ests_nof_tx_layers);
}

/// Calls the equalizer function for receive spatial diversity with the appropriate number of receive ports.
template <unsigned NOF_PORTS>
void equalize_zf_single_tx_layer(unsigned                              nof_ports,
                                 channel_equalizer::re_list&           eq_symbols,
                                 channel_equalizer::noise_var_list&    eq_noise_vars,
                                 const channel_equalizer::re_list&     ch_symbols,
                                 const channel_equalizer::ch_est_list& ch_estimates,
                                 float                                 noise_var,
                                 float                                 tx_scaling)
{
  if (NOF_PORTS != nof_ports) {
    // Recursive call.
    return equalize_zf_single_tx_layer<NOF_PORTS - 1>(
        nof_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
  }

  // Perform equalization.
  equalize_zf_1xn<NOF_PORTS>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

/// Specialization for a single receive port.
template <>
void equalize_zf_single_tx_layer<1>(unsigned /**/,
                                    channel_equalizer::re_list&           eq_symbols,
                                    channel_equalizer::noise_var_list&    eq_noise_vars,
                                    const channel_equalizer::re_list&     ch_symbols,
                                    const channel_equalizer::ch_est_list& ch_estimates,
                                    float                                 noise_var,
                                    float                                 tx_scaling)
{
  // Perform equalization.
  equalize_zf_1xn<1>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

void channel_equalizer_zf_impl::equalize(re_list&           eq_symbols,
                                         noise_var_list&    eq_noise_vars,
                                         const re_list&     ch_symbols,
                                         const ch_est_list& ch_estimates,
                                         span<const float>  noise_var_estimates,
                                         float              tx_scaling)
{
  // Make sure that the input and output symbol lists and channel estimate dimensions are valid.
  assert_sizes(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates);

  srsran_assert(tx_scaling > 0, "Tx scaling factor must be positive.");

  // Channel dimensions.
  unsigned nof_rx_ports  = ch_estimates.get_dimension_size(ch_est_list::dims::rx_port);
  unsigned nof_tx_layers = ch_estimates.get_dimension_size(ch_est_list::dims::tx_layer);

  // Compute the average noise variance.
  float noise_var = 0.0F;
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    noise_var += noise_var_estimates[i_rx_port];
  }
  noise_var /= static_cast<float>(nof_rx_ports);

  // Single transmit layer and any number of receive ports.
  if (nof_tx_layers == 1) {
    equalize_zf_single_tx_layer<MAX_PORTS>(
        nof_rx_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
    return;
  }

  // Simplified case of two transmit layers and two receive ports.
  if ((nof_rx_ports == 2) && (nof_tx_layers == 2)) {
    equalize_zf_2x2(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
    return;
  }

  srsran_assertion_failure("Invalid channel spatial topology: {} Rx ports, {} Tx layers.",
                           ch_estimates.get_dimension_size(ch_est_list::dims::rx_port),
                           ch_estimates.get_dimension_size(ch_est_list::dims::tx_layer));
}
