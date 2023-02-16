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
#include "gtest/gtest.h"

using namespace srsgnb;

class PdcchCandidatesFixture : public ::testing::TestWithParam<test_case_t>
{};

TEST_P(PdcchCandidatesFixture, Common)
{
  const test_case_t&   test_case  = GetParam();
  pdcch_candidate_list candidates = pdcch_candidates_common_ss_get_lowest_cce(test_case.config);
  ASSERT_EQ(span<const uint8_t>(test_case.candidates), span<const uint8_t>(candidates));
}

INSTANTIATE_TEST_SUITE_P(PdcchCandidatesCommon,
                         PdcchCandidatesFixture,
                         ::testing::ValuesIn(pdcch_candidates_common_test_data));