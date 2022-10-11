/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 11-Oct-2022:
//   + "srsPUCCHDetectorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pucch_detector.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

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
  {{{0, 1}, cyclic_prefix::NORMAL, 0, {}, 0, 14, 0, 1, 0, 9, 290, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols0.dat"}, {"test_data/pucch_detector_test_ch_estimates0.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 42, {}, 0, 14, 0, 1, 6, 0, 377, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols1.dat"}, {"test_data/pucch_detector_test_ch_estimates1.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 38, {}, 0, 14, 0, 1, 0, 3, 707, 1, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols2.dat"}, {"test_data/pucch_detector_test_ch_estimates2.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 46, {}, 0, 14, 0, 1, 5, 9, 322, 0, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols3.dat"}, {"test_data/pucch_detector_test_ch_estimates3.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 43, {}, 0, 14, 0, 1, 0, 0, 394, 1, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols4.dat"}, {"test_data/pucch_detector_test_ch_estimates4.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 7, {30}, 0, 14, 0, 1, 1, 9, 490, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols5.dat"}, {"test_data/pucch_detector_test_ch_estimates5.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 13, {37}, 0, 14, 0, 1, 1, 6, 948, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols6.dat"}, {"test_data/pucch_detector_test_ch_estimates6.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 36, {1}, 0, 14, 0, 1, 0, 3, 297, 1, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols7.dat"}, {"test_data/pucch_detector_test_ch_estimates7.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 43, {50}, 0, 14, 0, 1, 2, 6, 529, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols8.dat"}, {"test_data/pucch_detector_test_ch_estimates8.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 9, {13}, 0, 14, 0, 1, 2, 0, 86, 1, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols9.dat"}, {"test_data/pucch_detector_test_ch_estimates9.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 27, {}, 1, 13, 0, 1, 0, 6, 940, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols10.dat"}, {"test_data/pucch_detector_test_ch_estimates10.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 44, {}, 1, 13, 0, 1, 5, 0, 8, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols11.dat"}, {"test_data/pucch_detector_test_ch_estimates11.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 3, {}, 1, 13, 0, 1, 4, 9, 638, 1, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols12.dat"}, {"test_data/pucch_detector_test_ch_estimates12.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 48, {}, 1, 13, 0, 1, 4, 3, 127, 0, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols13.dat"}, {"test_data/pucch_detector_test_ch_estimates13.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 44, {}, 1, 13, 0, 1, 1, 9, 199, 1, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols14.dat"}, {"test_data/pucch_detector_test_ch_estimates14.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 42, {20}, 1, 13, 0, 1, 1, 0, 471, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols15.dat"}, {"test_data/pucch_detector_test_ch_estimates15.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 28, {50}, 1, 13, 0, 1, 0, 0, 149, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols16.dat"}, {"test_data/pucch_detector_test_ch_estimates16.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 23, {19}, 1, 13, 0, 1, 0, 3, 261, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols17.dat"}, {"test_data/pucch_detector_test_ch_estimates17.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 21, {45}, 1, 13, 0, 1, 0, 9, 405, 0, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols18.dat"}, {"test_data/pucch_detector_test_ch_estimates18.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 34, {5}, 1, 13, 0, 1, 1, 0, 277, 1, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols19.dat"}, {"test_data/pucch_detector_test_ch_estimates19.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 15, {}, 5, 5, 0, 1, 0, 0, 332, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols20.dat"}, {"test_data/pucch_detector_test_ch_estimates20.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 33, {}, 5, 5, 0, 1, 1, 6, 59, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols21.dat"}, {"test_data/pucch_detector_test_ch_estimates21.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 42, {}, 5, 5, 0, 1, 0, 6, 882, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols22.dat"}, {"test_data/pucch_detector_test_ch_estimates22.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 17, {}, 5, 5, 0, 1, 1, 3, 102, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols23.dat"}, {"test_data/pucch_detector_test_ch_estimates23.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 48, {}, 5, 5, 0, 1, 1, 9, 674, 1, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols24.dat"}, {"test_data/pucch_detector_test_ch_estimates24.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 50, {47}, 5, 5, 0, 1, 0, 9, 487, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols25.dat"}, {"test_data/pucch_detector_test_ch_estimates25.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 37, {39}, 5, 5, 0, 1, 0, 0, 631, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols26.dat"}, {"test_data/pucch_detector_test_ch_estimates26.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 25, {26}, 5, 5, 0, 1, 0, 6, 786, 1, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols27.dat"}, {"test_data/pucch_detector_test_ch_estimates27.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 46, {28}, 5, 5, 0, 1, 0, 9, 297, 0, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols28.dat"}, {"test_data/pucch_detector_test_ch_estimates28.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 8, {39}, 5, 5, 0, 1, 0, 0, 364, 1, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols29.dat"}, {"test_data/pucch_detector_test_ch_estimates29.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 13, {}, 10, 4, 0, 1, 0, 3, 148, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols30.dat"}, {"test_data/pucch_detector_test_ch_estimates30.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 36, {}, 10, 4, 0, 1, 0, 0, 1007, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols31.dat"}, {"test_data/pucch_detector_test_ch_estimates31.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 5, {}, 10, 4, 0, 1, 1, 6, 953, 1, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols32.dat"}, {"test_data/pucch_detector_test_ch_estimates32.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 50, {}, 10, 4, 0, 1, 0, 9, 228, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols33.dat"}, {"test_data/pucch_detector_test_ch_estimates33.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 30, {}, 10, 4, 0, 1, 1, 3, 370, 1, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols34.dat"}, {"test_data/pucch_detector_test_ch_estimates34.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 15, {32}, 10, 4, 0, 1, 0, 3, 796, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols35.dat"}, {"test_data/pucch_detector_test_ch_estimates35.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 10, {44}, 10, 4, 0, 1, 0, 0, 418, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols36.dat"}, {"test_data/pucch_detector_test_ch_estimates36.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 48, {27}, 10, 4, 0, 1, 0, 3, 231, 1, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols37.dat"}, {"test_data/pucch_detector_test_ch_estimates37.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 25, {19}, 10, 4, 0, 1, 0, 6, 481, 0, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols38.dat"}, {"test_data/pucch_detector_test_ch_estimates38.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 15, {48}, 10, 4, 0, 1, 0, 3, 725, 1, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols39.dat"}, {"test_data/pucch_detector_test_ch_estimates39.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 6, {}, 0, 14, 0, 1, 6, 0, 646, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols40.dat"}, {"test_data/pucch_detector_test_ch_estimates40.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 45, {}, 0, 14, 0, 1, 1, 9, 583, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols41.dat"}, {"test_data/pucch_detector_test_ch_estimates41.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 47, {}, 0, 14, 0, 1, 4, 3, 713, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols42.dat"}, {"test_data/pucch_detector_test_ch_estimates42.dat"}},
  {{{1, 13}, cyclic_prefix::NORMAL, 20, {}, 0, 14, 0, 1, 6, 6, 671, 0, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols43.dat"}, {"test_data/pucch_detector_test_ch_estimates43.dat"}},
  {{{1, 4}, cyclic_prefix::NORMAL, 24, {}, 0, 14, 0, 1, 6, 9, 760, 1, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols44.dat"}, {"test_data/pucch_detector_test_ch_estimates44.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 50, {42}, 0, 14, 0, 1, 0, 3, 376, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols45.dat"}, {"test_data/pucch_detector_test_ch_estimates45.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 26, {25}, 0, 14, 0, 1, 2, 3, 401, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols46.dat"}, {"test_data/pucch_detector_test_ch_estimates46.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 4, {6}, 0, 14, 0, 1, 1, 6, 969, 1, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols47.dat"}, {"test_data/pucch_detector_test_ch_estimates47.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 35, {38}, 0, 14, 0, 1, 2, 0, 974, 0, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols48.dat"}, {"test_data/pucch_detector_test_ch_estimates48.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 41, {15}, 0, 14, 0, 1, 0, 9, 912, 1, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols49.dat"}, {"test_data/pucch_detector_test_ch_estimates49.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 18, {}, 1, 13, 0, 1, 5, 9, 73, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols50.dat"}, {"test_data/pucch_detector_test_ch_estimates50.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 26, {}, 1, 13, 0, 1, 1, 3, 784, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols51.dat"}, {"test_data/pucch_detector_test_ch_estimates51.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 5, {}, 1, 13, 0, 1, 1, 0, 774, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols52.dat"}, {"test_data/pucch_detector_test_ch_estimates52.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 17, {}, 1, 13, 0, 1, 3, 9, 328, 0, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols53.dat"}, {"test_data/pucch_detector_test_ch_estimates53.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 22, {}, 1, 13, 0, 1, 4, 6, 321, 1, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols54.dat"}, {"test_data/pucch_detector_test_ch_estimates54.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 38, {11}, 1, 13, 0, 1, 0, 3, 821, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols55.dat"}, {"test_data/pucch_detector_test_ch_estimates55.dat"}},
  {{{1, 8}, cyclic_prefix::NORMAL, 12, {18}, 1, 13, 0, 1, 1, 9, 774, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols56.dat"}, {"test_data/pucch_detector_test_ch_estimates56.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 15, {23}, 1, 13, 0, 1, 1, 6, 73, 1, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols57.dat"}, {"test_data/pucch_detector_test_ch_estimates57.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 9, {47}, 1, 13, 0, 1, 1, 6, 353, 0, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols58.dat"}, {"test_data/pucch_detector_test_ch_estimates58.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 41, {50}, 1, 13, 0, 1, 0, 9, 872, 1, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols59.dat"}, {"test_data/pucch_detector_test_ch_estimates59.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 47, {}, 5, 5, 0, 1, 0, 9, 296, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols60.dat"}, {"test_data/pucch_detector_test_ch_estimates60.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 45, {}, 5, 5, 0, 1, 0, 3, 139, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols61.dat"}, {"test_data/pucch_detector_test_ch_estimates61.dat"}},
  {{{1, 16}, cyclic_prefix::NORMAL, 20, {}, 5, 5, 0, 1, 0, 6, 752, 1, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols62.dat"}, {"test_data/pucch_detector_test_ch_estimates62.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 40, {}, 5, 5, 0, 1, 1, 9, 816, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols63.dat"}, {"test_data/pucch_detector_test_ch_estimates63.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 12, {}, 5, 5, 0, 1, 1, 9, 70, 1, 2}, 0.01, {1}, {1, 0}, {"test_data/pucch_detector_test_received_symbols64.dat"}, {"test_data/pucch_detector_test_ch_estimates64.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 34, {11}, 5, 5, 0, 1, 0, 6, 92, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols65.dat"}, {"test_data/pucch_detector_test_ch_estimates65.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 13, {20}, 5, 5, 0, 1, 0, 6, 916, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols66.dat"}, {"test_data/pucch_detector_test_ch_estimates66.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 29, {20}, 5, 5, 0, 1, 0, 9, 28, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols67.dat"}, {"test_data/pucch_detector_test_ch_estimates67.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 21, {25}, 5, 5, 0, 1, 0, 3, 189, 0, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols68.dat"}, {"test_data/pucch_detector_test_ch_estimates68.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 45, {16}, 5, 5, 0, 1, 0, 0, 686, 1, 2}, 0.01, {0}, {1, 1}, {"test_data/pucch_detector_test_received_symbols69.dat"}, {"test_data/pucch_detector_test_ch_estimates69.dat"}},
  {{{1, 8}, cyclic_prefix::NORMAL, 7, {}, 10, 4, 0, 1, 1, 9, 544, 1, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols70.dat"}, {"test_data/pucch_detector_test_ch_estimates70.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 46, {}, 10, 4, 0, 1, 0, 6, 413, 0, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols71.dat"}, {"test_data/pucch_detector_test_ch_estimates71.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 35, {}, 10, 4, 0, 1, 1, 3, 206, 1, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols72.dat"}, {"test_data/pucch_detector_test_ch_estimates72.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 31, {}, 10, 4, 0, 1, 0, 3, 271, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols73.dat"}, {"test_data/pucch_detector_test_ch_estimates73.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 40, {}, 10, 4, 0, 1, 0, 9, 154, 1, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols74.dat"}, {"test_data/pucch_detector_test_ch_estimates74.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 21, {32}, 10, 4, 0, 1, 0, 9, 997, 1, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols75.dat"}, {"test_data/pucch_detector_test_ch_estimates75.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 43, {23}, 10, 4, 0, 1, 0, 0, 222, 0, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols76.dat"}, {"test_data/pucch_detector_test_ch_estimates76.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 5, {30}, 10, 4, 0, 1, 0, 3, 357, 1, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols77.dat"}, {"test_data/pucch_detector_test_ch_estimates77.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 27, {13}, 10, 4, 0, 1, 0, 9, 109, 0, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols78.dat"}, {"test_data/pucch_detector_test_ch_estimates78.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 29, {5}, 10, 4, 0, 1, 0, 3, 461, 1, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols79.dat"}, {"test_data/pucch_detector_test_ch_estimates79.dat"}},
    // clang-format on
};

} // namespace srsgnb
