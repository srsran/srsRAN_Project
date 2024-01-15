/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
//   + "srsLDPCRateMatcherUnittest.m"

#include "srsran/ran/modulation_scheme.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  unsigned             rm_length  = 0;
  unsigned             rv         = 0;
  modulation_scheme    mod_scheme = {};
  unsigned             n_ref      = 0;
  bool                 is_lbrm    = false;
  unsigned             nof_filler = 0;
  file_vector<uint8_t> full_cblock;
  file_vector<uint8_t> rm_cblock;
};

static const std::vector<test_case_t> ldpc_rate_matcher_test_data = {
    // clang-format off
  {277, 0, modulation_scheme::BPSK, 700, false, 0, {"test_data/ldpc_rate_matcher_test_input0.dat"}, {"test_data/ldpc_rate_matcher_test_output0.dat"}},
  {554, 1, modulation_scheme::QPSK, 700, true, 28, {"test_data/ldpc_rate_matcher_test_input1.dat"}, {"test_data/ldpc_rate_matcher_test_output1.dat"}},
  {924, 2, modulation_scheme::QAM16, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input2.dat"}, {"test_data/ldpc_rate_matcher_test_output2.dat"}},
  {4620, 3, modulation_scheme::QAM64, 700, false, 0, {"test_data/ldpc_rate_matcher_test_input3.dat"}, {"test_data/ldpc_rate_matcher_test_output3.dat"}},
  {9240, 0, modulation_scheme::QAM256, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input4.dat"}, {"test_data/ldpc_rate_matcher_test_output4.dat"}},
  {208, 1, modulation_scheme::QAM16, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input5.dat"}, {"test_data/ldpc_rate_matcher_test_output5.dat"}},
  {420, 0, modulation_scheme::QAM64, 700, false, 12, {"test_data/ldpc_rate_matcher_test_input6.dat"}, {"test_data/ldpc_rate_matcher_test_output6.dat"}},
  {700, 3, modulation_scheme::BPSK, 700, true, 12, {"test_data/ldpc_rate_matcher_test_input7.dat"}, {"test_data/ldpc_rate_matcher_test_output7.dat"}},
  {3500, 2, modulation_scheme::QPSK, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input8.dat"}, {"test_data/ldpc_rate_matcher_test_output8.dat"}},
  {7000, 1, modulation_scheme::BPSK, 700, false, 12, {"test_data/ldpc_rate_matcher_test_input9.dat"}, {"test_data/ldpc_rate_matcher_test_output9.dat"}},
  {924, 0, modulation_scheme::QPSK, 700, false, 0, {"test_data/ldpc_rate_matcher_test_input10.dat"}, {"test_data/ldpc_rate_matcher_test_output10.dat"}},
  {3500, 0, modulation_scheme::QAM16, 700, true, 12, {"test_data/ldpc_rate_matcher_test_input11.dat"}, {"test_data/ldpc_rate_matcher_test_output11.dat"}},
  {696, 1, modulation_scheme::QAM256, 700, false, 12, {"test_data/ldpc_rate_matcher_test_input12.dat"}, {"test_data/ldpc_rate_matcher_test_output12.dat"}},
  {3500, 1, modulation_scheme::BPSK, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input13.dat"}, {"test_data/ldpc_rate_matcher_test_output13.dat"}},
  {276, 2, modulation_scheme::QAM64, 700, true, 28, {"test_data/ldpc_rate_matcher_test_input14.dat"}, {"test_data/ldpc_rate_matcher_test_output14.dat"}},
  {420, 2, modulation_scheme::BPSK, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input15.dat"}, {"test_data/ldpc_rate_matcher_test_output15.dat"}},
  {9240, 2, modulation_scheme::QPSK, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input16.dat"}, {"test_data/ldpc_rate_matcher_test_output16.dat"}},
  {210, 3, modulation_scheme::QPSK, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input17.dat"}, {"test_data/ldpc_rate_matcher_test_output17.dat"}},
  {552, 3, modulation_scheme::QAM16, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input18.dat"}, {"test_data/ldpc_rate_matcher_test_output18.dat"}},
  {7000, 3, modulation_scheme::QAM16, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input19.dat"}, {"test_data/ldpc_rate_matcher_test_output19.dat"}},
  {924, 1, modulation_scheme::QAM64, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input20.dat"}, {"test_data/ldpc_rate_matcher_test_output20.dat"}},
  {6996, 1, modulation_scheme::QAM64, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input21.dat"}, {"test_data/ldpc_rate_matcher_test_output21.dat"}},
  {272, 2, modulation_scheme::QAM256, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input22.dat"}, {"test_data/ldpc_rate_matcher_test_output22.dat"}},
  {416, 3, modulation_scheme::QAM256, 700, true, 0, {"test_data/ldpc_rate_matcher_test_input23.dat"}, {"test_data/ldpc_rate_matcher_test_output23.dat"}},
  {4616, 0, modulation_scheme::QAM256, 700, false, 28, {"test_data/ldpc_rate_matcher_test_input24.dat"}, {"test_data/ldpc_rate_matcher_test_output24.dat"}},
    // clang-format on
};

} // namespace srsran
