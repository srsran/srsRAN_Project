#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsPBCHEncoderUnittest.m"

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pbch_encoder::pbch_msg_t pbch_msg;
  file_vector<uint8_t> encoded;
};

static const std::vector<test_case_t> pbch_encoder_test_data = {
    // clang-format off
  {{160, 0, 4, 0, {1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0}, 49, 15},{"pbch_encoder_test_output0.dat"}},
  {{330, 0, 4, 1, {1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1}, 948, 11},{"pbch_encoder_test_output1.dat"}},
  {{335, 0, 8, 0, {0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0}, 144, 8},{"pbch_encoder_test_output2.dat"}},
  {{93, 0, 8, 1, {1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1}, 805, 17},{"pbch_encoder_test_output3.dat"}},
  {{769, 0, 64, 0, {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1}, 644, 9},{"pbch_encoder_test_output4.dat"}},
  {{323, 0, 64, 1, {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}, 804, 29},{"pbch_encoder_test_output5.dat"}},
  {{652, 1, 4, 0, {0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1}, 632, 29},{"pbch_encoder_test_output6.dat"}},
  {{560, 1, 4, 1, {1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0}, 823, 7},{"pbch_encoder_test_output7.dat"}},
  {{601, 1, 8, 0, {0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1}, 594, 25},{"pbch_encoder_test_output8.dat"}},
  {{516, 1, 8, 1, {0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0}, 724, 18},{"pbch_encoder_test_output9.dat"}},
  {{770, 1, 64, 0, {0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0}, 572, 0},{"pbch_encoder_test_output10.dat"}},
  {{709, 1, 64, 1, {1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0}, 913, 9},{"pbch_encoder_test_output11.dat"}},
  {{856, 2, 4, 0, {0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1}, 635, 0},{"pbch_encoder_test_output12.dat"}},
  {{528, 2, 4, 1, {0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0}, 825, 24},{"pbch_encoder_test_output13.dat"}},
  {{530, 2, 8, 0, {1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0}, 342, 29},{"pbch_encoder_test_output14.dat"}},
  {{351, 2, 8, 1, {0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1}, 546, 5},{"pbch_encoder_test_output15.dat"}},
  {{801, 2, 64, 0, {0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0}, 909, 31},{"pbch_encoder_test_output16.dat"}},
  {{409, 2, 64, 1, {1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0}, 875, 19},{"pbch_encoder_test_output17.dat"}},
  {{463, 3, 4, 0, {0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0}, 167, 31},{"pbch_encoder_test_output18.dat"}},
  {{830, 3, 4, 1, {0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0}, 841, 2},{"pbch_encoder_test_output19.dat"}},
  {{273, 3, 8, 0, {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1}, 738, 24},{"pbch_encoder_test_output20.dat"}},
  {{755, 3, 8, 1, {1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1}, 854, 31},{"pbch_encoder_test_output21.dat"}},
  {{251, 3, 64, 0, {0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1}, 122, 31},{"pbch_encoder_test_output22.dat"}},
  {{910, 3, 64, 1, {1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1}, 20, 10},{"pbch_encoder_test_output23.dat"}},
  {{848, 4, 8, 0, {0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1}, 618, 16},{"pbch_encoder_test_output24.dat"}},
  {{710, 4, 8, 1, {0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1}, 375, 29},{"pbch_encoder_test_output25.dat"}},
  {{722, 4, 64, 0, {1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1}, 927, 23},{"pbch_encoder_test_output26.dat"}},
  {{277, 4, 64, 1, {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0}, 969, 7},{"pbch_encoder_test_output27.dat"}},
  {{557, 5, 8, 0, {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0}, 460, 28},{"pbch_encoder_test_output28.dat"}},
  {{622, 5, 8, 1, {0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1}, 187, 21},{"pbch_encoder_test_output29.dat"}},
  {{96, 5, 64, 0, {1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0}, 831, 19},{"pbch_encoder_test_output30.dat"}},
  {{965, 5, 64, 1, {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0}, 656, 21},{"pbch_encoder_test_output31.dat"}},
  {{862, 6, 8, 0, {1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0}, 341, 9},{"pbch_encoder_test_output32.dat"}},
  {{580, 6, 8, 1, {0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1}, 626, 23},{"pbch_encoder_test_output33.dat"}},
  {{827, 6, 64, 0, {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1}, 340, 1},{"pbch_encoder_test_output34.dat"}},
  {{353, 6, 64, 1, {0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0}, 501, 18},{"pbch_encoder_test_output35.dat"}},
  {{162, 7, 8, 0, {0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0}, 824, 21},{"pbch_encoder_test_output36.dat"}},
  {{406, 7, 8, 1, {0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1}, 722, 24},{"pbch_encoder_test_output37.dat"}},
  {{780, 7, 64, 0, {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1}, 769, 11},{"pbch_encoder_test_output38.dat"}},
  {{863, 7, 64, 1, {1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1}, 153, 19},{"pbch_encoder_test_output39.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H
