#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_ENCODER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_ENCODER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsLDPCEncoderUnittest.m"

#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  unsigned             nof_messages = 0;
  unsigned             bg           = 0;
  unsigned             ls           = 0;
  file_vector<uint8_t> messages;
  file_vector<uint8_t> codeblocks;
};

static const std::vector<test_case_t> ldpc_encoder_test_data = {
    // clang-format off
  {10, 1, 2, {"ldpc_encoder_test_input0.dat"}, {"ldpc_encoder_test_output0.dat"}},
  {10, 1, 3, {"ldpc_encoder_test_input1.dat"}, {"ldpc_encoder_test_output1.dat"}},
  {10, 1, 4, {"ldpc_encoder_test_input2.dat"}, {"ldpc_encoder_test_output2.dat"}},
  {10, 1, 5, {"ldpc_encoder_test_input3.dat"}, {"ldpc_encoder_test_output3.dat"}},
  {10, 1, 6, {"ldpc_encoder_test_input4.dat"}, {"ldpc_encoder_test_output4.dat"}},
  {10, 1, 7, {"ldpc_encoder_test_input5.dat"}, {"ldpc_encoder_test_output5.dat"}},
  {10, 1, 8, {"ldpc_encoder_test_input6.dat"}, {"ldpc_encoder_test_output6.dat"}},
  {10, 1, 9, {"ldpc_encoder_test_input7.dat"}, {"ldpc_encoder_test_output7.dat"}},
  {10, 1, 10, {"ldpc_encoder_test_input8.dat"}, {"ldpc_encoder_test_output8.dat"}},
  {10, 1, 11, {"ldpc_encoder_test_input9.dat"}, {"ldpc_encoder_test_output9.dat"}},
  {10, 1, 12, {"ldpc_encoder_test_input10.dat"}, {"ldpc_encoder_test_output10.dat"}},
  {10, 1, 13, {"ldpc_encoder_test_input11.dat"}, {"ldpc_encoder_test_output11.dat"}},
  {10, 1, 14, {"ldpc_encoder_test_input12.dat"}, {"ldpc_encoder_test_output12.dat"}},
  {10, 1, 15, {"ldpc_encoder_test_input13.dat"}, {"ldpc_encoder_test_output13.dat"}},
  {10, 1, 16, {"ldpc_encoder_test_input14.dat"}, {"ldpc_encoder_test_output14.dat"}},
  {10, 1, 18, {"ldpc_encoder_test_input15.dat"}, {"ldpc_encoder_test_output15.dat"}},
  {10, 1, 20, {"ldpc_encoder_test_input16.dat"}, {"ldpc_encoder_test_output16.dat"}},
  {10, 1, 22, {"ldpc_encoder_test_input17.dat"}, {"ldpc_encoder_test_output17.dat"}},
  {10, 1, 24, {"ldpc_encoder_test_input18.dat"}, {"ldpc_encoder_test_output18.dat"}},
  {10, 1, 26, {"ldpc_encoder_test_input19.dat"}, {"ldpc_encoder_test_output19.dat"}},
  {10, 1, 28, {"ldpc_encoder_test_input20.dat"}, {"ldpc_encoder_test_output20.dat"}},
  {10, 1, 30, {"ldpc_encoder_test_input21.dat"}, {"ldpc_encoder_test_output21.dat"}},
  {10, 1, 32, {"ldpc_encoder_test_input22.dat"}, {"ldpc_encoder_test_output22.dat"}},
  {10, 1, 36, {"ldpc_encoder_test_input23.dat"}, {"ldpc_encoder_test_output23.dat"}},
  {10, 1, 40, {"ldpc_encoder_test_input24.dat"}, {"ldpc_encoder_test_output24.dat"}},
  {10, 1, 44, {"ldpc_encoder_test_input25.dat"}, {"ldpc_encoder_test_output25.dat"}},
  {10, 1, 48, {"ldpc_encoder_test_input26.dat"}, {"ldpc_encoder_test_output26.dat"}},
  {10, 1, 52, {"ldpc_encoder_test_input27.dat"}, {"ldpc_encoder_test_output27.dat"}},
  {10, 1, 56, {"ldpc_encoder_test_input28.dat"}, {"ldpc_encoder_test_output28.dat"}},
  {10, 1, 60, {"ldpc_encoder_test_input29.dat"}, {"ldpc_encoder_test_output29.dat"}},
  {10, 1, 64, {"ldpc_encoder_test_input30.dat"}, {"ldpc_encoder_test_output30.dat"}},
  {10, 1, 72, {"ldpc_encoder_test_input31.dat"}, {"ldpc_encoder_test_output31.dat"}},
  {10, 1, 80, {"ldpc_encoder_test_input32.dat"}, {"ldpc_encoder_test_output32.dat"}},
  {10, 1, 88, {"ldpc_encoder_test_input33.dat"}, {"ldpc_encoder_test_output33.dat"}},
  {10, 1, 96, {"ldpc_encoder_test_input34.dat"}, {"ldpc_encoder_test_output34.dat"}},
  {10, 1, 104, {"ldpc_encoder_test_input35.dat"}, {"ldpc_encoder_test_output35.dat"}},
  {10, 1, 112, {"ldpc_encoder_test_input36.dat"}, {"ldpc_encoder_test_output36.dat"}},
  {10, 1, 120, {"ldpc_encoder_test_input37.dat"}, {"ldpc_encoder_test_output37.dat"}},
  {10, 1, 128, {"ldpc_encoder_test_input38.dat"}, {"ldpc_encoder_test_output38.dat"}},
  {10, 1, 144, {"ldpc_encoder_test_input39.dat"}, {"ldpc_encoder_test_output39.dat"}},
  {10, 1, 160, {"ldpc_encoder_test_input40.dat"}, {"ldpc_encoder_test_output40.dat"}},
  {10, 1, 176, {"ldpc_encoder_test_input41.dat"}, {"ldpc_encoder_test_output41.dat"}},
  {10, 1, 192, {"ldpc_encoder_test_input42.dat"}, {"ldpc_encoder_test_output42.dat"}},
  {10, 1, 208, {"ldpc_encoder_test_input43.dat"}, {"ldpc_encoder_test_output43.dat"}},
  {10, 1, 224, {"ldpc_encoder_test_input44.dat"}, {"ldpc_encoder_test_output44.dat"}},
  {10, 1, 240, {"ldpc_encoder_test_input45.dat"}, {"ldpc_encoder_test_output45.dat"}},
  {10, 1, 256, {"ldpc_encoder_test_input46.dat"}, {"ldpc_encoder_test_output46.dat"}},
  {10, 1, 288, {"ldpc_encoder_test_input47.dat"}, {"ldpc_encoder_test_output47.dat"}},
  {10, 1, 320, {"ldpc_encoder_test_input48.dat"}, {"ldpc_encoder_test_output48.dat"}},
  {10, 1, 352, {"ldpc_encoder_test_input49.dat"}, {"ldpc_encoder_test_output49.dat"}},
  {10, 1, 384, {"ldpc_encoder_test_input50.dat"}, {"ldpc_encoder_test_output50.dat"}},
  {10, 2, 2, {"ldpc_encoder_test_input51.dat"}, {"ldpc_encoder_test_output51.dat"}},
  {10, 2, 3, {"ldpc_encoder_test_input52.dat"}, {"ldpc_encoder_test_output52.dat"}},
  {10, 2, 4, {"ldpc_encoder_test_input53.dat"}, {"ldpc_encoder_test_output53.dat"}},
  {10, 2, 5, {"ldpc_encoder_test_input54.dat"}, {"ldpc_encoder_test_output54.dat"}},
  {10, 2, 6, {"ldpc_encoder_test_input55.dat"}, {"ldpc_encoder_test_output55.dat"}},
  {10, 2, 7, {"ldpc_encoder_test_input56.dat"}, {"ldpc_encoder_test_output56.dat"}},
  {10, 2, 8, {"ldpc_encoder_test_input57.dat"}, {"ldpc_encoder_test_output57.dat"}},
  {10, 2, 9, {"ldpc_encoder_test_input58.dat"}, {"ldpc_encoder_test_output58.dat"}},
  {10, 2, 10, {"ldpc_encoder_test_input59.dat"}, {"ldpc_encoder_test_output59.dat"}},
  {10, 2, 11, {"ldpc_encoder_test_input60.dat"}, {"ldpc_encoder_test_output60.dat"}},
  {10, 2, 12, {"ldpc_encoder_test_input61.dat"}, {"ldpc_encoder_test_output61.dat"}},
  {10, 2, 13, {"ldpc_encoder_test_input62.dat"}, {"ldpc_encoder_test_output62.dat"}},
  {10, 2, 14, {"ldpc_encoder_test_input63.dat"}, {"ldpc_encoder_test_output63.dat"}},
  {10, 2, 15, {"ldpc_encoder_test_input64.dat"}, {"ldpc_encoder_test_output64.dat"}},
  {10, 2, 16, {"ldpc_encoder_test_input65.dat"}, {"ldpc_encoder_test_output65.dat"}},
  {10, 2, 18, {"ldpc_encoder_test_input66.dat"}, {"ldpc_encoder_test_output66.dat"}},
  {10, 2, 20, {"ldpc_encoder_test_input67.dat"}, {"ldpc_encoder_test_output67.dat"}},
  {10, 2, 22, {"ldpc_encoder_test_input68.dat"}, {"ldpc_encoder_test_output68.dat"}},
  {10, 2, 24, {"ldpc_encoder_test_input69.dat"}, {"ldpc_encoder_test_output69.dat"}},
  {10, 2, 26, {"ldpc_encoder_test_input70.dat"}, {"ldpc_encoder_test_output70.dat"}},
  {10, 2, 28, {"ldpc_encoder_test_input71.dat"}, {"ldpc_encoder_test_output71.dat"}},
  {10, 2, 30, {"ldpc_encoder_test_input72.dat"}, {"ldpc_encoder_test_output72.dat"}},
  {10, 2, 32, {"ldpc_encoder_test_input73.dat"}, {"ldpc_encoder_test_output73.dat"}},
  {10, 2, 36, {"ldpc_encoder_test_input74.dat"}, {"ldpc_encoder_test_output74.dat"}},
  {10, 2, 40, {"ldpc_encoder_test_input75.dat"}, {"ldpc_encoder_test_output75.dat"}},
  {10, 2, 44, {"ldpc_encoder_test_input76.dat"}, {"ldpc_encoder_test_output76.dat"}},
  {10, 2, 48, {"ldpc_encoder_test_input77.dat"}, {"ldpc_encoder_test_output77.dat"}},
  {10, 2, 52, {"ldpc_encoder_test_input78.dat"}, {"ldpc_encoder_test_output78.dat"}},
  {10, 2, 56, {"ldpc_encoder_test_input79.dat"}, {"ldpc_encoder_test_output79.dat"}},
  {10, 2, 60, {"ldpc_encoder_test_input80.dat"}, {"ldpc_encoder_test_output80.dat"}},
  {10, 2, 64, {"ldpc_encoder_test_input81.dat"}, {"ldpc_encoder_test_output81.dat"}},
  {10, 2, 72, {"ldpc_encoder_test_input82.dat"}, {"ldpc_encoder_test_output82.dat"}},
  {10, 2, 80, {"ldpc_encoder_test_input83.dat"}, {"ldpc_encoder_test_output83.dat"}},
  {10, 2, 88, {"ldpc_encoder_test_input84.dat"}, {"ldpc_encoder_test_output84.dat"}},
  {10, 2, 96, {"ldpc_encoder_test_input85.dat"}, {"ldpc_encoder_test_output85.dat"}},
  {10, 2, 104, {"ldpc_encoder_test_input86.dat"}, {"ldpc_encoder_test_output86.dat"}},
  {10, 2, 112, {"ldpc_encoder_test_input87.dat"}, {"ldpc_encoder_test_output87.dat"}},
  {10, 2, 120, {"ldpc_encoder_test_input88.dat"}, {"ldpc_encoder_test_output88.dat"}},
  {10, 2, 128, {"ldpc_encoder_test_input89.dat"}, {"ldpc_encoder_test_output89.dat"}},
  {10, 2, 144, {"ldpc_encoder_test_input90.dat"}, {"ldpc_encoder_test_output90.dat"}},
  {10, 2, 160, {"ldpc_encoder_test_input91.dat"}, {"ldpc_encoder_test_output91.dat"}},
  {10, 2, 176, {"ldpc_encoder_test_input92.dat"}, {"ldpc_encoder_test_output92.dat"}},
  {10, 2, 192, {"ldpc_encoder_test_input93.dat"}, {"ldpc_encoder_test_output93.dat"}},
  {10, 2, 208, {"ldpc_encoder_test_input94.dat"}, {"ldpc_encoder_test_output94.dat"}},
  {10, 2, 224, {"ldpc_encoder_test_input95.dat"}, {"ldpc_encoder_test_output95.dat"}},
  {10, 2, 240, {"ldpc_encoder_test_input96.dat"}, {"ldpc_encoder_test_output96.dat"}},
  {10, 2, 256, {"ldpc_encoder_test_input97.dat"}, {"ldpc_encoder_test_output97.dat"}},
  {10, 2, 288, {"ldpc_encoder_test_input98.dat"}, {"ldpc_encoder_test_output98.dat"}},
  {10, 2, 320, {"ldpc_encoder_test_input99.dat"}, {"ldpc_encoder_test_output99.dat"}},
  {10, 2, 352, {"ldpc_encoder_test_input100.dat"}, {"ldpc_encoder_test_output100.dat"}},
  {10, 2, 384, {"ldpc_encoder_test_input101.dat"}, {"ldpc_encoder_test_output101.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_ENCODER_TEST_DATA_H
