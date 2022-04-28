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
    {{pucch_format::FORMAT_1, {0, 9}, pucch_group_hopping::NEITHER, 0, 6, 38, false, 0, 1, 0, 1, false, 760, 760, {7}}, {"test_data/dmrs_pucch_processor_test_output0.dat"}},
    {{pucch_format::FORMAT_1, {1, 17}, pucch_group_hopping::NEITHER, 0, 5, 41, false, 0, 1, 5, 0, false, 586, 586, {7}}, {"test_data/dmrs_pucch_processor_test_output1.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_TEST_DATA_H