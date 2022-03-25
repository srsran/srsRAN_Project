#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H

// This file was generated using the following MATLAB scripts:
//   + "srsModulationMapperUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/support/file_vector.h"
#include <array>

namespace srsgnb {

struct test_case_t {
  std::size_t nsymbols;
  modulation_scheme scheme;
  file_vector<uint8_t> data;
  file_vector<cf_t> symbols;
};

static const std::vector<test_case_t> modulation_mapper_test_data = {
    // clang-format off
  {257, modulation_scheme::BPSK,{"modulation_mapper_test_input0.dat"},{"modulation_mapper_test_output0.dat"}},
  {257, modulation_scheme::QPSK,{"modulation_mapper_test_input1.dat"},{"modulation_mapper_test_output1.dat"}},
  {257, modulation_scheme::QAM16,{"modulation_mapper_test_input2.dat"},{"modulation_mapper_test_output2.dat"}},
  {257, modulation_scheme::QAM64,{"modulation_mapper_test_input3.dat"},{"modulation_mapper_test_output3.dat"}},
  {257, modulation_scheme::QAM256,{"modulation_mapper_test_input4.dat"},{"modulation_mapper_test_output4.dat"}},
  {997, modulation_scheme::BPSK,{"modulation_mapper_test_input5.dat"},{"modulation_mapper_test_output5.dat"}},
  {997, modulation_scheme::QPSK,{"modulation_mapper_test_input6.dat"},{"modulation_mapper_test_output6.dat"}},
  {997, modulation_scheme::QAM16,{"modulation_mapper_test_input7.dat"},{"modulation_mapper_test_output7.dat"}},
  {997, modulation_scheme::QAM64,{"modulation_mapper_test_input8.dat"},{"modulation_mapper_test_output8.dat"}},
  {997, modulation_scheme::QAM256,{"modulation_mapper_test_input9.dat"},{"modulation_mapper_test_output9.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_TEST_DATA_H
