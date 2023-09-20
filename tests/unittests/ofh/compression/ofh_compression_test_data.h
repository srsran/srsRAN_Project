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
//   + "srsOFHCompressionUnittest.m"

#include "srsran/adt/complex.h"
#include "srsran/ofh/compression/compression_params.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  std::size_t           nof_prb;
  ofh::compression_type type;
  unsigned              cIQ_width;
  float                 iq_scaling;
  file_vector<cf_t>     symbols;
  file_vector<int16_t>  compressed_IQs;
  file_vector<uint8_t>  compressed_params;
};

static const std::vector<test_case_t> ofh_compression_test_data = {
    // clang-format off
  {25, ofh::compression_type::none, 8, 1, {"test_data/ofh_compression_test_input0.dat"}, {"test_data/ofh_compression_test_c_output0.dat"}, {"test_data/ofh_compression_test_c_param0.dat"}},
  {25, ofh::compression_type::none, 9, 0.73959, {"test_data/ofh_compression_test_input1.dat"}, {"test_data/ofh_compression_test_c_output1.dat"}, {"test_data/ofh_compression_test_c_param1.dat"}},
  {25, ofh::compression_type::none, 10, 0.57684, {"test_data/ofh_compression_test_input2.dat"}, {"test_data/ofh_compression_test_c_output2.dat"}, {"test_data/ofh_compression_test_c_param2.dat"}},
  {25, ofh::compression_type::none, 11, 1, {"test_data/ofh_compression_test_input3.dat"}, {"test_data/ofh_compression_test_c_output3.dat"}, {"test_data/ofh_compression_test_c_param3.dat"}},
  {25, ofh::compression_type::none, 12, 0.66318, {"test_data/ofh_compression_test_input4.dat"}, {"test_data/ofh_compression_test_c_output4.dat"}, {"test_data/ofh_compression_test_c_param4.dat"}},
  {25, ofh::compression_type::none, 13, 0.5015, {"test_data/ofh_compression_test_input5.dat"}, {"test_data/ofh_compression_test_c_output5.dat"}, {"test_data/ofh_compression_test_c_param5.dat"}},
  {25, ofh::compression_type::none, 14, 0.38347, {"test_data/ofh_compression_test_input6.dat"}, {"test_data/ofh_compression_test_c_output6.dat"}, {"test_data/ofh_compression_test_c_param6.dat"}},
  {25, ofh::compression_type::none, 15, 1, {"test_data/ofh_compression_test_input7.dat"}, {"test_data/ofh_compression_test_c_output7.dat"}, {"test_data/ofh_compression_test_c_param7.dat"}},
  {25, ofh::compression_type::none, 16, 0.84982, {"test_data/ofh_compression_test_input8.dat"}, {"test_data/ofh_compression_test_c_output8.dat"}, {"test_data/ofh_compression_test_c_param8.dat"}},
  {25, ofh::compression_type::BFP, 8, 0.88188, {"test_data/ofh_compression_test_input9.dat"}, {"test_data/ofh_compression_test_c_output9.dat"}, {"test_data/ofh_compression_test_c_param9.dat"}},
  {25, ofh::compression_type::BFP, 9, 0.48734, {"test_data/ofh_compression_test_input10.dat"}, {"test_data/ofh_compression_test_c_output10.dat"}, {"test_data/ofh_compression_test_c_param10.dat"}},
  {25, ofh::compression_type::BFP, 10, 0.6155, {"test_data/ofh_compression_test_input11.dat"}, {"test_data/ofh_compression_test_c_output11.dat"}, {"test_data/ofh_compression_test_c_param11.dat"}},
  {25, ofh::compression_type::BFP, 11, 0.79188, {"test_data/ofh_compression_test_input12.dat"}, {"test_data/ofh_compression_test_c_output12.dat"}, {"test_data/ofh_compression_test_c_param12.dat"}},
  {25, ofh::compression_type::BFP, 12, 1, {"test_data/ofh_compression_test_input13.dat"}, {"test_data/ofh_compression_test_c_output13.dat"}, {"test_data/ofh_compression_test_c_param13.dat"}},
  {25, ofh::compression_type::BFP, 13, 1, {"test_data/ofh_compression_test_input14.dat"}, {"test_data/ofh_compression_test_c_output14.dat"}, {"test_data/ofh_compression_test_c_param14.dat"}},
  {25, ofh::compression_type::BFP, 14, 1, {"test_data/ofh_compression_test_input15.dat"}, {"test_data/ofh_compression_test_c_output15.dat"}, {"test_data/ofh_compression_test_c_param15.dat"}},
  {25, ofh::compression_type::BFP, 15, 1, {"test_data/ofh_compression_test_input16.dat"}, {"test_data/ofh_compression_test_c_output16.dat"}, {"test_data/ofh_compression_test_c_param16.dat"}},
  {25, ofh::compression_type::BFP, 16, 0.46123, {"test_data/ofh_compression_test_input17.dat"}, {"test_data/ofh_compression_test_c_output17.dat"}, {"test_data/ofh_compression_test_c_param17.dat"}},
  {275, ofh::compression_type::none, 8, 0.67528, {"test_data/ofh_compression_test_input18.dat"}, {"test_data/ofh_compression_test_c_output18.dat"}, {"test_data/ofh_compression_test_c_param18.dat"}},
  {275, ofh::compression_type::none, 9, 0.97605, {"test_data/ofh_compression_test_input19.dat"}, {"test_data/ofh_compression_test_c_output19.dat"}, {"test_data/ofh_compression_test_c_param19.dat"}},
  {275, ofh::compression_type::none, 10, 0.68678, {"test_data/ofh_compression_test_input20.dat"}, {"test_data/ofh_compression_test_c_output20.dat"}, {"test_data/ofh_compression_test_c_param20.dat"}},
  {275, ofh::compression_type::none, 11, 1, {"test_data/ofh_compression_test_input21.dat"}, {"test_data/ofh_compression_test_c_output21.dat"}, {"test_data/ofh_compression_test_c_param21.dat"}},
  {275, ofh::compression_type::none, 12, 0.31654, {"test_data/ofh_compression_test_input22.dat"}, {"test_data/ofh_compression_test_c_output22.dat"}, {"test_data/ofh_compression_test_c_param22.dat"}},
  {275, ofh::compression_type::none, 13, 0.81136, {"test_data/ofh_compression_test_input23.dat"}, {"test_data/ofh_compression_test_c_output23.dat"}, {"test_data/ofh_compression_test_c_param23.dat"}},
  {275, ofh::compression_type::none, 14, 1, {"test_data/ofh_compression_test_input24.dat"}, {"test_data/ofh_compression_test_c_output24.dat"}, {"test_data/ofh_compression_test_c_param24.dat"}},
  {275, ofh::compression_type::none, 15, 0.49012, {"test_data/ofh_compression_test_input25.dat"}, {"test_data/ofh_compression_test_c_output25.dat"}, {"test_data/ofh_compression_test_c_param25.dat"}},
  {275, ofh::compression_type::none, 16, 0.2594, {"test_data/ofh_compression_test_input26.dat"}, {"test_data/ofh_compression_test_c_output26.dat"}, {"test_data/ofh_compression_test_c_param26.dat"}},
  {275, ofh::compression_type::BFP, 8, 0.41793, {"test_data/ofh_compression_test_input27.dat"}, {"test_data/ofh_compression_test_c_output27.dat"}, {"test_data/ofh_compression_test_c_param27.dat"}},
  {275, ofh::compression_type::BFP, 9, 0.35178, {"test_data/ofh_compression_test_input28.dat"}, {"test_data/ofh_compression_test_c_output28.dat"}, {"test_data/ofh_compression_test_c_param28.dat"}},
  {275, ofh::compression_type::BFP, 10, 0.85194, {"test_data/ofh_compression_test_input29.dat"}, {"test_data/ofh_compression_test_c_output29.dat"}, {"test_data/ofh_compression_test_c_param29.dat"}},
  {275, ofh::compression_type::BFP, 11, 1, {"test_data/ofh_compression_test_input30.dat"}, {"test_data/ofh_compression_test_c_output30.dat"}, {"test_data/ofh_compression_test_c_param30.dat"}},
  {275, ofh::compression_type::BFP, 12, 1, {"test_data/ofh_compression_test_input31.dat"}, {"test_data/ofh_compression_test_c_output31.dat"}, {"test_data/ofh_compression_test_c_param31.dat"}},
  {275, ofh::compression_type::BFP, 13, 0.95338, {"test_data/ofh_compression_test_input32.dat"}, {"test_data/ofh_compression_test_c_output32.dat"}, {"test_data/ofh_compression_test_c_param32.dat"}},
  {275, ofh::compression_type::BFP, 14, 1, {"test_data/ofh_compression_test_input33.dat"}, {"test_data/ofh_compression_test_c_output33.dat"}, {"test_data/ofh_compression_test_c_param33.dat"}},
  {275, ofh::compression_type::BFP, 15, 1, {"test_data/ofh_compression_test_input34.dat"}, {"test_data/ofh_compression_test_c_output34.dat"}, {"test_data/ofh_compression_test_c_param34.dat"}},
  {275, ofh::compression_type::BFP, 16, 0.82896, {"test_data/ofh_compression_test_input35.dat"}, {"test_data/ofh_compression_test_c_output35.dat"}, {"test_data/ofh_compression_test_c_param35.dat"}},
    // clang-format on
};

} // namespace srsran
