/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsPBCHEncoderUnittest.m"

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pbch_encoder::pbch_msg_t pbch_msg;
  file_vector<uint8_t>     encoded;
};

static const std::vector<test_case_t> pbch_encoder_test_data = {
    // clang-format off
  {{443, 0, 4, 0, {1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0}, 954, 28}, {"test_data/pbch_encoder_test_output0.dat"}},
  {{402, 0, 4, 1, {0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1}, 628, 5}, {"test_data/pbch_encoder_test_output1.dat"}},
  {{176, 0, 8, 0, {1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0}, 627, 2}, {"test_data/pbch_encoder_test_output2.dat"}},
  {{152, 0, 8, 1, {0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0}, 111, 9}, {"test_data/pbch_encoder_test_output3.dat"}},
  {{515, 0, 64, 0, {1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1}, 493, 29}, {"test_data/pbch_encoder_test_output4.dat"}},
  {{851, 0, 64, 1, {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1}, 1010, 18}, {"test_data/pbch_encoder_test_output5.dat"}},
  {{622, 1, 4, 0, {0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1}, 260, 8}, {"test_data/pbch_encoder_test_output6.dat"}},
  {{751, 1, 4, 1, {0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0}, 799, 19}, {"test_data/pbch_encoder_test_output7.dat"}},
  {{244, 1, 8, 0, {0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1}, 262, 30}, {"test_data/pbch_encoder_test_output8.dat"}},
  {{662, 1, 8, 1, {1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0}, 536, 20}, {"test_data/pbch_encoder_test_output9.dat"}},
  {{289, 1, 64, 0, {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1}, 494, 3}, {"test_data/pbch_encoder_test_output10.dat"}},
  {{811, 1, 64, 1, {0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0}, 140, 24}, {"test_data/pbch_encoder_test_output11.dat"}},
  {{134, 2, 4, 0, {0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, 812, 3}, {"test_data/pbch_encoder_test_output12.dat"}},
  {{886, 2, 4, 1, {1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1}, 488, 22}, {"test_data/pbch_encoder_test_output13.dat"}},
  {{438, 2, 8, 0, {0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, 390, 19}, {"test_data/pbch_encoder_test_output14.dat"}},
  {{71, 2, 8, 1, {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1}, 127, 9}, {"test_data/pbch_encoder_test_output15.dat"}},
  {{157, 2, 64, 0, {0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1}, 379, 3}, {"test_data/pbch_encoder_test_output16.dat"}},
  {{99, 2, 64, 1, {0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1}, 941, 22}, {"test_data/pbch_encoder_test_output17.dat"}},
  {{50, 3, 4, 0, {0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0}, 810, 19}, {"test_data/pbch_encoder_test_output18.dat"}},
  {{540, 3, 4, 1, {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1}, 21, 15}, {"test_data/pbch_encoder_test_output19.dat"}},
  {{361, 3, 8, 0, {1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0}, 337, 13}, {"test_data/pbch_encoder_test_output20.dat"}},
  {{471, 3, 8, 1, {1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1}, 735, 16}, {"test_data/pbch_encoder_test_output21.dat"}},
  {{595, 3, 64, 0, {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1}, 651, 28}, {"test_data/pbch_encoder_test_output22.dat"}},
  {{649, 3, 64, 1, {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0}, 624, 14}, {"test_data/pbch_encoder_test_output23.dat"}},
  {{571, 4, 8, 0, {0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0}, 566, 21}, {"test_data/pbch_encoder_test_output24.dat"}},
  {{370, 4, 8, 1, {0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1}, 55, 4}, {"test_data/pbch_encoder_test_output25.dat"}},
  {{773, 4, 64, 0, {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0}, 738, 3}, {"test_data/pbch_encoder_test_output26.dat"}},
  {{831, 4, 64, 1, {1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1}, 321, 9}, {"test_data/pbch_encoder_test_output27.dat"}},
  {{721, 5, 8, 0, {1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}, 101, 19}, {"test_data/pbch_encoder_test_output28.dat"}},
  {{126, 5, 8, 1, {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0}, 465, 19}, {"test_data/pbch_encoder_test_output29.dat"}},
  {{500, 5, 64, 0, {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1}, 618, 29}, {"test_data/pbch_encoder_test_output30.dat"}},
  {{860, 5, 64, 1, {1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1}, 22, 19}, {"test_data/pbch_encoder_test_output31.dat"}},
  {{701, 6, 8, 0, {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0}, 986, 3}, {"test_data/pbch_encoder_test_output32.dat"}},
  {{557, 6, 8, 1, {1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1}, 731, 4}, {"test_data/pbch_encoder_test_output33.dat"}},
  {{403, 6, 64, 0, {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1}, 153, 7}, {"test_data/pbch_encoder_test_output34.dat"}},
  {{354, 6, 64, 1, {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1}, 207, 27}, {"test_data/pbch_encoder_test_output35.dat"}},
  {{469, 7, 8, 0, {1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1}, 844, 28}, {"test_data/pbch_encoder_test_output36.dat"}},
  {{107, 7, 8, 1, {1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0}, 401, 9}, {"test_data/pbch_encoder_test_output37.dat"}},
  {{348, 7, 64, 0, {1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1}, 956, 14}, {"test_data/pbch_encoder_test_output38.dat"}},
  {{688, 7, 64, 1, {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1}, 937, 8}, {"test_data/pbch_encoder_test_output39.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_ENCODER_TEST_DATA_H
