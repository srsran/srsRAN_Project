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

pdcch_candidate_list
srsgnb::pdcch_candidates_common_ss_get_lowest_cce(const pdcch_candidates_common_ss_configuration& config)
{
  unsigned Y_p  = 0;
  unsigned n_ci = 0;
  unsigned L    = to_nof_cces(config.L);

  srsgnb_assert(config.nof_cce_coreset, "The number of CCE in the CORESET must not be zero.");
  srsgnb_assert(config.nof_candidates <= PDCCH_MAX_NOF_CANDIDATES_SS,
                "The number of candidates ({}) exceeds the maximum ({}).",
                config.nof_candidates,
                PDCCH_MAX_NOF_CANDIDATES_SS);
  srsgnb_assert(config.nof_candidates * L <= config.nof_cce_coreset,
                "The number of candidates ({}) for aggregation level ({}) exceeds the number of available CCE ({})",
                config.nof_candidates,
                L,
                config.nof_cce_coreset);

  pdcch_candidate_list candidates;

  for (unsigned candidate = 0; candidate != config.nof_candidates; ++candidate) {
    unsigned n_cce = L * ((Y_p + (candidate * config.nof_cce_coreset) / (L * config.nof_candidates) + n_ci) %
                          (config.nof_cce_coreset / L));
    candidates.push_back(n_cce);
  }

  return candidates;
}