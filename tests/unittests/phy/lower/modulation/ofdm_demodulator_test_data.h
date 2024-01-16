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
//   + "srsOFDMDemodulatorUnittest.m"

#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct ofdm_demodulator_test_configuration {
  ofdm_demodulator_configuration config;
  uint8_t                        port_idx;
  uint8_t                        slot_idx;
};

struct test_case_t {
  ofdm_demodulator_test_configuration                     test_config;
  file_vector<cf_t>                                       data;
  file_vector<resource_grid_writer_spy::expected_entry_t> demodulated;
};

static const std::vector<test_case_t> ofdm_demodulator_test_data = {
    // clang-format off
  {{{0, 12, 256, cyclic_prefix::NORMAL, 1, 0.81158, 2740100000}, 13, 0}, {"test_data/ofdm_demodulator_test_input0.dat"}, {"test_data/ofdm_demodulator_test_output0.dat"}},
  {{{0, 24, 512, cyclic_prefix::NORMAL, 11, -0.71789, 2073200000}, 13, 0}, {"test_data/ofdm_demodulator_test_input1.dat"}, {"test_data/ofdm_demodulator_test_output1.dat"}},
  {{{0, 48, 1024, cyclic_prefix::NORMAL, 34, 0.034698, 2558500000}, 8, 0}, {"test_data/ofdm_demodulator_test_input2.dat"}, {"test_data/ofdm_demodulator_test_output2.dat"}},
  {{{0, 96, 2048, cyclic_prefix::NORMAL, 56, -0.33197, 2364100000}, 0, 0}, {"test_data/ofdm_demodulator_test_input3.dat"}, {"test_data/ofdm_demodulator_test_output3.dat"}},
  {{{0, 192, 4096, cyclic_prefix::NORMAL, 87, 0.45209, 371700000}, 1, 0}, {"test_data/ofdm_demodulator_test_input4.dat"}, {"test_data/ofdm_demodulator_test_output4.dat"}},
  {{{1, 12, 256, cyclic_prefix::NORMAL, 2, -0.97497, 377600000}, 1, 1}, {"test_data/ofdm_demodulator_test_input5.dat"}, {"test_data/ofdm_demodulator_test_output5.dat"}},
  {{{1, 24, 512, cyclic_prefix::NORMAL, 14, -0.61155, 2883600000}, 2, 0}, {"test_data/ofdm_demodulator_test_input6.dat"}, {"test_data/ofdm_demodulator_test_output6.dat"}},
  {{{1, 48, 1024, cyclic_prefix::NORMAL, 18, -0.65414, 1498600000}, 0, 1}, {"test_data/ofdm_demodulator_test_input7.dat"}, {"test_data/ofdm_demodulator_test_output7.dat"}},
  {{{1, 96, 2048, cyclic_prefix::NORMAL, 39, 0.56578, 1344000000}, 1, 0}, {"test_data/ofdm_demodulator_test_input8.dat"}, {"test_data/ofdm_demodulator_test_output8.dat"}},
  {{{1, 192, 4096, cyclic_prefix::NORMAL, 129, 0.45813, 1570600000}, 4, 1}, {"test_data/ofdm_demodulator_test_input9.dat"}, {"test_data/ofdm_demodulator_test_output9.dat"}},
  {{{2, 12, 256, cyclic_prefix::NORMAL, 8, 0.2897, 11800000}, 6, 1}, {"test_data/ofdm_demodulator_test_input10.dat"}, {"test_data/ofdm_demodulator_test_output10.dat"}},
  {{{2, 12, 256, cyclic_prefix::EXTENDED, 2, -0.045445, 2801600000}, 8, 0}, {"test_data/ofdm_demodulator_test_input11.dat"}, {"test_data/ofdm_demodulator_test_output11.dat"}},
  {{{2, 24, 512, cyclic_prefix::NORMAL, 10, -0.10515, 2156600000}, 12, 3}, {"test_data/ofdm_demodulator_test_input12.dat"}, {"test_data/ofdm_demodulator_test_output12.dat"}},
  {{{2, 24, 512, cyclic_prefix::EXTENDED, 17, 0.14064, 432600000}, 12, 2}, {"test_data/ofdm_demodulator_test_input13.dat"}, {"test_data/ofdm_demodulator_test_output13.dat"}},
  {{{2, 48, 1024, cyclic_prefix::NORMAL, 6, 0.71586, 1187300000}, 7, 3}, {"test_data/ofdm_demodulator_test_input14.dat"}, {"test_data/ofdm_demodulator_test_output14.dat"}},
  {{{2, 48, 1024, cyclic_prefix::EXTENDED, 7, -0.36905, 257300000}, 8, 2}, {"test_data/ofdm_demodulator_test_input15.dat"}, {"test_data/ofdm_demodulator_test_output15.dat"}},
  {{{2, 96, 2048, cyclic_prefix::NORMAL, 10, -0.034334, 14600000}, 12, 2}, {"test_data/ofdm_demodulator_test_input16.dat"}, {"test_data/ofdm_demodulator_test_output16.dat"}},
  {{{2, 96, 2048, cyclic_prefix::EXTENDED, 9, -0.24786, 587300000}, 5, 1}, {"test_data/ofdm_demodulator_test_input17.dat"}, {"test_data/ofdm_demodulator_test_output17.dat"}},
  {{{2, 192, 4096, cyclic_prefix::NORMAL, 138, 0.79273, 409800000}, 11, 0}, {"test_data/ofdm_demodulator_test_input18.dat"}, {"test_data/ofdm_demodulator_test_output18.dat"}},
  {{{2, 192, 4096, cyclic_prefix::EXTENDED, 129, -0.5615, 2965400000}, 8, 3}, {"test_data/ofdm_demodulator_test_input19.dat"}, {"test_data/ofdm_demodulator_test_output19.dat"}},
    // clang-format on
};

} // namespace srsran
