/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "prbs_calculator.h"
#include "tbs_calculator.h"

using namespace srsran;

constexpr unsigned NOF_BITS_PER_BYTE = 8U;

/// \brief Estimation of the N_info for payloads above 3824 bits.
static float estimate_nof_info_payload_higher_3824_bits(unsigned payload_bits, float tcr)
{
  // Approximation of the inverse of the TBS derivation based on nof_info_prime.
  // TODO: Improve this estimation algorithm.
  unsigned nof_info_prime_estim = std::max(3840U, payload_bits + 24);

  unsigned C = 1;
  if (tcr <= 0.25F) {
    C = (nof_info_prime_estim + 24) / 3816;
  } else if (nof_info_prime_estim > 8424) {
    C = (nof_info_prime_estim + 24) / 8424;
  }

  return 8 * C * ((nof_info_prime_estim + 24) / (8 * C)) - 24;
}

/// \brief Obtain an initial estimate for the minimum number of PRBs needed so that the TBS >= payload size.
unsigned srsran::estimate_required_nof_prbs(const prbs_calculator_sch_config& sch_config,
                                            unsigned                          max_nof_available_rbs)
{
  // Convert size into bits, as per TS procedures for TBS.
  unsigned payload_size = sch_config.payload_size_bytes * NOF_BITS_PER_BYTE;

  float                 nof_info_estimate;
  static const unsigned payload_step_threshold = 3824;
  if (payload_size >= payload_step_threshold) {
    nof_info_estimate = estimate_nof_info_payload_higher_3824_bits(payload_size, sch_config.mcs_descr.target_code_rate);
  } else {
    // This is an estimate of the N_info (as per Section 5.1.3.2, TS 38.214), approximated as the TBS that is greater or
    // equal to the payload size. This guarantees that the estimated num. of PRBs we obtained yields a TBS that is
    // greater or equal to the payload.
    nof_info_estimate = static_cast<float>(tbs_calculator_table_find_smallest_not_less_than(payload_size));
  }

  unsigned nof_bit_per_symbol = get_bits_per_symbol(sch_config.mcs_descr.modulation);

  // Get N_re (as per Section 5.1.3.2, TS 38.214) from N_info.
  float nof_re =
      nof_info_estimate / (sch_config.mcs_descr.get_normalised_target_code_rate() *
                           static_cast<float>(nof_bit_per_symbol) * static_cast<float>(sch_config.nof_layers) *
                           tbs_calculator_pdsch_get_scaling_factor(sch_config.tb_scaling_field));

  // N_info_prime as per Section 5.1.3.2, TS 38.214.
  int nof_re_prime = static_cast<int>(NOF_SUBCARRIERS_PER_RB) * static_cast<int>(sch_config.nof_symb_sh) -
                     static_cast<int>(sch_config.nof_dmrs_prb) - static_cast<int>(sch_config.nof_oh_prb);
  srsran_assert(nof_re_prime > 0, "nof_re_prime is expected to be positive");

  // Get the estimated number of PRBs from the N_re and N_info_prime. Cap the returned value to the maximum nof RBs for
  // FR1.
  return std::min(max_nof_available_rbs,
                  divide_ceil(static_cast<unsigned>(nof_re), std::min(static_cast<unsigned>(nof_re_prime), 156U)));
}

/// \brief Linearly searches an upper-bound for the number of PRBs, such that the TBS >= payload size, starting from
/// an initial estimate \c nof_prbs_estimate.
///
/// \param pdsch_cfg Configuration received for PRB calculation.
/// \param nof_prbs_estimate Initial estimate for the number PRBs. The algorithm searches for the actual solution using
/// this value as a starting point.
/// \return Optimal number of PRBs and TBS.
static sch_prbs_tbs linear_search_nof_prbs_upper_bound(const prbs_calculator_sch_config& pdsch_cfg,
                                                       unsigned                          nof_prbs_estimate,
                                                       unsigned                          max_nof_available_rbs,
                                                       unsigned                          max_prb_inc_iterations = 5)
{
  unsigned                     payload_size_bits = pdsch_cfg.payload_size_bytes * NOF_BITS_PER_BYTE;
  tbs_calculator_configuration tbs_cfg{pdsch_cfg.nof_symb_sh,
                                       pdsch_cfg.nof_dmrs_prb,
                                       pdsch_cfg.nof_oh_prb,
                                       pdsch_cfg.mcs_descr,
                                       pdsch_cfg.nof_layers,
                                       pdsch_cfg.tb_scaling_field,
                                       nof_prbs_estimate};
  unsigned                     tbs_bits_ub = tbs_calculator_calculate(tbs_cfg);

  // Given that the nof_prbs_estimate is an estimate of the required PRBs, this can be greater (leading to a TBS >>
  // payload size) or smaller than the actual wanted value (leading to a TBS < payload size). Depending on the two
  // cases, we perform a fine tuning of the number of PRBs by reducing (former case) or by increasing the estimate of
  // RBs (latter case).

  // First case: the nof_prbs_estimate is too big.
  // Linearly searches for an "nof_prb_dec" integer value so that TBS(nof_prbs_estimate - nof_prb_dec) < payload size.
  // Once an "nof_prb_dec" is found, the function will return "nof_prbs_estimate - nof_prb_dec + 1" as the solution.
  unsigned tbs_bits_lb = tbs_bits_ub;
  for (unsigned nof_prb_dec = 1; nof_prb_dec < tbs_cfg.n_prb and tbs_bits_lb >= payload_size_bits; ++nof_prb_dec) {
    tbs_cfg.n_prb = nof_prbs_estimate - nof_prb_dec;
    tbs_bits_lb   = tbs_calculator_calculate(tbs_cfg);

    // if tbs_bits_lb < payload_size, return the previous iteration as the solution.
    if (tbs_bits_lb < payload_size_bits) {
      return {tbs_cfg.n_prb + 1, tbs_bits_ub / NOF_BITS_PER_BYTE};
    }
    tbs_bits_ub = tbs_bits_lb;
  }

  // Second case: the nof_prbs_estimate is too small.
  // Linearly searches for an "nof_prb_inc" so that TBS(nof_prb_estimate + nof_prb_inc) >= payload_size.
  // Implementation-defined value to avoid too many iterations in the search for the optimal TBS.
  for (unsigned nof_prb_inc = 1; nof_prb_inc < max_prb_inc_iterations and tbs_bits_ub < payload_size_bits and
                                 tbs_cfg.n_prb < max_nof_available_rbs;
       ++nof_prb_inc) {
    tbs_cfg.n_prb = nof_prbs_estimate + nof_prb_inc;
    tbs_bits_ub   = tbs_calculator_calculate(tbs_cfg);
  }

  return {tbs_cfg.n_prb, tbs_bits_ub / NOF_BITS_PER_BYTE};
}

sch_prbs_tbs srsran::get_nof_prbs(const prbs_calculator_sch_config& sch_config, unsigned max_nof_available_rbs)
{
  // Get a first estimate for the number of PRBs.
  unsigned nof_prbs_estimate = estimate_required_nof_prbs(sch_config, max_nof_available_rbs);

  // Linearly search for the optimal number of PRBs using "nof_prbs_estimate" as initial guess.
  return linear_search_nof_prbs_upper_bound(sch_config, nof_prbs_estimate, max_nof_available_rbs);
}
