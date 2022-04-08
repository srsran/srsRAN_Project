#ifndef SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsOFDMmodulatorUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/support/file_vector.h"
#include <array>

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
  {{0, 6, 128, cyclic_prefix::NORMAL, -0.52766, 5, 0},{"ofdm_modulator_test_input0.dat"},{"ofdm_modulator_test_output0.dat"}},
  {{0, 12, 256, cyclic_prefix::NORMAL, 0.37754, 3, 0},{"ofdm_modulator_test_input1.dat"},{"ofdm_modulator_test_output1.dat"}},
  {{0, 24, 512, cyclic_prefix::NORMAL, -0.40533, 0, 0},{"ofdm_modulator_test_input2.dat"},{"ofdm_modulator_test_output2.dat"}},
  {{0, 48, 1024, cyclic_prefix::NORMAL, 0.51414, 14, 0},{"ofdm_modulator_test_input3.dat"},{"ofdm_modulator_test_output3.dat"}},
  {{0, 96, 2048, cyclic_prefix::NORMAL, 0.83207, 11, 0},{"ofdm_modulator_test_input4.dat"},{"ofdm_modulator_test_output4.dat"}},
  {{0, 192, 4096, cyclic_prefix::NORMAL, -0.65235, 8, 0},{"ofdm_modulator_test_input5.dat"},{"ofdm_modulator_test_output5.dat"}},
  {{1, 6, 128, cyclic_prefix::NORMAL, 0.51243, 0, 0},{"ofdm_modulator_test_input6.dat"},{"ofdm_modulator_test_output6.dat"}},
  {{1, 12, 256, cyclic_prefix::NORMAL, -0.72877, 0, 0},{"ofdm_modulator_test_input7.dat"},{"ofdm_modulator_test_output7.dat"}},
  {{1, 24, 512, cyclic_prefix::NORMAL, -0.52994, 1, 0},{"ofdm_modulator_test_input8.dat"},{"ofdm_modulator_test_output8.dat"}},
  {{1, 48, 1024, cyclic_prefix::NORMAL, -0.87962, 10, 1},{"ofdm_modulator_test_input9.dat"},{"ofdm_modulator_test_output9.dat"}},
  {{1, 96, 2048, cyclic_prefix::NORMAL, 0.8624, 4, 1},{"ofdm_modulator_test_input10.dat"},{"ofdm_modulator_test_output10.dat"}},
  {{1, 192, 4096, cyclic_prefix::NORMAL, -0.4099, 5, 1},{"ofdm_modulator_test_input11.dat"},{"ofdm_modulator_test_output11.dat"}},
  {{2, 6, 128, cyclic_prefix::NORMAL, 0.19388, 8, 1},{"ofdm_modulator_test_input12.dat"},{"ofdm_modulator_test_output12.dat"}},
  {{2, 6, 128, cyclic_prefix::EXTENDED, 0.30468, 14, 2},{"ofdm_modulator_test_input13.dat"},{"ofdm_modulator_test_output13.dat"}},
  {{2, 12, 256, cyclic_prefix::NORMAL, -0.57765, 11, 0},{"ofdm_modulator_test_input14.dat"},{"ofdm_modulator_test_output14.dat"}},
  {{2, 12, 256, cyclic_prefix::EXTENDED, 0.99392, 2, 3},{"ofdm_modulator_test_input15.dat"},{"ofdm_modulator_test_output15.dat"}},
  {{2, 24, 512, cyclic_prefix::NORMAL, 0.57916, 0, 2},{"ofdm_modulator_test_input16.dat"},{"ofdm_modulator_test_output16.dat"}},
  {{2, 24, 512, cyclic_prefix::EXTENDED, 0.98211, 9, 0},{"ofdm_modulator_test_input17.dat"},{"ofdm_modulator_test_output17.dat"}},
  {{2, 48, 1024, cyclic_prefix::NORMAL, 0.71987, 4, 2},{"ofdm_modulator_test_input18.dat"},{"ofdm_modulator_test_output18.dat"}},
  {{2, 48, 1024, cyclic_prefix::EXTENDED, -0.6024, 13, 1},{"ofdm_modulator_test_input19.dat"},{"ofdm_modulator_test_output19.dat"}},
  {{2, 96, 2048, cyclic_prefix::NORMAL, 0.38683, 5, 2},{"ofdm_modulator_test_input20.dat"},{"ofdm_modulator_test_output20.dat"}},
  {{2, 96, 2048, cyclic_prefix::EXTENDED, -0.54333, 4, 1},{"ofdm_modulator_test_input21.dat"},{"ofdm_modulator_test_output21.dat"}},
  {{2, 192, 4096, cyclic_prefix::NORMAL, 0.59278, 10, 2},{"ofdm_modulator_test_input22.dat"},{"ofdm_modulator_test_output22.dat"}},
  {{2, 192, 4096, cyclic_prefix::EXTENDED, -0.16519, 8, 0},{"ofdm_modulator_test_input23.dat"},{"ofdm_modulator_test_output23.dat"}},
  {{3, 6, 128, cyclic_prefix::NORMAL, 0.81072, 15, 4},{"ofdm_modulator_test_input24.dat"},{"ofdm_modulator_test_output24.dat"}},
  {{3, 12, 256, cyclic_prefix::NORMAL, -0.9229, 7, 1},{"ofdm_modulator_test_input25.dat"},{"ofdm_modulator_test_output25.dat"}},
  {{3, 24, 512, cyclic_prefix::NORMAL, 0.69403, 8, 4},{"ofdm_modulator_test_input26.dat"},{"ofdm_modulator_test_output26.dat"}},
  {{3, 48, 1024, cyclic_prefix::NORMAL, 0.8914, 15, 1},{"ofdm_modulator_test_input27.dat"},{"ofdm_modulator_test_output27.dat"}},
  {{3, 96, 2048, cyclic_prefix::NORMAL, 0.32534, 6, 2},{"ofdm_modulator_test_input28.dat"},{"ofdm_modulator_test_output28.dat"}},
  {{3, 192, 4096, cyclic_prefix::NORMAL, 0.28493, 7, 5},{"ofdm_modulator_test_input29.dat"},{"ofdm_modulator_test_output29.dat"}},
  {{4, 6, 128, cyclic_prefix::NORMAL, 0.81134, 1, 13},{"ofdm_modulator_test_input30.dat"},{"ofdm_modulator_test_output30.dat"}},
  {{4, 12, 256, cyclic_prefix::NORMAL, 0.89082, 10, 9},{"ofdm_modulator_test_input31.dat"},{"ofdm_modulator_test_output31.dat"}},
  {{4, 24, 512, cyclic_prefix::NORMAL, 0.68267, 7, 0},{"ofdm_modulator_test_input32.dat"},{"ofdm_modulator_test_output32.dat"}},
  {{4, 48, 1024, cyclic_prefix::NORMAL, -0.19869, 2, 11},{"ofdm_modulator_test_input33.dat"},{"ofdm_modulator_test_output33.dat"}},
  {{4, 96, 2048, cyclic_prefix::NORMAL, -0.79016, 1, 6},{"ofdm_modulator_test_input34.dat"},{"ofdm_modulator_test_output34.dat"}},
  {{4, 192, 4096, cyclic_prefix::NORMAL, 0.54231, 13, 15},{"ofdm_modulator_test_input35.dat"},{"ofdm_modulator_test_output35.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H
