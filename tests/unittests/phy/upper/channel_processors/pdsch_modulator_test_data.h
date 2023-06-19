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

// This file was generated using the following MATLAB class on 09-06-2023:
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
  {{52882, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 176, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input0.dat"}, {"test_data/pdsch_modulator_test_output0.dat"}},
  {{39994, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 217, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input1.dat"}, {"test_data/pdsch_modulator_test_output1.dat"}},
  {{62207, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 1009, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input2.dat"}, {"test_data/pdsch_modulator_test_output2.dat"}},
  {{32233, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 791, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input3.dat"}, {"test_data/pdsch_modulator_test_output3.dat"}},
  {{11839, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 409, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input4.dat"}, {"test_data/pdsch_modulator_test_output4.dat"}},
  {{45967, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 13, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input5.dat"}, {"test_data/pdsch_modulator_test_output5.dat"}},
  {{61443, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 470, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input6.dat"}, {"test_data/pdsch_modulator_test_output6.dat"}},
  {{40732, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 844, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input7.dat"}, {"test_data/pdsch_modulator_test_output7.dat"}},
  {{1338, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 459, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input8.dat"}, {"test_data/pdsch_modulator_test_output8.dat"}},
  {{1852, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 900, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input9.dat"}, {"test_data/pdsch_modulator_test_output9.dat"}},
  {{30166, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 770, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input10.dat"}, {"test_data/pdsch_modulator_test_output10.dat"}},
  {{46349, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 959, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input11.dat"}, {"test_data/pdsch_modulator_test_output11.dat"}},
  {{25452, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 154, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input12.dat"}, {"test_data/pdsch_modulator_test_output12.dat"}},
  {{42679, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 678, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input13.dat"}, {"test_data/pdsch_modulator_test_output13.dat"}},
  {{14552, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 885, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input14.dat"}, {"test_data/pdsch_modulator_test_output14.dat"}},
  {{53310, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 306, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input15.dat"}, {"test_data/pdsch_modulator_test_output15.dat"}},
  {{61017, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 995, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input16.dat"}, {"test_data/pdsch_modulator_test_output16.dat"}},
  {{20412, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 953, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input17.dat"}, {"test_data/pdsch_modulator_test_output17.dat"}},
  {{42447, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 615, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input18.dat"}, {"test_data/pdsch_modulator_test_output18.dat"}},
  {{54719, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 327, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input19.dat"}, {"test_data/pdsch_modulator_test_output19.dat"}},
  {{18376, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 659, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input20.dat"}, {"test_data/pdsch_modulator_test_output20.dat"}},
  {{38361, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 478, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input21.dat"}, {"test_data/pdsch_modulator_test_output21.dat"}},
  {{64839, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 177, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input22.dat"}, {"test_data/pdsch_modulator_test_output22.dat"}},
  {{27391, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 519, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input23.dat"}, {"test_data/pdsch_modulator_test_output23.dat"}},
  {{1710, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 531, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input24.dat"}, {"test_data/pdsch_modulator_test_output24.dat"}},
  {{42604, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 584, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input25.dat"}, {"test_data/pdsch_modulator_test_output25.dat"}},
  {{17379, 52, 0, modulation_scheme::QPSK, modulation_scheme::QPSK, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 657, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input26.dat"}, {"test_data/pdsch_modulator_test_output26.dat"}},
  {{63075, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 832, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input27.dat"}, {"test_data/pdsch_modulator_test_output27.dat"}},
  {{2676, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 685, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input28.dat"}, {"test_data/pdsch_modulator_test_output28.dat"}},
  {{64266, 52, 0, modulation_scheme::QAM16, modulation_scheme::QAM16, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 863, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input29.dat"}, {"test_data/pdsch_modulator_test_output29.dat"}},
  {{62100, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 679, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input30.dat"}, {"test_data/pdsch_modulator_test_output30.dat"}},
  {{1656, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 818, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input31.dat"}, {"test_data/pdsch_modulator_test_output31.dat"}},
  {{47318, 52, 0, modulation_scheme::QAM64, modulation_scheme::QAM64, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 759, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input32.dat"}, {"test_data/pdsch_modulator_test_output32.dat"}},
  {{3409, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 779, 1, {}, make_wideband_identity(1)}, {"test_data/pdsch_modulator_test_input33.dat"}, {"test_data/pdsch_modulator_test_output33.dat"}},
  {{46513, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 112, 1, {}, make_wideband_identity(2)}, {"test_data/pdsch_modulator_test_input34.dat"}, {"test_data/pdsch_modulator_test_output34.dat"}},
  {{64946, 52, 0, modulation_scheme::QAM256, modulation_scheme::QAM256, rb_allocation::make_type1(0, 52), 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 105, 1, {}, make_wideband_identity(4)}, {"test_data/pdsch_modulator_test_input35.dat"}, {"test_data/pdsch_modulator_test_output35.dat"}},
    // clang-format on
};

} // namespace srsran
