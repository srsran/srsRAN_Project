#ifndef SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_TEST_DATA_H

// This file was generated using the following MATLAB scripts:
//   + "srsPBCHdmrsUnittest.m"

#include "../../resource_grid_test_doubles.h"
#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/to_array.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsgnb/support/file_vector.h"
#include <array>

namespace srsgnb {

struct test_case_t {
  dmrs_pbch_processor::config_t config;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pbch_processor_test_data = {
    // clang-format off
  {{551, 0, 4, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output0.dat"}},
  {{298, 0, 4, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output1.dat"}},
  {{750, 0, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output2.dat"}},
  {{190, 0, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output3.dat"}},
  {{692, 0, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output4.dat"}},
  {{184, 0, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output5.dat"}},
  {{371, 1, 4, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output6.dat"}},
  {{630, 1, 4, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output7.dat"}},
  {{786, 1, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output8.dat"}},
  {{81, 1, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output9.dat"}},
  {{936, 1, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output10.dat"}},
  {{781, 1, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output11.dat"}},
  {{490, 2, 4, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output12.dat"}},
  {{439, 2, 4, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output13.dat"}},
  {{450, 2, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output14.dat"}},
  {{308, 2, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output15.dat"}},
  {{512, 2, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output16.dat"}},
  {{514, 2, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output17.dat"}},
  {{824, 3, 4, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output18.dat"}},
  {{801, 3, 4, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output19.dat"}},
  {{649, 3, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output20.dat"}},
  {{381, 3, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output21.dat"}},
  {{818, 3, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output22.dat"}},
  {{537, 3, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output23.dat"}},
  {{353, 4, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output24.dat"}},
  {{946, 4, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output25.dat"}},
  {{882, 4, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output26.dat"}},
  {{554, 4, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output27.dat"}},
  {{627, 6, 8, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output28.dat"}},
  {{591, 6, 8, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output29.dat"}},
  {{209, 6, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output30.dat"}},
  {{303, 6, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output31.dat"}},
  {{474, 16, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output32.dat"}},
  {{232, 16, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output33.dat"}},
  {{851, 32, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output34.dat"}},
  {{196, 32, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output35.dat"}},
  {{227, 48, 64, 0, 0, 0, 1.0, { 0}},{"dmrs_pbch_processor_test_output36.dat"}},
  {{172, 48, 64, 0, 0, 1, 1.0, { 0}},{"dmrs_pbch_processor_test_output37.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_TEST_DATA_H
