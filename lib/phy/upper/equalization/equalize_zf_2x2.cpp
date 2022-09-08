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
/// \brief Zero Forcing equalization function implementation for a MIMO 2 X 2 channel.

#include "equalize_zf_2x2.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

namespace {

inline void equalize_zf_2x2_symbol(span<cf_t>       symbol_out_l0,
                                   span<cf_t>       symbol_out_l1,
                                   span<float>      noise_vars_l0,
                                   span<float>      noise_vars_l1,
                                   span<const cf_t> symbol_in_p0,
                                   span<const cf_t> symbol_in_p1,
                                   span<const cf_t> ch_estimates_p0_l0,
                                   span<const cf_t> ch_estimates_p0_l1,
                                   span<const cf_t> ch_estimates_p1_l0,
                                   span<const cf_t> ch_estimates_p1_l1,
                                   float            tx_scaling)
{
  const std::size_t nof_subcs = symbol_in_p0.size();

  for (std::size_t i_subc = 0; i_subc != nof_subcs; ++i_subc) {
    // Prepare data.
    const cf_t ch_est_p0_l0 = ch_estimates_p0_l0[i_subc];
    const cf_t ch_est_p0_l1 = ch_estimates_p0_l1[i_subc];
    const cf_t ch_est_p1_l0 = ch_estimates_p1_l0[i_subc];
    const cf_t ch_est_p1_l1 = ch_estimates_p1_l1[i_subc];

    const cf_t ch_est_p0_l0_conj = std::conj(ch_est_p0_l0);
    const cf_t ch_est_p0_l1_conj = std::conj(ch_est_p0_l1);
    const cf_t ch_est_p1_l0_conj = std::conj(ch_est_p1_l0);
    const cf_t ch_est_p1_l1_conj = std::conj(ch_est_p1_l1);

    const cf_t re_in_p0 = symbol_in_p0[i_subc];
    const cf_t re_in_p1 = symbol_in_p1[i_subc];

    // Apply Zero Forcing algorithm.
    const cf_t xi = (ch_est_p0_l0_conj * ch_est_p0_l1) + (ch_est_p1_l0_conj * ch_est_p1_l1);

    const cf_t chi_1 = (ch_est_p0_l0_conj * re_in_p0) + (ch_est_p1_l0_conj * re_in_p1);
    const cf_t chi_2 = (ch_est_p0_l1_conj * re_in_p0) + (ch_est_p1_l1_conj * re_in_p1);

    float norm_sq_psi_1 = abs_sq(ch_est_p0_l0) + abs_sq(ch_est_p1_l0);
    float norm_sq_psi_2 = abs_sq(ch_est_p0_l1) + abs_sq(ch_est_p1_l1);
    float d_zf          = tx_scaling * ((norm_sq_psi_1 * norm_sq_psi_2) - abs_sq(xi));

    if (std::isnormal(d_zf)) {
      symbol_out_l0[i_subc] = ((norm_sq_psi_2 * chi_1) - ((xi)*chi_2)) / d_zf;
      symbol_out_l1[i_subc] = ((norm_sq_psi_1 * chi_2) - (std::conj(xi) * chi_1)) / d_zf;
    }
  }

  // For now, do not compute the output noise variances.
  srsvec::zero(noise_vars_l0);
  srsvec::zero(noise_vars_l1);
}

} // namespace

void srsgnb::equalize_zf_2x2(equalizer_symbol_list&          eq_symbols,
                             equalizer_noise_var_list&       noise_vars,
                             const equalizer_ch_symbol_list& ch_symbols,
                             const channel_estimate&         ch_estimates,
                             float                           tx_scaling)
{
  const std::size_t nof_symbols = ch_symbols.size().nof_symbols;
  const std::size_t nof_subcs   = ch_symbols.size().nof_subc;

  span<const cf_t> channel_estimates_p0_l0 = ch_estimates.get_path_ch_estimate(0, 0);
  span<const cf_t> channel_estimates_p0_l1 = ch_estimates.get_path_ch_estimate(0, 1);
  span<const cf_t> channel_estimates_p1_l0 = ch_estimates.get_path_ch_estimate(1, 0);
  span<const cf_t> channel_estimates_p1_l1 = ch_estimates.get_path_ch_estimate(1, 1);

  std::size_t re_index = 0;

  // Equalize symbol by symbol.
  for (std::size_t i_symb = 0; i_symb != nof_symbols; ++i_symb) {
    equalize_zf_2x2_symbol(eq_symbols.get_symbol(i_symb, 0),
                           eq_symbols.get_symbol(i_symb, 1),
                           noise_vars.get_symbol(i_symb, 0),
                           noise_vars.get_symbol(i_symb, 1),
                           ch_symbols.get_symbol(i_symb, 0),
                           ch_symbols.get_symbol(i_symb, 1),
                           channel_estimates_p0_l0.subspan(re_index, nof_subcs),
                           channel_estimates_p0_l1.subspan(re_index, nof_subcs),
                           channel_estimates_p1_l0.subspan(re_index, nof_subcs),
                           channel_estimates_p1_l1.subspan(re_index, nof_subcs),
                           tx_scaling);
    re_index += nof_subcs;
  }
}