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

struct snr_mcs {
  double   min_snr;
  unsigned mcs;
};

static const std::array<snr_mcs, 29> ul_snr_mcs_table = {
    // clang-format off
    snr_mcs{-4.5625, 0},
    snr_mcs{ 0.5, 1},
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

unsigned srsgnb::map_snr_to_mcs(double snr)
{
  if (snr <= ul_snr_mcs_table.front().min_snr) {
    return ul_snr_mcs_table.front().mcs;
  }
  if (snr >= ul_snr_mcs_table.back().min_snr) {
    return ul_snr_mcs_table.back().mcs;
  }

  size_t lb_idx{0}, ub_idx{sizeof(ul_snr_mcs_table )};
  size_t mid_point;
  // Look in the first half.
  while (ub_idx - lb_idx > 1){
    mid_point = lb_idx + (ub_idx - lb_idx) / 2;
    // Look in the first half.
    if (snr < ul_snr_mcs_table[mid_point].min_snr){
      ub_idx = mid_point;
    }
    // Look in the second half.
    else {
      lb_idx = mid_point;
    }
  }
  return ul_snr_mcs_table[mid_point].mcs;
}

ul_mcs srsgnb::get_ul_mcs(tbs_calculator_configuration tbs_params, double ul_snr)
{
  return {};
}
