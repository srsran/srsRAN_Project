/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsOFDMmodulatorUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

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
  {{{0, 12, 256, cyclic_prefix::NORMAL, -0.077321, 2400000000}, 3, 0}, {"test_data/ofdm_modulator_test_input0.dat"}, {"test_data/ofdm_modulator_test_output0.dat"}},
  {{{0, 24, 512, cyclic_prefix::NORMAL, -0.31643, 2400000000}, 4, 0}, {"test_data/ofdm_modulator_test_input1.dat"}, {"test_data/ofdm_modulator_test_output1.dat"}},
  {{{0, 48, 1024, cyclic_prefix::NORMAL, 0.18179, 2400000000}, 4, 0}, {"test_data/ofdm_modulator_test_input2.dat"}, {"test_data/ofdm_modulator_test_output2.dat"}},
  {{{0, 96, 2048, cyclic_prefix::NORMAL, 0.99589, 2400000000}, 12, 0}, {"test_data/ofdm_modulator_test_input3.dat"}, {"test_data/ofdm_modulator_test_output3.dat"}},
  {{{0, 192, 4096, cyclic_prefix::NORMAL, -0.39043, 2400000000}, 8, 0}, {"test_data/ofdm_modulator_test_input4.dat"}, {"test_data/ofdm_modulator_test_output4.dat"}},
  {{{1, 12, 256, cyclic_prefix::NORMAL, 0.19839, 2400000000}, 7, 0}, {"test_data/ofdm_modulator_test_input5.dat"}, {"test_data/ofdm_modulator_test_output5.dat"}},
  {{{1, 24, 512, cyclic_prefix::NORMAL, 0.25688, 2400000000}, 7, 0}, {"test_data/ofdm_modulator_test_input6.dat"}, {"test_data/ofdm_modulator_test_output6.dat"}},
  {{{1, 48, 1024, cyclic_prefix::NORMAL, -0.43504, 2400000000}, 11, 1}, {"test_data/ofdm_modulator_test_input7.dat"}, {"test_data/ofdm_modulator_test_output7.dat"}},
  {{{1, 96, 2048, cyclic_prefix::NORMAL, 0.65067, 2400000000}, 15, 1}, {"test_data/ofdm_modulator_test_input8.dat"}, {"test_data/ofdm_modulator_test_output8.dat"}},
  {{{1, 192, 4096, cyclic_prefix::NORMAL, -0.65581, 2400000000}, 5, 0}, {"test_data/ofdm_modulator_test_input9.dat"}, {"test_data/ofdm_modulator_test_output9.dat"}},
  {{{2, 12, 256, cyclic_prefix::NORMAL, 0.44312, 2400000000}, 6, 1}, {"test_data/ofdm_modulator_test_input10.dat"}, {"test_data/ofdm_modulator_test_output10.dat"}},
  {{{2, 12, 256, cyclic_prefix::EXTENDED, 0.059091, 2400000000}, 8, 0}, {"test_data/ofdm_modulator_test_input11.dat"}, {"test_data/ofdm_modulator_test_output11.dat"}},
  {{{2, 24, 512, cyclic_prefix::NORMAL, 0.89402, 2400000000}, 1, 0}, {"test_data/ofdm_modulator_test_input12.dat"}, {"test_data/ofdm_modulator_test_output12.dat"}},
  {{{2, 24, 512, cyclic_prefix::EXTENDED, 0.75717, 2400000000}, 9, 2}, {"test_data/ofdm_modulator_test_input13.dat"}, {"test_data/ofdm_modulator_test_output13.dat"}},
  {{{2, 48, 1024, cyclic_prefix::NORMAL, -0.80326, 2400000000}, 11, 0}, {"test_data/ofdm_modulator_test_input14.dat"}, {"test_data/ofdm_modulator_test_output14.dat"}},
  {{{2, 48, 1024, cyclic_prefix::EXTENDED, -0.97861, 2400000000}, 15, 3}, {"test_data/ofdm_modulator_test_input15.dat"}, {"test_data/ofdm_modulator_test_output15.dat"}},
  {{{2, 96, 2048, cyclic_prefix::NORMAL, 0.066334, 2400000000}, 11, 1}, {"test_data/ofdm_modulator_test_input16.dat"}, {"test_data/ofdm_modulator_test_output16.dat"}},
  {{{2, 96, 2048, cyclic_prefix::EXTENDED, -0.78839, 2400000000}, 9, 2}, {"test_data/ofdm_modulator_test_input17.dat"}, {"test_data/ofdm_modulator_test_output17.dat"}},
  {{{2, 192, 4096, cyclic_prefix::NORMAL, 0.86889, 2400000000}, 15, 0}, {"test_data/ofdm_modulator_test_input18.dat"}, {"test_data/ofdm_modulator_test_output18.dat"}},
  {{{2, 192, 4096, cyclic_prefix::EXTENDED, 0.80938, 2400000000}, 1, 0}, {"test_data/ofdm_modulator_test_input19.dat"}, {"test_data/ofdm_modulator_test_output19.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H
