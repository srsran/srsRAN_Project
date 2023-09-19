/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
//   + "srsPUCCHdmrsUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  dmrs_pucch_processor::config_t                          config;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pucch_processor_test_data = {
    // clang-format off
  {{pucch_format::FORMAT_1, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 10, 47, false, 0, 1, 1, 1, false, 72, 72, {7}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
  {{pucch_format::FORMAT_1, {0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 8, false, 0, 1, 11, 6, false, 693, 693, {7}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
  {{pucch_format::FORMAT_1, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 21, true, 49, 1, 1, 2, false, 374, 374, {6}}, {"test_data/dmrs_pucch_processor_test_output2.dat"}},
  {{pucch_format::FORMAT_1, {0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 11, 48, true, 20, 1, 10, 1, false, 822, 822, {0}}, {"test_data/dmrs_pucch_processor_test_output3.dat"}},
  {{pucch_format::FORMAT_2, {0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 1, 1, false, 0, 5, 8, 0, false, 664, 664, {1}}, {"test_data/dmrs_pucch_processor_test_output4.dat"}},
  {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 1, 49, false, 0, 1, 3, 0, false, 672, 672, {5}}, {"test_data/dmrs_pucch_processor_test_output5.dat"}},
  {{pucch_format::FORMAT_2, {0, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 2, 25, true, 37, 8, 2, 0, false, 947, 947, {6}}, {"test_data/dmrs_pucch_processor_test_output6.dat"}},
  {{pucch_format::FORMAT_2, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 11, 2, 8, true, 19, 2, 7, 0, false, 192, 192, {5}}, {"test_data/dmrs_pucch_processor_test_output7.dat"}},
  {{pucch_format::FORMAT_1, {1, 11}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 9, 13, false, 0, 1, 9, 3, false, 694, 694, {1}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
  {{pucch_format::FORMAT_1, {1, 19}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 6, 7, false, 0, 1, 1, 0, false, 75, 75, {4}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
  {{pucch_format::FORMAT_1, {1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 6, 18, true, 32, 1, 11, 0, false, 732, 732, {1}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
  {{pucch_format::FORMAT_1, {1, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 10, 30, true, 39, 1, 9, 0, false, 326, 326, {2}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
  {{pucch_format::FORMAT_2, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 3, false, 0, 1, 6, 0, false, 601, 601, {3}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
  {{pucch_format::FORMAT_2, {1, 18}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 1, 24, false, 0, 1, 6, 0, false, 169, 169, {1}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
  {{pucch_format::FORMAT_2, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 2, 8, true, 34, 10, 6, 0, false, 561, 561, {2}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
  {{pucch_format::FORMAT_2, {1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 11, true, 31, 15, 1, 0, false, 988, 988, {3}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
    // clang-format on
};

} // namespace srsran
