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
/// \brief Channel equalizer implementation for the Zero Forcing and the Minimum Mean Square Error.

#include "channel_equalizer_generic_impl.h"
#include "equalize_mmse_1xn.h"
#include "equalize_zf_1xn.h"
#include "equalize_zf_2xn.h"
#include "srsran/adt/interval.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/upper/equalization/modular_ch_est_list.h"

using namespace srsran;

/// Assert that the dimensions of the equalizer input and output data structures match.
static inline void assert_sizes(span<const cf_t>                      eq_symbols,
                                span<const float>                     eq_noise_vars,
                                const re_buffer_reader<cbf16_t>&      ch_symbols,
                                const channel_equalizer::ch_est_list& ch_estimates,
                                span<const float>                     noise_var_estimates)
{
  // Rx symbols dimensions.
  unsigned ch_symb_nof_re       = ch_symbols.get_nof_re();
  unsigned ch_symb_nof_rx_ports = ch_symbols.get_nof_slices();

  // Output symbols dimensions.
  unsigned eq_symb_nof_re = eq_symbols.size();

  // Noise var estimates dimensions.
  unsigned nvar_ests_nof_rx_ports = noise_var_estimates.size();

  // Output noise vars dimensions.
  unsigned eq_nvars_nof_re = eq_noise_vars.size();

  // Channel estimates dimensions.
  unsigned ch_ests_nof_re        = ch_estimates.get_nof_re();
  unsigned ch_ests_nof_rx_ports  = ch_estimates.get_nof_rx_ports();
  unsigned ch_ests_nof_tx_layers = ch_estimates.get_nof_tx_layers();

  // Assert that the number of Resource Elements is the same for both inputs.
  srsran_assert(ch_symb_nof_re == ch_ests_nof_re,
                "The number of channel estimates (i.e., {}) is not equal to the number of input RE (i.e., {}).",
                ch_symb_nof_re,
                ch_ests_nof_re);

  // Assert that the number of Resource Elements is the same for both outputs.
  srsran_assert(eq_symb_nof_re == eq_nvars_nof_re,
                "The number of equalized RE (i.e., {}) is not equal to the number of noise variances (i.e., {}).",
                eq_symb_nof_re,
                eq_nvars_nof_re);

  // Assert that the number of receive ports is within the valid range.
  static constexpr interval<unsigned, true> nof_rx_ports_range(1, MAX_PORTS);
  srsran_assert(nof_rx_ports_range.contains(ch_ests_nof_rx_ports),
                "The number of receive ports (i.e., {}) must be in the range {}.",
                ch_ests_nof_rx_ports,
                nof_rx_ports_range);

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
  srsran_assert(ch_ests_nof_re * ch_ests_nof_tx_layers == eq_symb_nof_re,
                "The number of channel estimates (i.e., {}) and number of layers (i.e., {}) is not consistent with the "
                "number of equalized RE (i.e., {}).",
                ch_ests_nof_re,
                ch_ests_nof_tx_layers,
                eq_symb_nof_re);
}

