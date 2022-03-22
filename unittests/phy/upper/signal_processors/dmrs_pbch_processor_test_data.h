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
    {{434, 0, 4, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output0.dat"}},
    {{207, 0, 4, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output1.dat"}},
    {{899, 0, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output2.dat"}},
    {{111, 0, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output3.dat"}},
    {{507, 0, 64, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output4.dat"}},
    {{391, 0, 64, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output5.dat"}},
    {{45, 1, 4, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output6.dat"}},
    {{289, 1, 4, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output7.dat"}},
    {{72, 1, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output8.dat"}},
    {{586, 1, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output9.dat"}},
    {{580, 1, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output10.dat"}},
    {{737, 1, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output11.dat"}},
    {{596, 2, 4, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output12.dat"}},
    {{44, 2, 4, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output13.dat"}},
    {{941, 2, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output14.dat"}},
    {{80, 2, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output15.dat"}},
    {{692, 2, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output16.dat"}},
    {{978, 2, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output17.dat"}},
    {{924, 3, 4, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output18.dat"}},
    {{754, 3, 4, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output19.dat"}},
    {{924, 3, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output20.dat"}},
    {{226, 3, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output21.dat"}},
    {{727, 3, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output22.dat"}},
    {{463, 3, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output23.dat"}},
    {{990, 4, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output24.dat"}},
    {{839, 4, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output25.dat"}},
    {{578, 4, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output26.dat"}},
    {{494, 4, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output27.dat"}},
    {{781, 6, 8, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output28.dat"}},
    {{691, 6, 8, 0, 0, 1, 1.0, {0}}, {"dmrs_pbch_processor_test_output29.dat"}},
    {{76, 6, 64, 0, 0, 0, 1.0, {0}}, {"dmrs_pbch_processor_test_output30.dat"}},
    {{792, 6, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output31.dat"}},
    {{610, 16, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output32.dat"}},
    {{991, 16, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output33.dat"}},
    {{291, 32, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output34.dat"}},
    {{385, 32, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output35.dat"}},
    {{632, 48, 64, 0, 0, 0, 1.0, {0}},
     {"dmrs_pbch_processor_test_output36.dat"}},
    {{174, 48, 64, 0, 0, 1, 1.0, {0}},
     {"dmrs_pbch_processor_test_output37.dat"}},
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_TEST_DATA_H