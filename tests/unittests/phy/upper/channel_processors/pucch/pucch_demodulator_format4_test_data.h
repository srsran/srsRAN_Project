/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

// This file was generated using the following MATLAB class on 19-11-2024 (seed 0):
//   + "srsPUCCHDemodulatorFormat4Unittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct context_t {
  unsigned                                 grid_nof_prb;
  unsigned                                 grid_nof_symbols;
  float                                    noise_var;
  pucch_demodulator::format4_configuration config;
};

struct test_case_t {
  context_t                                               context;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
  file_vector<cf_t>                                       estimates;
  file_vector<log_likelihood_ratio>                       uci_codeword;
};

static const std::vector<test_case_t> pucch_demodulator_format4_test_data = {
    // clang-format off
  {{266, 14, 0.81, {{0}, 251, {}, 0, 14, 8323, 927, true, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols0.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates0.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits0.dat"}},
  {{170, 14, 0.16, {{0}, 149, {}, 0, 14, 17800, 19, true, false, 4, 2}}, {"test_data/pucch_demodulator_format4_test_input_symbols2.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates2.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits2.dat"}},
  {{151, 14, 0.01, {{0}, 132, {}, 0, 14, 6522, 814, true, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols4.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates4.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits4.dat"}},
  {{234, 14, 0.81, {{0}, 222, {}, 0, 14, 58321, 723, true, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols6.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates6.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits6.dat"}},
  {{273, 14, 0.09, {{0}, 259, {}, 0, 14, 65108, 82, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols8.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates8.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits8.dat"}},
  {{181, 14, 0.49, {{0}, 125, {}, 0, 14, 41366, 444, false, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols10.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates10.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits10.dat"}},
  {{233, 14, 0.64, {{0}, 69, {}, 0, 14, 64311, 341, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols12.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates12.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits12.dat"}},
  {{175, 14, 0.25, {{0}, 172, {}, 0, 14, 32009, 961, false, true, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols14.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates14.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits14.dat"}},
  {{263, 14, 0.64, {{0}, 254, 256, 0, 14, 41682, 101, true, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols16.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates16.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits16.dat"}},
  {{56, 14, 0.04, {{0}, 38, 51, 0, 14, 63908, 759, true, false, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols18.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates18.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits18.dat"}},
  {{163, 14, 0.04, {{0}, 105, 123, 0, 14, 34348, 992, true, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols20.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates20.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits20.dat"}},
  {{156, 14, 0.25, {{0}, 133, 148, 0, 14, 22469, 270, true, true, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols22.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates22.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits22.dat"}},
  {{175, 14, 0.09, {{0}, 93, 168, 0, 14, 55710, 275, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols24.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates24.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits24.dat"}},
  {{274, 14, 0.64, {{0}, 204, 240, 0, 14, 57200, 668, false, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols26.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates26.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits26.dat"}},
  {{143, 14, 0.16, {{0}, 104, 117, 0, 14, 58967, 519, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols28.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates28.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits28.dat"}},
  {{173, 14, 0.36, {{0}, 170, 162, 0, 14, 28669, 108, false, true, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols30.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates30.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits30.dat"}},
  {{243, 14, 0.36, {{0}, 88, {}, 1, 13, 30084, 983, true, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols32.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates32.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits32.dat"}},
  {{272, 14, 0.01, {{0}, 260, {}, 1, 13, 18233, 614, true, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols34.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates34.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits34.dat"}},
  {{23, 14, 0.04, {{0}, 12, {}, 1, 13, 11524, 987, true, true, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols36.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates36.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits36.dat"}},
  {{197, 14, 0.16, {{0}, 155, {}, 1, 13, 54686, 956, true, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols38.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates38.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits38.dat"}},
  {{244, 14, 0.04, {{0}, 242, {}, 1, 13, 39561, 352, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols40.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates40.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits40.dat"}},
  {{143, 14, 0.16, {{0}, 78, {}, 1, 13, 32422, 648, false, false, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols42.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates42.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits42.dat"}},
  {{271, 14, 0.49, {{0}, 269, {}, 1, 13, 9465, 594, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols44.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates44.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits44.dat"}},
  {{242, 14, 0.25, {{0}, 197, {}, 1, 13, 21870, 929, false, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols46.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates46.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits46.dat"}},
  {{234, 14, 0.16, {{0}, 160, 214, 1, 13, 2515, 22, true, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols48.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates48.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits48.dat"}},
  {{228, 14, 0.81, {{0}, 208, 216, 1, 13, 53298, 545, true, false, 4, 2}}, {"test_data/pucch_demodulator_format4_test_input_symbols50.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates50.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits50.dat"}},
  {{145, 14, 0.04, {{0}, 141, 140, 1, 13, 45241, 1016, true, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols52.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates52.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits52.dat"}},
  {{268, 14, 0.09, {{0}, 263, 267, 1, 13, 47309, 662, true, true, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols54.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates54.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits54.dat"}},
  {{182, 14, 1, {{0}, 166, 131, 1, 13, 36950, 335, false, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols56.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates56.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits56.dat"}},
  {{224, 14, 0.09, {{0}, 123, 60, 1, 13, 19208, 713, false, false, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols58.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates58.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits58.dat"}},
  {{158, 14, 1, {{0}, 101, 95, 1, 13, 6522, 71, false, true, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols60.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates60.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits60.dat"}},
  {{213, 14, 0.81, {{0}, 196, 208, 1, 13, 33084, 318, false, true, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols62.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates62.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits62.dat"}},
  {{133, 14, 0.25, {{0}, 128, {}, 5, 5, 36679, 771, true, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols64.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates64.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits64.dat"}},
  {{202, 14, 0.49, {{0}, 199, {}, 5, 5, 40171, 77, true, false, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols66.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates66.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits66.dat"}},
  {{249, 14, 0.81, {{0}, 244, {}, 5, 5, 36556, 305, true, true, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols68.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates68.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits68.dat"}},
  {{213, 14, 0.81, {{0}, 133, {}, 5, 5, 15363, 373, true, true, 4, 2}}, {"test_data/pucch_demodulator_format4_test_input_symbols70.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates70.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits70.dat"}},
  {{233, 14, 0.09, {{0}, 163, {}, 5, 5, 64895, 711, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols72.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates72.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits72.dat"}},
  {{234, 14, 1, {{0}, 199, {}, 5, 5, 22665, 919, false, false, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols74.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates74.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits74.dat"}},
  {{256, 14, 0.16, {{0}, 201, {}, 5, 5, 47693, 996, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols76.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates76.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits76.dat"}},
  {{91, 14, 1, {{0}, 37, {}, 5, 5, 56802, 799, false, true, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols78.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates78.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits78.dat"}},
  {{130, 14, 0.04, {{0}, 71, 82, 5, 5, 54938, 399, true, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols80.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates80.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits80.dat"}},
  {{252, 14, 0.16, {{0}, 200, 209, 5, 5, 20453, 722, true, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols82.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates82.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits82.dat"}},
  {{153, 14, 0.49, {{0}, 121, 118, 5, 5, 1198, 389, true, true, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols84.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates84.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits84.dat"}},
  {{251, 14, 0.49, {{0}, 218, 114, 5, 5, 46355, 329, true, true, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols86.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates86.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits86.dat"}},
  {{154, 14, 0.64, {{0}, 153, 148, 5, 5, 7574, 990, false, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols88.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates88.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits88.dat"}},
  {{207, 14, 0.04, {{0}, 206, 202, 5, 5, 259, 82, false, false, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols90.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates90.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits90.dat"}},
  {{202, 14, 0.36, {{0}, 170, 183, 5, 5, 22927, 430, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols92.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates92.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits92.dat"}},
  {{131, 14, 0.36, {{0}, 120, 130, 5, 5, 62760, 373, false, true, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols94.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates94.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits94.dat"}},
  {{64, 14, 0.04, {{0}, 49, {}, 10, 4, 1421, 152, true, false, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols96.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates96.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits96.dat"}},
  {{168, 14, 0.36, {{0}, 138, {}, 10, 4, 34828, 303, true, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols98.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates98.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits98.dat"}},
  {{267, 14, 0.09, {{0}, 240, {}, 10, 4, 33192, 647, true, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols100.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates100.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits100.dat"}},
  {{274, 14, 0.09, {{0}, 240, {}, 10, 4, 37965, 730, true, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols102.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates102.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits102.dat"}},
  {{237, 14, 0.36, {{0}, 231, {}, 10, 4, 62814, 1015, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols104.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates104.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits104.dat"}},
  {{126, 14, 0.16, {{0}, 99, {}, 10, 4, 19802, 285, false, false, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols106.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates106.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits106.dat"}},
  {{239, 14, 0.25, {{0}, 237, {}, 10, 4, 10629, 895, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols108.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates108.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits108.dat"}},
  {{187, 14, 0.49, {{0}, 123, {}, 10, 4, 34631, 791, false, true, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols110.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates110.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits110.dat"}},
  {{198, 14, 0.04, {{0}, 160, 170, 10, 4, 57520, 488, true, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols112.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates112.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits112.dat"}},
  {{205, 14, 0.81, {{0}, 149, 169, 10, 4, 12721, 295, true, false, 4, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols114.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates114.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits114.dat"}},
  {{171, 14, 0.64, {{0}, 123, 135, 10, 4, 22780, 668, true, true, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols116.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates116.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits116.dat"}},
  {{131, 14, 0.81, {{0}, 126, 128, 10, 4, 51378, 645, true, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols118.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates118.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits118.dat"}},
  {{43, 14, 0.36, {{0}, 21, 26, 10, 4, 22335, 1012, false, false, 2, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols120.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates120.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits120.dat"}},
  {{243, 14, 0.01, {{0}, 242, 237, 10, 4, 48633, 17, false, false, 4, 1}}, {"test_data/pucch_demodulator_format4_test_input_symbols122.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates122.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits122.dat"}},
  {{246, 14, 0.25, {{0}, 208, 154, 10, 4, 60173, 295, false, true, 2, 0}}, {"test_data/pucch_demodulator_format4_test_input_symbols124.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates124.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits124.dat"}},
  {{213, 14, 0.04, {{0}, 131, 111, 10, 4, 58214, 792, false, true, 4, 3}}, {"test_data/pucch_demodulator_format4_test_input_symbols126.dat"}, {"test_data/pucch_demodulator_format4_test_input_estimates126.dat"}, {"test_data/pucch_demodulator_format4_test_output_sch_soft_bits126.dat"}},
    // clang-format on
};

} // namespace srsran
