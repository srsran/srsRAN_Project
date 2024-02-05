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

// This file was generated using the following MATLAB class on 29-01-2024 (seed 0):
//   + "srsUCIDecoderUnittest.m"

#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"
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
  {1, 10, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output0.dat"}, {"test_data/uci_decoder_test_input0.dat"}},
  {1, 2, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output1.dat"}, {"test_data/uci_decoder_test_input1.dat"}},
  {1, 10, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output2.dat"}, {"test_data/uci_decoder_test_input2.dat"}},
  {1, 2, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output3.dat"}, {"test_data/uci_decoder_test_input3.dat"}},
  {1, 12, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output4.dat"}, {"test_data/uci_decoder_test_input4.dat"}},
  {1, 4, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output5.dat"}, {"test_data/uci_decoder_test_input5.dat"}},
  {1, 12, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output6.dat"}, {"test_data/uci_decoder_test_input6.dat"}},
  {1, 6, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output7.dat"}, {"test_data/uci_decoder_test_input7.dat"}},
  {1, 16, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output8.dat"}, {"test_data/uci_decoder_test_input8.dat"}},
  {1, 8, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output9.dat"}, {"test_data/uci_decoder_test_input9.dat"}},
  {2, 20, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output10.dat"}, {"test_data/uci_decoder_test_input10.dat"}},
  {2, 3, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output11.dat"}, {"test_data/uci_decoder_test_input11.dat"}},
  {2, 20, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output12.dat"}, {"test_data/uci_decoder_test_input12.dat"}},
  {2, 4, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output13.dat"}, {"test_data/uci_decoder_test_input13.dat"}},
  {2, 20, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output14.dat"}, {"test_data/uci_decoder_test_input14.dat"}},
  {2, 4, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output15.dat"}, {"test_data/uci_decoder_test_input15.dat"}},
  {2, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output16.dat"}, {"test_data/uci_decoder_test_input16.dat"}},
  {2, 6, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output17.dat"}, {"test_data/uci_decoder_test_input17.dat"}},
  {2, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output18.dat"}, {"test_data/uci_decoder_test_input18.dat"}},
  {2, 8, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output19.dat"}, {"test_data/uci_decoder_test_input19.dat"}},
  {3, 30, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output20.dat"}, {"test_data/uci_decoder_test_input20.dat"}},
  {3, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output21.dat"}, {"test_data/uci_decoder_test_input21.dat"}},
  {3, 30, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output22.dat"}, {"test_data/uci_decoder_test_input22.dat"}},
  {3, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output23.dat"}, {"test_data/uci_decoder_test_input23.dat"}},
  {3, 32, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output24.dat"}, {"test_data/uci_decoder_test_input24.dat"}},
  {3, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output25.dat"}, {"test_data/uci_decoder_test_input25.dat"}},
  {3, 30, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output26.dat"}, {"test_data/uci_decoder_test_input26.dat"}},
  {3, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output27.dat"}, {"test_data/uci_decoder_test_input27.dat"}},
  {3, 32, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output28.dat"}, {"test_data/uci_decoder_test_input28.dat"}},
  {3, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output29.dat"}, {"test_data/uci_decoder_test_input29.dat"}},
  {4, 40, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output30.dat"}, {"test_data/uci_decoder_test_input30.dat"}},
  {4, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output31.dat"}, {"test_data/uci_decoder_test_input31.dat"}},
  {4, 40, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output32.dat"}, {"test_data/uci_decoder_test_input32.dat"}},
  {4, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output33.dat"}, {"test_data/uci_decoder_test_input33.dat"}},
  {4, 40, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output34.dat"}, {"test_data/uci_decoder_test_input34.dat"}},
  {4, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output35.dat"}, {"test_data/uci_decoder_test_input35.dat"}},
  {4, 42, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output36.dat"}, {"test_data/uci_decoder_test_input36.dat"}},
  {4, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output37.dat"}, {"test_data/uci_decoder_test_input37.dat"}},
  {4, 40, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output38.dat"}, {"test_data/uci_decoder_test_input38.dat"}},
  {4, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output39.dat"}, {"test_data/uci_decoder_test_input39.dat"}},
  {5, 50, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output40.dat"}, {"test_data/uci_decoder_test_input40.dat"}},
  {5, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output41.dat"}, {"test_data/uci_decoder_test_input41.dat"}},
  {5, 50, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output42.dat"}, {"test_data/uci_decoder_test_input42.dat"}},
  {5, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output43.dat"}, {"test_data/uci_decoder_test_input43.dat"}},
  {5, 52, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output44.dat"}, {"test_data/uci_decoder_test_input44.dat"}},
  {5, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output45.dat"}, {"test_data/uci_decoder_test_input45.dat"}},
  {5, 54, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output46.dat"}, {"test_data/uci_decoder_test_input46.dat"}},
  {5, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output47.dat"}, {"test_data/uci_decoder_test_input47.dat"}},
  {5, 56, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output48.dat"}, {"test_data/uci_decoder_test_input48.dat"}},
  {5, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output49.dat"}, {"test_data/uci_decoder_test_input49.dat"}},
  {6, 60, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output50.dat"}, {"test_data/uci_decoder_test_input50.dat"}},
  {6, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output51.dat"}, {"test_data/uci_decoder_test_input51.dat"}},
  {6, 60, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output52.dat"}, {"test_data/uci_decoder_test_input52.dat"}},
  {6, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output53.dat"}, {"test_data/uci_decoder_test_input53.dat"}},
  {6, 60, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output54.dat"}, {"test_data/uci_decoder_test_input54.dat"}},
  {6, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output55.dat"}, {"test_data/uci_decoder_test_input55.dat"}},
  {6, 60, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output56.dat"}, {"test_data/uci_decoder_test_input56.dat"}},
  {6, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output57.dat"}, {"test_data/uci_decoder_test_input57.dat"}},
  {6, 64, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output58.dat"}, {"test_data/uci_decoder_test_input58.dat"}},
  {6, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output59.dat"}, {"test_data/uci_decoder_test_input59.dat"}},
  {7, 70, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output60.dat"}, {"test_data/uci_decoder_test_input60.dat"}},
  {7, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output61.dat"}, {"test_data/uci_decoder_test_input61.dat"}},
  {7, 70, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output62.dat"}, {"test_data/uci_decoder_test_input62.dat"}},
  {7, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output63.dat"}, {"test_data/uci_decoder_test_input63.dat"}},
  {7, 72, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output64.dat"}, {"test_data/uci_decoder_test_input64.dat"}},
  {7, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output65.dat"}, {"test_data/uci_decoder_test_input65.dat"}},
  {7, 72, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output66.dat"}, {"test_data/uci_decoder_test_input66.dat"}},
  {7, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output67.dat"}, {"test_data/uci_decoder_test_input67.dat"}},
  {7, 72, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output68.dat"}, {"test_data/uci_decoder_test_input68.dat"}},
  {7, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output69.dat"}, {"test_data/uci_decoder_test_input69.dat"}},
  {8, 80, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output70.dat"}, {"test_data/uci_decoder_test_input70.dat"}},
  {8, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output71.dat"}, {"test_data/uci_decoder_test_input71.dat"}},
  {8, 80, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output72.dat"}, {"test_data/uci_decoder_test_input72.dat"}},
  {8, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output73.dat"}, {"test_data/uci_decoder_test_input73.dat"}},
  {8, 80, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output74.dat"}, {"test_data/uci_decoder_test_input74.dat"}},
  {8, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output75.dat"}, {"test_data/uci_decoder_test_input75.dat"}},
  {8, 84, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output76.dat"}, {"test_data/uci_decoder_test_input76.dat"}},
  {8, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output77.dat"}, {"test_data/uci_decoder_test_input77.dat"}},
  {8, 80, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output78.dat"}, {"test_data/uci_decoder_test_input78.dat"}},
  {8, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output79.dat"}, {"test_data/uci_decoder_test_input79.dat"}},
  {9, 90, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output80.dat"}, {"test_data/uci_decoder_test_input80.dat"}},
  {9, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output81.dat"}, {"test_data/uci_decoder_test_input81.dat"}},
  {9, 90, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output82.dat"}, {"test_data/uci_decoder_test_input82.dat"}},
  {9, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output83.dat"}, {"test_data/uci_decoder_test_input83.dat"}},
  {9, 92, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output84.dat"}, {"test_data/uci_decoder_test_input84.dat"}},
  {9, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output85.dat"}, {"test_data/uci_decoder_test_input85.dat"}},
  {9, 90, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output86.dat"}, {"test_data/uci_decoder_test_input86.dat"}},
  {9, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output87.dat"}, {"test_data/uci_decoder_test_input87.dat"}},
  {9, 96, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output88.dat"}, {"test_data/uci_decoder_test_input88.dat"}},
  {9, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output89.dat"}, {"test_data/uci_decoder_test_input89.dat"}},
  {10, 100, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output90.dat"}, {"test_data/uci_decoder_test_input90.dat"}},
  {10, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output91.dat"}, {"test_data/uci_decoder_test_input91.dat"}},
  {10, 100, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output92.dat"}, {"test_data/uci_decoder_test_input92.dat"}},
  {10, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output93.dat"}, {"test_data/uci_decoder_test_input93.dat"}},
  {10, 100, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output94.dat"}, {"test_data/uci_decoder_test_input94.dat"}},
  {10, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output95.dat"}, {"test_data/uci_decoder_test_input95.dat"}},
  {10, 102, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output96.dat"}, {"test_data/uci_decoder_test_input96.dat"}},
  {10, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output97.dat"}, {"test_data/uci_decoder_test_input97.dat"}},
  {10, 104, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output98.dat"}, {"test_data/uci_decoder_test_input98.dat"}},
  {10, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output99.dat"}, {"test_data/uci_decoder_test_input99.dat"}},
  {11, 110, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output100.dat"}, {"test_data/uci_decoder_test_input100.dat"}},
  {11, 24, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output101.dat"}, {"test_data/uci_decoder_test_input101.dat"}},
  {11, 110, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output102.dat"}, {"test_data/uci_decoder_test_input102.dat"}},
  {11, 24, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output103.dat"}, {"test_data/uci_decoder_test_input103.dat"}},
  {11, 112, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output104.dat"}, {"test_data/uci_decoder_test_input104.dat"}},
  {11, 24, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output105.dat"}, {"test_data/uci_decoder_test_input105.dat"}},
  {11, 114, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output106.dat"}, {"test_data/uci_decoder_test_input106.dat"}},
  {11, 24, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output107.dat"}, {"test_data/uci_decoder_test_input107.dat"}},
  {11, 112, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output108.dat"}, {"test_data/uci_decoder_test_input108.dat"}},
  {11, 24, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output109.dat"}, {"test_data/uci_decoder_test_input109.dat"}},
  {12, 180, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output110.dat"}, {"test_data/uci_decoder_test_input110.dat"}},
  {12, 27, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output111.dat"}, {"test_data/uci_decoder_test_input111.dat"}},
  {12, 180, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output112.dat"}, {"test_data/uci_decoder_test_input112.dat"}},
  {12, 28, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output113.dat"}, {"test_data/uci_decoder_test_input113.dat"}},
  {12, 180, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output114.dat"}, {"test_data/uci_decoder_test_input114.dat"}},
  {12, 28, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output115.dat"}, {"test_data/uci_decoder_test_input115.dat"}},
  {12, 180, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output116.dat"}, {"test_data/uci_decoder_test_input116.dat"}},
  {12, 30, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output117.dat"}, {"test_data/uci_decoder_test_input117.dat"}},
  {12, 184, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output118.dat"}, {"test_data/uci_decoder_test_input118.dat"}},
  {12, 32, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output119.dat"}, {"test_data/uci_decoder_test_input119.dat"}},
  {19, 250, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output120.dat"}, {"test_data/uci_decoder_test_input120.dat"}},
  {19, 38, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output121.dat"}, {"test_data/uci_decoder_test_input121.dat"}},
  {19, 250, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output122.dat"}, {"test_data/uci_decoder_test_input122.dat"}},
  {19, 38, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output123.dat"}, {"test_data/uci_decoder_test_input123.dat"}},
  {19, 252, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output124.dat"}, {"test_data/uci_decoder_test_input124.dat"}},
  {19, 40, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output125.dat"}, {"test_data/uci_decoder_test_input125.dat"}},
  {19, 252, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output126.dat"}, {"test_data/uci_decoder_test_input126.dat"}},
  {19, 42, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output127.dat"}, {"test_data/uci_decoder_test_input127.dat"}},
  {19, 256, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output128.dat"}, {"test_data/uci_decoder_test_input128.dat"}},
  {19, 40, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output129.dat"}, {"test_data/uci_decoder_test_input129.dat"}},
  {20, 310, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output130.dat"}, {"test_data/uci_decoder_test_input130.dat"}},
  {20, 47, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output131.dat"}, {"test_data/uci_decoder_test_input131.dat"}},
  {20, 310, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output132.dat"}, {"test_data/uci_decoder_test_input132.dat"}},
  {20, 48, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output133.dat"}, {"test_data/uci_decoder_test_input133.dat"}},
  {20, 312, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output134.dat"}, {"test_data/uci_decoder_test_input134.dat"}},
  {20, 48, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output135.dat"}, {"test_data/uci_decoder_test_input135.dat"}},
  {20, 312, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output136.dat"}, {"test_data/uci_decoder_test_input136.dat"}},
  {20, 48, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output137.dat"}, {"test_data/uci_decoder_test_input137.dat"}},
  {20, 312, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output138.dat"}, {"test_data/uci_decoder_test_input138.dat"}},
  {20, 48, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output139.dat"}, {"test_data/uci_decoder_test_input139.dat"}},
  {200, 2110, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output140.dat"}, {"test_data/uci_decoder_test_input140.dat"}},
  {200, 317, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output141.dat"}, {"test_data/uci_decoder_test_input141.dat"}},
  {200, 2110, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output142.dat"}, {"test_data/uci_decoder_test_input142.dat"}},
  {200, 318, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output143.dat"}, {"test_data/uci_decoder_test_input143.dat"}},
  {200, 2112, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output144.dat"}, {"test_data/uci_decoder_test_input144.dat"}},
  {200, 320, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output145.dat"}, {"test_data/uci_decoder_test_input145.dat"}},
  {200, 2112, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output146.dat"}, {"test_data/uci_decoder_test_input146.dat"}},
  {200, 318, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output147.dat"}, {"test_data/uci_decoder_test_input147.dat"}},
  {200, 2112, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output148.dat"}, {"test_data/uci_decoder_test_input148.dat"}},
  {200, 320, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output149.dat"}, {"test_data/uci_decoder_test_input149.dat"}},
  {1706, 16384, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output150.dat"}, {"test_data/uci_decoder_test_input150.dat"}},
  {1706, 2592, {modulation_scheme::PI_2_BPSK}, {"test_data/uci_decoder_test_output151.dat"}, {"test_data/uci_decoder_test_input151.dat"}},
  {1706, 16384, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output152.dat"}, {"test_data/uci_decoder_test_input152.dat"}},
  {1706, 2592, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output153.dat"}, {"test_data/uci_decoder_test_input153.dat"}},
  {1706, 16384, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output154.dat"}, {"test_data/uci_decoder_test_input154.dat"}},
  {1706, 2592, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output155.dat"}, {"test_data/uci_decoder_test_input155.dat"}},
  {1706, 16380, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output156.dat"}, {"test_data/uci_decoder_test_input156.dat"}},
  {1706, 2592, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output157.dat"}, {"test_data/uci_decoder_test_input157.dat"}},
  {1706, 16384, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output158.dat"}, {"test_data/uci_decoder_test_input158.dat"}},
  {1706, 2592, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output159.dat"}, {"test_data/uci_decoder_test_input159.dat"}},
    // clang-format on
};

} // namespace srsran
