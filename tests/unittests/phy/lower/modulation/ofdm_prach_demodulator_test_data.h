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

// This file was generated using the following MATLAB class on 10-Aug-2022:
//   + "srsPRACHDemodulatorUnittest.m"

#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct prach_context {
  unsigned                              dft_size_15kHz;
  ofdm_prach_demodulator::configuration config;
};

struct test_case_t {
  prach_context     context;
  file_vector<cf_t> input;
  file_vector<cf_t> output;
};

static const std::vector<test_case_t> ofdm_prach_demodulator_test_data = {
    // clang-format off
  {{1024, {preamble_format::FORMAT0, 0, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input0.dat"}, {"test_data/ofdm_prach_demodulator_test_output0.dat"}},
  {{1024, {preamble_format::FORMAT0, 13, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input1.dat"}, {"test_data/ofdm_prach_demodulator_test_output1.dat"}},
  {{1024, {preamble_format::FORMAT0, 28, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input2.dat"}, {"test_data/ofdm_prach_demodulator_test_output2.dat"}},
  {{1024, {preamble_format::FORMAT1, 0, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input3.dat"}, {"test_data/ofdm_prach_demodulator_test_output3.dat"}},
  {{1024, {preamble_format::FORMAT1, 13, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input4.dat"}, {"test_data/ofdm_prach_demodulator_test_output4.dat"}},
  {{1024, {preamble_format::FORMAT1, 28, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input5.dat"}, {"test_data/ofdm_prach_demodulator_test_output5.dat"}},
  {{1024, {preamble_format::FORMAT2, 0, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input6.dat"}, {"test_data/ofdm_prach_demodulator_test_output6.dat"}},
  {{1024, {preamble_format::FORMAT2, 13, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input7.dat"}, {"test_data/ofdm_prach_demodulator_test_output7.dat"}},
  {{1024, {preamble_format::FORMAT2, 28, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input8.dat"}, {"test_data/ofdm_prach_demodulator_test_output8.dat"}},
  {{1024, {preamble_format::FORMAT3, 0, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input9.dat"}, {"test_data/ofdm_prach_demodulator_test_output9.dat"}},
  {{1024, {preamble_format::FORMAT3, 13, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input10.dat"}, {"test_data/ofdm_prach_demodulator_test_output10.dat"}},
  {{1024, {preamble_format::FORMAT3, 28, 52, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input11.dat"}, {"test_data/ofdm_prach_demodulator_test_output11.dat"}},
  {{2048, {preamble_format::FORMAT0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input12.dat"}, {"test_data/ofdm_prach_demodulator_test_output12.dat"}},
  {{2048, {preamble_format::FORMAT0, 13, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input13.dat"}, {"test_data/ofdm_prach_demodulator_test_output13.dat"}},
  {{2048, {preamble_format::FORMAT0, 28, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input14.dat"}, {"test_data/ofdm_prach_demodulator_test_output14.dat"}},
  {{2048, {preamble_format::FORMAT1, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input15.dat"}, {"test_data/ofdm_prach_demodulator_test_output15.dat"}},
  {{2048, {preamble_format::FORMAT1, 13, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input16.dat"}, {"test_data/ofdm_prach_demodulator_test_output16.dat"}},
  {{2048, {preamble_format::FORMAT1, 28, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input17.dat"}, {"test_data/ofdm_prach_demodulator_test_output17.dat"}},
  {{2048, {preamble_format::FORMAT2, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input18.dat"}, {"test_data/ofdm_prach_demodulator_test_output18.dat"}},
  {{2048, {preamble_format::FORMAT2, 13, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input19.dat"}, {"test_data/ofdm_prach_demodulator_test_output19.dat"}},
  {{2048, {preamble_format::FORMAT2, 28, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input20.dat"}, {"test_data/ofdm_prach_demodulator_test_output20.dat"}},
  {{2048, {preamble_format::FORMAT3, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input21.dat"}, {"test_data/ofdm_prach_demodulator_test_output21.dat"}},
  {{2048, {preamble_format::FORMAT3, 13, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input22.dat"}, {"test_data/ofdm_prach_demodulator_test_output22.dat"}},
  {{2048, {preamble_format::FORMAT3, 28, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input23.dat"}, {"test_data/ofdm_prach_demodulator_test_output23.dat"}},
  {{2048, {preamble_format::FORMAT0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input24.dat"}, {"test_data/ofdm_prach_demodulator_test_output24.dat"}},
  {{2048, {preamble_format::FORMAT0, 13, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input25.dat"}, {"test_data/ofdm_prach_demodulator_test_output25.dat"}},
  {{2048, {preamble_format::FORMAT0, 28, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input26.dat"}, {"test_data/ofdm_prach_demodulator_test_output26.dat"}},
  {{2048, {preamble_format::FORMAT1, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input27.dat"}, {"test_data/ofdm_prach_demodulator_test_output27.dat"}},
  {{2048, {preamble_format::FORMAT1, 13, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input28.dat"}, {"test_data/ofdm_prach_demodulator_test_output28.dat"}},
  {{2048, {preamble_format::FORMAT1, 28, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input29.dat"}, {"test_data/ofdm_prach_demodulator_test_output29.dat"}},
  {{2048, {preamble_format::FORMAT2, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input30.dat"}, {"test_data/ofdm_prach_demodulator_test_output30.dat"}},
  {{2048, {preamble_format::FORMAT2, 13, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input31.dat"}, {"test_data/ofdm_prach_demodulator_test_output31.dat"}},
  {{2048, {preamble_format::FORMAT2, 28, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input32.dat"}, {"test_data/ofdm_prach_demodulator_test_output32.dat"}},
  {{2048, {preamble_format::FORMAT3, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input33.dat"}, {"test_data/ofdm_prach_demodulator_test_output33.dat"}},
  {{2048, {preamble_format::FORMAT3, 13, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input34.dat"}, {"test_data/ofdm_prach_demodulator_test_output34.dat"}},
  {{2048, {preamble_format::FORMAT3, 28, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input35.dat"}, {"test_data/ofdm_prach_demodulator_test_output35.dat"}},
  {{2048, {preamble_format::FORMAT0, 0, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input36.dat"}, {"test_data/ofdm_prach_demodulator_test_output36.dat"}},
  {{2048, {preamble_format::FORMAT0, 13, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input37.dat"}, {"test_data/ofdm_prach_demodulator_test_output37.dat"}},
  {{2048, {preamble_format::FORMAT0, 28, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input38.dat"}, {"test_data/ofdm_prach_demodulator_test_output38.dat"}},
  {{2048, {preamble_format::FORMAT1, 0, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input39.dat"}, {"test_data/ofdm_prach_demodulator_test_output39.dat"}},
  {{2048, {preamble_format::FORMAT1, 13, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input40.dat"}, {"test_data/ofdm_prach_demodulator_test_output40.dat"}},
  {{2048, {preamble_format::FORMAT1, 28, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input41.dat"}, {"test_data/ofdm_prach_demodulator_test_output41.dat"}},
  {{2048, {preamble_format::FORMAT2, 0, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input42.dat"}, {"test_data/ofdm_prach_demodulator_test_output42.dat"}},
  {{2048, {preamble_format::FORMAT2, 13, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input43.dat"}, {"test_data/ofdm_prach_demodulator_test_output43.dat"}},
  {{2048, {preamble_format::FORMAT2, 28, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input44.dat"}, {"test_data/ofdm_prach_demodulator_test_output44.dat"}},
  {{2048, {preamble_format::FORMAT3, 0, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input45.dat"}, {"test_data/ofdm_prach_demodulator_test_output45.dat"}},
  {{2048, {preamble_format::FORMAT3, 13, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input46.dat"}, {"test_data/ofdm_prach_demodulator_test_output46.dat"}},
  {{2048, {preamble_format::FORMAT3, 28, 52, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input47.dat"}, {"test_data/ofdm_prach_demodulator_test_output47.dat"}},
  {{4096, {preamble_format::FORMAT0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input48.dat"}, {"test_data/ofdm_prach_demodulator_test_output48.dat"}},
  {{4096, {preamble_format::FORMAT0, 13, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input49.dat"}, {"test_data/ofdm_prach_demodulator_test_output49.dat"}},
  {{4096, {preamble_format::FORMAT0, 28, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input50.dat"}, {"test_data/ofdm_prach_demodulator_test_output50.dat"}},
  {{4096, {preamble_format::FORMAT1, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input51.dat"}, {"test_data/ofdm_prach_demodulator_test_output51.dat"}},
  {{4096, {preamble_format::FORMAT1, 13, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input52.dat"}, {"test_data/ofdm_prach_demodulator_test_output52.dat"}},
  {{4096, {preamble_format::FORMAT1, 28, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input53.dat"}, {"test_data/ofdm_prach_demodulator_test_output53.dat"}},
  {{4096, {preamble_format::FORMAT2, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input54.dat"}, {"test_data/ofdm_prach_demodulator_test_output54.dat"}},
  {{4096, {preamble_format::FORMAT2, 13, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input55.dat"}, {"test_data/ofdm_prach_demodulator_test_output55.dat"}},
  {{4096, {preamble_format::FORMAT2, 28, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input56.dat"}, {"test_data/ofdm_prach_demodulator_test_output56.dat"}},
  {{4096, {preamble_format::FORMAT3, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input57.dat"}, {"test_data/ofdm_prach_demodulator_test_output57.dat"}},
  {{4096, {preamble_format::FORMAT3, 13, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input58.dat"}, {"test_data/ofdm_prach_demodulator_test_output58.dat"}},
  {{4096, {preamble_format::FORMAT3, 28, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input59.dat"}, {"test_data/ofdm_prach_demodulator_test_output59.dat"}},
  {{4096, {preamble_format::FORMAT0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input60.dat"}, {"test_data/ofdm_prach_demodulator_test_output60.dat"}},
  {{4096, {preamble_format::FORMAT0, 13, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input61.dat"}, {"test_data/ofdm_prach_demodulator_test_output61.dat"}},
  {{4096, {preamble_format::FORMAT0, 28, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input62.dat"}, {"test_data/ofdm_prach_demodulator_test_output62.dat"}},
  {{4096, {preamble_format::FORMAT1, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input63.dat"}, {"test_data/ofdm_prach_demodulator_test_output63.dat"}},
  {{4096, {preamble_format::FORMAT1, 13, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input64.dat"}, {"test_data/ofdm_prach_demodulator_test_output64.dat"}},
  {{4096, {preamble_format::FORMAT1, 28, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input65.dat"}, {"test_data/ofdm_prach_demodulator_test_output65.dat"}},
  {{4096, {preamble_format::FORMAT2, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input66.dat"}, {"test_data/ofdm_prach_demodulator_test_output66.dat"}},
  {{4096, {preamble_format::FORMAT2, 13, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input67.dat"}, {"test_data/ofdm_prach_demodulator_test_output67.dat"}},
  {{4096, {preamble_format::FORMAT2, 28, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input68.dat"}, {"test_data/ofdm_prach_demodulator_test_output68.dat"}},
  {{4096, {preamble_format::FORMAT3, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input69.dat"}, {"test_data/ofdm_prach_demodulator_test_output69.dat"}},
  {{4096, {preamble_format::FORMAT3, 13, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input70.dat"}, {"test_data/ofdm_prach_demodulator_test_output70.dat"}},
  {{4096, {preamble_format::FORMAT3, 28, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input71.dat"}, {"test_data/ofdm_prach_demodulator_test_output71.dat"}},
    // clang-format on
};

} // namespace srsran
