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

// This file was generated using the following MATLAB class on 09-10-2025 (seed 0):
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
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B4"), 1, 2, 0, 0, 79}}, {"test_data/ofdm_prach_demodulator_test_input0.dat"}, {"test_data/ofdm_prach_demodulator_test_output0.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 14}, to_prach_format_type("A3"), 2, 2, 0, 2, 79}}, {"test_data/ofdm_prach_demodulator_test_input1.dat"}, {"test_data/ofdm_prach_demodulator_test_output1.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 2}, to_prach_format_type("2"), 1, 2, 0, 4, 79}}, {"test_data/ofdm_prach_demodulator_test_input2.dat"}, {"test_data/ofdm_prach_demodulator_test_output2.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B4"), 1, 2, 0, 30, 79}}, {"test_data/ofdm_prach_demodulator_test_input3.dat"}, {"test_data/ofdm_prach_demodulator_test_output3.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B4"), 1, 2, 0, 0, 106}}, {"test_data/ofdm_prach_demodulator_test_input4.dat"}, {"test_data/ofdm_prach_demodulator_test_output4.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B4"), 1, 2, 0, 2, 106}}, {"test_data/ofdm_prach_demodulator_test_input5.dat"}, {"test_data/ofdm_prach_demodulator_test_output5.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B1"), 7, 2, 0, 4, 106}}, {"test_data/ofdm_prach_demodulator_test_input6.dat"}, {"test_data/ofdm_prach_demodulator_test_output6.dat"}},
  {{sampling_rate::from_MHz(30.72), {{0, 4}, to_prach_format_type("B1"), 7, 2, 0, 30, 106}}, {"test_data/ofdm_prach_demodulator_test_input7.dat"}, {"test_data/ofdm_prach_demodulator_test_output7.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 14}, to_prach_format_type("A3"), 2, 2, 0, 0, 212}}, {"test_data/ofdm_prach_demodulator_test_input8.dat"}, {"test_data/ofdm_prach_demodulator_test_output8.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 14}, to_prach_format_type("C0"), 7, 2, 0, 2, 212}}, {"test_data/ofdm_prach_demodulator_test_input9.dat"}, {"test_data/ofdm_prach_demodulator_test_output9.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 4}, to_prach_format_type("A1"), 6, 2, 0, 4, 212}}, {"test_data/ofdm_prach_demodulator_test_input10.dat"}, {"test_data/ofdm_prach_demodulator_test_output10.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 14}, to_prach_format_type("C2"), 2, 2, 0, 30, 212}}, {"test_data/ofdm_prach_demodulator_test_input11.dat"}, {"test_data/ofdm_prach_demodulator_test_output11.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 2}, to_prach_format_type("2"), 1, 2, 0, 0, 275}}, {"test_data/ofdm_prach_demodulator_test_input12.dat"}, {"test_data/ofdm_prach_demodulator_test_output12.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 2}, to_prach_format_type("2"), 1, 2, 0, 2, 275}}, {"test_data/ofdm_prach_demodulator_test_input13.dat"}, {"test_data/ofdm_prach_demodulator_test_output13.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 12}, to_prach_format_type("A3/B3"), 2, 2, 0, 4, 275}}, {"test_data/ofdm_prach_demodulator_test_input14.dat"}, {"test_data/ofdm_prach_demodulator_test_output14.dat"}},
  {{sampling_rate::from_MHz(61.44), {{0, 2}, to_prach_format_type("2"), 1, 2, 0, 30, 275}}, {"test_data/ofdm_prach_demodulator_test_input15.dat"}, {"test_data/ofdm_prach_demodulator_test_output15.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 5}, to_prach_format_type("1"), 1, 2, 0, 0, 79}}, {"test_data/ofdm_prach_demodulator_test_input16.dat"}, {"test_data/ofdm_prach_demodulator_test_output16.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 19}, to_prach_format_type("0"), 1, 2, 0, 2, 79}}, {"test_data/ofdm_prach_demodulator_test_input17.dat"}, {"test_data/ofdm_prach_demodulator_test_output17.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 5}, to_prach_format_type("1"), 1, 2, 0, 4, 79}}, {"test_data/ofdm_prach_demodulator_test_input18.dat"}, {"test_data/ofdm_prach_demodulator_test_output18.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 39}, to_prach_format_type("A2"), 3, 2, 0, 30, 79}}, {"test_data/ofdm_prach_demodulator_test_input19.dat"}, {"test_data/ofdm_prach_demodulator_test_output19.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 39}, to_prach_format_type("A2/B2"), 3, 2, 0, 0, 106}}, {"test_data/ofdm_prach_demodulator_test_input20.dat"}, {"test_data/ofdm_prach_demodulator_test_output20.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 19}, to_prach_format_type("3"), 1, 2, 0, 2, 106}}, {"test_data/ofdm_prach_demodulator_test_input21.dat"}, {"test_data/ofdm_prach_demodulator_test_output21.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 39}, to_prach_format_type("A1/B1"), 6, 2, 2, 4, 106}}, {"test_data/ofdm_prach_demodulator_test_input22.dat"}, {"test_data/ofdm_prach_demodulator_test_output22.dat"}},
  {{sampling_rate::from_MHz(61.44), {{1, 5}, to_prach_format_type("1"), 1, 2, 0, 30, 106}}, {"test_data/ofdm_prach_demodulator_test_input23.dat"}, {"test_data/ofdm_prach_demodulator_test_output23.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("A3"), 2, 2, 0, 0, 212}}, {"test_data/ofdm_prach_demodulator_test_input24.dat"}, {"test_data/ofdm_prach_demodulator_test_output24.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 19}, to_prach_format_type("0"), 1, 2, 0, 2, 212}}, {"test_data/ofdm_prach_demodulator_test_input25.dat"}, {"test_data/ofdm_prach_demodulator_test_output25.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("A3/B3"), 2, 2, 0, 4, 212}}, {"test_data/ofdm_prach_demodulator_test_input26.dat"}, {"test_data/ofdm_prach_demodulator_test_output26.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("A3/B3"), 2, 2, 0, 30, 212}}, {"test_data/ofdm_prach_demodulator_test_input27.dat"}, {"test_data/ofdm_prach_demodulator_test_output27.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("C2"), 2, 2, 2, 0, 275}}, {"test_data/ofdm_prach_demodulator_test_input28.dat"}, {"test_data/ofdm_prach_demodulator_test_output28.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("A2/B2"), 3, 2, 0, 2, 275}}, {"test_data/ofdm_prach_demodulator_test_input29.dat"}, {"test_data/ofdm_prach_demodulator_test_output29.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 39}, to_prach_format_type("A1"), 6, 2, 0, 4, 275}}, {"test_data/ofdm_prach_demodulator_test_input30.dat"}, {"test_data/ofdm_prach_demodulator_test_output30.dat"}},
  {{sampling_rate::from_MHz(122.88), {{1, 19}, to_prach_format_type("3"), 1, 2, 0, 30, 275}}, {"test_data/ofdm_prach_demodulator_test_input31.dat"}, {"test_data/ofdm_prach_demodulator_test_output31.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("A3"), 2, 2, 0, 0, 79}}, {"test_data/ofdm_prach_demodulator_test_input32.dat"}, {"test_data/ofdm_prach_demodulator_test_output32.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("A2"), 3, 2, 0, 2, 79}}, {"test_data/ofdm_prach_demodulator_test_input33.dat"}, {"test_data/ofdm_prach_demodulator_test_output33.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("A2"), 3, 2, 0, 4, 79}}, {"test_data/ofdm_prach_demodulator_test_input34.dat"}, {"test_data/ofdm_prach_demodulator_test_output34.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("C2"), 2, 2, 0, 30, 79}}, {"test_data/ofdm_prach_demodulator_test_input35.dat"}, {"test_data/ofdm_prach_demodulator_test_output35.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("C2"), 2, 2, 0, 0, 106}}, {"test_data/ofdm_prach_demodulator_test_input36.dat"}, {"test_data/ofdm_prach_demodulator_test_output36.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("B1"), 6, 2, 2, 2, 106}}, {"test_data/ofdm_prach_demodulator_test_input37.dat"}, {"test_data/ofdm_prach_demodulator_test_output37.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("A1/B1"), 6, 2, 2, 4, 106}}, {"test_data/ofdm_prach_demodulator_test_input38.dat"}, {"test_data/ofdm_prach_demodulator_test_output38.dat"}},
  {{sampling_rate::from_MHz(245.76), {{3, 89}, to_prach_format_type("C2"), 2, 2, 0, 30, 106}}, {"test_data/ofdm_prach_demodulator_test_input39.dat"}, {"test_data/ofdm_prach_demodulator_test_output39.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A1/B1"), 6, 2, 2, 0, 212}}, {"test_data/ofdm_prach_demodulator_test_input40.dat"}, {"test_data/ofdm_prach_demodulator_test_output40.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A3/B3"), 2, 2, 2, 2, 212}}, {"test_data/ofdm_prach_demodulator_test_input41.dat"}, {"test_data/ofdm_prach_demodulator_test_output41.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("B1"), 6, 2, 2, 4, 212}}, {"test_data/ofdm_prach_demodulator_test_input42.dat"}, {"test_data/ofdm_prach_demodulator_test_output42.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A1"), 6, 2, 0, 30, 212}}, {"test_data/ofdm_prach_demodulator_test_input43.dat"}, {"test_data/ofdm_prach_demodulator_test_output43.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A3"), 2, 2, 0, 0, 275}}, {"test_data/ofdm_prach_demodulator_test_input44.dat"}, {"test_data/ofdm_prach_demodulator_test_output44.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A2"), 3, 2, 0, 2, 275}}, {"test_data/ofdm_prach_demodulator_test_input45.dat"}, {"test_data/ofdm_prach_demodulator_test_output45.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("B1"), 6, 2, 2, 4, 275}}, {"test_data/ofdm_prach_demodulator_test_input46.dat"}, {"test_data/ofdm_prach_demodulator_test_output46.dat"}},
  {{sampling_rate::from_MHz(491.52), {{3, 89}, to_prach_format_type("A3"), 2, 2, 0, 30, 275}}, {"test_data/ofdm_prach_demodulator_test_input47.dat"}, {"test_data/ofdm_prach_demodulator_test_output47.dat"}},
    // clang-format on
};

} // namespace srsran
