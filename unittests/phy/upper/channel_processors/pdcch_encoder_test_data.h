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

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsPDCCHEncoderUnittest.m"

#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pdcch_encoder::config_t config;
  file_vector<uint8_t>    message;
  file_vector<uint8_t>    encoded;
};

static const std::vector<test_case_t> pdcch_encoder_test_data = {
    // clang-format off
  {{108, 16486}, {"test_data/pdcch_encoder_test_input0.dat"}, {"test_data/pdcch_encoder_test_output0.dat"}},
  {{108, 38486}, {"test_data/pdcch_encoder_test_input1.dat"}, {"test_data/pdcch_encoder_test_output1.dat"}},
  {{108, 61462}, {"test_data/pdcch_encoder_test_input2.dat"}, {"test_data/pdcch_encoder_test_output2.dat"}},
  {{108, 37796}, {"test_data/pdcch_encoder_test_input3.dat"}, {"test_data/pdcch_encoder_test_output3.dat"}},
  {{108, 16334}, {"test_data/pdcch_encoder_test_input4.dat"}, {"test_data/pdcch_encoder_test_output4.dat"}},
  {{216, 35234}, {"test_data/pdcch_encoder_test_input5.dat"}, {"test_data/pdcch_encoder_test_output5.dat"}},
  {{216, 33409}, {"test_data/pdcch_encoder_test_input6.dat"}, {"test_data/pdcch_encoder_test_output6.dat"}},
  {{216, 2178}, {"test_data/pdcch_encoder_test_input7.dat"}, {"test_data/pdcch_encoder_test_output7.dat"}},
  {{216, 23604}, {"test_data/pdcch_encoder_test_input8.dat"}, {"test_data/pdcch_encoder_test_output8.dat"}},
  {{216, 56033}, {"test_data/pdcch_encoder_test_input9.dat"}, {"test_data/pdcch_encoder_test_output9.dat"}},
  {{216, 12378}, {"test_data/pdcch_encoder_test_input10.dat"}, {"test_data/pdcch_encoder_test_output10.dat"}},
  {{432, 22768}, {"test_data/pdcch_encoder_test_input11.dat"}, {"test_data/pdcch_encoder_test_output11.dat"}},
  {{432, 4541}, {"test_data/pdcch_encoder_test_input12.dat"}, {"test_data/pdcch_encoder_test_output12.dat"}},
  {{432, 47610}, {"test_data/pdcch_encoder_test_input13.dat"}, {"test_data/pdcch_encoder_test_output13.dat"}},
  {{432, 25019}, {"test_data/pdcch_encoder_test_input14.dat"}, {"test_data/pdcch_encoder_test_output14.dat"}},
  {{432, 21466}, {"test_data/pdcch_encoder_test_input15.dat"}, {"test_data/pdcch_encoder_test_output15.dat"}},
  {{432, 45009}, {"test_data/pdcch_encoder_test_input16.dat"}, {"test_data/pdcch_encoder_test_output16.dat"}},
  {{864, 5488}, {"test_data/pdcch_encoder_test_input17.dat"}, {"test_data/pdcch_encoder_test_output17.dat"}},
  {{864, 6841}, {"test_data/pdcch_encoder_test_input18.dat"}, {"test_data/pdcch_encoder_test_output18.dat"}},
  {{864, 62036}, {"test_data/pdcch_encoder_test_input19.dat"}, {"test_data/pdcch_encoder_test_output19.dat"}},
  {{864, 42252}, {"test_data/pdcch_encoder_test_input20.dat"}, {"test_data/pdcch_encoder_test_output20.dat"}},
  {{864, 54481}, {"test_data/pdcch_encoder_test_input21.dat"}, {"test_data/pdcch_encoder_test_output21.dat"}},
  {{864, 44186}, {"test_data/pdcch_encoder_test_input22.dat"}, {"test_data/pdcch_encoder_test_output22.dat"}},
  {{1728, 18631}, {"test_data/pdcch_encoder_test_input23.dat"}, {"test_data/pdcch_encoder_test_output23.dat"}},
  {{1728, 57305}, {"test_data/pdcch_encoder_test_input24.dat"}, {"test_data/pdcch_encoder_test_output24.dat"}},
  {{1728, 16393}, {"test_data/pdcch_encoder_test_input25.dat"}, {"test_data/pdcch_encoder_test_output25.dat"}},
  {{1728, 15035}, {"test_data/pdcch_encoder_test_input26.dat"}, {"test_data/pdcch_encoder_test_output26.dat"}},
  {{1728, 46750}, {"test_data/pdcch_encoder_test_input27.dat"}, {"test_data/pdcch_encoder_test_output27.dat"}},
  {{1728, 65371}, {"test_data/pdcch_encoder_test_input28.dat"}, {"test_data/pdcch_encoder_test_output28.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_TEST_DATA_H
