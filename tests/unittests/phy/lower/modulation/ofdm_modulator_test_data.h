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
//   + "srsOFDMModulatorUnittest.m"

#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct ofdm_modulator_test_configuration {
  ofdm_modulator_configuration config;
  uint8_t                      port_idx;
  uint8_t                      slot_idx;
};

struct test_case_t {
  ofdm_modulator_test_configuration                       test_config;
  file_vector<resource_grid_writer_spy::expected_entry_t> data;
  file_vector<cf_t>                                       modulated;
};

static const std::vector<test_case_t> ofdm_modulator_test_data = {
    // clang-format off
  {{{0, 12, 256, cyclic_prefix::NORMAL, 0.81158, 2740100000}, 13, 0}, {"test_data/ofdm_modulator_test_input0.dat"}, {"test_data/ofdm_modulator_test_output0.dat"}},
  {{{0, 24, 512, cyclic_prefix::NORMAL, 0.67645, 2196700000}, 7, 0}, {"test_data/ofdm_modulator_test_input1.dat"}, {"test_data/ofdm_modulator_test_output1.dat"}},
  {{{0, 48, 1024, cyclic_prefix::NORMAL, 0.356, 1552000000}, 1, 0}, {"test_data/ofdm_modulator_test_input2.dat"}, {"test_data/ofdm_modulator_test_output2.dat"}},
  {{{0, 96, 2048, cyclic_prefix::NORMAL, 0.93184, 97900000}, 10, 0}, {"test_data/ofdm_modulator_test_input3.dat"}, {"test_data/ofdm_modulator_test_output3.dat"}},
  {{{0, 192, 4096, cyclic_prefix::NORMAL, 0.87523, 1424700000}, 1, 0}, {"test_data/ofdm_modulator_test_input4.dat"}, {"test_data/ofdm_modulator_test_output4.dat"}},
  {{{1, 12, 256, cyclic_prefix::NORMAL, -0.77438, 2686500000}, 9, 0}, {"test_data/ofdm_modulator_test_input5.dat"}, {"test_data/ofdm_modulator_test_output5.dat"}},
  {{{1, 24, 512, cyclic_prefix::NORMAL, -0.47474, 293200000}, 8, 0}, {"test_data/ofdm_modulator_test_input6.dat"}, {"test_data/ofdm_modulator_test_output6.dat"}},
  {{{1, 48, 1024, cyclic_prefix::NORMAL, 0.27046, 1607000000}, 9, 1}, {"test_data/ofdm_modulator_test_input7.dat"}, {"test_data/ofdm_modulator_test_output7.dat"}},
  {{{1, 96, 2048, cyclic_prefix::NORMAL, 0.17815, 1462800000}, 15, 1}, {"test_data/ofdm_modulator_test_input8.dat"}, {"test_data/ofdm_modulator_test_output8.dat"}},
  {{{1, 192, 4096, cyclic_prefix::NORMAL, -0.54681, 619800000}, 1, 1}, {"test_data/ofdm_modulator_test_input9.dat"}, {"test_data/ofdm_modulator_test_output9.dat"}},
  {{{2, 12, 256, cyclic_prefix::NORMAL, -0.4685, 1773500000}, 6, 3}, {"test_data/ofdm_modulator_test_input10.dat"}, {"test_data/ofdm_modulator_test_output10.dat"}},
  {{{2, 12, 256, cyclic_prefix::EXTENDED, 0.73538, 837900000}, 1, 2}, {"test_data/ofdm_modulator_test_input11.dat"}, {"test_data/ofdm_modulator_test_output11.dat"}},
  {{{2, 24, 512, cyclic_prefix::NORMAL, 0.25911, 833300000}, 11, 3}, {"test_data/ofdm_modulator_test_input12.dat"}, {"test_data/ofdm_modulator_test_output12.dat"}},
  {{{2, 24, 512, cyclic_prefix::EXTENDED, -0.98178, 1502400000}, 2, 3}, {"test_data/ofdm_modulator_test_input13.dat"}, {"test_data/ofdm_modulator_test_output13.dat"}},
  {{{2, 48, 1024, cyclic_prefix::NORMAL, -0.14768, 2527000000}, 1, 3}, {"test_data/ofdm_modulator_test_input14.dat"}, {"test_data/ofdm_modulator_test_output14.dat"}},
  {{{2, 48, 1024, cyclic_prefix::EXTENDED, -0.12453, 383700000}, 9, 2}, {"test_data/ofdm_modulator_test_input15.dat"}, {"test_data/ofdm_modulator_test_output15.dat"}},
  {{{2, 96, 2048, cyclic_prefix::NORMAL, -0.89705, 2318900000}, 9, 1}, {"test_data/ofdm_modulator_test_input16.dat"}, {"test_data/ofdm_modulator_test_output16.dat"}},
  {{{2, 96, 2048, cyclic_prefix::EXTENDED, -0.93258, 2059300000}, 14, 3}, {"test_data/ofdm_modulator_test_input17.dat"}, {"test_data/ofdm_modulator_test_output17.dat"}},
  {{{2, 192, 4096, cyclic_prefix::NORMAL, -0.10732, 76300000}, 5, 1}, {"test_data/ofdm_modulator_test_input18.dat"}, {"test_data/ofdm_modulator_test_output18.dat"}},
  {{{2, 192, 4096, cyclic_prefix::EXTENDED, -0.82195, 605900000}, 3, 2}, {"test_data/ofdm_modulator_test_input19.dat"}, {"test_data/ofdm_modulator_test_output19.dat"}},
    // clang-format on
};

} // namespace srsran
