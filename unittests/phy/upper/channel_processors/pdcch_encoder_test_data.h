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
    {{108, 25692}, {"pdcch_encoder_test_input0.dat"}, {"pdcch_encoder_test_output0.dat"}},
    {{108, 50536}, {"pdcch_encoder_test_input1.dat"}, {"pdcch_encoder_test_output1.dat"}},
    {{108, 49918}, {"pdcch_encoder_test_input2.dat"}, {"pdcch_encoder_test_output2.dat"}},
    {{108, 59428}, {"pdcch_encoder_test_input3.dat"}, {"pdcch_encoder_test_output3.dat"}},
    {{108, 32903}, {"pdcch_encoder_test_input4.dat"}, {"pdcch_encoder_test_output4.dat"}},
    {{216, 32761}, {"pdcch_encoder_test_input5.dat"}, {"pdcch_encoder_test_output5.dat"}},
    {{216, 60955}, {"pdcch_encoder_test_input6.dat"}, {"pdcch_encoder_test_output6.dat"}},
    {{216, 46954}, {"pdcch_encoder_test_input7.dat"}, {"pdcch_encoder_test_output7.dat"}},
    {{216, 59284}, {"pdcch_encoder_test_input8.dat"}, {"pdcch_encoder_test_output8.dat"}},
    {{216, 37258}, {"pdcch_encoder_test_input9.dat"}, {"pdcch_encoder_test_output9.dat"}},
    {{216, 21964}, {"pdcch_encoder_test_input10.dat"}, {"pdcch_encoder_test_output10.dat"}},
    {{432, 9941}, {"pdcch_encoder_test_input11.dat"}, {"pdcch_encoder_test_output11.dat"}},
    {{432, 64413}, {"pdcch_encoder_test_input12.dat"}, {"pdcch_encoder_test_output12.dat"}},
    {{432, 24892}, {"pdcch_encoder_test_input13.dat"}, {"pdcch_encoder_test_output13.dat"}},
    {{432, 23645}, {"pdcch_encoder_test_input14.dat"}, {"pdcch_encoder_test_output14.dat"}},
    {{432, 29127}, {"pdcch_encoder_test_input15.dat"}, {"pdcch_encoder_test_output15.dat"}},
    {{432, 30828}, {"pdcch_encoder_test_input16.dat"}, {"pdcch_encoder_test_output16.dat"}},
    {{864, 1165}, {"pdcch_encoder_test_input17.dat"}, {"pdcch_encoder_test_output17.dat"}},
    {{864, 25625}, {"pdcch_encoder_test_input18.dat"}, {"pdcch_encoder_test_output18.dat"}},
    {{864, 39365}, {"pdcch_encoder_test_input19.dat"}, {"pdcch_encoder_test_output19.dat"}},
    {{864, 14780}, {"pdcch_encoder_test_input20.dat"}, {"pdcch_encoder_test_output20.dat"}},
    {{864, 14899}, {"pdcch_encoder_test_input21.dat"}, {"pdcch_encoder_test_output21.dat"}},
    {{864, 58850}, {"pdcch_encoder_test_input22.dat"}, {"pdcch_encoder_test_output22.dat"}},
    {{1728, 50222}, {"pdcch_encoder_test_input23.dat"}, {"pdcch_encoder_test_output23.dat"}},
    {{1728, 33208}, {"pdcch_encoder_test_input24.dat"}, {"pdcch_encoder_test_output24.dat"}},
    {{1728, 53780}, {"pdcch_encoder_test_input25.dat"}, {"pdcch_encoder_test_output25.dat"}},
    {{1728, 36185}, {"pdcch_encoder_test_input26.dat"}, {"pdcch_encoder_test_output26.dat"}},
    {{1728, 58614}, {"pdcch_encoder_test_input27.dat"}, {"pdcch_encoder_test_output27.dat"}},
    {{1728, 64435}, {"pdcch_encoder_test_input28.dat"}, {"pdcch_encoder_test_output28.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_TEST_DATA_H