/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "channel_equalizer_zf_impl.h"
#include "srsgnb/srsvec/prod.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

// Implementation of a Zero Forcing equalizer algorithm for the scalar, SISO case.
void channel_equalizer_zf_impl::equalize_1x1_zf(equalizer_symbol_list&          mod_symbols,
                                                equalizer_noise_var_list&       noise_vars,
                                                const equalizer_ch_symbol_list& ch_symbols,
                                                const channel_estimate&         ch_estimates,
                                                float                           tx_scaling)
{
  unsigned const nof_symbols = ch_symbols.size().nof_symbols;
  unsigned const nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> channel_estimates  = ch_estimates.get_path_ch_estimate(0, 0);
  const float      noise_var_estimate = ch_estimates.get_noise_variance(0, 0);

  float tx_scaling_sq = tx_scaling * tx_scaling;

  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    // Prepare input views.
    span<const cf_t> symbol_in        = ch_symbols.get_symbol(i_symb, 0);
    span<const cf_t> symbol_estimates = channel_estimates.subspan(i_symb * nof_subcs, nof_subcs);

    // Prepare output views.
    span<cf_t>  symbol_out     = mod_symbols.get_symbol(i_symb, 0);
    span<float> noise_vars_out = noise_vars.get_symbol(i_symb, 0);

    // Revert tx_scaling factor.
    srsvec::sc_prod(symbol_in, 1 / tx_scaling, symbol_out);

    for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
      // Apply Zero Forcing algorithm.
      symbol_out[i_subc] = symbol_out[i_subc] / symbol_estimates[i_subc];

      // Calculate noise variances.
      float ch_mod_sq = abs_sq(symbol_estimates[i_subc]);
      if (std::isnormal(ch_mod_sq)) {
        noise_vars_out[i_subc] = noise_var_estimate / (ch_mod_sq * tx_scaling_sq);
      }
    }
  }
}

// Implementation of a Zero Forcing equalizer algorithm for the 1 X 2 case.
void channel_equalizer_zf_impl::equalize_1x2_zf(equalizer_symbol_list&          mod_symbols,
                                                equalizer_noise_var_list&       noise_vars,
                                                const equalizer_ch_symbol_list& ch_symbols,
                                                const channel_estimate&         ch_estimates,
                                                float                           tx_scaling)
{
  unsigned const nof_symbols = ch_symbols.size().nof_symbols;
  unsigned const nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> ch_estimates_p0 = ch_estimates.get_path_ch_estimate(0, 0);
  span<const cf_t> ch_estimates_p1 = ch_estimates.get_path_ch_estimate(1, 0);

  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    // Prepare input views.
    span<const cf_t> symbol_in_p0        = ch_symbols.get_symbol(i_symb, 0);
    span<const cf_t> symbol_in_p1        = ch_symbols.get_symbol(i_symb, 1);
    span<const cf_t> symbol_estimates_p0 = ch_estimates_p0.subspan(i_symb * nof_subcs, nof_subcs);
    span<const cf_t> symbol_estimates_p1 = ch_estimates_p1.subspan(i_symb * nof_subcs, nof_subcs);

    // Prepare output views.
    span<cf_t>  symbol_out     = mod_symbols.get_symbol(i_symb, 0);
    span<float> noise_vars_out = noise_vars.get_symbol(i_symb, 0);

    for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
      // Apply Zero Forcing algorithm.
      float d_zf = tx_scaling * (abs_sq(ch_estimates_p0[i_subc]) + abs_sq(ch_estimates_p1[i_subc]));
      if (std::isnormal(d_zf)) {
        symbol_out[i_subc] = (symbol_in_p0[i_subc] * std::conj(symbol_estimates_p0[i_subc]) +
                              symbol_in_p1[i_subc] * std::conj(symbol_estimates_p1[i_subc])) /
                             d_zf;
      }
    }
    // For now, do not compute the output noise variances.
    srsvec::zero(noise_vars_out);
  }
}

