/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 18-Nov-2022:
//   + "srsDFTprocessorUnittest.m"

#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  dft_processor::configuration config;
  file_vector<cf_t>            data;
  file_vector<cf_t>            transform;
};

static const std::vector<test_case_t> dft_processor_test_data = {
    // clang-format off
  {{128, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input0.dat"}, {"test_data/dft_processor_test_output0.dat"}},
  {{128, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input1.dat"}, {"test_data/dft_processor_test_output1.dat"}},
  {{256, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input4.dat"}, {"test_data/dft_processor_test_output4.dat"}},
  {{256, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input5.dat"}, {"test_data/dft_processor_test_output5.dat"}},
  {{384, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input6.dat"}, {"test_data/dft_processor_test_output6.dat"}},
  {{384, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input7.dat"}, {"test_data/dft_processor_test_output7.dat"}},
  {{512, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input8.dat"}, {"test_data/dft_processor_test_output8.dat"}},
  {{512, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input9.dat"}, {"test_data/dft_processor_test_output9.dat"}},
  {{768, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input10.dat"}, {"test_data/dft_processor_test_output10.dat"}},
  {{768, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input11.dat"}, {"test_data/dft_processor_test_output11.dat"}},
  {{1024, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input14.dat"}, {"test_data/dft_processor_test_output14.dat"}},
  {{1024, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input15.dat"}, {"test_data/dft_processor_test_output15.dat"}},
  {{1536, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input16.dat"}, {"test_data/dft_processor_test_output16.dat"}},
  {{1536, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input17.dat"}, {"test_data/dft_processor_test_output17.dat"}},
  {{2048, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input18.dat"}, {"test_data/dft_processor_test_output18.dat"}},
  {{2048, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input19.dat"}, {"test_data/dft_processor_test_output19.dat"}},
  {{3072, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input20.dat"}, {"test_data/dft_processor_test_output20.dat"}},
  {{3072, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input21.dat"}, {"test_data/dft_processor_test_output21.dat"}},
  {{4096, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input22.dat"}, {"test_data/dft_processor_test_output22.dat"}},
  {{4096, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input23.dat"}, {"test_data/dft_processor_test_output23.dat"}},
  {{4608, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input24.dat"}, {"test_data/dft_processor_test_output24.dat"}},
  {{4608, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input25.dat"}, {"test_data/dft_processor_test_output25.dat"}},
  {{6144, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input26.dat"}, {"test_data/dft_processor_test_output26.dat"}},
  {{6144, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input27.dat"}, {"test_data/dft_processor_test_output27.dat"}},
  {{9216, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input28.dat"}, {"test_data/dft_processor_test_output28.dat"}},
  {{9216, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input29.dat"}, {"test_data/dft_processor_test_output29.dat"}},
  {{12288, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input30.dat"}, {"test_data/dft_processor_test_output30.dat"}},
  {{12288, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input31.dat"}, {"test_data/dft_processor_test_output31.dat"}},
  {{18432, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input32.dat"}, {"test_data/dft_processor_test_output32.dat"}},
  {{18432, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input33.dat"}, {"test_data/dft_processor_test_output33.dat"}},
  {{24576, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input34.dat"}, {"test_data/dft_processor_test_output34.dat"}},
  {{24576, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input35.dat"}, {"test_data/dft_processor_test_output35.dat"}},
  {{36864, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input36.dat"}, {"test_data/dft_processor_test_output36.dat"}},
  {{36864, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input37.dat"}, {"test_data/dft_processor_test_output37.dat"}},
  {{49152, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input38.dat"}, {"test_data/dft_processor_test_output38.dat"}},
  {{49152, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input39.dat"}, {"test_data/dft_processor_test_output39.dat"}},
    // clang-format on
};

} // namespace srsran
