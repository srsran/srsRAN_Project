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
//   + "srsPBCHModulatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pbch_modulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  pbch_modulator::config_t                                config;
  file_vector<uint8_t>                                    data;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pbch_modulator_test_data = {
    // clang-format off
  {{608, 0, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input0.dat"}, {"test_data/pbch_modulator_test_output0.dat"}},
  {{451, 1, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input1.dat"}, {"test_data/pbch_modulator_test_output1.dat"}},
  {{122, 2, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input2.dat"}, {"test_data/pbch_modulator_test_output2.dat"}},
  {{447, 3, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input3.dat"}, {"test_data/pbch_modulator_test_output3.dat"}},
  {{871, 4, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input4.dat"}, {"test_data/pbch_modulator_test_output4.dat"}},
  {{771, 5, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input5.dat"}, {"test_data/pbch_modulator_test_output5.dat"}},
  {{98, 6, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input6.dat"}, {"test_data/pbch_modulator_test_output6.dat"}},
  {{745, 7, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input7.dat"}, {"test_data/pbch_modulator_test_output7.dat"}},
    // clang-format on
};

} // namespace srsran
