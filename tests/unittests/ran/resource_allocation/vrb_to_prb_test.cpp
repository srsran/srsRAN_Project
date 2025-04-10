/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::vrb_to_prb;

class non_interleaved_other_test : public ::testing::Test
{
protected:
  const non_interleaved_mapping map = create_non_interleaved_other();
};

TEST_F(non_interleaved_other_test, vrb_to_prb_intervals_stay_the_same)
{
  vrb_interval vrbs{3, 7};
  const auto&  result = map.vrb_to_prb(vrbs);

  const prb_interval expected{3, 7};
  EXPECT_EQ(result, expected);
}

class non_interleaved_common_ss : public ::testing::Test
{
protected:
  non_interleaved_mapping map = create_non_interleaved_common_ss(10);
};

TEST_F(non_interleaved_common_ss, vrb_to_prb_interval)
{
  vrb_interval vrbs{3, 7};
  const auto&  result = map.vrb_to_prb(vrbs);

  const prb_interval expected{13, 17};
  EXPECT_EQ(result, expected);
}
