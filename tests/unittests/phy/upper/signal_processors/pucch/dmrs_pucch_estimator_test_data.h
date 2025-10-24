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

#pragma once

// This file was generated using the following MATLAB class on 03-07-2025 (seed 0):
//   + "srsPUCCHdmrsUnittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"
#include "srsran/support/file_vector.h"
#include <variant>

namespace srsran {

using estimator_config = std::variant<dmrs_pucch_estimator::format2_configuration,
                                      dmrs_pucch_estimator::format3_configuration,
                                      dmrs_pucch_estimator::format4_configuration>;

struct test_case_t {
  estimator_config                                        config;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pucch_estimator_test_data = {
    // clang-format off
  {dmrs_pucch_estimator::format2_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 2, 37, {}, {3}}, 6, 608}, {"test_data/dmrs_pucch_estimator_test_output0.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 1, 13, {}, {1}}, 15, 451}, {"test_data/dmrs_pucch_estimator_test_output1.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 2, 2, {23}, {5}}, 12, 122}, {"test_data/dmrs_pucch_estimator_test_output2.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 2, 41, {18}, {0}}, 11, 447}, {"test_data/dmrs_pucch_estimator_test_output3.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 6, 12, {}, {4}}, 871, 6, true}, {"test_data/dmrs_pucch_estimator_test_output4.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 4, 41, {}, {4}}, 771, 11, false}, {"test_data/dmrs_pucch_estimator_test_output5.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 10, 46, {7}, {6}}, 98, 6, false}, {"test_data/dmrs_pucch_estimator_test_output6.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 8, 48, {29}, {0}}, 745, 4, true}, {"test_data/dmrs_pucch_estimator_test_output7.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 6, 34, {}, {6}}, 173, false, 2}, {"test_data/dmrs_pucch_estimator_test_output8.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 10, 28, {}, {6}}, 332, true, 1}, {"test_data/dmrs_pucch_estimator_test_output9.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 9, 46, {37}, {7}}, 634, false, 0}, {"test_data/dmrs_pucch_estimator_test_output10.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{0, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 43, {35}, {5}}, 555, false, 0}, {"test_data/dmrs_pucch_estimator_test_output11.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 2, 34, {}, {1}}, 8, 970}, {"test_data/dmrs_pucch_estimator_test_output12.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 1, 48, {}, {6}}, 2, 395}, {"test_data/dmrs_pucch_estimator_test_output13.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 5, {16}, {1}}, 1, 931}, {"test_data/dmrs_pucch_estimator_test_output14.dat"}},
  {dmrs_pucch_estimator::format2_configuration{{{1, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 2, 26, {4}, {2}}, 15, 698}, {"test_data/dmrs_pucch_estimator_test_output15.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 1}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 9, 51, {}, {4}}, 293, 1, false}, {"test_data/dmrs_pucch_estimator_test_output16.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 7, 37, {}, {7}}, 243, 9, false}, {"test_data/dmrs_pucch_estimator_test_output17.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 20, {32}, {5}}, 280, 10, false}, {"test_data/dmrs_pucch_estimator_test_output18.dat"}},
  {dmrs_pucch_estimator::format3_configuration{{{1, 19}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 32, {25}, {4}}, 691, 5, false}, {"test_data/dmrs_pucch_estimator_test_output19.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 19}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 38, {}, {2}}, 31, true, 0}, {"test_data/dmrs_pucch_estimator_test_output20.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 10, 32, {}, {1}}, 642, false, 0}, {"test_data/dmrs_pucch_estimator_test_output21.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 14, {19}, {5}}, 360, true, 0}, {"test_data/dmrs_pucch_estimator_test_output22.dat"}},
  {dmrs_pucch_estimator::format4_configuration{{{1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 7, 45, {9}, {4}}, 407, true, 0}, {"test_data/dmrs_pucch_estimator_test_output23.dat"}},
    // clang-format on
};

} // namespace srsran