/// Calls the equalizer function for receive spatial diversity with the appropriate number of receive ports.
template <unsigned NOF_PORTS>
void equalize_zf_single_tx_layer(unsigned                              nof_ports,
                                 span<cf_t>                            eq_symbols,
                                 span<float>                           eq_noise_vars,
                                 const re_buffer_reader<cbf16_t>&      ch_symbols,
                                 const channel_equalizer::ch_est_list& ch_estimates,
                                 span<const float>                     noise_var,
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
                                    span<cf_t>                            eq_symbols,
                                    span<float>                           eq_noise_vars,
                                    const re_buffer_reader<cbf16_t>&      ch_symbols,
                                    const channel_equalizer::ch_est_list& ch_estimates,
                                    span<const float>                     noise_var,
                                    float                                 tx_scaling)
{
  // Perform equalization.
  equalize_zf_1xn<1>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

template <unsigned NOF_PORTS>
void equalize_zf_single_tx_layer_reduction(span<cf_t>                            eq_symbols,
                                           span<float>                           eq_noise_vars,
                                           const re_buffer_reader<cbf16_t>&      ch_symbols,
                                           const channel_equalizer::ch_est_list& ch_estimates,
                                           span<const float>                     noise_var,
                                           float                                 tx_scaling)
{
  unsigned nof_ports = noise_var.size();

  // Function for checking if a noise variance is valid.
  const auto func_valid_noise_var = [](float nvar) {
    return (nvar > 0) && (nvar < std::numeric_limits<float>::infinity());
  };

  // Count the number of valid noise variances.
  unsigned nof_valid_noise_var = std::count_if(noise_var.begin(), noise_var.end(), func_valid_noise_var);

  // No valid noise variances, fill output with invalid data.
  if (nof_valid_noise_var == 0) {
    srsvec::zero(eq_symbols);
    srsvec::fill(eq_noise_vars, std::numeric_limits<float>::infinity());
    return;
  }

  // Exclude the ports that are invalid.
  if (nof_valid_noise_var != nof_ports) {
    // Reduce ports.
    static_vector<float, NOF_PORTS>              reduced_noise_var(nof_valid_noise_var);
    modular_re_buffer_reader<cbf16_t, NOF_PORTS> reduced_ch_symbols(nof_ports, ch_symbols.get_nof_re());
    modular_ch_est_list<NOF_PORTS>               reduced_ch_estimates(nof_ports, ch_symbols.get_nof_re());
    for (unsigned i_port = 0, i_reduced_port = 0; i_port != nof_ports; ++i_port) {
      if (func_valid_noise_var(noise_var[i_port])) {
        reduced_noise_var[i_reduced_port] = noise_var[i_port];
        reduced_ch_symbols.set_slice(i_reduced_port, ch_symbols.get_slice(i_port));
        reduced_ch_estimates.set_channel(ch_estimates.get_channel(i_port, 0), i_reduced_port, 0);
        ++i_reduced_port;
      }
    }

    // Equalize. The number of ports must be at least one less than before.
    equalize_zf_single_tx_layer<NOF_PORTS - 1>(nof_valid_noise_var,
                                               eq_symbols,
                                               eq_noise_vars,
                                               reduced_ch_symbols,
                                               reduced_ch_estimates,
                                               reduced_noise_var,
                                               tx_scaling);
    return;
  }

  // Perform equalization for all ports.
  equalize_zf_single_tx_layer<NOF_PORTS>(
      nof_valid_noise_var, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

/// Calls the equalizer function for receive spatial diversity with the appropriate number of receive ports.
template <unsigned NOF_PORTS>
void equalize_mmse_single_tx_layer(unsigned                              nof_ports,
                                   span<cf_t>                            eq_symbols,
                                   span<float>                           eq_noise_vars,
                                   const re_buffer_reader<cbf16_t>&      ch_symbols,
                                   const channel_equalizer::ch_est_list& ch_estimates,
                                   span<const float>                     noise_var,
                                   float                                 tx_scaling)
{
  if (NOF_PORTS != nof_ports) {
    // Recursive call.
    return equalize_mmse_single_tx_layer<NOF_PORTS - 1>(
        nof_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
  }

  // Perform equalization.
  equalize_mmse_1xn<NOF_PORTS>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

/// Specialization for a single receive port.
template <>
void equalize_mmse_single_tx_layer<1>(unsigned /**/,
                                      span<cf_t>                            eq_symbols,
                                      span<float>                           eq_noise_vars,
                                      const re_buffer_reader<cbf16_t>&      ch_symbols,
                                      const channel_equalizer::ch_est_list& ch_estimates,
                                      span<const float>                     noise_var,
                                      float                                 tx_scaling)
{
  // Perform equalization.
  equalize_mmse_1xn<1>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

void channel_equalizer_generic_impl::equalize(span<cf_t>                       eq_symbols,
                                              span<float>                      eq_noise_vars,
                                              const re_buffer_reader<cbf16_t>& ch_symbols,
                                              const ch_est_list&               ch_estimates,
                                              span<const float>                noise_var_estimates,
                                              float                            tx_scaling)
{
  // Make sure that the input and output symbol lists and channel estimate dimensions are valid.
  assert_sizes(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates);

  srsran_assert(tx_scaling > 0, "Tx scaling factor must be positive.");

  // Channel dimensions.
  unsigned nof_rx_ports  = ch_estimates.get_nof_rx_ports();
  unsigned nof_tx_layers = ch_estimates.get_nof_tx_layers();

  // Select the most pessimistic noise variance.
  float noise_var = *std::max_element(noise_var_estimates.begin(), noise_var_estimates.end());

  // Zero Forcing algorithm.
  if (type == channel_equalizer_algorithm_type::zf) {
    // Single transmit layer and any number of receive ports.
    if (nof_tx_layers == 1) {
      equalize_zf_single_tx_layer_reduction<MAX_PORTS>(
          eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates, tx_scaling);
      return;
    }

    // Two transmit layers and two receive ports.
    if ((nof_rx_ports == 2) && (nof_tx_layers == 2)) {
      equalize_zf_2xn<2>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
      return;
    }

    // Two transmit layers and four receive ports.
    if ((nof_rx_ports == 4) && (nof_tx_layers == 2)) {
      equalize_zf_2xn<4>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
      return;
    }
  }

  // Minimum Mean Square Error algorithm.
  if (type == channel_equalizer_algorithm_type::mmse) {
    // Single transmit layer and any number of receive ports.
    if (nof_tx_layers == 1) {
      equalize_mmse_single_tx_layer<MAX_PORTS>(
          nof_rx_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var_estimates, tx_scaling);
      return;
    }
  }

  srsran_assertion_failure(
      "Invalid combination of channel spatial topology (i.e., {} Rx ports, {} Tx layers) and algorithm (i.e., {}).",
      ch_estimates.get_nof_rx_ports(),
      ch_estimates.get_nof_tx_layers(),
      to_string(type));
}
