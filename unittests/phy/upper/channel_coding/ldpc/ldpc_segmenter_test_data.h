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

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsLDPCSegmenterUnittest.m"

#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  unsigned             tbs            = 0;
  unsigned             bg             = 0;
  unsigned             nof_segments   = 0;
  unsigned             segment_length = 0;
  file_vector<uint8_t> trans_block;
  file_vector<uint8_t> segments;
};

static const std::vector<test_case_t> ldpc_segmenter_test_data = {
    // clang-format off
  {96, 1, 1, 132, {"test_data/ldpc_segmenter_test_input0.dat"}, {"test_data/ldpc_segmenter_test_output0.dat"}},
  {600, 1, 1, 616, {"test_data/ldpc_segmenter_test_input1.dat"}, {"test_data/ldpc_segmenter_test_output1.dat"}},
  {4000, 1, 1, 4224, {"test_data/ldpc_segmenter_test_input2.dat"}, {"test_data/ldpc_segmenter_test_output2.dat"}},
  {12000, 1, 2, 6336, {"test_data/ldpc_segmenter_test_input3.dat"}, {"test_data/ldpc_segmenter_test_output3.dat"}},
  {40000, 1, 5, 8448, {"test_data/ldpc_segmenter_test_input4.dat"}, {"test_data/ldpc_segmenter_test_output4.dat"}},
  {96, 2, 1, 200, {"test_data/ldpc_segmenter_test_input5.dat"}, {"test_data/ldpc_segmenter_test_output5.dat"}},
  {320, 2, 1, 440, {"test_data/ldpc_segmenter_test_input6.dat"}, {"test_data/ldpc_segmenter_test_output6.dat"}},
  {600, 2, 1, 720, {"test_data/ldpc_segmenter_test_input7.dat"}, {"test_data/ldpc_segmenter_test_output7.dat"}},
  {4000, 2, 2, 2080, {"test_data/ldpc_segmenter_test_input8.dat"}, {"test_data/ldpc_segmenter_test_output8.dat"}},
  {12000, 2, 4, 3200, {"test_data/ldpc_segmenter_test_input9.dat"}, {"test_data/ldpc_segmenter_test_output9.dat"}},
  {40000, 2, 11, 3840, {"test_data/ldpc_segmenter_test_input10.dat"}, {"test_data/ldpc_segmenter_test_output10.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_TEST_DATA_H
