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

#include "srsran/ran/resource_allocation/rb_bitmap.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/resource_allocation/vrb_to_prb.h"
#include "srsran/ran/resource_block.h"
#include <gtest/gtest.h>
#include <utility>

using namespace srsran;
using namespace srsran::vrb_to_prb;

class non_interleaved_test : public ::testing::TestWithParam<configuration>
{
protected:
  const configuration           config = GetParam();
  const non_interleaved_mapping map    = config;
};

TEST_P(non_interleaved_test, vrb_to_prb_intervals)
{
  const vrb_interval vrbs{3, 7};
  const prb_interval result = map.vrb_to_prb(vrbs);

  const prb_interval expected{config.coreset_start + 3, config.coreset_start + 7};
  EXPECT_EQ(result, expected);
}

TEST_P(non_interleaved_test, vrb_to_prb_bitmaps_every_vrb_goes_to_a_different_prb)
{
  static constexpr unsigned bwp_size = 133;

  prb_bitmap prbs(bwp_size);
  for (unsigned vrb = 0, vrb_end = bwp_size - config.coreset_start; vrb != vrb_end; ++vrb) {
    vrb_bitmap vrbs(bwp_size - config.coreset_start);
    vrbs.set(vrb);

    const prb_bitmap result = map.vrb_to_prb(bwp_size, vrbs);
    ASSERT_EQ(result.size(), bwp_size);
    ASSERT_EQ(result.count(), 1);

    const int prb = result.find_lowest();
    ASSERT_GE(prb, config.coreset_start);
    ASSERT_FALSE(prbs.test(prb));
    prbs.set(prb);
  }
}

TEST_P(non_interleaved_test, prb_to_vrb_bitmaps_every_prb_goes_to_a_different_vrb)
{
  static constexpr unsigned bwp_size = 133;

  vrb_bitmap vrbs(bwp_size - config.coreset_start);
  for (unsigned prb = config.coreset_start; prb != bwp_size; ++prb) {
    prb_bitmap prbs(bwp_size);
    prbs.set(prb);

    const vrb_bitmap result = map.prb_to_vrb(prbs);
    ASSERT_EQ(result.size(), bwp_size - config.coreset_start);
    ASSERT_EQ(result.count(), 1);

    const int vrb = result.find_lowest();
    ASSERT_FALSE(vrbs.test(vrb));
    vrbs.set(vrb);
  }
}

TEST_P(non_interleaved_test, vrb_to_crb_bitmaps_all_vrbs_end_inside_the_bwp)
{
  static constexpr unsigned bwp_start = 20;
  static constexpr unsigned bwp_size  = 133;

  vrb_bitmap vrbs(bwp_size - config.coreset_start);
  vrbs.fill();

  const crb_bitmap result = map.vrb_to_crb(bwp_start, bwp_size, vrbs);
  ASSERT_EQ(result.size(), bwp_start + bwp_size);
  ASSERT_EQ(result.count(), bwp_size - config.coreset_start);
  ASSERT_EQ(result.find_lowest(), bwp_start + config.coreset_start);
}

INSTANTIATE_TEST_SUITE_P(test_non_interleaved_mapping,
                         non_interleaved_test,
                         ::testing::Values(create_non_interleaved_other(),
                                           create_non_interleaved_common_ss(1),
                                           create_non_interleaved_common_ss(2),
                                           create_non_interleaved_common_ss(3),
                                           create_non_interleaved_common_ss(4),
                                           create_non_interleaved_common_ss(10)));

class interleaved_test : public ::testing::TestWithParam<configuration>
{
protected:
  const configuration       config = GetParam();
  const interleaved_mapping map    = config;
};

