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
//   + "srsShortBlockEncoderUnittest.m"

#include "srsran/ran/modulation_scheme.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned             nof_messages     = 0;
  unsigned             message_length   = 0;
  unsigned             codeblock_length = 0;
  modulation_scheme    mod              = {};
  file_vector<uint8_t> messages;
  file_vector<uint8_t> codeblocks;
};

static const std::vector<test_case_t> short_block_encoder_test_data = {
    // clang-format off
  {10, 1, 1, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input0.dat"}, {"test_data/short_block_encoder_test_output0.dat"}},
  {10, 1, 2, modulation_scheme::QPSK, {"test_data/short_block_encoder_test_input1.dat"}, {"test_data/short_block_encoder_test_output1.dat"}},
  {10, 1, 6, modulation_scheme::QAM16, {"test_data/short_block_encoder_test_input2.dat"}, {"test_data/short_block_encoder_test_output2.dat"}},
  {10, 1, 17, modulation_scheme::QAM64, {"test_data/short_block_encoder_test_input3.dat"}, {"test_data/short_block_encoder_test_output3.dat"}},
  {10, 1, 10, modulation_scheme::QAM256, {"test_data/short_block_encoder_test_input4.dat"}, {"test_data/short_block_encoder_test_output4.dat"}},
  {10, 2, 5, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input5.dat"}, {"test_data/short_block_encoder_test_output5.dat"}},
  {10, 2, 14, modulation_scheme::QPSK, {"test_data/short_block_encoder_test_input6.dat"}, {"test_data/short_block_encoder_test_output6.dat"}},
  {10, 2, 36, modulation_scheme::QAM16, {"test_data/short_block_encoder_test_input7.dat"}, {"test_data/short_block_encoder_test_output7.dat"}},
  {10, 2, 36, modulation_scheme::QAM64, {"test_data/short_block_encoder_test_input8.dat"}, {"test_data/short_block_encoder_test_output8.dat"}},
  {10, 2, 30, modulation_scheme::QAM256, {"test_data/short_block_encoder_test_input9.dat"}, {"test_data/short_block_encoder_test_output9.dat"}},
  {10, 3, 96, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input10.dat"}, {"test_data/short_block_encoder_test_output10.dat"}},
  {10, 4, 48, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input11.dat"}, {"test_data/short_block_encoder_test_output11.dat"}},
  {10, 5, 64, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input12.dat"}, {"test_data/short_block_encoder_test_output12.dat"}},
  {10, 6, 80, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input13.dat"}, {"test_data/short_block_encoder_test_output13.dat"}},
  {10, 7, 80, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input14.dat"}, {"test_data/short_block_encoder_test_output14.dat"}},
  {10, 8, 56, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input15.dat"}, {"test_data/short_block_encoder_test_output15.dat"}},
  {10, 9, 80, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input16.dat"}, {"test_data/short_block_encoder_test_output16.dat"}},
  {10, 10, 64, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input17.dat"}, {"test_data/short_block_encoder_test_output17.dat"}},
  {10, 11, 88, modulation_scheme::PI_2_BPSK, {"test_data/short_block_encoder_test_input18.dat"}, {"test_data/short_block_encoder_test_output18.dat"}},
    // clang-format on
};

} // namespace srsran
