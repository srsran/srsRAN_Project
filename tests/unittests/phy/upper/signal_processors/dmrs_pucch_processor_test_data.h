/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 07-11-2024 (seed 0):
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
  {{pucch_format::FORMAT_3, {0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 21, false, 0, 16, 4, 0, true, 173, 173, {7}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
  {{pucch_format::FORMAT_3, {0, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 11, 12, false, 0, 4, 7, 0, true, 332, 332, {2}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
  {{pucch_format::FORMAT_3, {0, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 4, 30, true, 43, 8, 6, 0, false, 634, 634, {4}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
  {{pucch_format::FORMAT_3, {0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 5, 21, true, 35, 3, 5, 0, true, 555, 555, {7}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
  {{pucch_format::FORMAT_4, {0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 4, 40, false, 0, 1, 11, 0, true, 970, 970, {6}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
  {{pucch_format::FORMAT_4, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 9, false, 0, 1, 2, 0, false, 395, 395, {4}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
  {{pucch_format::FORMAT_4, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 4, 5, true, 27, 1, 10, 0, false, 931, 931, {1}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
  {{pucch_format::FORMAT_4, {0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 4, 20, true, 7, 1, 1, 1, true, 698, 698, {5}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
  {{pucch_format::FORMAT_1, {1, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 10, 37, false, 0, 1, 11, 4, false, 293, 293, {7}}, {"test_data/dmrs_pucch_processor_test_output16.dat"}},
  {{pucch_format::FORMAT_1, {1, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 8, 43, false, 0, 1, 7, 3, false, 243, 243, {5}}, {"test_data/dmrs_pucch_processor_test_output17.dat"}},
  {{pucch_format::FORMAT_1, {1, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 7, 28, true, 18, 1, 11, 0, false, 280, 280, {4}}, {"test_data/dmrs_pucch_processor_test_output18.dat"}},
  {{pucch_format::FORMAT_1, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 36, true, 43, 1, 5, 0, false, 691, 691, {3}}, {"test_data/dmrs_pucch_processor_test_output19.dat"}},
  {{pucch_format::FORMAT_2, {1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 1, 18, false, 0, 11, 0, 0, false, 31, 31, {7}}, {"test_data/dmrs_pucch_processor_test_output20.dat"}},
  {{pucch_format::FORMAT_2, {1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 1, 34, false, 0, 1, 7, 0, false, 642, 642, {4}}, {"test_data/dmrs_pucch_processor_test_output21.dat"}},
  {{pucch_format::FORMAT_2, {1, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 10, 2, 14, true, 26, 12, 10, 0, false, 360, 360, {5}}, {"test_data/dmrs_pucch_processor_test_output22.dat"}},
  {{pucch_format::FORMAT_2, {1, 11}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 2, 26, true, 12, 8, 9, 0, false, 407, 407, {2}}, {"test_data/dmrs_pucch_processor_test_output23.dat"}},
  {{pucch_format::FORMAT_3, {1, 19}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 10, 9, false, 0, 10, 11, 0, true, 39, 39, {0}}, {"test_data/dmrs_pucch_processor_test_output24.dat"}},
  {{pucch_format::FORMAT_3, {1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 6, 50, false, 0, 2, 1, 0, true, 21, 21, {0}}, {"test_data/dmrs_pucch_processor_test_output25.dat"}},
  {{pucch_format::FORMAT_3, {1, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 18, true, 22, 2, 1, 0, true, 459, 459, {2}}, {"test_data/dmrs_pucch_processor_test_output26.dat"}},
  {{pucch_format::FORMAT_3, {1, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 11, 41, true, 1, 8, 8, 0, true, 959, 959, {2}}, {"test_data/dmrs_pucch_processor_test_output27.dat"}},
  {{pucch_format::FORMAT_4, {1, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 10, 45, false, 0, 1, 8, 1, true, 859, 859, {5}}, {"test_data/dmrs_pucch_processor_test_output28.dat"}},
  {{pucch_format::FORMAT_4, {1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 10, 24, false, 0, 1, 5, 2, false, 260, 260, {6}}, {"test_data/dmrs_pucch_processor_test_output29.dat"}},
  {{pucch_format::FORMAT_4, {1, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 11, 40, true, 14, 1, 9, 0, true, 302, 302, {2}}, {"test_data/dmrs_pucch_processor_test_output30.dat"}},
  {{pucch_format::FORMAT_4, {1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 45, true, 32, 1, 7, 1, true, 652, 652, {7}}, {"test_data/dmrs_pucch_processor_test_output31.dat"}},
    // clang-format on
};

} // namespace srsran
