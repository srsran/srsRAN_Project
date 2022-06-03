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

pdcch_candidate_list srsgnb::pdcch_candidates_common_ss_get(const pdcch_candidates_common_ss_configuration& config)
{
  srsran_assert(config.nof_cce_coreset, "The number of CCE in the CORESET must not be zero.");
  srsran_assert(config.aggregation_level, "Invalid aggregation level.");
  srsran_assert(config.nof_candidates * config.aggregation_level <= config.nof_cce_coreset,
                "The number of candidates ({}) for aggregation level ({}) exceeds the number of available CCE ({})",
                config.nof_candidates,
                config.aggregation_level,
                config.nof_cce_coreset);

  pdcch_candidate_list candidates;

  unsigned Y_p  = 0;
  unsigned n_ci = 0;

  for (unsigned candidate = 0; candidate != config.nof_candidates; ++candidate) {
    unsigned n_cce =
        config.aggregation_level *
        ((Y_p + (candidate * config.nof_cce_coreset) / (config.aggregation_level * config.nof_candidates) + n_ci) %
         (config.nof_cce_coreset / config.aggregation_level));
    candidates.push_back(n_cce);
  }

  return candidates;
}