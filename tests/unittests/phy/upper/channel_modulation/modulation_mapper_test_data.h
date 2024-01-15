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
//   + "srsModulationMapperUnittest.m"

#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  std::size_t          nsymbols;
  modulation_scheme    scheme;
  file_vector<uint8_t> data;
  file_vector<cf_t>    symbols;
};

static const std::vector<test_case_t> modulation_mapper_test_data = {
    // clang-format off
  {257, modulation_scheme::BPSK, {"test_data/modulation_mapper_test_input0.dat"}, {"test_data/modulation_mapper_test_output0.dat"}},
  {257, modulation_scheme::QPSK, {"test_data/modulation_mapper_test_input1.dat"}, {"test_data/modulation_mapper_test_output1.dat"}},
  {257, modulation_scheme::QAM16, {"test_data/modulation_mapper_test_input2.dat"}, {"test_data/modulation_mapper_test_output2.dat"}},
  {257, modulation_scheme::QAM64, {"test_data/modulation_mapper_test_input3.dat"}, {"test_data/modulation_mapper_test_output3.dat"}},
  {257, modulation_scheme::QAM256, {"test_data/modulation_mapper_test_input4.dat"}, {"test_data/modulation_mapper_test_output4.dat"}},
  {997, modulation_scheme::BPSK, {"test_data/modulation_mapper_test_input5.dat"}, {"test_data/modulation_mapper_test_output5.dat"}},
  {997, modulation_scheme::QPSK, {"test_data/modulation_mapper_test_input6.dat"}, {"test_data/modulation_mapper_test_output6.dat"}},
  {997, modulation_scheme::QAM16, {"test_data/modulation_mapper_test_input7.dat"}, {"test_data/modulation_mapper_test_output7.dat"}},
  {997, modulation_scheme::QAM64, {"test_data/modulation_mapper_test_input8.dat"}, {"test_data/modulation_mapper_test_output8.dat"}},
  {997, modulation_scheme::QAM256, {"test_data/modulation_mapper_test_input9.dat"}, {"test_data/modulation_mapper_test_output9.dat"}},
    // clang-format on
};

} // namespace srsran
