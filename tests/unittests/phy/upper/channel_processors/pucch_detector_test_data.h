/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 14-Oct-2022:
//   + "srsPUCCHDetectorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pucch_detector.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/pucch/pucch_mapping.h"
#include "srsgnb/support/file_vector.h"

namespace srsran {

struct test_case_t {
  pucch_detector::format1_configuration                   cfg       = {};
  float                                                   noise_var = 0;
  std::vector<uint8_t>                                    sr_bit;
  std::vector<uint8_t>                                    ack_bits;
  file_vector<resource_grid_reader_spy::expected_entry_t> received_symbols;
  file_vector<resource_grid_reader_spy::expected_entry_t> ch_estimates;
};

static const std::vector<test_case_t> pucch_detector_test_data = {
    // clang-format off
  {{{0, 9}, cyclic_prefix::NORMAL, 35, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 640, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols0.dat"}, {"test_data/pucch_detector_test_ch_estimates0.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 7, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 766, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols1.dat"}, {"test_data/pucch_detector_test_ch_estimates1.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 26, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 6, 3, 61, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols2.dat"}, {"test_data/pucch_detector_test_ch_estimates2.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 4, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 6, 6, 115, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols3.dat"}, {"test_data/pucch_detector_test_ch_estimates3.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 37, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 4, 6, 21, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols4.dat"}, {"test_data/pucch_detector_test_ch_estimates4.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 2, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 6, 3, 354, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols5.dat"}, {"test_data/pucch_detector_test_ch_estimates5.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 0, {38}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 58, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols6.dat"}, {"test_data/pucch_detector_test_ch_estimates6.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 24, {41}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 182, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols7.dat"}, {"test_data/pucch_detector_test_ch_estimates7.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 14, {29}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 714, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols8.dat"}, {"test_data/pucch_detector_test_ch_estimates8.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 38, {42}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 169, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols9.dat"}, {"test_data/pucch_detector_test_ch_estimates9.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 4, {32}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 842, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols10.dat"}, {"test_data/pucch_detector_test_ch_estimates10.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 34, {15}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 219, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols11.dat"}, {"test_data/pucch_detector_test_ch_estimates11.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 27, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 3, 762, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols12.dat"}, {"test_data/pucch_detector_test_ch_estimates12.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 16, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 5, 6, 200, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols13.dat"}, {"test_data/pucch_detector_test_ch_estimates13.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 49, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 869, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols14.dat"}, {"test_data/pucch_detector_test_ch_estimates14.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 9, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 3, 951, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols15.dat"}, {"test_data/pucch_detector_test_ch_estimates15.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 43, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 3, 9, 476, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols16.dat"}, {"test_data/pucch_detector_test_ch_estimates16.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 8, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 6, 833, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols17.dat"}, {"test_data/pucch_detector_test_ch_estimates17.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 25, {12}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 777, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols18.dat"}, {"test_data/pucch_detector_test_ch_estimates18.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 8, {5}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 603, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols19.dat"}, {"test_data/pucch_detector_test_ch_estimates19.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 30, {38}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 724, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols20.dat"}, {"test_data/pucch_detector_test_ch_estimates20.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 4, {2}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 176, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols21.dat"}, {"test_data/pucch_detector_test_ch_estimates21.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 5, {44}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 6, 463, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols22.dat"}, {"test_data/pucch_detector_test_ch_estimates22.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 37, {6}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 543, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols23.dat"}, {"test_data/pucch_detector_test_ch_estimates23.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 42, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 787, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols24.dat"}, {"test_data/pucch_detector_test_ch_estimates24.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 24, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 543, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols25.dat"}, {"test_data/pucch_detector_test_ch_estimates25.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 19, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 337, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols26.dat"}, {"test_data/pucch_detector_test_ch_estimates26.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 17, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 6, 142, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols27.dat"}, {"test_data/pucch_detector_test_ch_estimates27.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 2, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 938, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols28.dat"}, {"test_data/pucch_detector_test_ch_estimates28.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 1, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 977, 2}, 0.01, {0}, {1, 0}, {"test_data/pucch_detector_test_received_symbols29.dat"}, {"test_data/pucch_detector_test_ch_estimates29.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 34, {48}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 481, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols30.dat"}, {"test_data/pucch_detector_test_ch_estimates30.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 24, {40}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 181, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols31.dat"}, {"test_data/pucch_detector_test_ch_estimates31.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 38, {25}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 935, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols32.dat"}, {"test_data/pucch_detector_test_ch_estimates32.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 4, {11}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 521, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols33.dat"}, {"test_data/pucch_detector_test_ch_estimates33.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 13, {49}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 545, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols34.dat"}, {"test_data/pucch_detector_test_ch_estimates34.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 22, {20}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 354, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols35.dat"}, {"test_data/pucch_detector_test_ch_estimates35.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 42, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 511, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols36.dat"}, {"test_data/pucch_detector_test_ch_estimates36.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 41, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 909, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols37.dat"}, {"test_data/pucch_detector_test_ch_estimates37.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 14, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 921, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols38.dat"}, {"test_data/pucch_detector_test_ch_estimates38.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 32, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 6, 153, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols39.dat"}, {"test_data/pucch_detector_test_ch_estimates39.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 46, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 621, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols40.dat"}, {"test_data/pucch_detector_test_ch_estimates40.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 37, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 982, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols41.dat"}, {"test_data/pucch_detector_test_ch_estimates41.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 35, {5}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 240, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols42.dat"}, {"test_data/pucch_detector_test_ch_estimates42.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 45, {8}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 435, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols43.dat"}, {"test_data/pucch_detector_test_ch_estimates43.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 39, {49}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 572, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols44.dat"}, {"test_data/pucch_detector_test_ch_estimates44.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 41, {11}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 970, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols45.dat"}, {"test_data/pucch_detector_test_ch_estimates45.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 10, {13}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 463, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols46.dat"}, {"test_data/pucch_detector_test_ch_estimates46.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 1, {42}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 682, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols47.dat"}, {"test_data/pucch_detector_test_ch_estimates47.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 6, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 221, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols48.dat"}, {"test_data/pucch_detector_test_ch_estimates48.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 48, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 5, 3, 706, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols49.dat"}, {"test_data/pucch_detector_test_ch_estimates49.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 26, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 5, 9, 863, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols50.dat"}, {"test_data/pucch_detector_test_ch_estimates50.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 25, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 3, 9, 963, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols51.dat"}, {"test_data/pucch_detector_test_ch_estimates51.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 50, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 979, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols52.dat"}, {"test_data/pucch_detector_test_ch_estimates52.dat"}},
  {{{1, 15}, cyclic_prefix::NORMAL, 7, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 5, 0, 524, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols53.dat"}, {"test_data/pucch_detector_test_ch_estimates53.dat"}},
  {{{1, 15}, cyclic_prefix::NORMAL, 3, {4}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 125, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols54.dat"}, {"test_data/pucch_detector_test_ch_estimates54.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 33, {10}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 60, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols55.dat"}, {"test_data/pucch_detector_test_ch_estimates55.dat"}},
  {{{1, 8}, cyclic_prefix::NORMAL, 13, {39}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 565, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols56.dat"}, {"test_data/pucch_detector_test_ch_estimates56.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 39, {27}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 6, 338, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols57.dat"}, {"test_data/pucch_detector_test_ch_estimates57.dat"}},
  {{{1, 8}, cyclic_prefix::NORMAL, 30, {10}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 275, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols58.dat"}, {"test_data/pucch_detector_test_ch_estimates58.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 25, {36}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 215, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols59.dat"}, {"test_data/pucch_detector_test_ch_estimates59.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 16, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 98, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols60.dat"}, {"test_data/pucch_detector_test_ch_estimates60.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 44, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 3, 343, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols61.dat"}, {"test_data/pucch_detector_test_ch_estimates61.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 36, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 9, 638, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols62.dat"}, {"test_data/pucch_detector_test_ch_estimates62.dat"}},
  {{{1, 16}, cyclic_prefix::NORMAL, 3, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 5, 6, 246, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols63.dat"}, {"test_data/pucch_detector_test_ch_estimates63.dat"}},
  {{{1, 16}, cyclic_prefix::NORMAL, 27, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 0, 468, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols64.dat"}, {"test_data/pucch_detector_test_ch_estimates64.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 13, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 3, 6, 692, 2}, 0.01, {0}, {1, 0}, {"test_data/pucch_detector_test_received_symbols65.dat"}, {"test_data/pucch_detector_test_ch_estimates65.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 43, {11}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 209, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols66.dat"}, {"test_data/pucch_detector_test_ch_estimates66.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 34, {10}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 948, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols67.dat"}, {"test_data/pucch_detector_test_ch_estimates67.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 36, {10}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 617, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols68.dat"}, {"test_data/pucch_detector_test_ch_estimates68.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 27, {50}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 1005, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols69.dat"}, {"test_data/pucch_detector_test_ch_estimates69.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 50, {46}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 472, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols70.dat"}, {"test_data/pucch_detector_test_ch_estimates70.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 8, {28}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 425, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols71.dat"}, {"test_data/pucch_detector_test_ch_estimates71.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 7, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 895, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols72.dat"}, {"test_data/pucch_detector_test_ch_estimates72.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 9, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 826, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols73.dat"}, {"test_data/pucch_detector_test_ch_estimates73.dat"}},
  {{{1, 16}, cyclic_prefix::NORMAL, 10, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 780, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols74.dat"}, {"test_data/pucch_detector_test_ch_estimates74.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 31, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 235, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols75.dat"}, {"test_data/pucch_detector_test_ch_estimates75.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 14, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 83, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols76.dat"}, {"test_data/pucch_detector_test_ch_estimates76.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 0, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 6, 249, 2}, 0.01, {0}, {1, 0}, {"test_data/pucch_detector_test_received_symbols77.dat"}, {"test_data/pucch_detector_test_ch_estimates77.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 42, {16}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 335, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols78.dat"}, {"test_data/pucch_detector_test_ch_estimates78.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 15, {45}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 130, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols79.dat"}, {"test_data/pucch_detector_test_ch_estimates79.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 0, {15}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 300, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols80.dat"}, {"test_data/pucch_detector_test_ch_estimates80.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 41, {43}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 432, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols81.dat"}, {"test_data/pucch_detector_test_ch_estimates81.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 45, {1}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 510, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols82.dat"}, {"test_data/pucch_detector_test_ch_estimates82.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 1, {37}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 121, 2}, 0.01, {0}, {1, 1}, {"test_data/pucch_detector_test_received_symbols83.dat"}, {"test_data/pucch_detector_test_ch_estimates83.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 11, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 814, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols84.dat"}, {"test_data/pucch_detector_test_ch_estimates84.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 20, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 6, 500, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols85.dat"}, {"test_data/pucch_detector_test_ch_estimates85.dat"}},
  {{{1, 8}, cyclic_prefix::NORMAL, 19, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 813, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols86.dat"}, {"test_data/pucch_detector_test_ch_estimates86.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 28, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 444, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols87.dat"}, {"test_data/pucch_detector_test_ch_estimates87.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 37, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 616, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols88.dat"}, {"test_data/pucch_detector_test_ch_estimates88.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 29, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 57, 2}, 0.01, {1}, {1, 0}, {"test_data/pucch_detector_test_received_symbols89.dat"}, {"test_data/pucch_detector_test_ch_estimates89.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 28, {31}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 621, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols90.dat"}, {"test_data/pucch_detector_test_ch_estimates90.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 26, {8}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 315, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols91.dat"}, {"test_data/pucch_detector_test_ch_estimates91.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 45, {16}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 693, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols92.dat"}, {"test_data/pucch_detector_test_ch_estimates92.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 13, {49}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 163, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols93.dat"}, {"test_data/pucch_detector_test_ch_estimates93.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 50, {28}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 1006, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols94.dat"}, {"test_data/pucch_detector_test_ch_estimates94.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 33, {32}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 364, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols95.dat"}, {"test_data/pucch_detector_test_ch_estimates95.dat"}},
    // clang-format on
};

} // namespace srsran
