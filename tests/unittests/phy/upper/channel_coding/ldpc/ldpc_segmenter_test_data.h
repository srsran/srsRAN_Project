/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsLDPCSegmenterUnittest.m"

#include "srsran/support/file_vector.h"

namespace srsran {

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

} // namespace srsran
