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

// This file was generated using the following MATLAB class:
//   + "srsPBCHModulatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pbch_modulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pbch_modulator::config_t                                config;
  file_vector<uint8_t>                                    data;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pbch_modulator_test_data = {
    // clang-format off
  {{899, 0, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input0.dat"}, {"test_data/pbch_modulator_test_output0.dat"}},
  {{495, 1, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input1.dat"}, {"test_data/pbch_modulator_test_output1.dat"}},
  {{757, 2, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input2.dat"}, {"test_data/pbch_modulator_test_output2.dat"}},
  {{658, 3, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input3.dat"}, {"test_data/pbch_modulator_test_output3.dat"}},
  {{540, 4, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input4.dat"}, {"test_data/pbch_modulator_test_output4.dat"}},
  {{411, 5, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input5.dat"}, {"test_data/pbch_modulator_test_output5.dat"}},
  {{2, 6, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input6.dat"}, {"test_data/pbch_modulator_test_output6.dat"}},
  {{703, 7, 0, 0, 1, {0}}, {"test_data/pbch_modulator_test_input7.dat"}, {"test_data/pbch_modulator_test_output7.dat"}},
    // clang-format on
};

} // namespace srsgnb
