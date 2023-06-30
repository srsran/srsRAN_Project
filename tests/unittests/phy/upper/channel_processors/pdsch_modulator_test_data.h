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

// This file was generated using the following MATLAB class on 23-06-2023:
//   + "srsPDSCHModulatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  pdsch_modulator::config_t                               config;
  file_vector<uint8_t>                                    data;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pdsch_modulator_test_data = {
    // clang-format off
  {{252, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 881, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{41238, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 423, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{65296, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 892, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{4617, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 557, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{62459, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 54, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{45672, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 853, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{19386, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 70, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{43682, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 218, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{53719, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 985, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{1896, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 758, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{64624, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 260, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{17775, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 1003, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
  {{59857, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 233, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input12.dat"}, {"test_data/pdsch_modulator_test_output12.dat"}},
  {{63279, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 921, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input13.dat"}, {"test_data/pdsch_modulator_test_output13.dat"}},
  {{22495, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 51, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input14.dat"}, {"test_data/pdsch_modulator_test_output14.dat"}},
  {{62204, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 725, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input15.dat"}, {"test_data/pdsch_modulator_test_output15.dat"}},
  {{50023, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 869, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input16.dat"}, {"test_data/pdsch_modulator_test_output16.dat"}},
  {{64877, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 475, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input17.dat"}, {"test_data/pdsch_modulator_test_output17.dat"}},
  {{50175, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 435, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input18.dat"}, {"test_data/pdsch_modulator_test_output18.dat"}},
  {{22664, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 620, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input19.dat"}, {"test_data/pdsch_modulator_test_output19.dat"}},
  {{51150, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 873, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input20.dat"}, {"test_data/pdsch_modulator_test_output20.dat"}},
  {{58862, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 34, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input21.dat"}, {"test_data/pdsch_modulator_test_output21.dat"}},
  {{12298, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 567, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input22.dat"}, {"test_data/pdsch_modulator_test_output22.dat"}},
  {{14650, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 152, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input23.dat"}, {"test_data/pdsch_modulator_test_output23.dat"}},
  {{26449, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 494, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input24.dat"}, {"test_data/pdsch_modulator_test_output24.dat"}},
  {{5970, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 541, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input25.dat"}, {"test_data/pdsch_modulator_test_output25.dat"}},
  {{30230, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 731, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input26.dat"}, {"test_data/pdsch_modulator_test_output26.dat"}},
  {{25600, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 918, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input27.dat"}, {"test_data/pdsch_modulator_test_output27.dat"}},
  {{19318, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 279, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input28.dat"}, {"test_data/pdsch_modulator_test_output28.dat"}},
  {{17309, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 422, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input29.dat"}, {"test_data/pdsch_modulator_test_output29.dat"}},
  {{57843, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 671, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input30.dat"}, {"test_data/pdsch_modulator_test_output30.dat"}},
  {{43176, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 483, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input31.dat"}, {"test_data/pdsch_modulator_test_output31.dat"}},
  {{6588, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 335, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input32.dat"}, {"test_data/pdsch_modulator_test_output32.dat"}},
  {{10705, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 284, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input33.dat"}, {"test_data/pdsch_modulator_test_output33.dat"}},
  {{22668, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 227, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input34.dat"}, {"test_data/pdsch_modulator_test_output34.dat"}},
  {{26651, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 265, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input35.dat"}, {"test_data/pdsch_modulator_test_output35.dat"}},
    // clang-format on
};

} // namespace srsran
