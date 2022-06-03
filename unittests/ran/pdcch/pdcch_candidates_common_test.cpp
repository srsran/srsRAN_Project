/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_candidates_common_test_data.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;

int main()
{
  for (const test_case_t& test_case : pdcch_candidates_common_test_data) {
    pdcch_candidate_list candidates = pdcch_candidates_common_ss_get(test_case.config);
    TESTASSERT_EQ(span<const uint8_t>(test_case.candidates),
                  span<const uint8_t>(candidates),
                  "Failed for aggregation_level={} nof_candidates={} nof_cce_coreset={}.",
                  test_case.config.aggregation_level,
                  test_case.config.nof_candidates,
                  test_case.config.nof_cce_coreset);
  }

  return 0;
}