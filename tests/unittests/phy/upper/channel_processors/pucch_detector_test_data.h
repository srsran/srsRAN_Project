/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsPUCCHDetectorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch_detector.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/support/file_vector.h"

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
  {{{0, 9}, cyclic_prefix::NORMAL, 6, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 6, 6, 821, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols0.dat"}, {"test_data/pucch_detector_test_ch_estimates0.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 48, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 426, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols1.dat"}, {"test_data/pucch_detector_test_ch_estimates1.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 16, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 3, 6, 303, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols2.dat"}, {"test_data/pucch_detector_test_ch_estimates2.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 1, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 3, 0, 810, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols3.dat"}, {"test_data/pucch_detector_test_ch_estimates3.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 23, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 536, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols4.dat"}, {"test_data/pucch_detector_test_ch_estimates4.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 9, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 3, 6, 595, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols5.dat"}, {"test_data/pucch_detector_test_ch_estimates5.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 1, {31}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 658, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols6.dat"}, {"test_data/pucch_detector_test_ch_estimates6.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 9, {43}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 708, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols7.dat"}, {"test_data/pucch_detector_test_ch_estimates7.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 34, {42}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 118, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols8.dat"}, {"test_data/pucch_detector_test_ch_estimates8.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 9, {13}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 858, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols9.dat"}, {"test_data/pucch_detector_test_ch_estimates9.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 0, {41}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 933, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols10.dat"}, {"test_data/pucch_detector_test_ch_estimates10.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 32, {37}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 6, 637, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols11.dat"}, {"test_data/pucch_detector_test_ch_estimates11.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 46, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 5, 6, 286, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols12.dat"}, {"test_data/pucch_detector_test_ch_estimates12.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 33, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 5, 9, 581, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols13.dat"}, {"test_data/pucch_detector_test_ch_estimates13.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 6, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 902, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols14.dat"}, {"test_data/pucch_detector_test_ch_estimates14.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 41, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 570, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols15.dat"}, {"test_data/pucch_detector_test_ch_estimates15.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 38, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 903, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols16.dat"}, {"test_data/pucch_detector_test_ch_estimates16.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 19, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 9, 993, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols17.dat"}, {"test_data/pucch_detector_test_ch_estimates17.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 27, {40}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 530, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols18.dat"}, {"test_data/pucch_detector_test_ch_estimates18.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 28, {10}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 76, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols19.dat"}, {"test_data/pucch_detector_test_ch_estimates19.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 42, {32}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 428, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols20.dat"}, {"test_data/pucch_detector_test_ch_estimates20.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 8, {37}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 294, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols21.dat"}, {"test_data/pucch_detector_test_ch_estimates21.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 39, {29}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 614, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols22.dat"}, {"test_data/pucch_detector_test_ch_estimates22.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 33, {43}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 177, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols23.dat"}, {"test_data/pucch_detector_test_ch_estimates23.dat"}},
  {{{0, 9}, cyclic_prefix::NORMAL, 21, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 836, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols24.dat"}, {"test_data/pucch_detector_test_ch_estimates24.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 32, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 631, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols25.dat"}, {"test_data/pucch_detector_test_ch_estimates25.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 40, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 812, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols26.dat"}, {"test_data/pucch_detector_test_ch_estimates26.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 38, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 744, 1}, 0.01, {0}, {1}, {"test_data/pucch_detector_test_received_symbols27.dat"}, {"test_data/pucch_detector_test_ch_estimates27.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 40, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 804, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols28.dat"}, {"test_data/pucch_detector_test_ch_estimates28.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 29, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 883, 2}, 0.01, {1}, {1, 0}, {"test_data/pucch_detector_test_received_symbols29.dat"}, {"test_data/pucch_detector_test_ch_estimates29.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 1, {43}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 78, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols30.dat"}, {"test_data/pucch_detector_test_ch_estimates30.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 42, {8}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 784, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols31.dat"}, {"test_data/pucch_detector_test_ch_estimates31.dat"}},
  {{{0, 3}, cyclic_prefix::NORMAL, 40, {29}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 320, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols32.dat"}, {"test_data/pucch_detector_test_ch_estimates32.dat"}},
  {{{0, 0}, cyclic_prefix::NORMAL, 48, {13}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 192, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols33.dat"}, {"test_data/pucch_detector_test_ch_estimates33.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 9, {48}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 293, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols34.dat"}, {"test_data/pucch_detector_test_ch_estimates34.dat"}},
  {{{0, 1}, cyclic_prefix::NORMAL, 17, {28}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 429, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols35.dat"}, {"test_data/pucch_detector_test_ch_estimates35.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 31, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 190, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols36.dat"}, {"test_data/pucch_detector_test_ch_estimates36.dat"}},
  {{{0, 8}, cyclic_prefix::NORMAL, 12, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 178, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols37.dat"}, {"test_data/pucch_detector_test_ch_estimates37.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 19, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 670, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols38.dat"}, {"test_data/pucch_detector_test_ch_estimates38.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 40, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 241, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols39.dat"}, {"test_data/pucch_detector_test_ch_estimates39.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 22, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 45, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols40.dat"}, {"test_data/pucch_detector_test_ch_estimates40.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 16, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 168, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols41.dat"}, {"test_data/pucch_detector_test_ch_estimates41.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 32, {30}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 554, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols42.dat"}, {"test_data/pucch_detector_test_ch_estimates42.dat"}},
  {{{0, 5}, cyclic_prefix::NORMAL, 44, {41}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 243, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols43.dat"}, {"test_data/pucch_detector_test_ch_estimates43.dat"}},
  {{{0, 7}, cyclic_prefix::NORMAL, 17, {44}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 374, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols44.dat"}, {"test_data/pucch_detector_test_ch_estimates44.dat"}},
  {{{0, 6}, cyclic_prefix::NORMAL, 48, {39}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 286, 1}, 0.01, {1}, {1}, {"test_data/pucch_detector_test_received_symbols45.dat"}, {"test_data/pucch_detector_test_ch_estimates45.dat"}},
  {{{0, 4}, cyclic_prefix::NORMAL, 24, {9}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 495, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols46.dat"}, {"test_data/pucch_detector_test_ch_estimates46.dat"}},
  {{{0, 2}, cyclic_prefix::NORMAL, 27, {28}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 251, 2}, 0.01, {0}, {0, 1}, {"test_data/pucch_detector_test_received_symbols47.dat"}, {"test_data/pucch_detector_test_ch_estimates47.dat"}},
  {{{1, 0}, cyclic_prefix::NORMAL, 13, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 6, 3, 767, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols48.dat"}, {"test_data/pucch_detector_test_ch_estimates48.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 30, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 798, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols49.dat"}, {"test_data/pucch_detector_test_ch_estimates49.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 15, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 657, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols50.dat"}, {"test_data/pucch_detector_test_ch_estimates50.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 22, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 720, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols51.dat"}, {"test_data/pucch_detector_test_ch_estimates51.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 10, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 12, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols52.dat"}, {"test_data/pucch_detector_test_ch_estimates52.dat"}},
  {{{1, 4}, cyclic_prefix::NORMAL, 3, {}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 4, 6, 872, 2}, 0.01, {1}, {1, 0}, {"test_data/pucch_detector_test_received_symbols53.dat"}, {"test_data/pucch_detector_test_ch_estimates53.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 43, {18}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 489, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols54.dat"}, {"test_data/pucch_detector_test_ch_estimates54.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 38, {17}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 6, 773, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols55.dat"}, {"test_data/pucch_detector_test_ch_estimates55.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 0, {28}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 3, 328, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols56.dat"}, {"test_data/pucch_detector_test_ch_estimates56.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 29, {10}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 438, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols57.dat"}, {"test_data/pucch_detector_test_ch_estimates57.dat"}},
  {{{1, 14}, cyclic_prefix::NORMAL, 26, {25}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 386, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols58.dat"}, {"test_data/pucch_detector_test_ch_estimates58.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 15, {41}, 0, 14, pucch_group_hopping::NEITHER, 0, 1, 2, 6, 752, 2}, 0.01, {1}, {1, 1}, {"test_data/pucch_detector_test_received_symbols59.dat"}, {"test_data/pucch_detector_test_ch_estimates59.dat"}},
  {{{1, 10}, cyclic_prefix::NORMAL, 42, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 3, 0, 43, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols60.dat"}, {"test_data/pucch_detector_test_ch_estimates60.dat"}},
  {{{1, 3}, cyclic_prefix::NORMAL, 47, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 3, 0, 38, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols61.dat"}, {"test_data/pucch_detector_test_ch_estimates61.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 25, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 0, 190, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols62.dat"}, {"test_data/pucch_detector_test_ch_estimates62.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 2, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 752, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols63.dat"}, {"test_data/pucch_detector_test_ch_estimates63.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 49, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 1003, 2}, 0.01, {}, {0, 1}, {"test_data/pucch_detector_test_received_symbols64.dat"}, {"test_data/pucch_detector_test_ch_estimates64.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 30, {}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 4, 6, 858, 2}, 0.01, {1}, {0, 1}, {"test_data/pucch_detector_test_received_symbols65.dat"}, {"test_data/pucch_detector_test_ch_estimates65.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 40, {40}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 874, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols66.dat"}, {"test_data/pucch_detector_test_ch_estimates66.dat"}},
  {{{1, 16}, cyclic_prefix::NORMAL, 10, {22}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 0, 648, 0}, 0.01, {0}, {}, {"test_data/pucch_detector_test_received_symbols67.dat"}, {"test_data/pucch_detector_test_ch_estimates67.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 48, {1}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 403, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols68.dat"}, {"test_data/pucch_detector_test_ch_estimates68.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 39, {38}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 3, 554, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols69.dat"}, {"test_data/pucch_detector_test_ch_estimates69.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 45, {7}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 2, 9, 807, 2}, 0.01, {}, {0, 0}, {"test_data/pucch_detector_test_received_symbols70.dat"}, {"test_data/pucch_detector_test_ch_estimates70.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 12, {37}, 1, 13, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 139, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols71.dat"}, {"test_data/pucch_detector_test_ch_estimates71.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 48, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 404, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols72.dat"}, {"test_data/pucch_detector_test_ch_estimates72.dat"}},
  {{{1, 4}, cyclic_prefix::NORMAL, 21, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 287, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols73.dat"}, {"test_data/pucch_detector_test_ch_estimates73.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 19, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 919, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols74.dat"}, {"test_data/pucch_detector_test_ch_estimates74.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 37, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 66, 1}, 0.01, {1}, {0}, {"test_data/pucch_detector_test_received_symbols75.dat"}, {"test_data/pucch_detector_test_ch_estimates75.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 13, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 77, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols76.dat"}, {"test_data/pucch_detector_test_ch_estimates76.dat"}},
  {{{1, 5}, cyclic_prefix::NORMAL, 43, {}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 54, 2}, 0.01, {1}, {0, 0}, {"test_data/pucch_detector_test_received_symbols77.dat"}, {"test_data/pucch_detector_test_ch_estimates77.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 6, {46}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 119, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols78.dat"}, {"test_data/pucch_detector_test_ch_estimates78.dat"}},
  {{{1, 19}, cyclic_prefix::NORMAL, 26, {42}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 228, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols79.dat"}, {"test_data/pucch_detector_test_ch_estimates79.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 28, {40}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 756, 1}, 0.01, {}, {0}, {"test_data/pucch_detector_test_received_symbols80.dat"}, {"test_data/pucch_detector_test_ch_estimates80.dat"}},
  {{{1, 15}, cyclic_prefix::NORMAL, 39, {43}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 163, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols81.dat"}, {"test_data/pucch_detector_test_ch_estimates81.dat"}},
  {{{1, 4}, cyclic_prefix::NORMAL, 29, {35}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 535, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols82.dat"}, {"test_data/pucch_detector_test_ch_estimates82.dat"}},
  {{{1, 17}, cyclic_prefix::NORMAL, 7, {5}, 5, 5, pucch_group_hopping::NEITHER, 0, 1, 0, 0, 451, 2}, 0.01, {0}, {1, 1}, {"test_data/pucch_detector_test_received_symbols83.dat"}, {"test_data/pucch_detector_test_ch_estimates83.dat"}},
  {{{1, 11}, cyclic_prefix::NORMAL, 49, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 889, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols84.dat"}, {"test_data/pucch_detector_test_ch_estimates84.dat"}},
  {{{1, 2}, cyclic_prefix::NORMAL, 35, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 3, 467, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols85.dat"}, {"test_data/pucch_detector_test_ch_estimates85.dat"}},
  {{{1, 7}, cyclic_prefix::NORMAL, 10, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 9, 759, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols86.dat"}, {"test_data/pucch_detector_test_ch_estimates86.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 26, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 448, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols87.dat"}, {"test_data/pucch_detector_test_ch_estimates87.dat"}},
  {{{1, 12}, cyclic_prefix::NORMAL, 42, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 1, 0, 862, 2}, 0.01, {}, {1, 0}, {"test_data/pucch_detector_test_received_symbols88.dat"}, {"test_data/pucch_detector_test_ch_estimates88.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 37, {}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 726, 2}, 0.01, {0}, {0, 0}, {"test_data/pucch_detector_test_received_symbols89.dat"}, {"test_data/pucch_detector_test_ch_estimates89.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 10, {32}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 267, 0}, 0.01, {}, {}, {"test_data/pucch_detector_test_received_symbols90.dat"}, {"test_data/pucch_detector_test_ch_estimates90.dat"}},
  {{{1, 9}, cyclic_prefix::NORMAL, 42, {41}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 6, 603, 0}, 0.01, {1}, {}, {"test_data/pucch_detector_test_received_symbols91.dat"}, {"test_data/pucch_detector_test_ch_estimates91.dat"}},
  {{{1, 1}, cyclic_prefix::NORMAL, 13, {5}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 341, 1}, 0.01, {}, {1}, {"test_data/pucch_detector_test_received_symbols92.dat"}, {"test_data/pucch_detector_test_ch_estimates92.dat"}},
  {{{1, 6}, cyclic_prefix::NORMAL, 37, {18}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 563, 1}, 0.01, {0}, {0}, {"test_data/pucch_detector_test_received_symbols93.dat"}, {"test_data/pucch_detector_test_ch_estimates93.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 17, {2}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 9, 501, 2}, 0.01, {}, {1, 1}, {"test_data/pucch_detector_test_received_symbols94.dat"}, {"test_data/pucch_detector_test_ch_estimates94.dat"}},
  {{{1, 18}, cyclic_prefix::NORMAL, 43, {21}, 10, 4, pucch_group_hopping::NEITHER, 0, 1, 0, 3, 762, 2}, 0.01, {0}, {1, 0}, {"test_data/pucch_detector_test_received_symbols95.dat"}, {"test_data/pucch_detector_test_ch_estimates95.dat"}},
    // clang-format on
};

} // namespace srsran
