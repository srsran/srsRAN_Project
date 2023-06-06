/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 06-06-2023:
//   + "srsPDSCHModulatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/file_vector.h"

namespace srsran {

static const precoding_configuration default_precoding = make_single_port();

struct test_case_t {
  pdsch_modulator::config_t                               config;
  file_vector<uint8_t>                                    data;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pdsch_modulator_test_data = {
    // clang-format off
  {{37457, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 335, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{41310, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 108, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{59155, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 391, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{14028, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 17, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{58807, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 781, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{1317, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 931, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{16953, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 457, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{43309, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 249, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{17235, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 612, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{8858, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 749, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{62996, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 631, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{22419, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 22, 1, {}, default_precoding}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
    // clang-format on
};

} // namespace srsran
