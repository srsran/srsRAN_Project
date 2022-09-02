/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_equalizer_impl.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

// Implementation of a Zero Forcing equalizer algorithm for the scalar, SISO case.
// TODO(joaquim): consider changing the variable names from symbols to RE's or subcarriers.
static void equalize_1x1_zf(equalizer_symbol_list&          mod_symbols,
                            equalizer_noise_var_list&       noise_vars,
                            const equalizer_ch_symbol_list& ch_symbols,
                            const channel_estimate&         ch_estimates,
                            float                           tx_scaling)
{
  // Prepare input views.
  span<const cf_t> symbols_in         = ch_symbols.get_slice(0);
  span<const cf_t> channel_estimates  = ch_estimates.get_path_ch_estimate(0, 0);
  const float      noise_var_estimate = ch_estimates.get_noise_variance(0, 0);

  // Prepare output views.
  span<cf_t>       symbols_out        = mod_symbols.get_slice(0);
  span<float>      noise_vars_out     = noise_vars.get_slice(0);

  unsigned len = symbols_in.size();

  // Revert tx_scaling factor.
  srsvec::sc_prod(symbols_in, 1 / tx_scaling, symbols_out);

  // Invert the channel effects.
  for (unsigned i_symb = 0; i_symb != len; ++i_symb) {
    symbols_out[i_symb] = symbols_out[i_symb] / channel_estimates[i_symb];
  }

  // Calculate noise variances.
  float tx_scaling_sq = tx_scaling * tx_scaling;
  for (unsigned i_symb = 0; i_symb != len; ++i_symb) {
    float ch_mod_sq        = abs_sq(channel_estimates[i_symb]);
    noise_vars_out[i_symb] = noise_var_estimate / (ch_mod_sq * tx_scaling_sq);
  }
}

void channel_equalizer_impl::equalize(equalizer_symbol_list&          mod_symbols,
                                      equalizer_noise_var_list&       noise_vars,
                                      const equalizer_ch_symbol_list& ch_symbols,
                                      const channel_estimate&         ch_estimates,
                                      float                           tx_scaling)
{
  // Determine the MIMO configuration.
  unsigned nof_tx_layers = ch_estimates.size().nof_tx_layers;
  unsigned nof_rx_ports  = ch_estimates.size().nof_rx_ports;

  // TODO(joaquim): Add more exhaustive size assertions.
  srsgnb_assert(nof_rx_ports == ch_symbols.size().nof_slices, "Sizes do not match");
  srsgnb_assert(nof_tx_layers == mod_symbols.size().nof_slices, "Sizes do not match");
  srsgnb_assert(nof_tx_layers == noise_vars.size().nof_slices, "Sizes do not match");

  // SISO Channel.
  if (nof_tx_layers == 1 && nof_rx_ports == 1) {
    equalize_1x1_zf(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
  }
  // 1 X 2 Channel.
  else if (nof_tx_layers == 1 && nof_rx_ports == 2) {
    // Do nothing.
  }
}
