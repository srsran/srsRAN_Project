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
//   + "srsPDSCHEncoderUnittest.m"

#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  segmenter_config     config;
  file_vector<uint8_t> transport_block;
  file_vector<uint8_t> encoded;
};

static const std::vector<test_case_t> pdsch_encoder_test_data = {
    // clang-format off
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input0.dat"}, {"test_data/pdsch_encoder_test_output0.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input1.dat"}, {"test_data/pdsch_encoder_test_output1.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input2.dat"}, {"test_data/pdsch_encoder_test_output2.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input3.dat"}, {"test_data/pdsch_encoder_test_output3.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QPSK, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input4.dat"}, {"test_data/pdsch_encoder_test_output4.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input5.dat"}, {"test_data/pdsch_encoder_test_output5.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input6.dat"}, {"test_data/pdsch_encoder_test_output6.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input7.dat"}, {"test_data/pdsch_encoder_test_output7.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input8.dat"}, {"test_data/pdsch_encoder_test_output8.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input9.dat"}, {"test_data/pdsch_encoder_test_output9.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input10.dat"}, {"test_data/pdsch_encoder_test_output10.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input11.dat"}, {"test_data/pdsch_encoder_test_output11.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input12.dat"}, {"test_data/pdsch_encoder_test_output12.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input13.dat"}, {"test_data/pdsch_encoder_test_output13.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input14.dat"}, {"test_data/pdsch_encoder_test_output14.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input15.dat"}, {"test_data/pdsch_encoder_test_output15.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input16.dat"}, {"test_data/pdsch_encoder_test_output16.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input17.dat"}, {"test_data/pdsch_encoder_test_output17.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input18.dat"}, {"test_data/pdsch_encoder_test_output18.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input19.dat"}, {"test_data/pdsch_encoder_test_output19.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input20.dat"}, {"test_data/pdsch_encoder_test_output20.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input21.dat"}, {"test_data/pdsch_encoder_test_output21.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input22.dat"}, {"test_data/pdsch_encoder_test_output22.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input23.dat"}, {"test_data/pdsch_encoder_test_output23.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input24.dat"}, {"test_data/pdsch_encoder_test_output24.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input25.dat"}, {"test_data/pdsch_encoder_test_output25.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input26.dat"}, {"test_data/pdsch_encoder_test_output26.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3900}, {"test_data/pdsch_encoder_test_input27.dat"}, {"test_data/pdsch_encoder_test_output27.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 780}, {"test_data/pdsch_encoder_test_input28.dat"}, {"test_data/pdsch_encoder_test_output28.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1872}, {"test_data/pdsch_encoder_test_input29.dat"}, {"test_data/pdsch_encoder_test_output29.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 2496}, {"test_data/pdsch_encoder_test_input30.dat"}, {"test_data/pdsch_encoder_test_output30.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3432}, {"test_data/pdsch_encoder_test_input31.dat"}, {"test_data/pdsch_encoder_test_output31.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 2340}, {"test_data/pdsch_encoder_test_input32.dat"}, {"test_data/pdsch_encoder_test_output32.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 1716}, {"test_data/pdsch_encoder_test_input33.dat"}, {"test_data/pdsch_encoder_test_output33.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 2184}, {"test_data/pdsch_encoder_test_input34.dat"}, {"test_data/pdsch_encoder_test_output34.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 1716}, {"test_data/pdsch_encoder_test_input35.dat"}, {"test_data/pdsch_encoder_test_output35.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2184}, {"test_data/pdsch_encoder_test_input36.dat"}, {"test_data/pdsch_encoder_test_output36.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2184}, {"test_data/pdsch_encoder_test_input37.dat"}, {"test_data/pdsch_encoder_test_output37.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2184}, {"test_data/pdsch_encoder_test_input38.dat"}, {"test_data/pdsch_encoder_test_output38.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3120}, {"test_data/pdsch_encoder_test_input39.dat"}, {"test_data/pdsch_encoder_test_output39.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2340}, {"test_data/pdsch_encoder_test_input40.dat"}, {"test_data/pdsch_encoder_test_output40.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2808}, {"test_data/pdsch_encoder_test_input41.dat"}, {"test_data/pdsch_encoder_test_output41.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM64, 0, 1, 468}, {"test_data/pdsch_encoder_test_input42.dat"}, {"test_data/pdsch_encoder_test_output42.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1404}, {"test_data/pdsch_encoder_test_input43.dat"}, {"test_data/pdsch_encoder_test_output43.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1248}, {"test_data/pdsch_encoder_test_input44.dat"}, {"test_data/pdsch_encoder_test_output44.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2808}, {"test_data/pdsch_encoder_test_input45.dat"}, {"test_data/pdsch_encoder_test_output45.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1872}, {"test_data/pdsch_encoder_test_input46.dat"}, {"test_data/pdsch_encoder_test_output46.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2808}, {"test_data/pdsch_encoder_test_input47.dat"}, {"test_data/pdsch_encoder_test_output47.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2028}, {"test_data/pdsch_encoder_test_input48.dat"}, {"test_data/pdsch_encoder_test_output48.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2184}, {"test_data/pdsch_encoder_test_input49.dat"}, {"test_data/pdsch_encoder_test_output49.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1560}, {"test_data/pdsch_encoder_test_input50.dat"}, {"test_data/pdsch_encoder_test_output50.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 780}, {"test_data/pdsch_encoder_test_input51.dat"}, {"test_data/pdsch_encoder_test_output51.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2340}, {"test_data/pdsch_encoder_test_input52.dat"}, {"test_data/pdsch_encoder_test_output52.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2340}, {"test_data/pdsch_encoder_test_input53.dat"}, {"test_data/pdsch_encoder_test_output53.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1872}, {"test_data/pdsch_encoder_test_input54.dat"}, {"test_data/pdsch_encoder_test_output54.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2808}, {"test_data/pdsch_encoder_test_input55.dat"}, {"test_data/pdsch_encoder_test_output55.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input56.dat"}, {"test_data/pdsch_encoder_test_output56.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input57.dat"}, {"test_data/pdsch_encoder_test_output57.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input58.dat"}, {"test_data/pdsch_encoder_test_output58.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input59.dat"}, {"test_data/pdsch_encoder_test_output59.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QPSK, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input60.dat"}, {"test_data/pdsch_encoder_test_output60.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input61.dat"}, {"test_data/pdsch_encoder_test_output61.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input62.dat"}, {"test_data/pdsch_encoder_test_output62.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input63.dat"}, {"test_data/pdsch_encoder_test_output63.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input64.dat"}, {"test_data/pdsch_encoder_test_output64.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input65.dat"}, {"test_data/pdsch_encoder_test_output65.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input66.dat"}, {"test_data/pdsch_encoder_test_output66.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input67.dat"}, {"test_data/pdsch_encoder_test_output67.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input68.dat"}, {"test_data/pdsch_encoder_test_output68.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input69.dat"}, {"test_data/pdsch_encoder_test_output69.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input70.dat"}, {"test_data/pdsch_encoder_test_output70.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input71.dat"}, {"test_data/pdsch_encoder_test_output71.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input72.dat"}, {"test_data/pdsch_encoder_test_output72.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input73.dat"}, {"test_data/pdsch_encoder_test_output73.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input74.dat"}, {"test_data/pdsch_encoder_test_output74.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input75.dat"}, {"test_data/pdsch_encoder_test_output75.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input76.dat"}, {"test_data/pdsch_encoder_test_output76.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input77.dat"}, {"test_data/pdsch_encoder_test_output77.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input78.dat"}, {"test_data/pdsch_encoder_test_output78.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input79.dat"}, {"test_data/pdsch_encoder_test_output79.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input80.dat"}, {"test_data/pdsch_encoder_test_output80.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input81.dat"}, {"test_data/pdsch_encoder_test_output81.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input82.dat"}, {"test_data/pdsch_encoder_test_output82.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3600}, {"test_data/pdsch_encoder_test_input83.dat"}, {"test_data/pdsch_encoder_test_output83.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1296}, {"test_data/pdsch_encoder_test_input84.dat"}, {"test_data/pdsch_encoder_test_output84.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1872}, {"test_data/pdsch_encoder_test_input85.dat"}, {"test_data/pdsch_encoder_test_output85.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 2304}, {"test_data/pdsch_encoder_test_input86.dat"}, {"test_data/pdsch_encoder_test_output86.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 576}, {"test_data/pdsch_encoder_test_input87.dat"}, {"test_data/pdsch_encoder_test_output87.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1440}, {"test_data/pdsch_encoder_test_input88.dat"}, {"test_data/pdsch_encoder_test_output88.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 1584}, {"test_data/pdsch_encoder_test_input89.dat"}, {"test_data/pdsch_encoder_test_output89.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 2160}, {"test_data/pdsch_encoder_test_input90.dat"}, {"test_data/pdsch_encoder_test_output90.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3312}, {"test_data/pdsch_encoder_test_input91.dat"}, {"test_data/pdsch_encoder_test_output91.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2016}, {"test_data/pdsch_encoder_test_input92.dat"}, {"test_data/pdsch_encoder_test_output92.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2304}, {"test_data/pdsch_encoder_test_input93.dat"}, {"test_data/pdsch_encoder_test_output93.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2160}, {"test_data/pdsch_encoder_test_input94.dat"}, {"test_data/pdsch_encoder_test_output94.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1728}, {"test_data/pdsch_encoder_test_input95.dat"}, {"test_data/pdsch_encoder_test_output95.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2592}, {"test_data/pdsch_encoder_test_input96.dat"}, {"test_data/pdsch_encoder_test_output96.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2592}, {"test_data/pdsch_encoder_test_input97.dat"}, {"test_data/pdsch_encoder_test_output97.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1440}, {"test_data/pdsch_encoder_test_input98.dat"}, {"test_data/pdsch_encoder_test_output98.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1440}, {"test_data/pdsch_encoder_test_input99.dat"}, {"test_data/pdsch_encoder_test_output99.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2016}, {"test_data/pdsch_encoder_test_input100.dat"}, {"test_data/pdsch_encoder_test_output100.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1728}, {"test_data/pdsch_encoder_test_input101.dat"}, {"test_data/pdsch_encoder_test_output101.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1728}, {"test_data/pdsch_encoder_test_input102.dat"}, {"test_data/pdsch_encoder_test_output102.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2592}, {"test_data/pdsch_encoder_test_input103.dat"}, {"test_data/pdsch_encoder_test_output103.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3168}, {"test_data/pdsch_encoder_test_input104.dat"}, {"test_data/pdsch_encoder_test_output104.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 576}, {"test_data/pdsch_encoder_test_input105.dat"}, {"test_data/pdsch_encoder_test_output105.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2160}, {"test_data/pdsch_encoder_test_input106.dat"}, {"test_data/pdsch_encoder_test_output106.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1296}, {"test_data/pdsch_encoder_test_input107.dat"}, {"test_data/pdsch_encoder_test_output107.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1296}, {"test_data/pdsch_encoder_test_input108.dat"}, {"test_data/pdsch_encoder_test_output108.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1872}, {"test_data/pdsch_encoder_test_input109.dat"}, {"test_data/pdsch_encoder_test_output109.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2592}, {"test_data/pdsch_encoder_test_input110.dat"}, {"test_data/pdsch_encoder_test_output110.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 720}, {"test_data/pdsch_encoder_test_input111.dat"}, {"test_data/pdsch_encoder_test_output111.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input112.dat"}, {"test_data/pdsch_encoder_test_output112.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input113.dat"}, {"test_data/pdsch_encoder_test_output113.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input114.dat"}, {"test_data/pdsch_encoder_test_output114.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input115.dat"}, {"test_data/pdsch_encoder_test_output115.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input116.dat"}, {"test_data/pdsch_encoder_test_output116.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input117.dat"}, {"test_data/pdsch_encoder_test_output117.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input118.dat"}, {"test_data/pdsch_encoder_test_output118.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input119.dat"}, {"test_data/pdsch_encoder_test_output119.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input120.dat"}, {"test_data/pdsch_encoder_test_output120.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input121.dat"}, {"test_data/pdsch_encoder_test_output121.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input122.dat"}, {"test_data/pdsch_encoder_test_output122.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input123.dat"}, {"test_data/pdsch_encoder_test_output123.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input124.dat"}, {"test_data/pdsch_encoder_test_output124.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input125.dat"}, {"test_data/pdsch_encoder_test_output125.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input126.dat"}, {"test_data/pdsch_encoder_test_output126.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input127.dat"}, {"test_data/pdsch_encoder_test_output127.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input128.dat"}, {"test_data/pdsch_encoder_test_output128.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input129.dat"}, {"test_data/pdsch_encoder_test_output129.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input130.dat"}, {"test_data/pdsch_encoder_test_output130.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input131.dat"}, {"test_data/pdsch_encoder_test_output131.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input132.dat"}, {"test_data/pdsch_encoder_test_output132.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input133.dat"}, {"test_data/pdsch_encoder_test_output133.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input134.dat"}, {"test_data/pdsch_encoder_test_output134.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input135.dat"}, {"test_data/pdsch_encoder_test_output135.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input136.dat"}, {"test_data/pdsch_encoder_test_output136.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input137.dat"}, {"test_data/pdsch_encoder_test_output137.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input138.dat"}, {"test_data/pdsch_encoder_test_output138.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input139.dat"}, {"test_data/pdsch_encoder_test_output139.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 2640}, {"test_data/pdsch_encoder_test_input140.dat"}, {"test_data/pdsch_encoder_test_output140.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 3300}, {"test_data/pdsch_encoder_test_input141.dat"}, {"test_data/pdsch_encoder_test_output141.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1584}, {"test_data/pdsch_encoder_test_input142.dat"}, {"test_data/pdsch_encoder_test_output142.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1848}, {"test_data/pdsch_encoder_test_input143.dat"}, {"test_data/pdsch_encoder_test_output143.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QPSK, 0, 1, 1056}, {"test_data/pdsch_encoder_test_input144.dat"}, {"test_data/pdsch_encoder_test_output144.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 1056}, {"test_data/pdsch_encoder_test_input145.dat"}, {"test_data/pdsch_encoder_test_output145.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 792}, {"test_data/pdsch_encoder_test_input146.dat"}, {"test_data/pdsch_encoder_test_output146.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2376}, {"test_data/pdsch_encoder_test_input147.dat"}, {"test_data/pdsch_encoder_test_output147.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 1848}, {"test_data/pdsch_encoder_test_input148.dat"}, {"test_data/pdsch_encoder_test_output148.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM16, 0, 1, 1188}, {"test_data/pdsch_encoder_test_input149.dat"}, {"test_data/pdsch_encoder_test_output149.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM16, 0, 1, 2376}, {"test_data/pdsch_encoder_test_input150.dat"}, {"test_data/pdsch_encoder_test_output150.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1980}, {"test_data/pdsch_encoder_test_input151.dat"}, {"test_data/pdsch_encoder_test_output151.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1584}, {"test_data/pdsch_encoder_test_input152.dat"}, {"test_data/pdsch_encoder_test_output152.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1452}, {"test_data/pdsch_encoder_test_input153.dat"}, {"test_data/pdsch_encoder_test_output153.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1980}, {"test_data/pdsch_encoder_test_input154.dat"}, {"test_data/pdsch_encoder_test_output154.dat"}},
  {{ldpc_base_graph_type::BG2, 0, modulation_scheme::QAM64, 0, 1, 660}, {"test_data/pdsch_encoder_test_input155.dat"}, {"test_data/pdsch_encoder_test_output155.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 2904}, {"test_data/pdsch_encoder_test_input156.dat"}, {"test_data/pdsch_encoder_test_output156.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1848}, {"test_data/pdsch_encoder_test_input157.dat"}, {"test_data/pdsch_encoder_test_output157.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1716}, {"test_data/pdsch_encoder_test_input158.dat"}, {"test_data/pdsch_encoder_test_output158.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM64, 0, 1, 1716}, {"test_data/pdsch_encoder_test_input159.dat"}, {"test_data/pdsch_encoder_test_output159.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1716}, {"test_data/pdsch_encoder_test_input160.dat"}, {"test_data/pdsch_encoder_test_output160.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1980}, {"test_data/pdsch_encoder_test_input161.dat"}, {"test_data/pdsch_encoder_test_output161.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1584}, {"test_data/pdsch_encoder_test_input162.dat"}, {"test_data/pdsch_encoder_test_output162.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2772}, {"test_data/pdsch_encoder_test_input163.dat"}, {"test_data/pdsch_encoder_test_output163.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1188}, {"test_data/pdsch_encoder_test_input164.dat"}, {"test_data/pdsch_encoder_test_output164.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1056}, {"test_data/pdsch_encoder_test_input165.dat"}, {"test_data/pdsch_encoder_test_output165.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 1980}, {"test_data/pdsch_encoder_test_input166.dat"}, {"test_data/pdsch_encoder_test_output166.dat"}},
  {{ldpc_base_graph_type::BG1, 0, modulation_scheme::QAM256, 0, 1, 2772}, {"test_data/pdsch_encoder_test_input167.dat"}, {"test_data/pdsch_encoder_test_output167.dat"}},
    // clang-format on
};

} // namespace srsran
