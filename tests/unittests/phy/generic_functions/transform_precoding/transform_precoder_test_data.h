/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

// This file was generated using the following MATLAB class on 08-11-2024 (seed 0):
//   + "srsTransformPrecoderUnittest.m"

#include "srsran/adt/complex.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned           M_rb;
  file_vector<cf_t>  deprecode_data_input;
  file_vector<float> deprecode_noise_input;
  file_vector<cf_t>  deprecode_data_output;
  file_vector<float> deprecode_noise_output;
};

static const std::vector<test_case_t> transform_precoder_test_data = {
    // clang-format off
  {1, {"test_data/transform_precoder_test_input_data0.dat"}, {"test_data/transform_precoder_test_input_noise0.dat"}, {"test_data/transform_precoder_test_output_data0.dat"}, {"test_data/transform_precoder_test_output_noise0.dat"}},
  {2, {"test_data/transform_precoder_test_input_data2.dat"}, {"test_data/transform_precoder_test_input_noise2.dat"}, {"test_data/transform_precoder_test_output_data2.dat"}, {"test_data/transform_precoder_test_output_noise2.dat"}},
  {3, {"test_data/transform_precoder_test_input_data4.dat"}, {"test_data/transform_precoder_test_input_noise4.dat"}, {"test_data/transform_precoder_test_output_data4.dat"}, {"test_data/transform_precoder_test_output_noise4.dat"}},
  {4, {"test_data/transform_precoder_test_input_data6.dat"}, {"test_data/transform_precoder_test_input_noise6.dat"}, {"test_data/transform_precoder_test_output_data6.dat"}, {"test_data/transform_precoder_test_output_noise6.dat"}},
  {5, {"test_data/transform_precoder_test_input_data8.dat"}, {"test_data/transform_precoder_test_input_noise8.dat"}, {"test_data/transform_precoder_test_output_data8.dat"}, {"test_data/transform_precoder_test_output_noise8.dat"}},
  {6, {"test_data/transform_precoder_test_input_data10.dat"}, {"test_data/transform_precoder_test_input_noise10.dat"}, {"test_data/transform_precoder_test_output_data10.dat"}, {"test_data/transform_precoder_test_output_noise10.dat"}},
  {8, {"test_data/transform_precoder_test_input_data12.dat"}, {"test_data/transform_precoder_test_input_noise12.dat"}, {"test_data/transform_precoder_test_output_data12.dat"}, {"test_data/transform_precoder_test_output_noise12.dat"}},
  {9, {"test_data/transform_precoder_test_input_data14.dat"}, {"test_data/transform_precoder_test_input_noise14.dat"}, {"test_data/transform_precoder_test_output_data14.dat"}, {"test_data/transform_precoder_test_output_noise14.dat"}},
  {10, {"test_data/transform_precoder_test_input_data16.dat"}, {"test_data/transform_precoder_test_input_noise16.dat"}, {"test_data/transform_precoder_test_output_data16.dat"}, {"test_data/transform_precoder_test_output_noise16.dat"}},
  {12, {"test_data/transform_precoder_test_input_data18.dat"}, {"test_data/transform_precoder_test_input_noise18.dat"}, {"test_data/transform_precoder_test_output_data18.dat"}, {"test_data/transform_precoder_test_output_noise18.dat"}},
  {15, {"test_data/transform_precoder_test_input_data20.dat"}, {"test_data/transform_precoder_test_input_noise20.dat"}, {"test_data/transform_precoder_test_output_data20.dat"}, {"test_data/transform_precoder_test_output_noise20.dat"}},
  {16, {"test_data/transform_precoder_test_input_data22.dat"}, {"test_data/transform_precoder_test_input_noise22.dat"}, {"test_data/transform_precoder_test_output_data22.dat"}, {"test_data/transform_precoder_test_output_noise22.dat"}},
  {18, {"test_data/transform_precoder_test_input_data24.dat"}, {"test_data/transform_precoder_test_input_noise24.dat"}, {"test_data/transform_precoder_test_output_data24.dat"}, {"test_data/transform_precoder_test_output_noise24.dat"}},
  {20, {"test_data/transform_precoder_test_input_data26.dat"}, {"test_data/transform_precoder_test_input_noise26.dat"}, {"test_data/transform_precoder_test_output_data26.dat"}, {"test_data/transform_precoder_test_output_noise26.dat"}},
  {24, {"test_data/transform_precoder_test_input_data28.dat"}, {"test_data/transform_precoder_test_input_noise28.dat"}, {"test_data/transform_precoder_test_output_data28.dat"}, {"test_data/transform_precoder_test_output_noise28.dat"}},
  {25, {"test_data/transform_precoder_test_input_data30.dat"}, {"test_data/transform_precoder_test_input_noise30.dat"}, {"test_data/transform_precoder_test_output_data30.dat"}, {"test_data/transform_precoder_test_output_noise30.dat"}},
  {27, {"test_data/transform_precoder_test_input_data32.dat"}, {"test_data/transform_precoder_test_input_noise32.dat"}, {"test_data/transform_precoder_test_output_data32.dat"}, {"test_data/transform_precoder_test_output_noise32.dat"}},
  {30, {"test_data/transform_precoder_test_input_data34.dat"}, {"test_data/transform_precoder_test_input_noise34.dat"}, {"test_data/transform_precoder_test_output_data34.dat"}, {"test_data/transform_precoder_test_output_noise34.dat"}},
  {32, {"test_data/transform_precoder_test_input_data36.dat"}, {"test_data/transform_precoder_test_input_noise36.dat"}, {"test_data/transform_precoder_test_output_data36.dat"}, {"test_data/transform_precoder_test_output_noise36.dat"}},
  {36, {"test_data/transform_precoder_test_input_data38.dat"}, {"test_data/transform_precoder_test_input_noise38.dat"}, {"test_data/transform_precoder_test_output_data38.dat"}, {"test_data/transform_precoder_test_output_noise38.dat"}},
  {40, {"test_data/transform_precoder_test_input_data40.dat"}, {"test_data/transform_precoder_test_input_noise40.dat"}, {"test_data/transform_precoder_test_output_data40.dat"}, {"test_data/transform_precoder_test_output_noise40.dat"}},
  {45, {"test_data/transform_precoder_test_input_data42.dat"}, {"test_data/transform_precoder_test_input_noise42.dat"}, {"test_data/transform_precoder_test_output_data42.dat"}, {"test_data/transform_precoder_test_output_noise42.dat"}},
  {48, {"test_data/transform_precoder_test_input_data44.dat"}, {"test_data/transform_precoder_test_input_noise44.dat"}, {"test_data/transform_precoder_test_output_data44.dat"}, {"test_data/transform_precoder_test_output_noise44.dat"}},
  {50, {"test_data/transform_precoder_test_input_data46.dat"}, {"test_data/transform_precoder_test_input_noise46.dat"}, {"test_data/transform_precoder_test_output_data46.dat"}, {"test_data/transform_precoder_test_output_noise46.dat"}},
  {54, {"test_data/transform_precoder_test_input_data48.dat"}, {"test_data/transform_precoder_test_input_noise48.dat"}, {"test_data/transform_precoder_test_output_data48.dat"}, {"test_data/transform_precoder_test_output_noise48.dat"}},
  {60, {"test_data/transform_precoder_test_input_data50.dat"}, {"test_data/transform_precoder_test_input_noise50.dat"}, {"test_data/transform_precoder_test_output_data50.dat"}, {"test_data/transform_precoder_test_output_noise50.dat"}},
  {64, {"test_data/transform_precoder_test_input_data52.dat"}, {"test_data/transform_precoder_test_input_noise52.dat"}, {"test_data/transform_precoder_test_output_data52.dat"}, {"test_data/transform_precoder_test_output_noise52.dat"}},
  {72, {"test_data/transform_precoder_test_input_data54.dat"}, {"test_data/transform_precoder_test_input_noise54.dat"}, {"test_data/transform_precoder_test_output_data54.dat"}, {"test_data/transform_precoder_test_output_noise54.dat"}},
  {75, {"test_data/transform_precoder_test_input_data56.dat"}, {"test_data/transform_precoder_test_input_noise56.dat"}, {"test_data/transform_precoder_test_output_data56.dat"}, {"test_data/transform_precoder_test_output_noise56.dat"}},
  {80, {"test_data/transform_precoder_test_input_data58.dat"}, {"test_data/transform_precoder_test_input_noise58.dat"}, {"test_data/transform_precoder_test_output_data58.dat"}, {"test_data/transform_precoder_test_output_noise58.dat"}},
  {81, {"test_data/transform_precoder_test_input_data60.dat"}, {"test_data/transform_precoder_test_input_noise60.dat"}, {"test_data/transform_precoder_test_output_data60.dat"}, {"test_data/transform_precoder_test_output_noise60.dat"}},
  {90, {"test_data/transform_precoder_test_input_data62.dat"}, {"test_data/transform_precoder_test_input_noise62.dat"}, {"test_data/transform_precoder_test_output_data62.dat"}, {"test_data/transform_precoder_test_output_noise62.dat"}},
  {96, {"test_data/transform_precoder_test_input_data64.dat"}, {"test_data/transform_precoder_test_input_noise64.dat"}, {"test_data/transform_precoder_test_output_data64.dat"}, {"test_data/transform_precoder_test_output_noise64.dat"}},
  {100, {"test_data/transform_precoder_test_input_data66.dat"}, {"test_data/transform_precoder_test_input_noise66.dat"}, {"test_data/transform_precoder_test_output_data66.dat"}, {"test_data/transform_precoder_test_output_noise66.dat"}},
  {108, {"test_data/transform_precoder_test_input_data68.dat"}, {"test_data/transform_precoder_test_input_noise68.dat"}, {"test_data/transform_precoder_test_output_data68.dat"}, {"test_data/transform_precoder_test_output_noise68.dat"}},
  {120, {"test_data/transform_precoder_test_input_data70.dat"}, {"test_data/transform_precoder_test_input_noise70.dat"}, {"test_data/transform_precoder_test_output_data70.dat"}, {"test_data/transform_precoder_test_output_noise70.dat"}},
  {125, {"test_data/transform_precoder_test_input_data72.dat"}, {"test_data/transform_precoder_test_input_noise72.dat"}, {"test_data/transform_precoder_test_output_data72.dat"}, {"test_data/transform_precoder_test_output_noise72.dat"}},
  {128, {"test_data/transform_precoder_test_input_data74.dat"}, {"test_data/transform_precoder_test_input_noise74.dat"}, {"test_data/transform_precoder_test_output_data74.dat"}, {"test_data/transform_precoder_test_output_noise74.dat"}},
  {135, {"test_data/transform_precoder_test_input_data76.dat"}, {"test_data/transform_precoder_test_input_noise76.dat"}, {"test_data/transform_precoder_test_output_data76.dat"}, {"test_data/transform_precoder_test_output_noise76.dat"}},
  {144, {"test_data/transform_precoder_test_input_data78.dat"}, {"test_data/transform_precoder_test_input_noise78.dat"}, {"test_data/transform_precoder_test_output_data78.dat"}, {"test_data/transform_precoder_test_output_noise78.dat"}},
  {150, {"test_data/transform_precoder_test_input_data80.dat"}, {"test_data/transform_precoder_test_input_noise80.dat"}, {"test_data/transform_precoder_test_output_data80.dat"}, {"test_data/transform_precoder_test_output_noise80.dat"}},
  {160, {"test_data/transform_precoder_test_input_data82.dat"}, {"test_data/transform_precoder_test_input_noise82.dat"}, {"test_data/transform_precoder_test_output_data82.dat"}, {"test_data/transform_precoder_test_output_noise82.dat"}},
  {162, {"test_data/transform_precoder_test_input_data84.dat"}, {"test_data/transform_precoder_test_input_noise84.dat"}, {"test_data/transform_precoder_test_output_data84.dat"}, {"test_data/transform_precoder_test_output_noise84.dat"}},
  {180, {"test_data/transform_precoder_test_input_data86.dat"}, {"test_data/transform_precoder_test_input_noise86.dat"}, {"test_data/transform_precoder_test_output_data86.dat"}, {"test_data/transform_precoder_test_output_noise86.dat"}},
  {192, {"test_data/transform_precoder_test_input_data88.dat"}, {"test_data/transform_precoder_test_input_noise88.dat"}, {"test_data/transform_precoder_test_output_data88.dat"}, {"test_data/transform_precoder_test_output_noise88.dat"}},
  {200, {"test_data/transform_precoder_test_input_data90.dat"}, {"test_data/transform_precoder_test_input_noise90.dat"}, {"test_data/transform_precoder_test_output_data90.dat"}, {"test_data/transform_precoder_test_output_noise90.dat"}},
  {216, {"test_data/transform_precoder_test_input_data92.dat"}, {"test_data/transform_precoder_test_input_noise92.dat"}, {"test_data/transform_precoder_test_output_data92.dat"}, {"test_data/transform_precoder_test_output_noise92.dat"}},
  {225, {"test_data/transform_precoder_test_input_data94.dat"}, {"test_data/transform_precoder_test_input_noise94.dat"}, {"test_data/transform_precoder_test_output_data94.dat"}, {"test_data/transform_precoder_test_output_noise94.dat"}},
  {240, {"test_data/transform_precoder_test_input_data96.dat"}, {"test_data/transform_precoder_test_input_noise96.dat"}, {"test_data/transform_precoder_test_output_data96.dat"}, {"test_data/transform_precoder_test_output_noise96.dat"}},
  {243, {"test_data/transform_precoder_test_input_data98.dat"}, {"test_data/transform_precoder_test_input_noise98.dat"}, {"test_data/transform_precoder_test_output_data98.dat"}, {"test_data/transform_precoder_test_output_noise98.dat"}},
  {250, {"test_data/transform_precoder_test_input_data100.dat"}, {"test_data/transform_precoder_test_input_noise100.dat"}, {"test_data/transform_precoder_test_output_data100.dat"}, {"test_data/transform_precoder_test_output_noise100.dat"}},
  {256, {"test_data/transform_precoder_test_input_data102.dat"}, {"test_data/transform_precoder_test_input_noise102.dat"}, {"test_data/transform_precoder_test_output_data102.dat"}, {"test_data/transform_precoder_test_output_noise102.dat"}},
  {270, {"test_data/transform_precoder_test_input_data104.dat"}, {"test_data/transform_precoder_test_input_noise104.dat"}, {"test_data/transform_precoder_test_output_data104.dat"}, {"test_data/transform_precoder_test_output_noise104.dat"}},
    // clang-format on
};

} // namespace srsran
