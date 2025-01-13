/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "context_repository_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace ofh;

namespace {
/// Test fixture for tests involving a single array of bytes.
class context_repository_helpers_param_test : public ::testing::TestWithParam<std::tuple<subcarrier_spacing, unsigned>>
{
protected:
  subcarrier_spacing scs           = std::get<0>(GetParam());
  unsigned           min_repo_size = std::get<1>(GetParam());
};

} // namespace

TEST_P(context_repository_helpers_param_test, first_and_last_repo_size_slot_index_share_same_index)
{
  unsigned repo_size = calculate_repository_size(scs, min_repo_size);

  for (unsigned i = 0, b = repo_size; i != repo_size; i++, --b) {
    slot_point initial_wrap = slot_point(scs, 0, 0) + i;
    slot_point end_wrap     = slot_point(scs, 0, 0) - b;
    ASSERT_EQ(calculate_repository_index(initial_wrap, repo_size), calculate_repository_index(end_wrap, repo_size));
  }
}

TEST_P(context_repository_helpers_param_test, next_repo_size_position_of_the_repo_do_not_share_index)
{
  unsigned repo_size = calculate_repository_size(scs, min_repo_size);

  for (unsigned b = repo_size; b; --b) {
    slot_point end_wrap = slot_point(scs, 0, 0) - b;
    // Check that in the next repo_size slots, there is no repeated index.
    for (unsigned i = 1; i != repo_size; i++) {
      slot_point initial_wrap = end_wrap + i;
      ASSERT_NE(calculate_repository_index(initial_wrap, repo_size), calculate_repository_index(end_wrap, repo_size));
    }
  }
}

TEST_P(context_repository_helpers_param_test, repositories_sizes_not_being_multiple_of_total_number_of_slots_fails)
{
  unsigned repo_size = min_repo_size;

  bool result = true;
  for (unsigned b = repo_size; b; --b) {
    slot_point end_wrap = slot_point(scs, 0, 0) - b;
    // Check that in the next repo_size slots, there is no repeated index.
    for (unsigned i = 1; i != repo_size && result; i++) {
      slot_point initial_wrap = end_wrap + i;
      result = calculate_repository_index(initial_wrap, repo_size) != calculate_repository_index(end_wrap, repo_size);
      if (!result) {
        break;
      }
    }
  }

  // If the total number of slots is not multiple of the repository size, result should be false.
  if (SFN_MAX_VALUE * get_nof_slots_per_subframe(scs) * NOF_SUBFRAMES_PER_FRAME % min_repo_size) {
    ASSERT_FALSE(result);
  } else {
    ASSERT_TRUE(result);
  }
}

INSTANTIATE_TEST_SUITE_P(context_repository_helpers_param_test_suite,
                         context_repository_helpers_param_test,
                         ::testing::Combine(::testing::Values(subcarrier_spacing::kHz15,
                                                              subcarrier_spacing::kHz30,
                                                              subcarrier_spacing::kHz60,
                                                              subcarrier_spacing::kHz120,
                                                              subcarrier_spacing::kHz240),
                                            ::testing::Range(4U, 124U, 4U)));
