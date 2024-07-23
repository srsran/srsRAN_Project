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

// This file was generated using the following MATLAB class on 19-07-2024 (seed 0):
//   + "srsTransformPrecoderUnittest.m"

#include "srsran/adt/complex.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned          M_rb;
  file_vector<cf_t> deprecode_input;
  file_vector<cf_t> deprecode_output;
};

static const std::vector<test_case_t> transform_precoder_test_data = {
    // clang-format off
  {1, {"test_data/transform_precoder_test_input0.dat"}, {"test_data/transform_precoder_test_output0.dat"}},
  {2, {"test_data/transform_precoder_test_input1.dat"}, {"test_data/transform_precoder_test_output1.dat"}},
  {3, {"test_data/transform_precoder_test_input2.dat"}, {"test_data/transform_precoder_test_output2.dat"}},
  {4, {"test_data/transform_precoder_test_input3.dat"}, {"test_data/transform_precoder_test_output3.dat"}},
  {5, {"test_data/transform_precoder_test_input4.dat"}, {"test_data/transform_precoder_test_output4.dat"}},
  {6, {"test_data/transform_precoder_test_input5.dat"}, {"test_data/transform_precoder_test_output5.dat"}},
  {8, {"test_data/transform_precoder_test_input6.dat"}, {"test_data/transform_precoder_test_output6.dat"}},
  {9, {"test_data/transform_precoder_test_input7.dat"}, {"test_data/transform_precoder_test_output7.dat"}},
  {10, {"test_data/transform_precoder_test_input8.dat"}, {"test_data/transform_precoder_test_output8.dat"}},
  {12, {"test_data/transform_precoder_test_input9.dat"}, {"test_data/transform_precoder_test_output9.dat"}},
  {15, {"test_data/transform_precoder_test_input10.dat"}, {"test_data/transform_precoder_test_output10.dat"}},
  {16, {"test_data/transform_precoder_test_input11.dat"}, {"test_data/transform_precoder_test_output11.dat"}},
  {18, {"test_data/transform_precoder_test_input12.dat"}, {"test_data/transform_precoder_test_output12.dat"}},
  {20, {"test_data/transform_precoder_test_input13.dat"}, {"test_data/transform_precoder_test_output13.dat"}},
  {24, {"test_data/transform_precoder_test_input14.dat"}, {"test_data/transform_precoder_test_output14.dat"}},
  {25, {"test_data/transform_precoder_test_input15.dat"}, {"test_data/transform_precoder_test_output15.dat"}},
  {27, {"test_data/transform_precoder_test_input16.dat"}, {"test_data/transform_precoder_test_output16.dat"}},
  {30, {"test_data/transform_precoder_test_input17.dat"}, {"test_data/transform_precoder_test_output17.dat"}},
  {32, {"test_data/transform_precoder_test_input18.dat"}, {"test_data/transform_precoder_test_output18.dat"}},
  {36, {"test_data/transform_precoder_test_input19.dat"}, {"test_data/transform_precoder_test_output19.dat"}},
  {40, {"test_data/transform_precoder_test_input20.dat"}, {"test_data/transform_precoder_test_output20.dat"}},
  {45, {"test_data/transform_precoder_test_input21.dat"}, {"test_data/transform_precoder_test_output21.dat"}},
  {48, {"test_data/transform_precoder_test_input22.dat"}, {"test_data/transform_precoder_test_output22.dat"}},
  {50, {"test_data/transform_precoder_test_input23.dat"}, {"test_data/transform_precoder_test_output23.dat"}},
  {54, {"test_data/transform_precoder_test_input24.dat"}, {"test_data/transform_precoder_test_output24.dat"}},
  {60, {"test_data/transform_precoder_test_input25.dat"}, {"test_data/transform_precoder_test_output25.dat"}},
  {64, {"test_data/transform_precoder_test_input26.dat"}, {"test_data/transform_precoder_test_output26.dat"}},
  {72, {"test_data/transform_precoder_test_input27.dat"}, {"test_data/transform_precoder_test_output27.dat"}},
  {75, {"test_data/transform_precoder_test_input28.dat"}, {"test_data/transform_precoder_test_output28.dat"}},
  {80, {"test_data/transform_precoder_test_input29.dat"}, {"test_data/transform_precoder_test_output29.dat"}},
  {81, {"test_data/transform_precoder_test_input30.dat"}, {"test_data/transform_precoder_test_output30.dat"}},
  {90, {"test_data/transform_precoder_test_input31.dat"}, {"test_data/transform_precoder_test_output31.dat"}},
  {96, {"test_data/transform_precoder_test_input32.dat"}, {"test_data/transform_precoder_test_output32.dat"}},
  {100, {"test_data/transform_precoder_test_input33.dat"}, {"test_data/transform_precoder_test_output33.dat"}},
  {108, {"test_data/transform_precoder_test_input34.dat"}, {"test_data/transform_precoder_test_output34.dat"}},
  {120, {"test_data/transform_precoder_test_input35.dat"}, {"test_data/transform_precoder_test_output35.dat"}},
  {125, {"test_data/transform_precoder_test_input36.dat"}, {"test_data/transform_precoder_test_output36.dat"}},
  {128, {"test_data/transform_precoder_test_input37.dat"}, {"test_data/transform_precoder_test_output37.dat"}},
  {135, {"test_data/transform_precoder_test_input38.dat"}, {"test_data/transform_precoder_test_output38.dat"}},
  {144, {"test_data/transform_precoder_test_input39.dat"}, {"test_data/transform_precoder_test_output39.dat"}},
  {150, {"test_data/transform_precoder_test_input40.dat"}, {"test_data/transform_precoder_test_output40.dat"}},
  {160, {"test_data/transform_precoder_test_input41.dat"}, {"test_data/transform_precoder_test_output41.dat"}},
  {162, {"test_data/transform_precoder_test_input42.dat"}, {"test_data/transform_precoder_test_output42.dat"}},
  {180, {"test_data/transform_precoder_test_input43.dat"}, {"test_data/transform_precoder_test_output43.dat"}},
  {192, {"test_data/transform_precoder_test_input44.dat"}, {"test_data/transform_precoder_test_output44.dat"}},
  {200, {"test_data/transform_precoder_test_input45.dat"}, {"test_data/transform_precoder_test_output45.dat"}},
  {216, {"test_data/transform_precoder_test_input46.dat"}, {"test_data/transform_precoder_test_output46.dat"}},
  {225, {"test_data/transform_precoder_test_input47.dat"}, {"test_data/transform_precoder_test_output47.dat"}},
  {240, {"test_data/transform_precoder_test_input48.dat"}, {"test_data/transform_precoder_test_output48.dat"}},
  {243, {"test_data/transform_precoder_test_input49.dat"}, {"test_data/transform_precoder_test_output49.dat"}},
  {250, {"test_data/transform_precoder_test_input50.dat"}, {"test_data/transform_precoder_test_output50.dat"}},
  {256, {"test_data/transform_precoder_test_input51.dat"}, {"test_data/transform_precoder_test_output51.dat"}},
  {270, {"test_data/transform_precoder_test_input52.dat"}, {"test_data/transform_precoder_test_output52.dat"}},
    // clang-format on
};

} // namespace srsran
