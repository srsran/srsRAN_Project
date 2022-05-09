#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DATA_H

// This file was generated using the following MATLAB class:
//   + "srsPDSCHModulatorUnittest.m"

#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pdsch_modulator::config_t                               config;
  file_vector<uint8_t>                                    data;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pdsch_modulator_test_data = {
    // clang-format off
  {{62057, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 296, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{17373, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 156, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{58166, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 97, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{64360, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 940, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{14653, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 611, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{47514, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 52, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{26848, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 743, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{11523, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 63, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{44178, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 345, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{47432, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 310, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{28136, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 579, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{29138, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation({0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51}, vrb_to_prb_mapping_type::NON_INTERLEAVED), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 905, 1, {}, 0, {0}}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
    // clang-format on
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_MODULATOR_TEST_DATA_H
