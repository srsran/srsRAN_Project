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

// This file was generated using the following MATLAB class on 15-12-2022:
//   + "srsPUCCHdmrsUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  dmrs_pucch_processor::config_t                          config;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pucch_processor_test_data = {
    // clang-format off
  {{pucch_format::FORMAT_1, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 9, 39, false, 0, 1, 10, 3, false, 533, 533, {4}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
  {{pucch_format::FORMAT_1, {0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 11, false, 0, 1, 3, 0, false, 724, 724, {7}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
  {{pucch_format::FORMAT_1, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 22, true, 23, 1, 9, 1, false, 196, 196, {4}}, {"test_data/dmrs_pucch_processor_test_output2.dat"}},
  {{pucch_format::FORMAT_1, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 50, true, 3, 1, 2, 2, false, 843, 843, {1}}, {"test_data/dmrs_pucch_processor_test_output3.dat"}},
  {{pucch_format::FORMAT_2, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 11, 2, 6, false, 0, 5, 0, 0, false, 220, 220, {4}}, {"test_data/dmrs_pucch_processor_test_output4.dat"}},
  {{pucch_format::FORMAT_2, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 10, 2, 17, false, 0, 6, 11, 0, false, 223, 223, {1}}, {"test_data/dmrs_pucch_processor_test_output5.dat"}},
  {{pucch_format::FORMAT_2, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 2, 4, true, 17, 3, 7, 0, false, 257, 257, {7}}, {"test_data/dmrs_pucch_processor_test_output6.dat"}},
  {{pucch_format::FORMAT_2, {0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 2, 40, true, 27, 4, 3, 0, false, 120, 120, {2}}, {"test_data/dmrs_pucch_processor_test_output7.dat"}},
  {{pucch_format::FORMAT_1, {1, 1}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 6, 46, false, 0, 1, 2, 1, false, 480, 480, {6}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
  {{pucch_format::FORMAT_1, {1, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 7, 43, false, 0, 1, 8, 1, false, 687, 687, {1}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
  {{pucch_format::FORMAT_1, {1, 11}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 11, 7, true, 36, 1, 8, 1, false, 650, 650, {1}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
  {{pucch_format::FORMAT_1, {1, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 27, true, 38, 1, 1, 2, false, 953, 953, {1}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
  {{pucch_format::FORMAT_2, {1, 16}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 2, 31, false, 0, 5, 2, 0, false, 584, 584, {2}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
  {{pucch_format::FORMAT_2, {1, 18}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 1, 19, false, 0, 12, 1, 0, false, 592, 592, {0}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
  {{pucch_format::FORMAT_2, {1, 17}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 11, 2, 36, true, 46, 3, 8, 0, false, 944, 944, {4}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
  {{pucch_format::FORMAT_2, {1, 11}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 2, 32, true, 40, 3, 7, 0, false, 95, 95, {3}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
    // clang-format on
};

} // namespace srsgnb
