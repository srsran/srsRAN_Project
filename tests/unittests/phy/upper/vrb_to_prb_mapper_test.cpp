/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/phy/upper/vrb_to_prb_mapper.h"
#include "srsran/support/srsran_test.h"
#include <array>

using namespace srsran;

static void test_case1_non_interleaved_common_ss()
{
  unsigned                N_start_coreset = 123;
  unsigned                N_bwp_size      = 48;
  const vrb_to_prb_mapper mapper          = vrb_to_prb_mapper::create_non_interleaved_common_ss(N_start_coreset);
  TESTASSERT_EQ(mapper.is_interleaved(), false);
  TESTASSERT_EQ(mapper.get_coreset_start(), N_start_coreset);

  static_vector<uint16_t, MAX_RB> vrb_to_prb_indices = mapper.get_allocation_indices(N_bwp_size);

  static_vector<uint16_t, MAX_RB> gold_vrb_to_prb_indices(N_bwp_size);
  for (unsigned vrb_index = 0; vrb_index != N_bwp_size; ++vrb_index) {
    gold_vrb_to_prb_indices[vrb_index] = N_start_coreset + vrb_index;
  }

  TESTASSERT_EQ(span<uint16_t>(vrb_to_prb_indices), span<uint16_t>(gold_vrb_to_prb_indices));
}

static void test_case2_non_interleaved_other()
{
  unsigned                N_bwp_size = 48;
  const vrb_to_prb_mapper mapper     = vrb_to_prb_mapper::create_non_interleaved_other();
  TESTASSERT_EQ(mapper.is_interleaved(), false);
  TESTASSERT_EQ(mapper.get_coreset_start(), 0);

  static_vector<uint16_t, MAX_RB> vrb_to_prb_indices = mapper.get_allocation_indices(N_bwp_size);

  static_vector<uint16_t, MAX_RB> gold_vrb_to_prb_indices(N_bwp_size);
  for (unsigned vrb_index = 0; vrb_index != N_bwp_size; ++vrb_index) {
    gold_vrb_to_prb_indices[vrb_index] = vrb_index;
  }

  TESTASSERT_EQ(span<uint16_t>(vrb_to_prb_indices), span<uint16_t>(gold_vrb_to_prb_indices));
}

int main()
{
  test_case1_non_interleaved_common_ss();
  test_case2_non_interleaved_other();
}