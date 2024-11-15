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

// This file was generated using the following MATLAB class on 13-11-2024 (seed 0):
//   + "srsPUCCHdmrsUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_estimator.h"
#include "srsran/support/file_vector.h"
#include <variant>

namespace srsran {

using estimator_config = std::variant<dmrs_pucch_estimator::format1_configuration,
                                      dmrs_pucch_estimator::format2_configuration,
                                      dmrs_pucch_estimator::format3_configuration,
                                      dmrs_pucch_estimator::format4_configuration>;

struct test_case_t {
  estimator_config                                        config;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pucch_estimator_test_data = {
    // clang-format off
  {dmrs_pucch_estimator::format1_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 8, 37, {}, 608, {3}}, 2, 2}, {"test_data/dmrs_pucch_estimator_test_output0.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 13, {}, 451, {1}}, 11, 1}, {"test_data/dmrs_pucch_estimator_test_output1.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 4, 9, {18}, 122, {0}}, 7, 0}, {"test_data/dmrs_pucch_estimator_test_output2.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 1, {39}, 447, {3}}, 7, 2}, {"test_data/dmrs_pucch_estimator_test_output3.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 1, 29, {}, 871, {3}}, 9, 871}, {"test_data/dmrs_pucch_estimator_test_output4.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 2, 51, {}, 771, {1}}, 1, 771}, {"test_data/dmrs_pucch_estimator_test_output5.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 48, {27}, 98, {7}}, 1, 98}, {"test_data/dmrs_pucch_estimator_test_output6.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 2, 7, {12}, 745, {7}}, 4, 745}, {"test_data/dmrs_pucch_estimator_test_output7.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 21, {}, 173, {7}}, 16, true}, {"test_data/dmrs_pucch_estimator_test_output8.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 7, 34, {}, 332, {6}}, 3, true}, {"test_data/dmrs_pucch_estimator_test_output9.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 10, 30, {20}, 634, {4}}, 8, false}, {"test_data/dmrs_pucch_estimator_test_output10.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 9, 18, {0}, 555, {2}}, 15, false}, {"test_data/dmrs_pucch_estimator_test_output11.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 4, 36, {}, 970, {5}}, false, 3}, {"test_data/dmrs_pucch_estimator_test_output12.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 6, 11, {}, 395, {5}}, false, 1}, {"test_data/dmrs_pucch_estimator_test_output13.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 1}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 5, 48, {45}, 931, {6}}, false, 0}, {"test_data/dmrs_pucch_estimator_test_output14.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 7, 40, {47}, 698, {0}}, true, 0}, {"test_data/dmrs_pucch_estimator_test_output15.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{1, 12}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 9, 2, {}, 293, {0}}, 4, 3}, {"test_data/dmrs_pucch_estimator_test_output16.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 8, 7, {}, 243, {3}}, 10, 3}, {"test_data/dmrs_pucch_estimator_test_output17.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 5, 48, {32}, 280, {4}}, 4, 0}, {"test_data/dmrs_pucch_estimator_test_output18.dat"}},
  {dmrs_pucch_estimator::format1_configuration{{{1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 7, 43, {50}, 691, {3}}, 8, 0}, {"test_data/dmrs_pucch_estimator_test_output19.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 10}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 1, 18, {}, 31, {2}}, 13, 31}, {"test_data/dmrs_pucch_estimator_test_output20.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 13}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 2, 43, {}, 642, {7}}, 9, 642}, {"test_data/dmrs_pucch_estimator_test_output21.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 2, 11, {34}, 360, {5}}, 12, 360}, {"test_data/dmrs_pucch_estimator_test_output22.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 2, 12, {35}, 407, {2}}, 12, 407}, {"test_data/dmrs_pucch_estimator_test_output23.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 30, {}, 39, {6}}, 5, false}, {"test_data/dmrs_pucch_estimator_test_output24.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 17}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 50, {}, 21, {2}}, 1, false}, {"test_data/dmrs_pucch_estimator_test_output25.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 13}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 18, {42}, 459, {4}}, 9, false}, {"test_data/dmrs_pucch_estimator_test_output26.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 13}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 6, 6, {21}, 959, {1}}, 4, false}, {"test_data/dmrs_pucch_estimator_test_output27.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 8, 48, {}, 859, {7}}, true, 1}, {"test_data/dmrs_pucch_estimator_test_output28.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 10}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 7, 2, {}, 260, {5}}, true, 1}, {"test_data/dmrs_pucch_estimator_test_output29.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 17}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 47, {42}, 302, {4}}, true, 3}, {"test_data/dmrs_pucch_estimator_test_output30.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 13, {25}, 652, {3}}, false, 1}, {"test_data/dmrs_pucch_estimator_test_output31.dat"}},
    // clang-format on
};

} // namespace srsran
