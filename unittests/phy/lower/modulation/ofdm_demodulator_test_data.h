/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsOFDMdemodulatorUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

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
  {{{0, 12, 256, cyclic_prefix::NORMAL, -0.041579, 2400000000}, 2, 0}, {"test_data/ofdm_demodulator_test_input0.dat"}, {"test_data/ofdm_demodulator_test_output0.dat"}},
  {{{0, 24, 512, cyclic_prefix::NORMAL, 0.78236, 2400000000}, 2, 0}, {"test_data/ofdm_demodulator_test_input1.dat"}, {"test_data/ofdm_demodulator_test_output1.dat"}},
  {{{0, 48, 1024, cyclic_prefix::NORMAL, 0.99233, 2400000000}, 0, 0}, {"test_data/ofdm_demodulator_test_input2.dat"}, {"test_data/ofdm_demodulator_test_output2.dat"}},
  {{{0, 96, 2048, cyclic_prefix::NORMAL, 0.6769, 2400000000}, 0, 0}, {"test_data/ofdm_demodulator_test_input3.dat"}, {"test_data/ofdm_demodulator_test_output3.dat"}},
  {{{0, 192, 4096, cyclic_prefix::NORMAL, -0.24407, 2400000000}, 14, 0}, {"test_data/ofdm_demodulator_test_input4.dat"}, {"test_data/ofdm_demodulator_test_output4.dat"}},
  {{{1, 12, 256, cyclic_prefix::NORMAL, -0.55135, 2400000000}, 2, 0}, {"test_data/ofdm_demodulator_test_input5.dat"}, {"test_data/ofdm_demodulator_test_output5.dat"}},
  {{{1, 24, 512, cyclic_prefix::NORMAL, -0.27703, 2400000000}, 14, 1}, {"test_data/ofdm_demodulator_test_input6.dat"}, {"test_data/ofdm_demodulator_test_output6.dat"}},
  {{{1, 48, 1024, cyclic_prefix::NORMAL, -0.61172, 2400000000}, 12, 0}, {"test_data/ofdm_demodulator_test_input7.dat"}, {"test_data/ofdm_demodulator_test_output7.dat"}},
  {{{1, 96, 2048, cyclic_prefix::NORMAL, 0.8531, 2400000000}, 5, 0}, {"test_data/ofdm_demodulator_test_input8.dat"}, {"test_data/ofdm_demodulator_test_output8.dat"}},
  {{{1, 192, 4096, cyclic_prefix::NORMAL, 0.85515, 2400000000}, 11, 0}, {"test_data/ofdm_demodulator_test_input9.dat"}, {"test_data/ofdm_demodulator_test_output9.dat"}},
  {{{2, 12, 256, cyclic_prefix::NORMAL, -0.010553, 2400000000}, 0, 1}, {"test_data/ofdm_demodulator_test_input10.dat"}, {"test_data/ofdm_demodulator_test_output10.dat"}},
  {{{2, 12, 256, cyclic_prefix::EXTENDED, 0.060099, 2400000000}, 8, 1}, {"test_data/ofdm_demodulator_test_input11.dat"}, {"test_data/ofdm_demodulator_test_output11.dat"}},
  {{{2, 24, 512, cyclic_prefix::NORMAL, 0.29863, 2400000000}, 3, 2}, {"test_data/ofdm_demodulator_test_input12.dat"}, {"test_data/ofdm_demodulator_test_output12.dat"}},
  {{{2, 24, 512, cyclic_prefix::EXTENDED, -0.24279, 2400000000}, 15, 2}, {"test_data/ofdm_demodulator_test_input13.dat"}, {"test_data/ofdm_demodulator_test_output13.dat"}},
  {{{2, 48, 1024, cyclic_prefix::NORMAL, 0.87493, 2400000000}, 1, 0}, {"test_data/ofdm_demodulator_test_input14.dat"}, {"test_data/ofdm_demodulator_test_output14.dat"}},
  {{{2, 48, 1024, cyclic_prefix::EXTENDED, 0.54672, 2400000000}, 12, 0}, {"test_data/ofdm_demodulator_test_input15.dat"}, {"test_data/ofdm_demodulator_test_output15.dat"}},
  {{{2, 96, 2048, cyclic_prefix::NORMAL, -0.055368, 2400000000}, 0, 1}, {"test_data/ofdm_demodulator_test_input16.dat"}, {"test_data/ofdm_demodulator_test_output16.dat"}},
  {{{2, 96, 2048, cyclic_prefix::EXTENDED, -0.50787, 2400000000}, 10, 1}, {"test_data/ofdm_demodulator_test_input17.dat"}, {"test_data/ofdm_demodulator_test_output17.dat"}},
  {{{2, 192, 4096, cyclic_prefix::NORMAL, -0.38778, 2400000000}, 10, 0}, {"test_data/ofdm_demodulator_test_input18.dat"}, {"test_data/ofdm_demodulator_test_output18.dat"}},
  {{{2, 192, 4096, cyclic_prefix::EXTENDED, -0.038806, 2400000000}, 10, 0}, {"test_data/ofdm_demodulator_test_input19.dat"}, {"test_data/ofdm_demodulator_test_output19.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_DEMODULATOR_TEST_DATA_H
