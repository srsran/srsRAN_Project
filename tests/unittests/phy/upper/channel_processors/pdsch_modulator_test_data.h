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
  {{59362, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 834, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{50772, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 221, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{27916, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 380, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{15956, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 252, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{45269, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 529, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{6559, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 679, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{59497, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 163, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{40218, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 767, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{21494, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 720, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{20207, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 468, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{20226, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 357, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{44026, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 740, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
  {{22301, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 773, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input12.dat"}, {"test_data/pdsch_modulator_test_output12.dat"}},
  {{20506, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 601, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input13.dat"}, {"test_data/pdsch_modulator_test_output13.dat"}},
  {{58190, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 87, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input14.dat"}, {"test_data/pdsch_modulator_test_output14.dat"}},
  {{39140, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 849, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input15.dat"}, {"test_data/pdsch_modulator_test_output15.dat"}},
  {{14651, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 985, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input16.dat"}, {"test_data/pdsch_modulator_test_output16.dat"}},
  {{7023, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 479, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input17.dat"}, {"test_data/pdsch_modulator_test_output17.dat"}},
  {{26479, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 514, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input18.dat"}, {"test_data/pdsch_modulator_test_output18.dat"}},
  {{5256, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 851, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input19.dat"}, {"test_data/pdsch_modulator_test_output19.dat"}},
  {{49628, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 726, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input20.dat"}, {"test_data/pdsch_modulator_test_output20.dat"}},
  {{11742, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 273, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input21.dat"}, {"test_data/pdsch_modulator_test_output21.dat"}},
  {{17069, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 315, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input22.dat"}, {"test_data/pdsch_modulator_test_output22.dat"}},
  {{63124, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 521, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input23.dat"}, {"test_data/pdsch_modulator_test_output23.dat"}},
  {{46026, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 482, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input24.dat"}, {"test_data/pdsch_modulator_test_output24.dat"}},
  {{5206, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 861, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input25.dat"}, {"test_data/pdsch_modulator_test_output25.dat"}},
  {{48772, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 164, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input26.dat"}, {"test_data/pdsch_modulator_test_output26.dat"}},
  {{9311, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 982, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input27.dat"}, {"test_data/pdsch_modulator_test_output27.dat"}},
  {{62828, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 221, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input28.dat"}, {"test_data/pdsch_modulator_test_output28.dat"}},
  {{2973, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 938, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input29.dat"}, {"test_data/pdsch_modulator_test_output29.dat"}},
  {{13563, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 693, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input30.dat"}, {"test_data/pdsch_modulator_test_output30.dat"}},
  {{29030, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 691, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input31.dat"}, {"test_data/pdsch_modulator_test_output31.dat"}},
  {{23117, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 169, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input32.dat"}, {"test_data/pdsch_modulator_test_output32.dat"}},
  {{2615, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 879, 1, {}, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/pdsch_modulator_test_input33.dat"}, {"test_data/pdsch_modulator_test_output33.dat"}},
  {{6078, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 184, 1, {}, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/pdsch_modulator_test_input34.dat"}, {"test_data/pdsch_modulator_test_output34.dat"}},
  {{41488, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 537, 1, {}, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/pdsch_modulator_test_input35.dat"}, {"test_data/pdsch_modulator_test_output35.dat"}},
    // clang-format on
};

} // namespace srsran
