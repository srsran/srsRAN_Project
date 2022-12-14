/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/support/mcs_calculator.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// SNR vs MCS entry for the table.
struct snr_mcs {
  // Minimum SNR required for a given MCS index.
  double min_snr;
  // MCS index.
  unsigned mcs;
};

// Duplicate of the SNR vs MCS table in \ref lib/scheduler/support/mcs_calculator.cpp, for test purposes.
static const std::array<snr_mcs, 29> ul_snr_mcs_table = {
    // clang-format off
    snr_mcs{-4.5625, 0},
    snr_mcs{ -3.5, 1},
    snr_mcs{-2.5625, 2},
    snr_mcs{-1.3125, 3},
    snr_mcs{-0.5, 4},
    snr_mcs{0.5, 5},
    snr_mcs{1.375 , 6},
    snr_mcs{2.3125, 7},
    snr_mcs{3.1875, 8},
    snr_mcs{4.125, 9},
    snr_mcs{5, 10},
    snr_mcs{5.625, 11},
    snr_mcs{6.4375, 12},
    snr_mcs{7.4375, 13},
    snr_mcs{8.4375, 14},
    snr_mcs{9.25, 15},
    snr_mcs{10, 16},
    snr_mcs{10.625 , 17},
    snr_mcs{11.125, 18},
    snr_mcs{12.0625, 19},
    snr_mcs{13.125, 20},
    snr_mcs{13.9375, 21},
    snr_mcs{14.8125, 22},
    snr_mcs{15.8125, 23},
    snr_mcs{16.5625, 24},
    snr_mcs{17.375, 25},
    snr_mcs{18.1875, 26},
    snr_mcs{19.0625, 27},
    snr_mcs{20.5625, 28}
    // clang-format on
};

// SNR vs MCS entry for the table.
struct snr_mcs_test_entry {
  // Minimum SNR required for a given MCS index.
  double snr;
  // MCS index.
  unsigned mcs;
};

// Helper that searches (linearly) for the maximum MCS that can be used for a given SNR. Used to benchmark \ref
// map_snr_to_mcs_ul().
static unsigned linear_search_over_mcs_table(double snr)
{
  // Check of the SNR is lower than the minimum, or greater than the maximum. If so, return the min or max MCS.
  if (snr <= ul_snr_mcs_table[0].min_snr) {
    return ul_snr_mcs_table[0].mcs;
  }

  // Go through the table, starting the lowest MCS and by increamenting by 1.
  for (size_t idx = 0; idx != ul_snr_mcs_table.size() - 1; ++idx) {
    if (snr >= ul_snr_mcs_table[idx].min_snr and snr < ul_snr_mcs_table[idx + 1].min_snr) {
      return ul_snr_mcs_table[idx].mcs;
    }
  }
  return ul_snr_mcs_table[28].mcs;
}

TEST(test_lower_upper_bound, test)
{
  // Lower bound.
  ASSERT_EQ(ul_snr_mcs_table.front().mcs, map_snr_to_mcs_ul(ul_snr_mcs_table.front().min_snr));
  ASSERT_EQ(ul_snr_mcs_table.front().mcs, map_snr_to_mcs_ul(ul_snr_mcs_table.front().min_snr - 0.1));

  // Upper bound
  ASSERT_EQ(ul_snr_mcs_table.back().mcs, map_snr_to_mcs_ul(ul_snr_mcs_table.back().min_snr));
  ASSERT_EQ(ul_snr_mcs_table.back().mcs, map_snr_to_mcs_ul(ul_snr_mcs_table.back().min_snr + 0.1));
}

class snr_to_ul_mcs_tester : public ::testing::TestWithParam<double>
{};

TEST_P(snr_to_ul_mcs_tester, compare_bin_search_with_linear)
{
  ASSERT_EQ(linear_search_over_mcs_table(GetParam()), map_snr_to_mcs_ul(GetParam()).to_uint());
}

// Compare the binary search with linear search for a given interval.
INSTANTIATE_TEST_SUITE_P(test_snr_range, snr_to_ul_mcs_tester, testing::Range(-20.0, 40.0, 0.0133));
