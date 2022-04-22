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
  {{{0, 12, 256, cyclic_prefix::NORMAL, -0.075446, 2400000000}, 15, 0},{"ofdm_modulator_test_input0.dat"},{"ofdm_modulator_test_output0.dat"}},
  {{{0, 24, 512, cyclic_prefix::NORMAL, -0.02374, 2400000000}, 12, 0},{"ofdm_modulator_test_input1.dat"},{"ofdm_modulator_test_output1.dat"}},
  {{{0, 48, 1024, cyclic_prefix::NORMAL, 0.37296, 2400000000}, 9, 0},{"ofdm_modulator_test_input2.dat"},{"ofdm_modulator_test_output2.dat"}},
  {{{0, 96, 2048, cyclic_prefix::NORMAL, 0.074953, 2400000000}, 2, 0},{"ofdm_modulator_test_input3.dat"},{"ofdm_modulator_test_output3.dat"}},
  {{{0, 192, 4096, cyclic_prefix::NORMAL, 0.71332, 2400000000}, 10, 0},{"ofdm_modulator_test_input4.dat"},{"ofdm_modulator_test_output4.dat"}},
  {{{1, 12, 256, cyclic_prefix::NORMAL, 0.21169, 2400000000}, 14, 1},{"ofdm_modulator_test_input5.dat"},{"ofdm_modulator_test_output5.dat"}},
  {{{1, 24, 512, cyclic_prefix::NORMAL, -0.20238, 2400000000}, 0, 1},{"ofdm_modulator_test_input6.dat"},{"ofdm_modulator_test_output6.dat"}},
  {{{1, 48, 1024, cyclic_prefix::NORMAL, -0.69719, 2400000000}, 4, 0},{"ofdm_modulator_test_input7.dat"},{"ofdm_modulator_test_output7.dat"}},
  {{{1, 96, 2048, cyclic_prefix::NORMAL, 0.65561, 2400000000}, 9, 0},{"ofdm_modulator_test_input8.dat"},{"ofdm_modulator_test_output8.dat"}},
  {{{1, 192, 4096, cyclic_prefix::NORMAL, 0.58504, 2400000000}, 13, 1},{"ofdm_modulator_test_input9.dat"},{"ofdm_modulator_test_output9.dat"}},
  {{{2, 12, 256, cyclic_prefix::NORMAL, -0.060158, 2400000000}, 15, 1},{"ofdm_modulator_test_input10.dat"},{"ofdm_modulator_test_output10.dat"}},
  {{{2, 12, 256, cyclic_prefix::EXTENDED, 0.33799, 2400000000}, 10, 2},{"ofdm_modulator_test_input11.dat"},{"ofdm_modulator_test_output11.dat"}},
  {{{2, 24, 512, cyclic_prefix::NORMAL, 0.9345, 2400000000}, 13, 3},{"ofdm_modulator_test_input12.dat"},{"ofdm_modulator_test_output12.dat"}},
  {{{2, 24, 512, cyclic_prefix::EXTENDED, -0.19516, 2400000000}, 9, 3},{"ofdm_modulator_test_input13.dat"},{"ofdm_modulator_test_output13.dat"}},
  {{{2, 48, 1024, cyclic_prefix::NORMAL, -0.67163, 2400000000}, 2, 3},{"ofdm_modulator_test_input14.dat"},{"ofdm_modulator_test_output14.dat"}},
  {{{2, 48, 1024, cyclic_prefix::EXTENDED, 0.96446, 2400000000}, 11, 2},{"ofdm_modulator_test_input15.dat"},{"ofdm_modulator_test_output15.dat"}},
  {{{2, 96, 2048, cyclic_prefix::NORMAL, -0.78495, 2400000000}, 7, 3},{"ofdm_modulator_test_input16.dat"},{"ofdm_modulator_test_output16.dat"}},
  {{{2, 96, 2048, cyclic_prefix::EXTENDED, 0.55779, 2400000000}, 9, 3},{"ofdm_modulator_test_input17.dat"},{"ofdm_modulator_test_output17.dat"}},
  {{{2, 192, 4096, cyclic_prefix::NORMAL, -0.23754, 2400000000}, 11, 0},{"ofdm_modulator_test_input18.dat"},{"ofdm_modulator_test_output18.dat"}},
  {{{2, 192, 4096, cyclic_prefix::EXTENDED, -0.052593, 2400000000}, 5, 2},{"ofdm_modulator_test_input19.dat"},{"ofdm_modulator_test_output19.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_LOWER_MODULATION_OFDM_MODULATOR_TEST_DATA_H
