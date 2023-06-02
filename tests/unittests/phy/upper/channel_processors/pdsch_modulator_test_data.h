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

// This file was generated using the following MATLAB class on 01-06-2023:
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
  {{25377, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 912, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{10422, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 294, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{47613, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 114, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{20319, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 6, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{15465, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 938, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{46196, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 27, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{37467, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 152, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{17419, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 489, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{39947, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 167, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{52948, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 289, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{13754, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 186, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{23740, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 197, 1, {}, 0, default_precoding}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
    // clang-format on
};

} // namespace srsran
