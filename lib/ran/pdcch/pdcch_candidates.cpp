/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ran/pdcch/pdcch_candidates.h"

using namespace srsgnb;

static pdcch_candidate_list pdcch_candidates_get_lowest_cce(unsigned Y_p,
                                                            unsigned n_ci,
                                                            unsigned L,
                                                            unsigned nof_candidates,
                                                            unsigned nof_cce_coreset)
{
  pdcch_candidate_list candidates;

  for (unsigned candidate = 0; candidate != nof_candidates; ++candidate) {
    unsigned n_cce = L * ((Y_p + (candidate * nof_cce_coreset) / (L * nof_candidates) + n_ci) % (nof_cce_coreset / L));
    candidates.push_back(n_cce);
  }

  return candidates;
}

static unsigned pdcch_candidates_ue_ss_get_Y_p(unsigned A_p, unsigned D, unsigned rnti, unsigned n)
{
  if (n == 0) {
    return (A_p * rnti) % D;
  }

  return (A_p * pdcch_candidates_ue_ss_get_Y_p(A_p, D, rnti, n - 1)) % D;
}

pdcch_candidate_list
srsgnb::pdcch_candidates_common_ss_get_lowest_cce(const pdcch_candidates_common_ss_configuration& config)
{
  unsigned Y_p  = 0;
  unsigned n_ci = 0;

  return pdcch_candidates_get_lowest_cce(
      Y_p, n_ci, to_nof_cces(config.L), config.nof_candidates, config.nof_cce_coreset);
}

pdcch_candidate_list srsgnb::pdcch_candidates_ue_ss_get_lowest_cce(const pdcch_candidates_ue_ss_configuration& config)
{
  static const std::array<unsigned, 3> A_p_values = {39827, 39829, 39839};
  unsigned                             n_ci       = 0;

  unsigned A_p = A_p_values[config.coreset_id % A_p_values.size()];
  unsigned D   = 65537;
  unsigned Y_p = pdcch_candidates_ue_ss_get_Y_p(A_p, D, config.rnti, config.slot_index);

  return pdcch_candidates_get_lowest_cce(
      Y_p, n_ci, to_nof_cces(config.L), config.nof_candidates, config.nof_cce_coreset);
}