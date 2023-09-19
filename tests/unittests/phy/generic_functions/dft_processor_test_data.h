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
//   + "srsDFTProcessorUnittest.m"

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
  {{256, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input2.dat"}, {"test_data/dft_processor_test_output2.dat"}},
  {{256, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input3.dat"}, {"test_data/dft_processor_test_output3.dat"}},
  {{384, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input4.dat"}, {"test_data/dft_processor_test_output4.dat"}},
  {{384, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input5.dat"}, {"test_data/dft_processor_test_output5.dat"}},
  {{512, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input6.dat"}, {"test_data/dft_processor_test_output6.dat"}},
  {{512, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input7.dat"}, {"test_data/dft_processor_test_output7.dat"}},
  {{768, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input8.dat"}, {"test_data/dft_processor_test_output8.dat"}},
  {{768, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input9.dat"}, {"test_data/dft_processor_test_output9.dat"}},
  {{1024, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input10.dat"}, {"test_data/dft_processor_test_output10.dat"}},
  {{1024, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input11.dat"}, {"test_data/dft_processor_test_output11.dat"}},
  {{1536, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input12.dat"}, {"test_data/dft_processor_test_output12.dat"}},
  {{1536, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input13.dat"}, {"test_data/dft_processor_test_output13.dat"}},
  {{2048, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input14.dat"}, {"test_data/dft_processor_test_output14.dat"}},
  {{2048, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input15.dat"}, {"test_data/dft_processor_test_output15.dat"}},
  {{3072, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input16.dat"}, {"test_data/dft_processor_test_output16.dat"}},
  {{3072, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input17.dat"}, {"test_data/dft_processor_test_output17.dat"}},
  {{4096, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input18.dat"}, {"test_data/dft_processor_test_output18.dat"}},
  {{4096, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input19.dat"}, {"test_data/dft_processor_test_output19.dat"}},
  {{4608, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input20.dat"}, {"test_data/dft_processor_test_output20.dat"}},
  {{4608, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input21.dat"}, {"test_data/dft_processor_test_output21.dat"}},
  {{6144, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input22.dat"}, {"test_data/dft_processor_test_output22.dat"}},
  {{6144, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input23.dat"}, {"test_data/dft_processor_test_output23.dat"}},
  {{9216, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input24.dat"}, {"test_data/dft_processor_test_output24.dat"}},
  {{9216, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input25.dat"}, {"test_data/dft_processor_test_output25.dat"}},
  {{12288, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input26.dat"}, {"test_data/dft_processor_test_output26.dat"}},
  {{12288, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input27.dat"}, {"test_data/dft_processor_test_output27.dat"}},
  {{18432, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input28.dat"}, {"test_data/dft_processor_test_output28.dat"}},
  {{18432, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input29.dat"}, {"test_data/dft_processor_test_output29.dat"}},
  {{24576, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input30.dat"}, {"test_data/dft_processor_test_output30.dat"}},
  {{24576, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input31.dat"}, {"test_data/dft_processor_test_output31.dat"}},
  {{36864, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input32.dat"}, {"test_data/dft_processor_test_output32.dat"}},
  {{36864, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input33.dat"}, {"test_data/dft_processor_test_output33.dat"}},
  {{49152, dft_processor::direction::DIRECT}, {"test_data/dft_processor_test_input34.dat"}, {"test_data/dft_processor_test_output34.dat"}},
  {{49152, dft_processor::direction::INVERSE}, {"test_data/dft_processor_test_input35.dat"}, {"test_data/dft_processor_test_output35.dat"}},
    // clang-format on
};

} // namespace srsran
