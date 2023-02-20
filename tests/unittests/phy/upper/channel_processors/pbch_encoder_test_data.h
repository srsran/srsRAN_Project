/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

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
 {{906, 0, 4, 0, {1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0}, 401, 15}, {"test_data/pbch_encoder_test_output0.dat"}},
 {{802, 0, 4, 1, {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0}, 892, 8}, {"test_data/pbch_encoder_test_output1.dat"}},
 {{66, 0, 8, 0, {0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1}, 432, 6}, {"test_data/pbch_encoder_test_output2.dat"}},
 {{967, 0, 8, 1, {1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0}, 260, 9}, {"test_data/pbch_encoder_test_output3.dat"}},
 {{271, 0, 64, 0, {1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1}, 99, 10}, {"test_data/pbch_encoder_test_output4.dat"}},
 {{726, 0, 64, 1, {1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1}, 911, 9}, {"test_data/pbch_encoder_test_output5.dat"}},
 {{789, 1, 4, 0, {0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0}, 273, 20}, {"test_data/pbch_encoder_test_output6.dat"}},
 {{508, 1, 4, 1, {1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1}, 49, 15}, {"test_data/pbch_encoder_test_output7.dat"}},
 {{506, 1, 8, 0, {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0}, 327, 5}, {"test_data/pbch_encoder_test_output8.dat"}},
 {{580, 1, 8, 1, {0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0}, 174, 18}, {"test_data/pbch_encoder_test_output9.dat"}},
 {{621, 1, 64, 0, {0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0}, 621, 8}, {"test_data/pbch_encoder_test_output10.dat"}},
 {{340, 1, 64, 1, {1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1}, 836, 6}, {"test_data/pbch_encoder_test_output11.dat"}},
 {{440, 2, 4, 0, {0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1}, 578, 20}, {"test_data/pbch_encoder_test_output12.dat"}},
 {{78, 2, 4, 1, {0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0}, 277, 22}, {"test_data/pbch_encoder_test_output13.dat"}},
 {{826, 2, 8, 0, {1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1}, 853, 22}, {"test_data/pbch_encoder_test_output14.dat"}},
 {{595, 2, 8, 1, {0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0}, 888, 4}, {"test_data/pbch_encoder_test_output15.dat"}},
 {{406, 2, 64, 0, {0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0}, 931, 10}, {"test_data/pbch_encoder_test_output16.dat"}},
 {{497, 2, 64, 1, {1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0}, 988, 4}, {"test_data/pbch_encoder_test_output17.dat"}},
 {{605, 3, 4, 0, {1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0}, 122, 5}, {"test_data/pbch_encoder_test_output18.dat"}},
 {{654, 3, 4, 1, {0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1}, 165, 5}, {"test_data/pbch_encoder_test_output19.dat"}},
 {{90, 3, 8, 0, {0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1}, 402, 16}, {"test_data/pbch_encoder_test_output20.dat"}},
 {{362, 3, 8, 1, {0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1}, 924, 7}, {"test_data/pbch_encoder_test_output21.dat"}},
 {{72, 3, 64, 0, {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, 148, 1}, {"test_data/pbch_encoder_test_output22.dat"}},
 {{420, 3, 64, 1, {0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0}, 298, 10}, {"test_data/pbch_encoder_test_output23.dat"}},
 {{989, 4, 8, 0, {0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 981, 11}, {"test_data/pbch_encoder_test_output24.dat"}},
 {{236, 4, 8, 1, {0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0}, 182, 19}, {"test_data/pbch_encoder_test_output25.dat"}},
 {{728, 4, 64, 0, {1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0}, 427, 6}, {"test_data/pbch_encoder_test_output26.dat"}},
 {{904, 4, 64, 1, {0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0}, 393, 2}, {"test_data/pbch_encoder_test_output27.dat"}},
 {{798, 5, 8, 0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1}, 852, 22}, {"test_data/pbch_encoder_test_output28.dat"}},
 {{870, 5, 8, 1, {0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1}, 874, 6}, {"test_data/pbch_encoder_test_output29.dat"}},
 {{164, 5, 64, 0, {0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1}, 999, 7}, {"test_data/pbch_encoder_test_output30.dat"}},
 {{241, 5, 64, 1, {1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1}, 11, 4}, {"test_data/pbch_encoder_test_output31.dat"}},
 {{418, 6, 8, 0, {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1}, 552, 13}, {"test_data/pbch_encoder_test_output32.dat"}},
 {{647, 6, 8, 1, {1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0}, 548, 4}, {"test_data/pbch_encoder_test_output33.dat"}},
 {{265, 6, 64, 0, {0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0}, 201, 9}, {"test_data/pbch_encoder_test_output34.dat"}},
 {{398, 6, 64, 1, {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1}, 435, 6}, {"test_data/pbch_encoder_test_output35.dat"}},
 {{178, 7, 8, 0, {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1}, 817, 17}, {"test_data/pbch_encoder_test_output36.dat"}},
 {{536, 7, 8, 1, {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1}, 0, 6}, {"test_data/pbch_encoder_test_output37.dat"}},
 {{216, 7, 64, 0, {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1}, 67, 8}, {"test_data/pbch_encoder_test_output38.dat"}},
 {{505, 7, 64, 1, {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1}, 615, 10}, {"test_data/pbch_encoder_test_output39.dat"}},
    // clang-format on
};

} // namespace srsgnb
