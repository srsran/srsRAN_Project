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

// SNR vs MCS entry for the table.
struct snr_mcs {
  // Minimum SNR required for a given MCS index.
  double min_snr;
  // MCS index.
  unsigned mcs;
};

// TODO: This table is temporary. Note that we might eventually change this table for a SNR vs Spectral Efficiency
//       table.
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

sch_mcs_index srsgnb::map_snr_to_mcs_ul(double snr)
{
  // Check of the SNR is lower than the minimum, or greater than the maximum. If so, return the min or max MCS.
  if (snr <= ul_snr_mcs_table.front().min_snr) {
    return ul_snr_mcs_table.front().mcs;
  }
  if (snr >= ul_snr_mcs_table.back().min_snr) {
    return ul_snr_mcs_table.back().mcs;
  }

  // Binary search over the SNR to MCS table. The objective of this search is to find the maximum MCS that can be used
  // for a given SNR. By means of bisection, the search looks for the SNR interval [SNR_n, SNR_{n+1}) that contains the
  // input SNR, where SNR_n are the SNR values of the \ref ul_snr_mcs_table; the MCS that gets returned is the one
  // corresponding to SNR_n.
  size_t   lb_idx{0}, ub_idx{ul_snr_mcs_table.size() - 1};
  size_t   mid_point;
  unsigned mcs{ul_snr_mcs_table[lb_idx].mcs};
  // Keep looking until the upper_bound index and lower_bound index
  while (ub_idx - lb_idx > 1) {
    mid_point = lb_idx + (ub_idx - lb_idx) / 2;
    // Look in the first half (left hand-side half) of the vector.
    if (snr < ul_snr_mcs_table[mid_point].min_snr) {
      ub_idx = mid_point;
    }
    // Look in the second half.
    else {
      lb_idx = mid_point;
    }
    // Save the MCS corresponding to the lower bound of the SNR interval.
    mcs = ul_snr_mcs_table[lb_idx].mcs;
  }
  return mcs;
}
