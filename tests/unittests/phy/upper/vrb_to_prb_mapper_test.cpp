/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/vrb_to_prb_mapper.h"
#include "srsgnb/support/srsgnb_test.h"
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