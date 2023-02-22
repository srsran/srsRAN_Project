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

#include "pdcch_candidates_ue_test_data.h"
#include "srsran/adt/span.h"
#include "gtest/gtest.h"

using namespace srsran;

class PdcchCandidatesFixture : public ::testing::TestWithParam<test_case_t>
{};

TEST_P(PdcchCandidatesFixture, Ue)
{
  const test_case_t&   test_case  = GetParam();
  pdcch_candidate_list candidates = pdcch_candidates_ue_ss_get_lowest_cce(test_case.config);
  ASSERT_EQ(span<const uint8_t>(test_case.candidates), span<const uint8_t>(candidates));
}

INSTANTIATE_TEST_SUITE_P(PdcchCandidatesUe, PdcchCandidatesFixture, ::testing::ValuesIn(pdcch_candidates_ue_test_data));