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

// This file was generated using the following MATLAB class on 14-Dec-2022:
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
  {{pucch_format::FORMAT_1, {0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 7, 19, false, 0, 1, 5, 2, false, 987, 987, {1}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
  {{pucch_format::FORMAT_1, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 6, 7, 2, false, 0, 1, 6, 2, false, 399, 399, {5}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
  {{pucch_format::FORMAT_1, {0, 3}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 14, 43, true, 46, 1, 6, 0, false, 901, 901, {4}}, {"test_data/dmrs_pucch_processor_test_output2.dat"}},
  {{pucch_format::FORMAT_1, {0, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 10, 36, true, 29, 1, 2, 1, false, 366, 366, {1}}, {"test_data/dmrs_pucch_processor_test_output3.dat"}},
  {{pucch_format::FORMAT_2, {0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 2, 7, false, 0, 11, 1, 0, false, 939, 939, {1}}, {"test_data/dmrs_pucch_processor_test_output4.dat"}},
  {{pucch_format::FORMAT_2, {0, 1}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 1, 20, false, 0, 13, 9, 0, false, 885, 885, {3}}, {"test_data/dmrs_pucch_processor_test_output5.dat"}},
  {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 1, 5, true, 32, 3, 5, 0, false, 189, 189, {1}}, {"test_data/dmrs_pucch_processor_test_output6.dat"}},
  {{pucch_format::FORMAT_2, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 11, 2, 39, true, 19, 13, 10, 0, false, 631, 631, {5}}, {"test_data/dmrs_pucch_processor_test_output7.dat"}},
  {{pucch_format::FORMAT_1, {1, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 40, false, 0, 1, 8, 3, false, 409, 409, {5}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
  {{pucch_format::FORMAT_1, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 8, false, 0, 1, 11, 4, false, 694, 694, {4}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
  {{pucch_format::FORMAT_1, {1, 5}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 10, 6, true, 31, 1, 5, 0, false, 208, 208, {5}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
  {{pucch_format::FORMAT_1, {1, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 6, 1, true, 19, 1, 1, 0, false, 250, 250, {1}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
  {{pucch_format::FORMAT_2, {1, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 5, 1, 20, false, 0, 4, 6, 0, false, 237, 237, {7}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
  {{pucch_format::FORMAT_2, {1, 18}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 2, 0, false, 0, 10, 7, 0, false, 185, 185, {3}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
  {{pucch_format::FORMAT_2, {1, 10}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 1, 16, true, 1, 13, 11, 0, false, 948, 948, {6}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
  {{pucch_format::FORMAT_2, {1, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 2, 8, true, 36, 8, 0, 0, false, 572, 572, {2}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
    // clang-format on
};

} // namespace srsgnb
