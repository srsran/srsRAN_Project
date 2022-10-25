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
/// \brief Zero Forcing equalization function implementation.

#include "channel_equalizer_zf_impl.h"
#include "equalize_zf_1xn.h"
#include "equalize_zf_2x2.h"
#include "srsgnb/ran/spatial_topology.h"

using namespace srsgnb;

// Assert that the dimensions of the equalizer input and output data structures match.
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
  srsgnb_assert((ch_symb_nof_re == ch_ests_nof_re) && (ch_symb_nof_re == eq_symb_nof_re) &&
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
  srsgnb_assert((ch_ests_nof_rx_ports == ch_symb_nof_rx_ports) && (ch_ests_nof_rx_ports == nvar_ests_nof_rx_ports),
                "Number of Rx ports does not match: \n"
                "Received symbols Rx ports:\t {}\n"
                "Noise variance estimates Rx ports: \t {}\n"
                "Channel estimates Rx ports:\t {}",
                ch_symb_nof_rx_ports,
                nvar_ests_nof_rx_ports,
                ch_ests_nof_rx_ports);

  // Assert that the number of transmit layers matches.
  srsgnb_assert((ch_ests_nof_tx_layers == eq_symb_nof_tx_layers) && (ch_ests_nof_tx_layers == eq_nvars_nof_tx_layers),
                "Number of Tx layers does not match: \n"
                "Output symbols Tx layers:\t {}\n"
                "Output noise variances Tx layers: \t {}\n"
                "Channel estimates Tx layers:\t {}",
                eq_symb_nof_tx_layers,
                eq_nvars_nof_tx_layers,
                ch_ests_nof_tx_layers);
}

// Empty list. Acts as a default case.
template <class none = void>
void equalize_zf_single_tx_layer(unsigned                              nof_ports,
                                 channel_equalizer::re_list&           /**/,
                                 channel_equalizer::noise_var_list&    /**/,
                                 const channel_equalizer::re_list&     /**/,
                                 const channel_equalizer::ch_est_list& /**/,
                                 float                                 /**/,
                                 float                                 /**/)
{
  srsgnb_assertion_failure("Invalid number of receive ports: {}", nof_ports);
};

// Calls the equalizer function for receive spatial diversity with the appropriate number of ports, given by nof_ports.
// Recursively evaluates a list of number of ports passed as an argument pack. Performs equalization when the right
// number of ports value is found.
template <unsigned NPorts, unsigned... NPortList>
void equalize_zf_single_tx_layer(unsigned                              nof_ports,
                                 channel_equalizer::re_list&           eq_symbols,
                                 channel_equalizer::noise_var_list&    eq_noise_vars,
                                 const channel_equalizer::re_list&     ch_symbols,
                                 const channel_equalizer::ch_est_list& ch_estimates,
                                 float                                 noise_var,
                                 float                                 tx_scaling)
{
  if (NPorts != nof_ports) {
    // Recursive call, discarding NPorts from the list.
    return equalize_zf_single_tx_layer<NPortList...>(
        nof_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
  }

  // Perform equalization.
  equalize_zf_1xn<NPorts>(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
}

// Deduces the NPortList argument pack from a std::integer_sequence numeric sequence generated at compile time.
template <unsigned... NPortList>
void equalize_zf_single_tx_layer(unsigned nof_ports,
                                 std::integer_sequence<unsigned, NPortList...>,
                                 channel_equalizer::re_list&           eq_symbols,
                                 channel_equalizer::noise_var_list&    eq_noise_vars,
                                 const channel_equalizer::re_list&     ch_symbols,
                                 const channel_equalizer::ch_est_list& ch_estimates,
                                 float                                 noise_var,
                                 float                                 tx_scaling)
{
  equalize_zf_single_tx_layer<NPortList...>(
      nof_ports, eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
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

  srsgnb_assert(tx_scaling > 0, "Tx scaling factor must be positive.");

  // Determine the channel spatial topology based on the provided channel estimates.
  spatial_topology topology(ch_estimates.get_dimension_size(ch_est_list::dims::rx_port),
                            ch_estimates.get_dimension_size(ch_est_list::dims::tx_layer));

  // For now, use the noise variance of a single Rx port.
  float noise_var = noise_var_estimates[0];

  // Select the appropriate equalization algorithm based on the channel topology.
  switch (topology.get_topology()) {
    case spatial_topology::siso:
    case spatial_topology::simo:
      equalize_zf_single_tx_layer(topology.get_nof_rx_ports(),
                                  std::make_integer_sequence<unsigned, MAX_PORTS>(),
                                  eq_symbols,
                                  eq_noise_vars,
                                  ch_symbols,
                                  ch_estimates,
                                  noise_var,
                                  tx_scaling);
      break;
    case spatial_topology::mimo:
      equalize_zf_2x2(eq_symbols, eq_noise_vars, ch_symbols, ch_estimates, noise_var, tx_scaling);
      break;
    case spatial_topology::invalid:
    default:
      srsgnb_assertion_failure("Invalid channel spatial topology: {} Tx layers, {} Rx ports.",
                               ch_estimates.get_dimension_size(ch_est_list::dims::rx_port),
                               ch_estimates.get_dimension_size(ch_est_list::dims::tx_layer));
  }
}
