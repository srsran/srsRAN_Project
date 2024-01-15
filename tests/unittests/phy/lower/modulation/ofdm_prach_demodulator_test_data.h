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
//   + "srsPRACHDemodulatorUnittest.m"

#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"
#include "srsran/phy/lower/sampling_rate.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct prach_context {
  sampling_rate                         srate;
  ofdm_prach_demodulator::configuration config;
};

struct test_case_t {
  prach_context     context;
  file_vector<cf_t> input;
  file_vector<cf_t> output;
};

static const std::vector<test_case_t> ofdm_prach_demodulator_test_data = {
    // clang-format off
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("0"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input0.dat"}, {"test_data/ofdm_prach_demodulator_test_output0.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("0"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input1.dat"}, {"test_data/ofdm_prach_demodulator_test_output1.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("1"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input2.dat"}, {"test_data/ofdm_prach_demodulator_test_output2.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("1"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input3.dat"}, {"test_data/ofdm_prach_demodulator_test_output3.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("2"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input4.dat"}, {"test_data/ofdm_prach_demodulator_test_output4.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("2"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input5.dat"}, {"test_data/ofdm_prach_demodulator_test_output5.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("3"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input6.dat"}, {"test_data/ofdm_prach_demodulator_test_output6.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("3"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input7.dat"}, {"test_data/ofdm_prach_demodulator_test_output7.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1"), 6, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input8.dat"}, {"test_data/ofdm_prach_demodulator_test_output8.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1"), 6, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input9.dat"}, {"test_data/ofdm_prach_demodulator_test_output9.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1/B1"), 7, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input10.dat"}, {"test_data/ofdm_prach_demodulator_test_output10.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1/B1"), 7, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input11.dat"}, {"test_data/ofdm_prach_demodulator_test_output11.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2"), 3, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input12.dat"}, {"test_data/ofdm_prach_demodulator_test_output12.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2"), 3, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input13.dat"}, {"test_data/ofdm_prach_demodulator_test_output13.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2/B2"), 3, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input14.dat"}, {"test_data/ofdm_prach_demodulator_test_output14.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2/B2"), 3, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input15.dat"}, {"test_data/ofdm_prach_demodulator_test_output15.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3"), 2, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input16.dat"}, {"test_data/ofdm_prach_demodulator_test_output16.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3"), 2, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input17.dat"}, {"test_data/ofdm_prach_demodulator_test_output17.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3/B3"), 2, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input18.dat"}, {"test_data/ofdm_prach_demodulator_test_output18.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3/B3"), 2, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input19.dat"}, {"test_data/ofdm_prach_demodulator_test_output19.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B1"), 7, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input20.dat"}, {"test_data/ofdm_prach_demodulator_test_output20.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B1"), 7, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input21.dat"}, {"test_data/ofdm_prach_demodulator_test_output21.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B4"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input22.dat"}, {"test_data/ofdm_prach_demodulator_test_output22.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B4"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input23.dat"}, {"test_data/ofdm_prach_demodulator_test_output23.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C0"), 7, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input24.dat"}, {"test_data/ofdm_prach_demodulator_test_output24.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C0"), 7, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input25.dat"}, {"test_data/ofdm_prach_demodulator_test_output25.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C2"), 2, 2, 0, 0, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input26.dat"}, {"test_data/ofdm_prach_demodulator_test_output26.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C2"), 2, 2, 0, 2, 79, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input27.dat"}, {"test_data/ofdm_prach_demodulator_test_output27.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("0"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input28.dat"}, {"test_data/ofdm_prach_demodulator_test_output28.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("0"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input29.dat"}, {"test_data/ofdm_prach_demodulator_test_output29.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("1"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input30.dat"}, {"test_data/ofdm_prach_demodulator_test_output30.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("1"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input31.dat"}, {"test_data/ofdm_prach_demodulator_test_output31.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("2"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input32.dat"}, {"test_data/ofdm_prach_demodulator_test_output32.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("2"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input33.dat"}, {"test_data/ofdm_prach_demodulator_test_output33.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("3"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input34.dat"}, {"test_data/ofdm_prach_demodulator_test_output34.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("3"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input35.dat"}, {"test_data/ofdm_prach_demodulator_test_output35.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1"), 6, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input36.dat"}, {"test_data/ofdm_prach_demodulator_test_output36.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1"), 6, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input37.dat"}, {"test_data/ofdm_prach_demodulator_test_output37.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1/B1"), 7, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input38.dat"}, {"test_data/ofdm_prach_demodulator_test_output38.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A1/B1"), 7, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input39.dat"}, {"test_data/ofdm_prach_demodulator_test_output39.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2"), 3, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input40.dat"}, {"test_data/ofdm_prach_demodulator_test_output40.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2"), 3, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input41.dat"}, {"test_data/ofdm_prach_demodulator_test_output41.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2/B2"), 3, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input42.dat"}, {"test_data/ofdm_prach_demodulator_test_output42.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A2/B2"), 3, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input43.dat"}, {"test_data/ofdm_prach_demodulator_test_output43.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3"), 2, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input44.dat"}, {"test_data/ofdm_prach_demodulator_test_output44.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3"), 2, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input45.dat"}, {"test_data/ofdm_prach_demodulator_test_output45.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3/B3"), 2, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input46.dat"}, {"test_data/ofdm_prach_demodulator_test_output46.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("A3/B3"), 2, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input47.dat"}, {"test_data/ofdm_prach_demodulator_test_output47.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B1"), 7, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input48.dat"}, {"test_data/ofdm_prach_demodulator_test_output48.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B1"), 7, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input49.dat"}, {"test_data/ofdm_prach_demodulator_test_output49.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B4"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input50.dat"}, {"test_data/ofdm_prach_demodulator_test_output50.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("B4"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input51.dat"}, {"test_data/ofdm_prach_demodulator_test_output51.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C0"), 7, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input52.dat"}, {"test_data/ofdm_prach_demodulator_test_output52.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C0"), 7, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input53.dat"}, {"test_data/ofdm_prach_demodulator_test_output53.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C2"), 2, 2, 0, 0, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input54.dat"}, {"test_data/ofdm_prach_demodulator_test_output54.dat"}},
  {{sampling_rate::from_MHz(30.72), {to_prach_format_type("C2"), 2, 2, 0, 2, 106, subcarrier_spacing::kHz15}}, {"test_data/ofdm_prach_demodulator_test_input55.dat"}, {"test_data/ofdm_prach_demodulator_test_output55.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("0"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input56.dat"}, {"test_data/ofdm_prach_demodulator_test_output56.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("0"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input57.dat"}, {"test_data/ofdm_prach_demodulator_test_output57.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("1"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input58.dat"}, {"test_data/ofdm_prach_demodulator_test_output58.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("1"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input59.dat"}, {"test_data/ofdm_prach_demodulator_test_output59.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("2"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input60.dat"}, {"test_data/ofdm_prach_demodulator_test_output60.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("2"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input61.dat"}, {"test_data/ofdm_prach_demodulator_test_output61.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("3"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input62.dat"}, {"test_data/ofdm_prach_demodulator_test_output62.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("3"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input63.dat"}, {"test_data/ofdm_prach_demodulator_test_output63.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1"), 6, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input64.dat"}, {"test_data/ofdm_prach_demodulator_test_output64.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1"), 6, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input65.dat"}, {"test_data/ofdm_prach_demodulator_test_output65.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1/B1"), 6, 2, 2, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input66.dat"}, {"test_data/ofdm_prach_demodulator_test_output66.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1/B1"), 6, 2, 2, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input67.dat"}, {"test_data/ofdm_prach_demodulator_test_output67.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2"), 3, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input68.dat"}, {"test_data/ofdm_prach_demodulator_test_output68.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2"), 3, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input69.dat"}, {"test_data/ofdm_prach_demodulator_test_output69.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2/B2"), 3, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input70.dat"}, {"test_data/ofdm_prach_demodulator_test_output70.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2/B2"), 3, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input71.dat"}, {"test_data/ofdm_prach_demodulator_test_output71.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3"), 2, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input72.dat"}, {"test_data/ofdm_prach_demodulator_test_output72.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3"), 2, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input73.dat"}, {"test_data/ofdm_prach_demodulator_test_output73.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3/B3"), 2, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input74.dat"}, {"test_data/ofdm_prach_demodulator_test_output74.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3/B3"), 2, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input75.dat"}, {"test_data/ofdm_prach_demodulator_test_output75.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B1"), 6, 2, 2, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input76.dat"}, {"test_data/ofdm_prach_demodulator_test_output76.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B1"), 6, 2, 2, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input77.dat"}, {"test_data/ofdm_prach_demodulator_test_output77.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B4"), 1, 2, 0, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input78.dat"}, {"test_data/ofdm_prach_demodulator_test_output78.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B4"), 1, 2, 0, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input79.dat"}, {"test_data/ofdm_prach_demodulator_test_output79.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C0"), 6, 2, 2, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input80.dat"}, {"test_data/ofdm_prach_demodulator_test_output80.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C0"), 6, 2, 2, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input81.dat"}, {"test_data/ofdm_prach_demodulator_test_output81.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C2"), 2, 2, 2, 0, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input82.dat"}, {"test_data/ofdm_prach_demodulator_test_output82.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C2"), 2, 2, 2, 2, 79, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input83.dat"}, {"test_data/ofdm_prach_demodulator_test_output83.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("0"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input84.dat"}, {"test_data/ofdm_prach_demodulator_test_output84.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("0"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input85.dat"}, {"test_data/ofdm_prach_demodulator_test_output85.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("1"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input86.dat"}, {"test_data/ofdm_prach_demodulator_test_output86.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("1"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input87.dat"}, {"test_data/ofdm_prach_demodulator_test_output87.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("2"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input88.dat"}, {"test_data/ofdm_prach_demodulator_test_output88.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("2"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input89.dat"}, {"test_data/ofdm_prach_demodulator_test_output89.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("3"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input90.dat"}, {"test_data/ofdm_prach_demodulator_test_output90.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("3"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input91.dat"}, {"test_data/ofdm_prach_demodulator_test_output91.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1"), 6, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input92.dat"}, {"test_data/ofdm_prach_demodulator_test_output92.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1"), 6, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input93.dat"}, {"test_data/ofdm_prach_demodulator_test_output93.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1/B1"), 6, 2, 2, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input94.dat"}, {"test_data/ofdm_prach_demodulator_test_output94.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A1/B1"), 6, 2, 2, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input95.dat"}, {"test_data/ofdm_prach_demodulator_test_output95.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2"), 3, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input96.dat"}, {"test_data/ofdm_prach_demodulator_test_output96.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2"), 3, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input97.dat"}, {"test_data/ofdm_prach_demodulator_test_output97.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2/B2"), 3, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input98.dat"}, {"test_data/ofdm_prach_demodulator_test_output98.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A2/B2"), 3, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input99.dat"}, {"test_data/ofdm_prach_demodulator_test_output99.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3"), 2, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input100.dat"}, {"test_data/ofdm_prach_demodulator_test_output100.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3"), 2, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input101.dat"}, {"test_data/ofdm_prach_demodulator_test_output101.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3/B3"), 2, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input102.dat"}, {"test_data/ofdm_prach_demodulator_test_output102.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("A3/B3"), 2, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input103.dat"}, {"test_data/ofdm_prach_demodulator_test_output103.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B1"), 6, 2, 2, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input104.dat"}, {"test_data/ofdm_prach_demodulator_test_output104.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B1"), 6, 2, 2, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input105.dat"}, {"test_data/ofdm_prach_demodulator_test_output105.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B4"), 1, 2, 0, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input106.dat"}, {"test_data/ofdm_prach_demodulator_test_output106.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("B4"), 1, 2, 0, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input107.dat"}, {"test_data/ofdm_prach_demodulator_test_output107.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C0"), 6, 2, 2, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input108.dat"}, {"test_data/ofdm_prach_demodulator_test_output108.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C0"), 6, 2, 2, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input109.dat"}, {"test_data/ofdm_prach_demodulator_test_output109.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C2"), 2, 2, 2, 0, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input110.dat"}, {"test_data/ofdm_prach_demodulator_test_output110.dat"}},
  {{sampling_rate::from_MHz(61.44), {to_prach_format_type("C2"), 2, 2, 2, 2, 106, subcarrier_spacing::kHz30}}, {"test_data/ofdm_prach_demodulator_test_input111.dat"}, {"test_data/ofdm_prach_demodulator_test_output111.dat"}},
    // clang-format on
};

} // namespace srsran
