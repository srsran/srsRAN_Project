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

// This file was generated using the following MATLAB class on 04-08-2023:
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
  {1, 2285, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output0.dat"}, {"test_data/uci_decoder_test_input0.dat"}},
  {1, 3736, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output1.dat"}, {"test_data/uci_decoder_test_input1.dat"}},
  {1, 5600, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output2.dat"}, {"test_data/uci_decoder_test_input2.dat"}},
  {1, 6858, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output3.dat"}, {"test_data/uci_decoder_test_input3.dat"}},
  {1, 2512, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output4.dat"}, {"test_data/uci_decoder_test_input4.dat"}},
  {2, 5001, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output5.dat"}, {"test_data/uci_decoder_test_input5.dat"}},
  {2, 4218, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output6.dat"}, {"test_data/uci_decoder_test_input6.dat"}},
  {2, 3172, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output7.dat"}, {"test_data/uci_decoder_test_input7.dat"}},
  {2, 4740, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output8.dat"}, {"test_data/uci_decoder_test_input8.dat"}},
  {2, 360, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output9.dat"}, {"test_data/uci_decoder_test_input9.dat"}},
  {3, 6590, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output10.dat"}, {"test_data/uci_decoder_test_input10.dat"}},
  {3, 3478, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output11.dat"}, {"test_data/uci_decoder_test_input11.dat"}},
  {3, 8124, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output12.dat"}, {"test_data/uci_decoder_test_input12.dat"}},
  {3, 7206, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output13.dat"}, {"test_data/uci_decoder_test_input13.dat"}},
  {3, 1536, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output14.dat"}, {"test_data/uci_decoder_test_input14.dat"}},
  {4, 7674, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output15.dat"}, {"test_data/uci_decoder_test_input15.dat"}},
  {4, 6108, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output16.dat"}, {"test_data/uci_decoder_test_input16.dat"}},
  {4, 1568, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output17.dat"}, {"test_data/uci_decoder_test_input17.dat"}},
  {4, 1296, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output18.dat"}, {"test_data/uci_decoder_test_input18.dat"}},
  {4, 8104, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output19.dat"}, {"test_data/uci_decoder_test_input19.dat"}},
  {5, 4142, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output20.dat"}, {"test_data/uci_decoder_test_input20.dat"}},
  {5, 7704, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output21.dat"}, {"test_data/uci_decoder_test_input21.dat"}},
  {5, 4468, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output22.dat"}, {"test_data/uci_decoder_test_input22.dat"}},
  {5, 4998, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output23.dat"}, {"test_data/uci_decoder_test_input23.dat"}},
  {5, 4168, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output24.dat"}, {"test_data/uci_decoder_test_input24.dat"}},
  {6, 4968, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output25.dat"}, {"test_data/uci_decoder_test_input25.dat"}},
  {6, 2482, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output26.dat"}, {"test_data/uci_decoder_test_input26.dat"}},
  {6, 4488, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output27.dat"}, {"test_data/uci_decoder_test_input27.dat"}},
  {6, 1878, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output28.dat"}, {"test_data/uci_decoder_test_input28.dat"}},
  {6, 6536, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output29.dat"}, {"test_data/uci_decoder_test_input29.dat"}},
  {7, 216, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output30.dat"}, {"test_data/uci_decoder_test_input30.dat"}},
  {7, 6726, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output31.dat"}, {"test_data/uci_decoder_test_input31.dat"}},
  {7, 3460, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output32.dat"}, {"test_data/uci_decoder_test_input32.dat"}},
  {7, 3138, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output33.dat"}, {"test_data/uci_decoder_test_input33.dat"}},
  {7, 1568, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output34.dat"}, {"test_data/uci_decoder_test_input34.dat"}},
  {8, 5088, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output35.dat"}, {"test_data/uci_decoder_test_input35.dat"}},
  {8, 1906, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output36.dat"}, {"test_data/uci_decoder_test_input36.dat"}},
  {8, 4960, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output37.dat"}, {"test_data/uci_decoder_test_input37.dat"}},
  {8, 4962, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output38.dat"}, {"test_data/uci_decoder_test_input38.dat"}},
  {8, 1664, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output39.dat"}, {"test_data/uci_decoder_test_input39.dat"}},
  {9, 1827, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output40.dat"}, {"test_data/uci_decoder_test_input40.dat"}},
  {9, 3142, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output41.dat"}, {"test_data/uci_decoder_test_input41.dat"}},
  {9, 8172, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output42.dat"}, {"test_data/uci_decoder_test_input42.dat"}},
  {9, 3498, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output43.dat"}, {"test_data/uci_decoder_test_input43.dat"}},
  {9, 3520, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output44.dat"}, {"test_data/uci_decoder_test_input44.dat"}},
  {10, 2890, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output45.dat"}, {"test_data/uci_decoder_test_input45.dat"}},
  {10, 6332, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output46.dat"}, {"test_data/uci_decoder_test_input46.dat"}},
  {10, 6928, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output47.dat"}, {"test_data/uci_decoder_test_input47.dat"}},
  {10, 2448, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output48.dat"}, {"test_data/uci_decoder_test_input48.dat"}},
  {10, 160, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output49.dat"}, {"test_data/uci_decoder_test_input49.dat"}},
  {11, 6863, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output50.dat"}, {"test_data/uci_decoder_test_input50.dat"}},
  {11, 2666, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output51.dat"}, {"test_data/uci_decoder_test_input51.dat"}},
  {11, 3324, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output52.dat"}, {"test_data/uci_decoder_test_input52.dat"}},
  {11, 138, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output53.dat"}, {"test_data/uci_decoder_test_input53.dat"}},
  {11, 7936, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output54.dat"}, {"test_data/uci_decoder_test_input54.dat"}},
  {12, 506, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output55.dat"}, {"test_data/uci_decoder_test_input55.dat"}},
  {12, 4454, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output56.dat"}, {"test_data/uci_decoder_test_input56.dat"}},
  {12, 408, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output57.dat"}, {"test_data/uci_decoder_test_input57.dat"}},
  {12, 7518, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output58.dat"}, {"test_data/uci_decoder_test_input58.dat"}},
  {12, 4032, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output59.dat"}, {"test_data/uci_decoder_test_input59.dat"}},
  {19, 6955, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output60.dat"}, {"test_data/uci_decoder_test_input60.dat"}},
  {19, 3972, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output61.dat"}, {"test_data/uci_decoder_test_input61.dat"}},
  {19, 5472, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output62.dat"}, {"test_data/uci_decoder_test_input62.dat"}},
  {19, 2862, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output63.dat"}, {"test_data/uci_decoder_test_input63.dat"}},
  {19, 232, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output64.dat"}, {"test_data/uci_decoder_test_input64.dat"}},
  {20, 7589, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output65.dat"}, {"test_data/uci_decoder_test_input65.dat"}},
  {20, 6976, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output66.dat"}, {"test_data/uci_decoder_test_input66.dat"}},
  {20, 6468, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output67.dat"}, {"test_data/uci_decoder_test_input67.dat"}},
  {20, 4290, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output68.dat"}, {"test_data/uci_decoder_test_input68.dat"}},
  {20, 2816, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output69.dat"}, {"test_data/uci_decoder_test_input69.dat"}},
  {200, 7076, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output70.dat"}, {"test_data/uci_decoder_test_input70.dat"}},
  {200, 7306, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output71.dat"}, {"test_data/uci_decoder_test_input71.dat"}},
  {200, 7604, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output72.dat"}, {"test_data/uci_decoder_test_input72.dat"}},
  {200, 4854, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output73.dat"}, {"test_data/uci_decoder_test_input73.dat"}},
  {200, 4096, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output74.dat"}, {"test_data/uci_decoder_test_input74.dat"}},
  {500, 4198, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output75.dat"}, {"test_data/uci_decoder_test_input75.dat"}},
  {500, 7562, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output76.dat"}, {"test_data/uci_decoder_test_input76.dat"}},
  {500, 5420, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output77.dat"}, {"test_data/uci_decoder_test_input77.dat"}},
  {500, 6642, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output78.dat"}, {"test_data/uci_decoder_test_input78.dat"}},
  {500, 6328, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output79.dat"}, {"test_data/uci_decoder_test_input79.dat"}},
  {1000, 7846, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output80.dat"}, {"test_data/uci_decoder_test_input80.dat"}},
  {1000, 6292, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output81.dat"}, {"test_data/uci_decoder_test_input81.dat"}},
  {1000, 3632, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output82.dat"}, {"test_data/uci_decoder_test_input82.dat"}},
  {1000, 1602, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output83.dat"}, {"test_data/uci_decoder_test_input83.dat"}},
  {1000, 6952, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output84.dat"}, {"test_data/uci_decoder_test_input84.dat"}},
  {1706, 13399, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output85.dat"}, {"test_data/uci_decoder_test_input85.dat"}},
  {1706, 2848, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output86.dat"}, {"test_data/uci_decoder_test_input86.dat"}},
  {1706, 10220, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output87.dat"}, {"test_data/uci_decoder_test_input87.dat"}},
  {1706, 4662, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output88.dat"}, {"test_data/uci_decoder_test_input88.dat"}},
  {1706, 4616, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output89.dat"}, {"test_data/uci_decoder_test_input89.dat"}},
    // clang-format on
};

} // namespace srsran