TEST_P(interleaved_test, vrb_to_prb_intervals)
{
  const std::vector<vrb_interval> test_cases = {
      {},
      {0, 10},
      {3, 20},
      {0, config.nof_rbs / 2},
      {config.nof_rbs / 2, config.nof_rbs},
      {0, config.nof_rbs},
  };

  for (auto vrbs : test_cases) {
    const std::pair<prb_interval, prb_interval> result = map.vrb_to_prb(vrbs);
    EXPECT_EQ(result.first.length() + result.second.length(), vrbs.length());

    // Bitset representing VRBs of the original interval that are present in the resulting PRB intervals.
    bounded_bitset<MAX_NOF_PRBS> vrb_bits(vrbs.length());
    for (auto prbs : {result.first, result.second}) {
      for (unsigned prb = prbs.start(), prb_end = prbs.stop(); prb != prb_end; ++prb) {
        unsigned vrb = map.prb_to_vrb(prb);
        EXPECT_TRUE(vrb >= vrbs.start() && vrb < vrbs.stop());
        EXPECT_FALSE(vrb_bits.test(vrb - vrbs.start()));
        vrb_bits.set(vrb - vrbs.start());
      }
    }
  }
}

TEST_P(interleaved_test, vrb_to_prb_bitmaps_every_vrb_goes_to_a_different_prb)
{
  const unsigned bwp_size = config.coreset_start + config.nof_rbs;

  prb_bitmap prbs(bwp_size);
  for (unsigned vrb = 0, vrb_end = bwp_size - config.coreset_start; vrb != vrb_end; ++vrb) {
    vrb_bitmap vrbs(config.nof_rbs);
    vrbs.set(vrb);

    const prb_bitmap result = map.vrb_to_prb(bwp_size, vrbs);
    ASSERT_EQ(result.size(), bwp_size);
    ASSERT_EQ(result.count(), 1);

    const int prb = result.find_lowest();
    ASSERT_GE(prb, config.coreset_start);
    ASSERT_FALSE(prbs.test(prb));
    prbs.set(prb);
  }
}

TEST_P(interleaved_test, prb_to_vrb_bitmaps_every_prb_goes_to_a_different_vrb)
{
  const unsigned bwp_size = config.coreset_start + config.nof_rbs;

  vrb_bitmap vrbs(config.nof_rbs);
  for (unsigned prb = config.coreset_start; prb != bwp_size; ++prb) {
    prb_bitmap prbs(bwp_size);
    prbs.set(prb);

    const vrb_bitmap result = map.prb_to_vrb(prbs);
    ASSERT_EQ(result.size(), config.nof_rbs);
    ASSERT_EQ(result.count(), 1);

    const int vrb = result.find_lowest();
    ASSERT_FALSE(vrbs.test(vrb));
    vrbs.set(vrb);
  }
}

TEST_P(interleaved_test, vrb_to_crb_bitmaps_all_vrbs_end_inside_the_bwp)
{
  static constexpr unsigned bwp_start = 20;
  const unsigned            bwp_size  = config.coreset_start + config.nof_rbs;

  vrb_bitmap vrbs(config.nof_rbs);
  vrbs.fill();

  const crb_bitmap result = map.vrb_to_crb(bwp_start, bwp_size, vrbs);
  ASSERT_EQ(result.size(), bwp_start + bwp_size);
  ASSERT_EQ(result.count(), bwp_size - config.coreset_start);
  ASSERT_EQ(result.find_lowest(), bwp_start + config.coreset_start);
}

INSTANTIATE_TEST_SUITE_P(test_interleaved_mapping,
                         interleaved_test,
                         ::testing::Values(create_interleaved_coreset0(10, 133),
                                           create_interleaved_common_ss(10, 0, 133),
                                           create_interleaved_common_ss(10, 5, 133),
                                           create_interleaved_other(0, 133, mapping_type::interleaved_n2),
                                           create_interleaved_other(0, 133, mapping_type::interleaved_n4),
                                           create_interleaved_other(5, 133, mapping_type::interleaved_n2),
                                           create_interleaved_other(5, 133, mapping_type::interleaved_n4)));
