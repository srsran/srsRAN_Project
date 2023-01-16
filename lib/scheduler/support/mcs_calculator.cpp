/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mcs_calculator.h"

using namespace srsgnb;

// TODO: This table is temporary. Note that we might eventually change this table for a SNR vs Spectral Efficiency
//       table.
// This table contains the minimum required SNR for a given MCS index; the n-th (n = {0, ..., size - 1}) element of the
// table is the minimum SNR for MCS index n.
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

sch_mcs_index srsgnb::map_snr_to_mcs_ul(double snr)
{
  // The objective of this function is to find the maximum MCS that can be used for a given SNR. A possible approach to
  // this problem would be to get the iterator to the biggest element of the SNR vector not greater than the target SNR.
  // Instead, we do the following: (i) we rely on the built-in function std::upper_bound(), which returns an iterator to
  // the smallest element greater than the target SNR; (ii) we need to subtract 1 from the iterator returned by
  // std::upper_bound(), to obtain wanted MCS.
  const unsigned MAX_MCS = 28;
  const unsigned MIN_MCS = 0;

  // Check of the SNR is lower than the minimum, or greater than the maximum. If so, return the min or max MCS.
  if (snr <= ul_snr_mcs_table.front()) {
    return MIN_MCS;
  }
  // NOTE: The sign > is not sufficient, as, with an input snr == ul_snr_mcs_table.back(), the std::upper_bound()
  // below would return ul_snr_mcs_table.end(). To prevent this, we need the sign >= in the comparison.
  if (snr >= ul_snr_mcs_table.back()) {
    return MAX_MCS;
  }

  auto it_ub = std::upper_bound(ul_snr_mcs_table.begin(), ul_snr_mcs_table.end(), snr);

  // NOTE: By design, it_ub > ul_snr_mcs_table.begin(). All SNR values such it_ub == ul_snr_mcs_table.begin() are
  // handled above, in the "if (snr <= ul_snr_mcs_table.front())" statement above.
  return it_ub - ul_snr_mcs_table.begin() - 1;
}
