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
#include "equalize_zf_1x1.h"
#include "equalize_zf_1xn.h"
#include "equalize_zf_2x2.h"
#include "srsgnb/ran/spatial_topology.h"

using namespace srsgnb;

static inline void assert_sizes(re_measurement_dimensions&                     mod_symbols_size,
                                re_measurement_dimensions&                     noise_vars_size,
                                re_measurement_dimensions&                     ch_symbols_size,
                                channel_estimate::channel_estimate_dimensions& ch_estimates_size)
{
  srsgnb_assert(mod_symbols_size == noise_vars_size,
                "Noise variance dimensions do not match equalized symbols dimensions:\n"
                "Equalized symbols dimensions\t: ({} subcarriers x {} symbols x {} layers)\n"
                "Noise variance dimensions\t: ({} subcarriers x {} symbols x {} layers)",
                mod_symbols_size.nof_subc,
                mod_symbols_size.nof_symbols,
                mod_symbols_size.nof_slices,
                noise_vars_size.nof_subc,
                noise_vars_size.nof_symbols,
                noise_vars_size.nof_slices);

  srsgnb_assert(ch_estimates_size.nof_rx_ports == ch_symbols_size.nof_slices,
                "Channel estimate Rx ports ({}) do not match received symbol ports ({}).",
                ch_estimates_size.nof_rx_ports,
                ch_symbols_size.nof_slices);
  srsgnb_assert(ch_estimates_size.nof_tx_layers == mod_symbols_size.nof_slices,
                "Channel estimate Tx layers ({}) do not match equalized symbol layers ({}).",
                ch_estimates_size.nof_tx_layers,
                mod_symbols_size.nof_slices);

  srsgnb_assert((mod_symbols_size.nof_subc == ch_symbols_size.nof_subc) &&
                    (mod_symbols_size.nof_subc == ch_symbols_size.nof_subc),
                "Number of Subcarriers does not match:\n"
                "Equalized symbols subcarriers:\t {}\n"
                "Received symbols subcarriers:\t {}\n"
                "Channel estimates subcarriers: \t {}",
                mod_symbols_size.nof_subc,
                ch_symbols_size.nof_subc,
                ch_estimates_size.nof_prb * NRE);

  srsgnb_assert((mod_symbols_size.nof_symbols == ch_symbols_size.nof_symbols) &&
                    (mod_symbols_size.nof_symbols == ch_symbols_size.nof_symbols),
                "Number of OFDM symbols does not match:\n"
                "Equalized symbols symbols:\t {}\n"
                "Received symbols symbols:\t {}\n"
                "Channel estimates symbols: \t {}",
                mod_symbols_size.nof_symbols,
                ch_symbols_size.nof_symbols,
                ch_estimates_size.nof_symbols);
}

void channel_equalizer_zf_impl::equalize(equalizer_symbol_list&          mod_symbols,
                                         equalizer_noise_var_list&       noise_vars,
                                         const equalizer_ch_symbol_list& ch_symbols,
                                         const channel_estimate&         ch_estimates,
                                         float                           tx_scaling)
{
  re_measurement_dimensions                     mod_symbols_size  = mod_symbols.size();
  re_measurement_dimensions                     noise_vars_size   = noise_vars.size();
  re_measurement_dimensions                     ch_symbols_size   = ch_symbols.size();
  channel_estimate::channel_estimate_dimensions ch_estimates_size = ch_estimates.size();

  // Make sure that the input and output symbol lists and channel estimate dimensions are valid.
  assert_sizes(mod_symbols_size, noise_vars_size, ch_symbols_size, ch_estimates_size);

  srsgnb_assert(tx_scaling > 0, "Tx scaling factor must be positive.");

  // Determine the channel spatial topology based on the provided channel estimates.
  spatial_topology topology(ch_estimates.size().nof_rx_ports, ch_estimates.size().nof_tx_layers);

  switch (topology.get_topology()) {
    case spatial_topology::siso:
      equalize_zf_1x1(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
      break;
    case spatial_topology::miso_2x1:
      equalize_zf_1xn<spatial_topology::get_nof_rx_ports(spatial_topology::miso_2x1)>(
          mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
      break;
    case spatial_topology::miso_3x1:
      equalize_zf_1xn<spatial_topology::get_nof_rx_ports(spatial_topology::miso_3x1)>(
          mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
      break;
    case spatial_topology::miso_4x1:
      equalize_zf_1xn<spatial_topology::get_nof_rx_ports(spatial_topology::miso_4x1)>(
          mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
      break;
    case spatial_topology::mimo_2x2:
      equalize_zf_2x2(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
      break;
    case spatial_topology::invalid:
    default:
      srsgnb_assertion_failure("Invalid channel spatial topology: {} Tx layers, {} Rx ports.",
                               ch_estimates.size().nof_tx_layers,
                               ch_estimates.size().nof_rx_ports);
  }
}
