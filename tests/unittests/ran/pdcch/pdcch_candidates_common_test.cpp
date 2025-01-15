/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_candidates_common_test_data.h"
#include "srsran/adt/span.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>

namespace srsran {

std::ostream& operator<<(std::ostream& os, const pdcch_candidates_common_ss_configuration& config)
{
  fmt::print(os,
             "L={} nof_candidates={} nof_coreset_cce={}",
             to_aggregation_level_index(config.al),
             config.nof_candidates,
             config.nof_cce_coreset);
  return os;
}

std::ostream& operator<<(std::ostream& os, const pdcch_candidate_list& list)
{
  fmt::print(os, "candidates=[{}]", span<const uint8_t>(list));
  return os;
}

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  os << test_case.config << " " << test_case.candidates;
  return os;
}

} // namespace srsran

using namespace srsran;

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