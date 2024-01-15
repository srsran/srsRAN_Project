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

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsLDPCEncoderUnittest.m"

#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned             nof_messages = 0;
  unsigned             bg           = 0;
  unsigned             ls           = 0;
  file_vector<uint8_t> messages;
  file_vector<uint8_t> codeblocks;
};

static const std::vector<test_case_t> ldpc_encoder_test_data = {
    // clang-format off
  {10, 1, 2, {"test_data/ldpc_encoder_test_input0.dat"}, {"test_data/ldpc_encoder_test_output0.dat"}},
  {10, 1, 3, {"test_data/ldpc_encoder_test_input1.dat"}, {"test_data/ldpc_encoder_test_output1.dat"}},
  {10, 1, 4, {"test_data/ldpc_encoder_test_input2.dat"}, {"test_data/ldpc_encoder_test_output2.dat"}},
  {10, 1, 5, {"test_data/ldpc_encoder_test_input3.dat"}, {"test_data/ldpc_encoder_test_output3.dat"}},
  {10, 1, 6, {"test_data/ldpc_encoder_test_input4.dat"}, {"test_data/ldpc_encoder_test_output4.dat"}},
  {10, 1, 7, {"test_data/ldpc_encoder_test_input5.dat"}, {"test_data/ldpc_encoder_test_output5.dat"}},
  {10, 1, 8, {"test_data/ldpc_encoder_test_input6.dat"}, {"test_data/ldpc_encoder_test_output6.dat"}},
  {10, 1, 9, {"test_data/ldpc_encoder_test_input7.dat"}, {"test_data/ldpc_encoder_test_output7.dat"}},
  {10, 1, 10, {"test_data/ldpc_encoder_test_input8.dat"}, {"test_data/ldpc_encoder_test_output8.dat"}},
  {10, 1, 11, {"test_data/ldpc_encoder_test_input9.dat"}, {"test_data/ldpc_encoder_test_output9.dat"}},
  {10, 1, 12, {"test_data/ldpc_encoder_test_input10.dat"}, {"test_data/ldpc_encoder_test_output10.dat"}},
  {10, 1, 13, {"test_data/ldpc_encoder_test_input11.dat"}, {"test_data/ldpc_encoder_test_output11.dat"}},
  {10, 1, 14, {"test_data/ldpc_encoder_test_input12.dat"}, {"test_data/ldpc_encoder_test_output12.dat"}},
  {10, 1, 15, {"test_data/ldpc_encoder_test_input13.dat"}, {"test_data/ldpc_encoder_test_output13.dat"}},
  {10, 1, 16, {"test_data/ldpc_encoder_test_input14.dat"}, {"test_data/ldpc_encoder_test_output14.dat"}},
  {10, 1, 18, {"test_data/ldpc_encoder_test_input15.dat"}, {"test_data/ldpc_encoder_test_output15.dat"}},
  {10, 1, 20, {"test_data/ldpc_encoder_test_input16.dat"}, {"test_data/ldpc_encoder_test_output16.dat"}},
  {10, 1, 22, {"test_data/ldpc_encoder_test_input17.dat"}, {"test_data/ldpc_encoder_test_output17.dat"}},
  {10, 1, 24, {"test_data/ldpc_encoder_test_input18.dat"}, {"test_data/ldpc_encoder_test_output18.dat"}},
  {10, 1, 26, {"test_data/ldpc_encoder_test_input19.dat"}, {"test_data/ldpc_encoder_test_output19.dat"}},
  {10, 1, 28, {"test_data/ldpc_encoder_test_input20.dat"}, {"test_data/ldpc_encoder_test_output20.dat"}},
  {10, 1, 30, {"test_data/ldpc_encoder_test_input21.dat"}, {"test_data/ldpc_encoder_test_output21.dat"}},
  {10, 1, 32, {"test_data/ldpc_encoder_test_input22.dat"}, {"test_data/ldpc_encoder_test_output22.dat"}},
  {10, 1, 36, {"test_data/ldpc_encoder_test_input23.dat"}, {"test_data/ldpc_encoder_test_output23.dat"}},
  {10, 1, 40, {"test_data/ldpc_encoder_test_input24.dat"}, {"test_data/ldpc_encoder_test_output24.dat"}},
  {10, 1, 44, {"test_data/ldpc_encoder_test_input25.dat"}, {"test_data/ldpc_encoder_test_output25.dat"}},
  {10, 1, 48, {"test_data/ldpc_encoder_test_input26.dat"}, {"test_data/ldpc_encoder_test_output26.dat"}},
  {10, 1, 52, {"test_data/ldpc_encoder_test_input27.dat"}, {"test_data/ldpc_encoder_test_output27.dat"}},
  {10, 1, 56, {"test_data/ldpc_encoder_test_input28.dat"}, {"test_data/ldpc_encoder_test_output28.dat"}},
  {10, 1, 60, {"test_data/ldpc_encoder_test_input29.dat"}, {"test_data/ldpc_encoder_test_output29.dat"}},
  {10, 1, 64, {"test_data/ldpc_encoder_test_input30.dat"}, {"test_data/ldpc_encoder_test_output30.dat"}},
  {10, 1, 72, {"test_data/ldpc_encoder_test_input31.dat"}, {"test_data/ldpc_encoder_test_output31.dat"}},
  {10, 1, 80, {"test_data/ldpc_encoder_test_input32.dat"}, {"test_data/ldpc_encoder_test_output32.dat"}},
  {10, 1, 88, {"test_data/ldpc_encoder_test_input33.dat"}, {"test_data/ldpc_encoder_test_output33.dat"}},
  {10, 1, 96, {"test_data/ldpc_encoder_test_input34.dat"}, {"test_data/ldpc_encoder_test_output34.dat"}},
  {10, 1, 104, {"test_data/ldpc_encoder_test_input35.dat"}, {"test_data/ldpc_encoder_test_output35.dat"}},
  {10, 1, 112, {"test_data/ldpc_encoder_test_input36.dat"}, {"test_data/ldpc_encoder_test_output36.dat"}},
  {10, 1, 120, {"test_data/ldpc_encoder_test_input37.dat"}, {"test_data/ldpc_encoder_test_output37.dat"}},
  {10, 1, 128, {"test_data/ldpc_encoder_test_input38.dat"}, {"test_data/ldpc_encoder_test_output38.dat"}},
  {10, 1, 144, {"test_data/ldpc_encoder_test_input39.dat"}, {"test_data/ldpc_encoder_test_output39.dat"}},
  {10, 1, 160, {"test_data/ldpc_encoder_test_input40.dat"}, {"test_data/ldpc_encoder_test_output40.dat"}},
  {10, 1, 176, {"test_data/ldpc_encoder_test_input41.dat"}, {"test_data/ldpc_encoder_test_output41.dat"}},
  {10, 1, 192, {"test_data/ldpc_encoder_test_input42.dat"}, {"test_data/ldpc_encoder_test_output42.dat"}},
  {10, 1, 208, {"test_data/ldpc_encoder_test_input43.dat"}, {"test_data/ldpc_encoder_test_output43.dat"}},
  {10, 1, 224, {"test_data/ldpc_encoder_test_input44.dat"}, {"test_data/ldpc_encoder_test_output44.dat"}},
  {10, 1, 240, {"test_data/ldpc_encoder_test_input45.dat"}, {"test_data/ldpc_encoder_test_output45.dat"}},
  {10, 1, 256, {"test_data/ldpc_encoder_test_input46.dat"}, {"test_data/ldpc_encoder_test_output46.dat"}},
  {10, 1, 288, {"test_data/ldpc_encoder_test_input47.dat"}, {"test_data/ldpc_encoder_test_output47.dat"}},
  {10, 1, 320, {"test_data/ldpc_encoder_test_input48.dat"}, {"test_data/ldpc_encoder_test_output48.dat"}},
  {10, 1, 352, {"test_data/ldpc_encoder_test_input49.dat"}, {"test_data/ldpc_encoder_test_output49.dat"}},
  {10, 1, 384, {"test_data/ldpc_encoder_test_input50.dat"}, {"test_data/ldpc_encoder_test_output50.dat"}},
  {10, 2, 2, {"test_data/ldpc_encoder_test_input51.dat"}, {"test_data/ldpc_encoder_test_output51.dat"}},
  {10, 2, 3, {"test_data/ldpc_encoder_test_input52.dat"}, {"test_data/ldpc_encoder_test_output52.dat"}},
  {10, 2, 4, {"test_data/ldpc_encoder_test_input53.dat"}, {"test_data/ldpc_encoder_test_output53.dat"}},
  {10, 2, 5, {"test_data/ldpc_encoder_test_input54.dat"}, {"test_data/ldpc_encoder_test_output54.dat"}},
  {10, 2, 6, {"test_data/ldpc_encoder_test_input55.dat"}, {"test_data/ldpc_encoder_test_output55.dat"}},
  {10, 2, 7, {"test_data/ldpc_encoder_test_input56.dat"}, {"test_data/ldpc_encoder_test_output56.dat"}},
  {10, 2, 8, {"test_data/ldpc_encoder_test_input57.dat"}, {"test_data/ldpc_encoder_test_output57.dat"}},
  {10, 2, 9, {"test_data/ldpc_encoder_test_input58.dat"}, {"test_data/ldpc_encoder_test_output58.dat"}},
  {10, 2, 10, {"test_data/ldpc_encoder_test_input59.dat"}, {"test_data/ldpc_encoder_test_output59.dat"}},
  {10, 2, 11, {"test_data/ldpc_encoder_test_input60.dat"}, {"test_data/ldpc_encoder_test_output60.dat"}},
  {10, 2, 12, {"test_data/ldpc_encoder_test_input61.dat"}, {"test_data/ldpc_encoder_test_output61.dat"}},
  {10, 2, 13, {"test_data/ldpc_encoder_test_input62.dat"}, {"test_data/ldpc_encoder_test_output62.dat"}},
  {10, 2, 14, {"test_data/ldpc_encoder_test_input63.dat"}, {"test_data/ldpc_encoder_test_output63.dat"}},
  {10, 2, 15, {"test_data/ldpc_encoder_test_input64.dat"}, {"test_data/ldpc_encoder_test_output64.dat"}},
  {10, 2, 16, {"test_data/ldpc_encoder_test_input65.dat"}, {"test_data/ldpc_encoder_test_output65.dat"}},
  {10, 2, 18, {"test_data/ldpc_encoder_test_input66.dat"}, {"test_data/ldpc_encoder_test_output66.dat"}},
  {10, 2, 20, {"test_data/ldpc_encoder_test_input67.dat"}, {"test_data/ldpc_encoder_test_output67.dat"}},
  {10, 2, 22, {"test_data/ldpc_encoder_test_input68.dat"}, {"test_data/ldpc_encoder_test_output68.dat"}},
  {10, 2, 24, {"test_data/ldpc_encoder_test_input69.dat"}, {"test_data/ldpc_encoder_test_output69.dat"}},
  {10, 2, 26, {"test_data/ldpc_encoder_test_input70.dat"}, {"test_data/ldpc_encoder_test_output70.dat"}},
  {10, 2, 28, {"test_data/ldpc_encoder_test_input71.dat"}, {"test_data/ldpc_encoder_test_output71.dat"}},
  {10, 2, 30, {"test_data/ldpc_encoder_test_input72.dat"}, {"test_data/ldpc_encoder_test_output72.dat"}},
  {10, 2, 32, {"test_data/ldpc_encoder_test_input73.dat"}, {"test_data/ldpc_encoder_test_output73.dat"}},
  {10, 2, 36, {"test_data/ldpc_encoder_test_input74.dat"}, {"test_data/ldpc_encoder_test_output74.dat"}},
  {10, 2, 40, {"test_data/ldpc_encoder_test_input75.dat"}, {"test_data/ldpc_encoder_test_output75.dat"}},
  {10, 2, 44, {"test_data/ldpc_encoder_test_input76.dat"}, {"test_data/ldpc_encoder_test_output76.dat"}},
  {10, 2, 48, {"test_data/ldpc_encoder_test_input77.dat"}, {"test_data/ldpc_encoder_test_output77.dat"}},
  {10, 2, 52, {"test_data/ldpc_encoder_test_input78.dat"}, {"test_data/ldpc_encoder_test_output78.dat"}},
  {10, 2, 56, {"test_data/ldpc_encoder_test_input79.dat"}, {"test_data/ldpc_encoder_test_output79.dat"}},
  {10, 2, 60, {"test_data/ldpc_encoder_test_input80.dat"}, {"test_data/ldpc_encoder_test_output80.dat"}},
  {10, 2, 64, {"test_data/ldpc_encoder_test_input81.dat"}, {"test_data/ldpc_encoder_test_output81.dat"}},
  {10, 2, 72, {"test_data/ldpc_encoder_test_input82.dat"}, {"test_data/ldpc_encoder_test_output82.dat"}},
  {10, 2, 80, {"test_data/ldpc_encoder_test_input83.dat"}, {"test_data/ldpc_encoder_test_output83.dat"}},
  {10, 2, 88, {"test_data/ldpc_encoder_test_input84.dat"}, {"test_data/ldpc_encoder_test_output84.dat"}},
  {10, 2, 96, {"test_data/ldpc_encoder_test_input85.dat"}, {"test_data/ldpc_encoder_test_output85.dat"}},
  {10, 2, 104, {"test_data/ldpc_encoder_test_input86.dat"}, {"test_data/ldpc_encoder_test_output86.dat"}},
  {10, 2, 112, {"test_data/ldpc_encoder_test_input87.dat"}, {"test_data/ldpc_encoder_test_output87.dat"}},
  {10, 2, 120, {"test_data/ldpc_encoder_test_input88.dat"}, {"test_data/ldpc_encoder_test_output88.dat"}},
  {10, 2, 128, {"test_data/ldpc_encoder_test_input89.dat"}, {"test_data/ldpc_encoder_test_output89.dat"}},
  {10, 2, 144, {"test_data/ldpc_encoder_test_input90.dat"}, {"test_data/ldpc_encoder_test_output90.dat"}},
  {10, 2, 160, {"test_data/ldpc_encoder_test_input91.dat"}, {"test_data/ldpc_encoder_test_output91.dat"}},
  {10, 2, 176, {"test_data/ldpc_encoder_test_input92.dat"}, {"test_data/ldpc_encoder_test_output92.dat"}},
  {10, 2, 192, {"test_data/ldpc_encoder_test_input93.dat"}, {"test_data/ldpc_encoder_test_output93.dat"}},
  {10, 2, 208, {"test_data/ldpc_encoder_test_input94.dat"}, {"test_data/ldpc_encoder_test_output94.dat"}},
  {10, 2, 224, {"test_data/ldpc_encoder_test_input95.dat"}, {"test_data/ldpc_encoder_test_output95.dat"}},
  {10, 2, 240, {"test_data/ldpc_encoder_test_input96.dat"}, {"test_data/ldpc_encoder_test_output96.dat"}},
  {10, 2, 256, {"test_data/ldpc_encoder_test_input97.dat"}, {"test_data/ldpc_encoder_test_output97.dat"}},
  {10, 2, 288, {"test_data/ldpc_encoder_test_input98.dat"}, {"test_data/ldpc_encoder_test_output98.dat"}},
  {10, 2, 320, {"test_data/ldpc_encoder_test_input99.dat"}, {"test_data/ldpc_encoder_test_output99.dat"}},
  {10, 2, 352, {"test_data/ldpc_encoder_test_input100.dat"}, {"test_data/ldpc_encoder_test_output100.dat"}},
  {10, 2, 384, {"test_data/ldpc_encoder_test_input101.dat"}, {"test_data/ldpc_encoder_test_output101.dat"}},
    // clang-format on
};

} // namespace srsran
