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

// This file was generated using the following MATLAB class on 05-11-2024 (seed 0):
//   + "srsPUCCHDemodulatorFormat2Unittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct context_t {
  unsigned                                 grid_nof_prb;
  unsigned                                 grid_nof_symbols;
  float                                    noise_var;
  pucch_demodulator::format2_configuration config;
};

struct test_case_t {
  context_t                                               context;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
  file_vector<cf_t>                                       estimates;
  file_vector<log_likelihood_ratio>                       uci_codeword;
};

static const std::vector<test_case_t> pucch_demodulator_format2_test_data = {
    // clang-format off
  {{266, 14, 1, {{0}, 251, {}, 1, 0, 1, 8323, 927}}, {"test_data/pucch_demodulator_format2_test_input_symbols0.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates0.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits0.dat"}},
  {{267, 14, 0.16, {{0}, 237, {}, 2, 0, 1, 28209, 958}}, {"test_data/pucch_demodulator_format2_test_input_symbols2.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates2.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits2.dat"}},
  {{92, 14, 0.09, {{0}, 71, {}, 3, 0, 1, 19963, 288}}, {"test_data/pucch_demodulator_format2_test_input_symbols4.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates4.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits4.dat"}},
  {{269, 14, 1, {{0}, 262, {}, 4, 0, 1, 40107, 282}}, {"test_data/pucch_demodulator_format2_test_input_symbols6.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates6.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits6.dat"}},
  {{255, 14, 0.49, {{0}, 230, {}, 5, 0, 1, 43469, 910}}, {"test_data/pucch_demodulator_format2_test_input_symbols8.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates8.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits8.dat"}},
  {{118, 14, 0.49, {{0}, 30, {}, 6, 0, 1, 61745, 611}}, {"test_data/pucch_demodulator_format2_test_input_symbols10.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates10.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits10.dat"}},
  {{117, 14, 0.09, {{0}, 47, {}, 7, 0, 1, 21141, 316}}, {"test_data/pucch_demodulator_format2_test_input_symbols12.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates12.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits12.dat"}},
  {{235, 14, 0.49, {{0}, 84, {}, 8, 0, 1, 23516, 846}}, {"test_data/pucch_demodulator_format2_test_input_symbols14.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates14.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits14.dat"}},
  {{159, 14, 0.16, {{0}, 106, {}, 9, 0, 1, 12313, 627}}, {"test_data/pucch_demodulator_format2_test_input_symbols16.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates16.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits16.dat"}},
  {{274, 14, 0.64, {{0}, 225, {}, 10, 0, 1, 21611, 139}}, {"test_data/pucch_demodulator_format2_test_input_symbols18.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates18.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits18.dat"}},
  {{190, 14, 0.09, {{0}, 161, {}, 11, 0, 1, 35881, 701}}, {"test_data/pucch_demodulator_format2_test_input_symbols20.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates20.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits20.dat"}},
  {{248, 14, 0.36, {{0}, 219, {}, 12, 0, 1, 4205, 875}}, {"test_data/pucch_demodulator_format2_test_input_symbols22.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates22.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits22.dat"}},
  {{206, 14, 0.09, {{0}, 173, {}, 13, 0, 1, 1747, 900}}, {"test_data/pucch_demodulator_format2_test_input_symbols24.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates24.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits24.dat"}},
  {{100, 14, 0.49, {{0}, 80, {}, 14, 0, 1, 41067, 158}}, {"test_data/pucch_demodulator_format2_test_input_symbols26.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates26.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits26.dat"}},
  {{270, 14, 0.81, {{0}, 245, {}, 15, 0, 1, 31470, 613}}, {"test_data/pucch_demodulator_format2_test_input_symbols28.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates28.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits28.dat"}},
  {{247, 14, 0.64, {{0}, 224, {}, 16, 0, 1, 1060, 896}}, {"test_data/pucch_demodulator_format2_test_input_symbols30.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates30.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits30.dat"}},
  {{157, 14, 0.64, {{0}, 110, {}, 1, 6, 2, 9272, 282}}, {"test_data/pucch_demodulator_format2_test_input_symbols32.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates32.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits32.dat"}},
  {{233, 14, 0.25, {{0}, 216, {}, 2, 6, 2, 24652, 560}}, {"test_data/pucch_demodulator_format2_test_input_symbols34.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates34.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits34.dat"}},
  {{255, 14, 0.64, {{0}, 120, {}, 3, 6, 2, 50794, 1023}}, {"test_data/pucch_demodulator_format2_test_input_symbols36.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates36.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits36.dat"}},
  {{89, 14, 1, {{0}, 80, {}, 4, 6, 2, 36049, 548}}, {"test_data/pucch_demodulator_format2_test_input_symbols38.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates38.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits38.dat"}},
  {{269, 14, 0.01, {{0}, 256, {}, 5, 6, 2, 27553, 681}}, {"test_data/pucch_demodulator_format2_test_input_symbols40.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates40.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits40.dat"}},
  {{262, 14, 0.36, {{0}, 256, {}, 6, 6, 2, 55819, 622}}, {"test_data/pucch_demodulator_format2_test_input_symbols42.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates42.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits42.dat"}},
  {{171, 14, 0.81, {{0}, 92, {}, 7, 6, 2, 1886, 709}}, {"test_data/pucch_demodulator_format2_test_input_symbols44.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates44.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits44.dat"}},
  {{154, 14, 0.25, {{0}, 127, {}, 8, 6, 2, 57419, 12}}, {"test_data/pucch_demodulator_format2_test_input_symbols46.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates46.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits46.dat"}},
  {{217, 14, 1, {{0}, 181, {}, 9, 6, 2, 7091, 712}}, {"test_data/pucch_demodulator_format2_test_input_symbols48.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates48.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits48.dat"}},
  {{85, 14, 0.81, {{0}, 44, {}, 10, 6, 2, 60965, 668}}, {"test_data/pucch_demodulator_format2_test_input_symbols50.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates50.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits50.dat"}},
  {{98, 14, 0.36, {{0}, 74, {}, 11, 6, 2, 4545, 479}}, {"test_data/pucch_demodulator_format2_test_input_symbols52.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates52.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits52.dat"}},
  {{165, 14, 0.49, {{0}, 114, {}, 12, 6, 2, 489, 413}}, {"test_data/pucch_demodulator_format2_test_input_symbols54.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates54.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits54.dat"}},
  {{139, 14, 0.64, {{0}, 89, {}, 13, 6, 2, 8520, 780}}, {"test_data/pucch_demodulator_format2_test_input_symbols56.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates56.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits56.dat"}},
  {{244, 14, 0.16, {{0}, 169, {}, 14, 6, 2, 22925, 346}}, {"test_data/pucch_demodulator_format2_test_input_symbols58.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates58.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits58.dat"}},
  {{212, 14, 0.49, {{0}, 188, {}, 15, 6, 2, 12360, 184}}, {"test_data/pucch_demodulator_format2_test_input_symbols60.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates60.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits60.dat"}},
  {{259, 14, 0.16, {{0}, 203, {}, 16, 6, 2, 12854, 726}}, {"test_data/pucch_demodulator_format2_test_input_symbols62.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates62.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits62.dat"}},
  {{222, 14, 0.49, {{0}, 217, {}, 1, 12, 2, 14208, 1020}}, {"test_data/pucch_demodulator_format2_test_input_symbols64.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates64.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits64.dat"}},
  {{145, 14, 0.16, {{0}, 109, {}, 2, 12, 2, 6407, 360}}, {"test_data/pucch_demodulator_format2_test_input_symbols66.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates66.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits66.dat"}},
  {{210, 14, 0.25, {{0}, 162, {}, 3, 12, 2, 439, 850}}, {"test_data/pucch_demodulator_format2_test_input_symbols68.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates68.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits68.dat"}},
  {{130, 14, 0.49, {{0}, 103, {}, 4, 12, 2, 3542, 971}}, {"test_data/pucch_demodulator_format2_test_input_symbols70.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates70.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits70.dat"}},
  {{244, 14, 0.49, {{0}, 217, {}, 5, 12, 2, 8322, 275}}, {"test_data/pucch_demodulator_format2_test_input_symbols72.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates72.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits72.dat"}},
  {{275, 14, 0.49, {{0}, 268, {}, 6, 12, 2, 41838, 356}}, {"test_data/pucch_demodulator_format2_test_input_symbols74.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates74.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits74.dat"}},
  {{268, 14, 0.01, {{0}, 256, {}, 7, 12, 2, 39116, 25}}, {"test_data/pucch_demodulator_format2_test_input_symbols76.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates76.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits76.dat"}},
  {{176, 14, 0.64, {{0}, 162, {}, 8, 12, 2, 6357, 962}}, {"test_data/pucch_demodulator_format2_test_input_symbols78.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates78.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits78.dat"}},
  {{265, 14, 0.09, {{0}, 207, {}, 9, 12, 2, 56753, 725}}, {"test_data/pucch_demodulator_format2_test_input_symbols80.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates80.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits80.dat"}},
  {{261, 14, 0.49, {{0}, 248, {}, 10, 12, 2, 24232, 624}}, {"test_data/pucch_demodulator_format2_test_input_symbols82.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates82.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits82.dat"}},
  {{250, 14, 0.81, {{0}, 198, {}, 11, 12, 2, 1075, 702}}, {"test_data/pucch_demodulator_format2_test_input_symbols84.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates84.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits84.dat"}},
  {{266, 14, 0.36, {{0}, 253, {}, 12, 12, 2, 7854, 180}}, {"test_data/pucch_demodulator_format2_test_input_symbols86.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates86.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits86.dat"}},
  {{275, 14, 1, {{0}, 159, {}, 13, 12, 2, 5546, 559}}, {"test_data/pucch_demodulator_format2_test_input_symbols88.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates88.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits88.dat"}},
  {{271, 14, 0.09, {{0}, 217, {}, 14, 12, 2, 19743, 77}}, {"test_data/pucch_demodulator_format2_test_input_symbols90.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates90.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits90.dat"}},
  {{201, 14, 0.25, {{0}, 139, {}, 15, 12, 2, 13974, 531}}, {"test_data/pucch_demodulator_format2_test_input_symbols92.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates92.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits92.dat"}},
  {{274, 14, 1, {{0}, 228, {}, 16, 12, 2, 2880, 884}}, {"test_data/pucch_demodulator_format2_test_input_symbols94.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates94.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits94.dat"}},
  {{237, 14, 0.09, {{0}, 211, 203, 1, 6, 2, 39178, 541}}, {"test_data/pucch_demodulator_format2_test_input_symbols96.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates96.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits96.dat"}},
  {{220, 14, 0.25, {{0}, 162, 123, 2, 6, 2, 45534, 284}}, {"test_data/pucch_demodulator_format2_test_input_symbols98.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates98.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits98.dat"}},
  {{252, 14, 0.49, {{0}, 214, 198, 3, 6, 2, 24056, 1022}}, {"test_data/pucch_demodulator_format2_test_input_symbols100.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates100.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits100.dat"}},
  {{147, 14, 0.16, {{0}, 87, 119, 4, 6, 2, 37001, 459}}, {"test_data/pucch_demodulator_format2_test_input_symbols102.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates102.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits102.dat"}},
  {{99, 14, 0.49, {{0}, 76, 77, 5, 6, 2, 43075, 587}}, {"test_data/pucch_demodulator_format2_test_input_symbols104.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates104.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits104.dat"}},
  {{221, 14, 0.36, {{0}, 182, 197, 6, 6, 2, 14291, 365}}, {"test_data/pucch_demodulator_format2_test_input_symbols106.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates106.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits106.dat"}},
  {{253, 14, 0.64, {{0}, 243, 243, 7, 6, 2, 14542, 384}}, {"test_data/pucch_demodulator_format2_test_input_symbols108.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates108.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits108.dat"}},
  {{260, 14, 0.04, {{0}, 245, 247, 8, 6, 2, 39483, 59}}, {"test_data/pucch_demodulator_format2_test_input_symbols110.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates110.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits110.dat"}},
  {{174, 14, 0.09, {{0}, 85, 128, 9, 6, 2, 33238, 80}}, {"test_data/pucch_demodulator_format2_test_input_symbols112.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates112.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits112.dat"}},
  {{235, 14, 0.49, {{0}, 129, 155, 10, 6, 2, 2504, 592}}, {"test_data/pucch_demodulator_format2_test_input_symbols114.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates114.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits114.dat"}},
  {{192, 14, 1, {{0}, 180, 174, 11, 6, 2, 47715, 94}}, {"test_data/pucch_demodulator_format2_test_input_symbols116.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates116.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits116.dat"}},
  {{221, 14, 0.36, {{0}, 173, 160, 12, 6, 2, 18665, 26}}, {"test_data/pucch_demodulator_format2_test_input_symbols118.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates118.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits118.dat"}},
  {{253, 14, 0.36, {{0}, 225, 222, 13, 6, 2, 22191, 622}}, {"test_data/pucch_demodulator_format2_test_input_symbols120.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates120.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits120.dat"}},
  {{259, 14, 0.49, {{0}, 245, 242, 14, 6, 2, 3089, 823}}, {"test_data/pucch_demodulator_format2_test_input_symbols122.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates122.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits122.dat"}},
  {{216, 14, 0.01, {{0}, 137, 83, 15, 6, 2, 12254, 791}}, {"test_data/pucch_demodulator_format2_test_input_symbols124.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates124.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits124.dat"}},
  {{255, 14, 1, {{0}, 239, 239, 16, 6, 2, 9730, 389}}, {"test_data/pucch_demodulator_format2_test_input_symbols126.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates126.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits126.dat"}},
  {{229, 14, 0.49, {{0}, 191, 188, 1, 12, 2, 54590, 261}}, {"test_data/pucch_demodulator_format2_test_input_symbols128.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates128.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits128.dat"}},
  {{175, 14, 0.16, {{0}, 121, 173, 2, 12, 2, 48545, 915}}, {"test_data/pucch_demodulator_format2_test_input_symbols130.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates130.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits130.dat"}},
  {{252, 14, 1, {{0}, 124, 126, 3, 12, 2, 12792, 830}}, {"test_data/pucch_demodulator_format2_test_input_symbols132.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates132.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits132.dat"}},
  {{140, 14, 0.64, {{0}, 136, 132, 4, 12, 2, 32951, 399}}, {"test_data/pucch_demodulator_format2_test_input_symbols134.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates134.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits134.dat"}},
  {{269, 14, 0.04, {{0}, 261, 253, 5, 12, 2, 24527, 48}}, {"test_data/pucch_demodulator_format2_test_input_symbols136.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates136.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits136.dat"}},
  {{151, 14, 0.64, {{0}, 104, 70, 6, 12, 2, 52911, 299}}, {"test_data/pucch_demodulator_format2_test_input_symbols138.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates138.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits138.dat"}},
  {{161, 14, 0.49, {{0}, 83, 140, 7, 12, 2, 13399, 483}}, {"test_data/pucch_demodulator_format2_test_input_symbols140.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates140.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits140.dat"}},
  {{111, 14, 0.16, {{0}, 79, 85, 8, 12, 2, 6368, 960}}, {"test_data/pucch_demodulator_format2_test_input_symbols142.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates142.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits142.dat"}},
  {{233, 14, 0.64, {{0}, 223, 221, 9, 12, 2, 41800, 648}}, {"test_data/pucch_demodulator_format2_test_input_symbols144.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates144.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits144.dat"}},
  {{267, 14, 0.36, {{0}, 257, 257, 10, 12, 2, 11923, 7}}, {"test_data/pucch_demodulator_format2_test_input_symbols146.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates146.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits146.dat"}},
  {{116, 14, 0.49, {{0}, 79, 90, 11, 12, 2, 524, 905}}, {"test_data/pucch_demodulator_format2_test_input_symbols148.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates148.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits148.dat"}},
  {{269, 14, 0.49, {{0}, 206, 212, 12, 12, 2, 33351, 967}}, {"test_data/pucch_demodulator_format2_test_input_symbols150.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates150.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits150.dat"}},
  {{216, 14, 0.36, {{0}, 88, 164, 13, 12, 2, 617, 495}}, {"test_data/pucch_demodulator_format2_test_input_symbols152.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates152.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits152.dat"}},
  {{275, 14, 0.64, {{0}, 233, 241, 14, 12, 2, 712, 1}}, {"test_data/pucch_demodulator_format2_test_input_symbols154.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates154.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits154.dat"}},
  {{185, 14, 0.81, {{0}, 145, 139, 15, 12, 2, 41390, 363}}, {"test_data/pucch_demodulator_format2_test_input_symbols156.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates156.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits156.dat"}},
  {{243, 14, 0.81, {{0}, 195, 222, 16, 12, 2, 25049, 31}}, {"test_data/pucch_demodulator_format2_test_input_symbols158.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates158.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits158.dat"}},
    // clang-format on
};

} // namespace srsran
