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

#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsModulationMapperUnittest.m"

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H
