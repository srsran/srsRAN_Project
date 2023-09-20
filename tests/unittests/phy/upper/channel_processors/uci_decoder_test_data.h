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
//   + "srsUCIDecoderUnittest.m"

#include "srsran/phy/upper/channel_processors/uci_decoder.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/modulation_scheme.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned                          message_length = 0;
  unsigned                          llr_length     = 0;
  uci_decoder::configuration        config;
  file_vector<uint8_t>              message;
  file_vector<log_likelihood_ratio> llr;
};

static const std::vector<test_case_t> uci_decoder_test_data = {
    // clang-format off
  {1, 7421, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output0.dat"}, {"test_data/uci_decoder_test_input0.dat"}},
  {1, 5446, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output1.dat"}, {"test_data/uci_decoder_test_input1.dat"}},
  {1, 5484, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output2.dat"}, {"test_data/uci_decoder_test_input2.dat"}},
  {1, 7842, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output3.dat"}, {"test_data/uci_decoder_test_input3.dat"}},
  {1, 2688, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output4.dat"}, {"test_data/uci_decoder_test_input4.dat"}},
  {2, 1443, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output5.dat"}, {"test_data/uci_decoder_test_input5.dat"}},
  {2, 6390, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output6.dat"}, {"test_data/uci_decoder_test_input6.dat"}},
  {2, 2428, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output7.dat"}, {"test_data/uci_decoder_test_input7.dat"}},
  {2, 2460, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output8.dat"}, {"test_data/uci_decoder_test_input8.dat"}},
  {2, 7296, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output9.dat"}, {"test_data/uci_decoder_test_input9.dat"}},
  {3, 6241, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output10.dat"}, {"test_data/uci_decoder_test_input10.dat"}},
  {3, 3752, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output11.dat"}, {"test_data/uci_decoder_test_input11.dat"}},
  {3, 6892, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output12.dat"}, {"test_data/uci_decoder_test_input12.dat"}},
  {3, 5604, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output13.dat"}, {"test_data/uci_decoder_test_input13.dat"}},
  {3, 6216, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output14.dat"}, {"test_data/uci_decoder_test_input14.dat"}},
  {4, 6754, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output15.dat"}, {"test_data/uci_decoder_test_input15.dat"}},
  {4, 1326, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output16.dat"}, {"test_data/uci_decoder_test_input16.dat"}},
  {4, 7656, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output17.dat"}, {"test_data/uci_decoder_test_input17.dat"}},
  {4, 450, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output18.dat"}, {"test_data/uci_decoder_test_input18.dat"}},
  {4, 992, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output19.dat"}, {"test_data/uci_decoder_test_input19.dat"}},
  {5, 1241, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output20.dat"}, {"test_data/uci_decoder_test_input20.dat"}},
  {5, 898, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output21.dat"}, {"test_data/uci_decoder_test_input21.dat"}},
  {5, 3228, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output22.dat"}, {"test_data/uci_decoder_test_input22.dat"}},
  {5, 6336, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output23.dat"}, {"test_data/uci_decoder_test_input23.dat"}},
  {5, 1904, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output24.dat"}, {"test_data/uci_decoder_test_input24.dat"}},
  {6, 3559, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output25.dat"}, {"test_data/uci_decoder_test_input25.dat"}},
  {6, 3536, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output26.dat"}, {"test_data/uci_decoder_test_input26.dat"}},
  {6, 2524, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output27.dat"}, {"test_data/uci_decoder_test_input27.dat"}},
  {6, 6036, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output28.dat"}, {"test_data/uci_decoder_test_input28.dat"}},
  {6, 7248, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output29.dat"}, {"test_data/uci_decoder_test_input29.dat"}},
  {7, 6256, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output30.dat"}, {"test_data/uci_decoder_test_input30.dat"}},
  {7, 862, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output31.dat"}, {"test_data/uci_decoder_test_input31.dat"}},
  {7, 7756, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output32.dat"}, {"test_data/uci_decoder_test_input32.dat"}},
  {7, 3900, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output33.dat"}, {"test_data/uci_decoder_test_input33.dat"}},
  {7, 6536, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output34.dat"}, {"test_data/uci_decoder_test_input34.dat"}},
  {8, 1070, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output35.dat"}, {"test_data/uci_decoder_test_input35.dat"}},
  {8, 5900, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output36.dat"}, {"test_data/uci_decoder_test_input36.dat"}},
  {8, 6012, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output37.dat"}, {"test_data/uci_decoder_test_input37.dat"}},
  {8, 6570, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output38.dat"}, {"test_data/uci_decoder_test_input38.dat"}},
  {8, 4712, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output39.dat"}, {"test_data/uci_decoder_test_input39.dat"}},
  {9, 4926, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output40.dat"}, {"test_data/uci_decoder_test_input40.dat"}},
  {9, 4726, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output41.dat"}, {"test_data/uci_decoder_test_input41.dat"}},
  {9, 7244, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output42.dat"}, {"test_data/uci_decoder_test_input42.dat"}},
  {9, 5058, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output43.dat"}, {"test_data/uci_decoder_test_input43.dat"}},
  {9, 1336, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output44.dat"}, {"test_data/uci_decoder_test_input44.dat"}},
  {10, 2303, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output45.dat"}, {"test_data/uci_decoder_test_input45.dat"}},
  {10, 3526, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output46.dat"}, {"test_data/uci_decoder_test_input46.dat"}},
  {10, 4348, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output47.dat"}, {"test_data/uci_decoder_test_input47.dat"}},
  {10, 7332, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output48.dat"}, {"test_data/uci_decoder_test_input48.dat"}},
  {10, 5648, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output49.dat"}, {"test_data/uci_decoder_test_input49.dat"}},
  {11, 137, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output50.dat"}, {"test_data/uci_decoder_test_input50.dat"}},
  {11, 8064, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output51.dat"}, {"test_data/uci_decoder_test_input51.dat"}},
  {11, 6284, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output52.dat"}, {"test_data/uci_decoder_test_input52.dat"}},
  {11, 2652, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output53.dat"}, {"test_data/uci_decoder_test_input53.dat"}},
  {11, 4072, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output54.dat"}, {"test_data/uci_decoder_test_input54.dat"}},
  {12, 2899, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output55.dat"}, {"test_data/uci_decoder_test_input55.dat"}},
  {12, 5754, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output56.dat"}, {"test_data/uci_decoder_test_input56.dat"}},
  {12, 2312, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output57.dat"}, {"test_data/uci_decoder_test_input57.dat"}},
  {12, 186, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output58.dat"}, {"test_data/uci_decoder_test_input58.dat"}},
  {12, 4904, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output59.dat"}, {"test_data/uci_decoder_test_input59.dat"}},
  {19, 38, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output60.dat"}, {"test_data/uci_decoder_test_input60.dat"}},
  {19, 6024, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output61.dat"}, {"test_data/uci_decoder_test_input61.dat"}},
  {19, 3760, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output62.dat"}, {"test_data/uci_decoder_test_input62.dat"}},
  {19, 6504, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output63.dat"}, {"test_data/uci_decoder_test_input63.dat"}},
  {19, 656, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output64.dat"}, {"test_data/uci_decoder_test_input64.dat"}},
  {20, 8133, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output65.dat"}, {"test_data/uci_decoder_test_input65.dat"}},
  {20, 3820, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output66.dat"}, {"test_data/uci_decoder_test_input66.dat"}},
  {20, 3580, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output67.dat"}, {"test_data/uci_decoder_test_input67.dat"}},
  {20, 7722, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output68.dat"}, {"test_data/uci_decoder_test_input68.dat"}},
  {20, 1720, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output69.dat"}, {"test_data/uci_decoder_test_input69.dat"}},
  {200, 5389, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output70.dat"}, {"test_data/uci_decoder_test_input70.dat"}},
  {200, 7796, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output71.dat"}, {"test_data/uci_decoder_test_input71.dat"}},
  {200, 5808, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output72.dat"}, {"test_data/uci_decoder_test_input72.dat"}},
  {200, 7320, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output73.dat"}, {"test_data/uci_decoder_test_input73.dat"}},
  {200, 1032, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output74.dat"}, {"test_data/uci_decoder_test_input74.dat"}},
  {500, 910, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output75.dat"}, {"test_data/uci_decoder_test_input75.dat"}},
  {500, 3288, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output76.dat"}, {"test_data/uci_decoder_test_input76.dat"}},
  {500, 6368, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output77.dat"}, {"test_data/uci_decoder_test_input77.dat"}},
  {500, 3666, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output78.dat"}, {"test_data/uci_decoder_test_input78.dat"}},
  {500, 6136, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output79.dat"}, {"test_data/uci_decoder_test_input79.dat"}},
  {1000, 4837, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output80.dat"}, {"test_data/uci_decoder_test_input80.dat"}},
  {1000, 7314, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output81.dat"}, {"test_data/uci_decoder_test_input81.dat"}},
  {1000, 7948, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output82.dat"}, {"test_data/uci_decoder_test_input82.dat"}},
  {1000, 7080, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output83.dat"}, {"test_data/uci_decoder_test_input83.dat"}},
  {1000, 4896, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output84.dat"}, {"test_data/uci_decoder_test_input84.dat"}},
  {1706, 7166, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output85.dat"}, {"test_data/uci_decoder_test_input85.dat"}},
  {1706, 14596, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output86.dat"}, {"test_data/uci_decoder_test_input86.dat"}},
  {1706, 7860, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output87.dat"}, {"test_data/uci_decoder_test_input87.dat"}},
  {1706, 11028, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output88.dat"}, {"test_data/uci_decoder_test_input88.dat"}},
  {1706, 8768, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output89.dat"}, {"test_data/uci_decoder_test_input89.dat"}},
    // clang-format on
};

} // namespace srsran
