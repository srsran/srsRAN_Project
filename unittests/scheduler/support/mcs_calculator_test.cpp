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

// Duplicate of the SNR vs MCS table in \ref lib/scheduler/support/mcs_calculator.cpp, for test purposes.
static const std::array<double, 29> ul_snr_mcs_table = {
    // clang-format off
    /* MCS 0      1        2        3        4       5        6         7        8        9  */
     -4.5625, -3.5000, -2.5625, -1.3125, -0.5000,  0.5000,  1.3750,  2.3125,  3.1875,  4.1250,
    /* MCS 10    11       12       13       14      15       16        17       18       19  */
      5.0000,  5.6250,  6.4375,  7.4375,  8.4375,  9.2500, 10.0000, 10.6250, 11.1250, 12.0625,
    /* MCS 20    21       22       23       24      25       26        27       28 */
     13.1250, 13.9375,  14.8125, 15.8125, 16.5625, 17.3750, 18.1875, 19.0625, 20.5625
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
  const unsigned MAX_MCS = 28;
  const unsigned MIN_MCS = 0;

  // Check of the SNR is lower than the minimum, or greater than the maximum. If so, return the min or max MCS.
  if (snr <= ul_snr_mcs_table[0]) {
    return MIN_MCS;
  }

  // Go through the table, starting the lowest MCS and by increamenting by 1.
  for (size_t idx = 0; idx != ul_snr_mcs_table.size() - 1; ++idx) {
    if (snr >= ul_snr_mcs_table[idx] and snr < ul_snr_mcs_table[idx + 1]) {
      return idx;
    }
  }
  return MAX_MCS;
}

TEST(test_lower_upper_bound, test)
{
  // Lower bound.
  ASSERT_EQ(0, map_snr_to_mcs_ul(ul_snr_mcs_table.front()));
  ASSERT_EQ(0, map_snr_to_mcs_ul(ul_snr_mcs_table.front() - 0.1));

  // Upper bound
  ASSERT_EQ(28, map_snr_to_mcs_ul(ul_snr_mcs_table.back()));
  ASSERT_EQ(28, map_snr_to_mcs_ul(ul_snr_mcs_table.back() + 0.1));
}

TEST(test_manual_values, test_some_benchmark_values)
{
  ASSERT_EQ(1, map_snr_to_mcs_ul(ul_snr_mcs_table[1]));
  ASSERT_EQ(1, map_snr_to_mcs_ul(-3.0));
  ASSERT_EQ(9, map_snr_to_mcs_ul(ul_snr_mcs_table[9]));
  ASSERT_EQ(9, map_snr_to_mcs_ul(4.5));
  ASSERT_EQ(14, map_snr_to_mcs_ul(ul_snr_mcs_table[14]));
  ASSERT_EQ(14, map_snr_to_mcs_ul(8.7));
  ASSERT_EQ(19, map_snr_to_mcs_ul(ul_snr_mcs_table[19]));
  ASSERT_EQ(19, map_snr_to_mcs_ul(12.55));
  ASSERT_EQ(27, map_snr_to_mcs_ul(ul_snr_mcs_table[27]));
  ASSERT_EQ(27, map_snr_to_mcs_ul(20.51));
}

class snr_to_ul_mcs_tester : public ::testing::TestWithParam<double>
{};

TEST_P(snr_to_ul_mcs_tester, compare_bin_search_with_linear)
{
  ASSERT_EQ(linear_search_over_mcs_table(GetParam()), map_snr_to_mcs_ul(GetParam()).to_uint());
}

// Compare the binary search with linear search for a given interval.
INSTANTIATE_TEST_SUITE_P(test_snr_range, snr_to_ul_mcs_tester, testing::Range(-20.0, 40.0, 0.0133));
