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
//   + "srsDemodulationMapperUnittest.m"

#include "srsran/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  std::size_t                       nsymbols;
  modulation_scheme                 scheme;
  file_vector<cf_t>                 symbols;
  file_vector<float>                noise_var;
  file_vector<log_likelihood_ratio> soft_bits;
  file_vector<uint8_t>              hard_bits;
};

static const std::vector<test_case_t> demodulation_mapper_test_data = {
    // clang-format off
  {257, modulation_scheme::BPSK, {"test_data/demodulation_mapper_test_input0.dat"}, {"test_data/demodulation_mapper_test_noisevar0.dat"}, {"test_data/demodulation_mapper_test_soft_bits0.dat"}, {"test_data/demodulation_mapper_test_hard_bits0.dat"}},
  {257, modulation_scheme::PI_2_BPSK, {"test_data/demodulation_mapper_test_input1.dat"}, {"test_data/demodulation_mapper_test_noisevar1.dat"}, {"test_data/demodulation_mapper_test_soft_bits1.dat"}, {"test_data/demodulation_mapper_test_hard_bits1.dat"}},
  {257, modulation_scheme::QPSK, {"test_data/demodulation_mapper_test_input2.dat"}, {"test_data/demodulation_mapper_test_noisevar2.dat"}, {"test_data/demodulation_mapper_test_soft_bits2.dat"}, {"test_data/demodulation_mapper_test_hard_bits2.dat"}},
  {257, modulation_scheme::QAM16, {"test_data/demodulation_mapper_test_input3.dat"}, {"test_data/demodulation_mapper_test_noisevar3.dat"}, {"test_data/demodulation_mapper_test_soft_bits3.dat"}, {"test_data/demodulation_mapper_test_hard_bits3.dat"}},
  {257, modulation_scheme::QAM64, {"test_data/demodulation_mapper_test_input4.dat"}, {"test_data/demodulation_mapper_test_noisevar4.dat"}, {"test_data/demodulation_mapper_test_soft_bits4.dat"}, {"test_data/demodulation_mapper_test_hard_bits4.dat"}},
  {257, modulation_scheme::QAM256, {"test_data/demodulation_mapper_test_input5.dat"}, {"test_data/demodulation_mapper_test_noisevar5.dat"}, {"test_data/demodulation_mapper_test_soft_bits5.dat"}, {"test_data/demodulation_mapper_test_hard_bits5.dat"}},
  {997, modulation_scheme::BPSK, {"test_data/demodulation_mapper_test_input6.dat"}, {"test_data/demodulation_mapper_test_noisevar6.dat"}, {"test_data/demodulation_mapper_test_soft_bits6.dat"}, {"test_data/demodulation_mapper_test_hard_bits6.dat"}},
  {997, modulation_scheme::PI_2_BPSK, {"test_data/demodulation_mapper_test_input7.dat"}, {"test_data/demodulation_mapper_test_noisevar7.dat"}, {"test_data/demodulation_mapper_test_soft_bits7.dat"}, {"test_data/demodulation_mapper_test_hard_bits7.dat"}},
  {997, modulation_scheme::QPSK, {"test_data/demodulation_mapper_test_input8.dat"}, {"test_data/demodulation_mapper_test_noisevar8.dat"}, {"test_data/demodulation_mapper_test_soft_bits8.dat"}, {"test_data/demodulation_mapper_test_hard_bits8.dat"}},
  {997, modulation_scheme::QAM16, {"test_data/demodulation_mapper_test_input9.dat"}, {"test_data/demodulation_mapper_test_noisevar9.dat"}, {"test_data/demodulation_mapper_test_soft_bits9.dat"}, {"test_data/demodulation_mapper_test_hard_bits9.dat"}},
  {997, modulation_scheme::QAM64, {"test_data/demodulation_mapper_test_input10.dat"}, {"test_data/demodulation_mapper_test_noisevar10.dat"}, {"test_data/demodulation_mapper_test_soft_bits10.dat"}, {"test_data/demodulation_mapper_test_hard_bits10.dat"}},
  {997, modulation_scheme::QAM256, {"test_data/demodulation_mapper_test_input11.dat"}, {"test_data/demodulation_mapper_test_noisevar11.dat"}, {"test_data/demodulation_mapper_test_soft_bits11.dat"}, {"test_data/demodulation_mapper_test_hard_bits11.dat"}},
    // clang-format on
};

} // namespace srsran
