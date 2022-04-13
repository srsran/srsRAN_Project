#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsDemodulationMapperUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/upper/channel_modulation/demodulation_mapper.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  std::size_t          nsymbols;
  modulation_scheme    scheme;
  file_vector<cf_t>    symbols;
  file_vector<float>   noise_var;
  file_vector<int8_t>  soft_bits;
  file_vector<uint8_t> hard_bits;
};

static const std::vector<test_case_t> demodulation_mapper_test_data = {
    // clang-format off
  {257, modulation_scheme::BPSK, {"demodulation_mapper_test_input0.dat"}, {"demodulation_mapper_test_noisevar0.dat"}, {"demodulation_mapper_test_soft_bits0.dat"}, {"demodulation_mapper_test_hard_bits0.dat"}},
  {257, modulation_scheme::QPSK, {"demodulation_mapper_test_input1.dat"}, {"demodulation_mapper_test_noisevar1.dat"}, {"demodulation_mapper_test_soft_bits1.dat"}, {"demodulation_mapper_test_hard_bits1.dat"}},
  {257, modulation_scheme::QAM16, {"demodulation_mapper_test_input2.dat"}, {"demodulation_mapper_test_noisevar2.dat"}, {"demodulation_mapper_test_soft_bits2.dat"}, {"demodulation_mapper_test_hard_bits2.dat"}},
  {257, modulation_scheme::QAM64, {"demodulation_mapper_test_input3.dat"}, {"demodulation_mapper_test_noisevar3.dat"}, {"demodulation_mapper_test_soft_bits3.dat"}, {"demodulation_mapper_test_hard_bits3.dat"}},
  {257, modulation_scheme::QAM256, {"demodulation_mapper_test_input4.dat"}, {"demodulation_mapper_test_noisevar4.dat"}, {"demodulation_mapper_test_soft_bits4.dat"}, {"demodulation_mapper_test_hard_bits4.dat"}},
  {997, modulation_scheme::BPSK, {"demodulation_mapper_test_input5.dat"}, {"demodulation_mapper_test_noisevar5.dat"}, {"demodulation_mapper_test_soft_bits5.dat"}, {"demodulation_mapper_test_hard_bits5.dat"}},
  {997, modulation_scheme::QPSK, {"demodulation_mapper_test_input6.dat"}, {"demodulation_mapper_test_noisevar6.dat"}, {"demodulation_mapper_test_soft_bits6.dat"}, {"demodulation_mapper_test_hard_bits6.dat"}},
  {997, modulation_scheme::QAM16, {"demodulation_mapper_test_input7.dat"}, {"demodulation_mapper_test_noisevar7.dat"}, {"demodulation_mapper_test_soft_bits7.dat"}, {"demodulation_mapper_test_hard_bits7.dat"}},
  {997, modulation_scheme::QAM64, {"demodulation_mapper_test_input8.dat"}, {"demodulation_mapper_test_noisevar8.dat"}, {"demodulation_mapper_test_soft_bits8.dat"}, {"demodulation_mapper_test_hard_bits8.dat"}},
  {997, modulation_scheme::QAM256, {"demodulation_mapper_test_input9.dat"}, {"demodulation_mapper_test_noisevar9.dat"}, {"demodulation_mapper_test_soft_bits9.dat"}, {"demodulation_mapper_test_hard_bits9.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_TEST_DATA_H
