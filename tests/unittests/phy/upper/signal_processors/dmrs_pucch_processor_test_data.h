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

// This file was generated using the following MATLAB class on 13-12-2023 (seed 0):
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
  {{pucch_format::FORMAT_1, {0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 8, 37, false, 0, 1, 2, 2, false, 608, 608, {3}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
  {{pucch_format::FORMAT_1, {0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 13, false, 0, 1, 11, 1, false, 451, 451, {1}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
  {{pucch_format::FORMAT_1, {0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 4, 9, true, 18, 1, 7, 0, false, 122, 122, {0}}, {"test_data/dmrs_pucch_processor_test_output2.dat"}},
  {{pucch_format::FORMAT_1, {0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 1, true, 39, 1, 7, 2, false, 447, 447, {3}}, {"test_data/dmrs_pucch_processor_test_output3.dat"}},
  {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 1, 29, false, 0, 9, 7, 0, false, 871, 871, {3}}, {"test_data/dmrs_pucch_processor_test_output4.dat"}},
  {{pucch_format::FORMAT_2, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 2, 51, false, 0, 1, 9, 0, false, 771, 771, {1}}, {"test_data/dmrs_pucch_processor_test_output5.dat"}},
  {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 48, true, 27, 1, 0, 0, false, 98, 98, {7}}, {"test_data/dmrs_pucch_processor_test_output6.dat"}},
  {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 2, 7, true, 12, 4, 7, 0, false, 745, 745, {7}}, {"test_data/dmrs_pucch_processor_test_output7.dat"}},
  {{pucch_format::FORMAT_1, {1, 12}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 21, false, 0, 1, 4, 6, false, 173, 173, {7}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
  {{pucch_format::FORMAT_1, {1, 13}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 11, 17, false, 0, 1, 9, 1, false, 332, 332, {7}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
  {{pucch_format::FORMAT_1, {1, 13}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 31, true, 29, 1, 4, 1, false, 634, 634, {4}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
  {{pucch_format::FORMAT_1, {1, 10}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 7, 51, true, 17, 1, 8, 0, false, 555, 555, {0}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
  {{pucch_format::FORMAT_2, {1, 17}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 1, 11, false, 0, 3, 4, 0, false, 970, 970, {3}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
  {{pucch_format::FORMAT_2, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 2, 43, false, 0, 9, 0, 0, false, 395, 395, {5}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
  {{pucch_format::FORMAT_2, {1, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 2, 11, true, 33, 5, 8, 0, false, 931, 931, {5}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
  {{pucch_format::FORMAT_2, {1, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 2, 39, true, 2, 13, 9, 0, false, 698, 698, {1}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
    // clang-format on
};

} // namespace srsran
