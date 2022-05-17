/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsPUCCHdmrsUnittest.m"

#include "../../resource_grid_test_doubles.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/to_array.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/support/file_vector.h"
#include <array>

namespace srsgnb {

struct test_case_t {
  dmrs_pucch_processor::config_t                          config;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pucch_processor_test_data = {
    // clang-format off
    {{pucch_format::FORMAT_1, {0, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 17, false, 0, 1, 5, 0, false, 546, 546, {0}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
    {{pucch_format::FORMAT_1, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 3, 10, 8, false, 0, 1, 5, 4, false, 637, 637, {7}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
    {{pucch_format::FORMAT_1, {0, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 12, 51, true, 5, 1, 11, 0, false, 137, 137, {6}}, {"test_data/dmrs_pucch_processor_test_output2.dat"}},
    {{pucch_format::FORMAT_1, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 1, 12, 23, true, 30, 1, 8, 2, false, 536, 536, {1}}, {"test_data/dmrs_pucch_processor_test_output3.dat"}},
    {{pucch_format::FORMAT_2, {0, 4}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 10, 2, 37, false, 0, 10, 1, 0, false, 419, 419, {1}}, {"test_data/dmrs_pucch_processor_test_output4.dat"}},
    {{pucch_format::FORMAT_2, {0, 2}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 17, false, 0, 11, 1, 0, false, 900, 900, {4}}, {"test_data/dmrs_pucch_processor_test_output5.dat"}},
    {{pucch_format::FORMAT_2, {0, 8}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 1, 23, true, 15, 5, 3, 0, false, 763, 763, {3}}, {"test_data/dmrs_pucch_processor_test_output6.dat"}},
    {{pucch_format::FORMAT_2, {0, 9}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 1, 10, true, 16, 5, 9, 0, false, 338, 338, {2}}, {"test_data/dmrs_pucch_processor_test_output7.dat"}},
    {{pucch_format::FORMAT_1, {1, 7}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 2, 11, 43, false, 0, 1, 5, 3, false, 714, 714, {3}}, {"test_data/dmrs_pucch_processor_test_output8.dat"}},
    {{pucch_format::FORMAT_1, {1, 6}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 5, false, 0, 1, 7, 3, false, 116, 116, {1}}, {"test_data/dmrs_pucch_processor_test_output9.dat"}},
    {{pucch_format::FORMAT_1, {1, 15}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 0, 13, 4, true, 3, 1, 2, 0, false, 1000, 1000, {4}}, {"test_data/dmrs_pucch_processor_test_output10.dat"}},
    {{pucch_format::FORMAT_1, {1, 11}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 7, 5, 47, true, 34, 1, 5, 0, false, 287, 287, {3}}, {"test_data/dmrs_pucch_processor_test_output11.dat"}},
    {{pucch_format::FORMAT_2, {1, 18}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 4, 1, 13, false, 0, 10, 2, 0, false, 322, 322, {4}}, {"test_data/dmrs_pucch_processor_test_output12.dat"}},
    {{pucch_format::FORMAT_2, {1, 0}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 2, 34, false, 0, 12, 2, 0, false, 516, 516, {6}}, {"test_data/dmrs_pucch_processor_test_output13.dat"}},
    {{pucch_format::FORMAT_2, {1, 14}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 9, 1, 51, true, 50, 1, 1, 0, false, 643, 643, {5}}, {"test_data/dmrs_pucch_processor_test_output14.dat"}},
    {{pucch_format::FORMAT_2, {1, 10}, cyclic_prefix::NORMAL, pucch_group_hopping::NEITHER, 8, 1, 33, true, 42, 9, 1, 0, false, 566, 566, {4}}, {"test_data/dmrs_pucch_processor_test_output15.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_TEST_DATA_H