// Implementation of a Zero Forcing equalizer algorithm for the 2 X 2 case.
void channel_equalizer_zf_impl::equalize_2x2_zf(equalizer_symbol_list&          mod_symbols,
                                                equalizer_noise_var_list&       noise_vars,
                                                const equalizer_ch_symbol_list& ch_symbols,
                                                const channel_estimate&         ch_estimates,
                                                float                           tx_scaling)
{
  unsigned const nof_symbols = ch_symbols.size().nof_symbols;
  unsigned const nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> channel_estimates_p0_l0 = ch_estimates.get_path_ch_estimate(0, 0);
  span<const cf_t> channel_estimates_p0_l1 = ch_estimates.get_path_ch_estimate(0, 1);
  span<const cf_t> channel_estimates_p1_l0 = ch_estimates.get_path_ch_estimate(1, 0);
  span<const cf_t> channel_estimates_p1_l1 = ch_estimates.get_path_ch_estimate(1, 1);

  for (unsigned i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    // Prepare input views.
    span<const cf_t> symbol_in_p0           = ch_symbols.get_symbol(i_symb, 0);
    span<const cf_t> symbol_in_p1           = ch_symbols.get_symbol(i_symb, 1);
    span<const cf_t> symbol_estimates_p0_l0 = channel_estimates_p0_l0.subspan(i_symb * nof_subcs, nof_subcs);
    span<const cf_t> symbol_estimates_p0_l1 = channel_estimates_p0_l1.subspan(i_symb * nof_subcs, nof_subcs);
    span<const cf_t> symbol_estimates_p1_l0 = channel_estimates_p1_l0.subspan(i_symb * nof_subcs, nof_subcs);
    span<const cf_t> symbol_estimates_p1_l1 = channel_estimates_p1_l1.subspan(i_symb * nof_subcs, nof_subcs);

    // Prepare output views.
    span<cf_t>  symbol_out_l0     = mod_symbols.get_symbol(i_symb, 0);
    span<cf_t>  symbol_out_l1     = mod_symbols.get_symbol(i_symb, 1);
    span<float> noise_vars_out_l0 = noise_vars.get_symbol(i_symb, 0);
    span<float> noise_vars_out_l1 = noise_vars.get_symbol(i_symb, 1);

    for (unsigned i_subc = 0; i_subc != nof_subcs; ++i_subc) {
      // Apply Zero Forcing algorithm.
      cf_t xi = (std::conj(symbol_estimates_p0_l0[i_subc]) * symbol_estimates_p0_l1[i_subc]) +
                (std::conj(symbol_estimates_p1_l0[i_subc]) * symbol_estimates_p1_l1[i_subc]);

      cf_t chi_1 = (std::conj(symbol_estimates_p0_l0[i_subc]) * symbol_in_p0[i_subc]) +
                   (std::conj(symbol_estimates_p1_l0[i_subc]) * symbol_in_p1[i_subc]);

      cf_t chi_2 = (std::conj(symbol_estimates_p0_l1[i_subc]) * symbol_in_p0[i_subc]) +
                   (std::conj(symbol_estimates_p1_l1[i_subc]) * symbol_in_p1[i_subc]);

      float norm_sq_psi_1 = abs_sq(symbol_estimates_p0_l0[i_subc]) + abs_sq(symbol_estimates_p1_l0[i_subc]);
      float norm_sq_psi_2 = abs_sq(symbol_estimates_p0_l1[i_subc]) + abs_sq(symbol_estimates_p1_l1[i_subc]);
      float d_zf          = tx_scaling * ((norm_sq_psi_1 * norm_sq_psi_2) - abs_sq(xi));

      if (std::isnormal(d_zf)) {
        symbol_out_l0[i_subc] = ((norm_sq_psi_2 * chi_1) - ((xi)*chi_2)) / d_zf;
        symbol_out_l1[i_subc] = ((norm_sq_psi_1 * chi_2) - (std::conj(xi) * chi_1)) / d_zf;
      }
    }

    // For now, do not compute the output noise variances.
    srsvec::zero(noise_vars_out_l0);
    srsvec::zero(noise_vars_out_l1);
  }
}

void channel_equalizer_zf_impl::equalize(equalizer_symbol_list&          mod_symbols,
                                         equalizer_noise_var_list&       noise_vars,
                                         const equalizer_ch_symbol_list& ch_symbols,
                                         const channel_estimate&         ch_estimates,
                                         float                           tx_scaling)
{
  // Determine the MIMO configuration.
  // TODO(joaquim): Use an enum in the implementation class for MIMO configs, and a helper function P, L -> mimo cfg
  unsigned nof_tx_layers = ch_estimates.size().nof_tx_layers;
  unsigned nof_rx_ports  = ch_estimates.size().nof_rx_ports;

  // TODO(joaquim): Add more exhaustive size assertions.
  srsgnb_assert(nof_rx_ports == ch_symbols.size().nof_slices, "Sizes do not match.");
  srsgnb_assert(nof_tx_layers == mod_symbols.size().nof_slices, "Sizes do not match.");
  srsgnb_assert(nof_tx_layers == noise_vars.size().nof_slices, "Sizes do not match.");
  srsgnb_assert(tx_scaling > 0, "Tx scaling factor must be positive");

  if (nof_tx_layers == 1) {
    if (nof_rx_ports == 1) {
      // SISO channel.
      equalize_1x1_zf(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
    } else if (nof_rx_ports == 2) {
      // 1 X 2 channel.
      equalize_1x2_zf(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
    }
  } else if (nof_tx_layers == 2 && nof_rx_ports == 2) {
    // 2 X 2 MIMO channel.
    equalize_2x2_zf(mod_symbols, noise_vars, ch_symbols, ch_estimates, tx_scaling);
  }
}
