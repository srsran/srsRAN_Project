/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 03-Aug-2022:
//   + "srsPRACHDemodulatorUnittest.m"

#include "srsgnb/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct prach_context {
  unsigned                              nof_prb_ul_grid;
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
  {{52, 1024, {preamble_format::FORMAT0, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input0.dat"}, {"test_data/ofdm_prach_demodulator_test_output0.dat"}},
  {{52, 1024, {preamble_format::FORMAT0, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input1.dat"}, {"test_data/ofdm_prach_demodulator_test_output1.dat"}},
  {{52, 1024, {preamble_format::FORMAT1, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input2.dat"}, {"test_data/ofdm_prach_demodulator_test_output2.dat"}},
  {{52, 1024, {preamble_format::FORMAT1, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input3.dat"}, {"test_data/ofdm_prach_demodulator_test_output3.dat"}},
  {{52, 1024, {preamble_format::FORMAT2, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input4.dat"}, {"test_data/ofdm_prach_demodulator_test_output4.dat"}},
  {{52, 1024, {preamble_format::FORMAT2, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input5.dat"}, {"test_data/ofdm_prach_demodulator_test_output5.dat"}},
  {{52, 1024, {preamble_format::FORMAT3, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input6.dat"}, {"test_data/ofdm_prach_demodulator_test_output6.dat"}},
  {{52, 1024, {preamble_format::FORMAT3, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input7.dat"}, {"test_data/ofdm_prach_demodulator_test_output7.dat"}},
  {{106, 2048, {preamble_format::FORMAT0, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input8.dat"}, {"test_data/ofdm_prach_demodulator_test_output8.dat"}},
  {{106, 2048, {preamble_format::FORMAT0, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input9.dat"}, {"test_data/ofdm_prach_demodulator_test_output9.dat"}},
  {{106, 2048, {preamble_format::FORMAT1, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input10.dat"}, {"test_data/ofdm_prach_demodulator_test_output10.dat"}},
  {{106, 2048, {preamble_format::FORMAT1, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input11.dat"}, {"test_data/ofdm_prach_demodulator_test_output11.dat"}},
  {{106, 2048, {preamble_format::FORMAT2, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input12.dat"}, {"test_data/ofdm_prach_demodulator_test_output12.dat"}},
  {{106, 2048, {preamble_format::FORMAT2, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input13.dat"}, {"test_data/ofdm_prach_demodulator_test_output13.dat"}},
  {{106, 2048, {preamble_format::FORMAT3, 0, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input14.dat"}, {"test_data/ofdm_prach_demodulator_test_output14.dat"}},
  {{106, 2048, {preamble_format::FORMAT3, 13, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input15.dat"}, {"test_data/ofdm_prach_demodulator_test_output15.dat"}},
  {{52, 2048, {preamble_format::FORMAT0, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input16.dat"}, {"test_data/ofdm_prach_demodulator_test_output16.dat"}},
  {{52, 2048, {preamble_format::FORMAT0, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input17.dat"}, {"test_data/ofdm_prach_demodulator_test_output17.dat"}},
  {{52, 2048, {preamble_format::FORMAT1, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input18.dat"}, {"test_data/ofdm_prach_demodulator_test_output18.dat"}},
  {{52, 2048, {preamble_format::FORMAT1, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input19.dat"}, {"test_data/ofdm_prach_demodulator_test_output19.dat"}},
  {{52, 2048, {preamble_format::FORMAT2, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input20.dat"}, {"test_data/ofdm_prach_demodulator_test_output20.dat"}},
  {{52, 2048, {preamble_format::FORMAT2, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input21.dat"}, {"test_data/ofdm_prach_demodulator_test_output21.dat"}},
  {{52, 2048, {preamble_format::FORMAT3, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input22.dat"}, {"test_data/ofdm_prach_demodulator_test_output22.dat"}},
  {{52, 2048, {preamble_format::FORMAT3, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input23.dat"}, {"test_data/ofdm_prach_demodulator_test_output23.dat"}},
  {{106, 4096, {preamble_format::FORMAT0, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input24.dat"}, {"test_data/ofdm_prach_demodulator_test_output24.dat"}},
  {{106, 4096, {preamble_format::FORMAT0, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input25.dat"}, {"test_data/ofdm_prach_demodulator_test_output25.dat"}},
  {{106, 4096, {preamble_format::FORMAT1, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input26.dat"}, {"test_data/ofdm_prach_demodulator_test_output26.dat"}},
  {{106, 4096, {preamble_format::FORMAT1, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input27.dat"}, {"test_data/ofdm_prach_demodulator_test_output27.dat"}},
  {{106, 4096, {preamble_format::FORMAT2, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input28.dat"}, {"test_data/ofdm_prach_demodulator_test_output28.dat"}},
  {{106, 4096, {preamble_format::FORMAT2, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input29.dat"}, {"test_data/ofdm_prach_demodulator_test_output29.dat"}},
  {{106, 4096, {preamble_format::FORMAT3, 0, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input30.dat"}, {"test_data/ofdm_prach_demodulator_test_output30.dat"}},
  {{106, 4096, {preamble_format::FORMAT3, 13, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input31.dat"}, {"test_data/ofdm_prach_demodulator_test_output31.dat"}},
    // clang-format on
};

} // namespace srsgnb
