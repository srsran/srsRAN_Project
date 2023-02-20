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

// This file was generated using the following MATLAB class on 13-01-2023:
//   + "srsPUCCHDemodulatorFormat2Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch_demodulator.h"
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
  {{226, 14, 0.04, {{0}, 224, 1, 0, 1, 60420, 514}}, {"test_data/pucch_demodulator_format2_test_input_symbols0.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates0.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits0.dat"}},
  {{257, 14, 0.64, {{0}, 217, 2, 0, 1, 8251, 1003}}, {"test_data/pucch_demodulator_format2_test_input_symbols2.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates2.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits2.dat"}},
  {{199, 14, 0.81, {{0}, 127, 3, 0, 1, 33840, 841}}, {"test_data/pucch_demodulator_format2_test_input_symbols4.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates4.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits4.dat"}},
  {{263, 14, 1, {{0}, 178, 4, 0, 1, 40066, 1005}}, {"test_data/pucch_demodulator_format2_test_input_symbols6.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates6.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits6.dat"}},
  {{176, 14, 0.16, {{0}, 142, 5, 0, 1, 9355, 309}}, {"test_data/pucch_demodulator_format2_test_input_symbols8.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates8.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits8.dat"}},
  {{212, 14, 0.16, {{0}, 189, 6, 0, 1, 28195, 155}}, {"test_data/pucch_demodulator_format2_test_input_symbols10.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates10.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits10.dat"}},
  {{226, 14, 0.64, {{0}, 198, 7, 0, 1, 62499, 709}}, {"test_data/pucch_demodulator_format2_test_input_symbols12.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates12.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits12.dat"}},
  {{262, 14, 0.09, {{0}, 218, 8, 0, 1, 30070, 133}}, {"test_data/pucch_demodulator_format2_test_input_symbols14.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates14.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits14.dat"}},
  {{184, 14, 0.81, {{0}, 123, 9, 0, 1, 37606, 543}}, {"test_data/pucch_demodulator_format2_test_input_symbols16.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates16.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits16.dat"}},
  {{236, 14, 1, {{0}, 197, 10, 0, 1, 1909, 445}}, {"test_data/pucch_demodulator_format2_test_input_symbols18.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates18.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits18.dat"}},
  {{263, 14, 0.16, {{0}, 247, 11, 0, 1, 25993, 989}}, {"test_data/pucch_demodulator_format2_test_input_symbols20.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates20.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits20.dat"}},
  {{237, 14, 0.25, {{0}, 211, 12, 0, 1, 49804, 901}}, {"test_data/pucch_demodulator_format2_test_input_symbols22.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates22.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits22.dat"}},
  {{131, 14, 0.01, {{0}, 112, 13, 0, 1, 11444, 290}}, {"test_data/pucch_demodulator_format2_test_input_symbols24.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates24.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits24.dat"}},
  {{198, 14, 0.49, {{0}, 176, 14, 0, 1, 22815, 571}}, {"test_data/pucch_demodulator_format2_test_input_symbols26.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates26.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits26.dat"}},
  {{239, 14, 0.04, {{0}, 215, 15, 0, 1, 403, 983}}, {"test_data/pucch_demodulator_format2_test_input_symbols28.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates28.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits28.dat"}},
  {{271, 14, 0.64, {{0}, 251, 16, 0, 1, 35908, 249}}, {"test_data/pucch_demodulator_format2_test_input_symbols30.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates30.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits30.dat"}},
  {{232, 14, 0.25, {{0}, 118, 1, 6, 2, 50913, 342}}, {"test_data/pucch_demodulator_format2_test_input_symbols32.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates32.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits32.dat"}},
  {{220, 14, 0.25, {{0}, 184, 2, 6, 2, 39786, 570}}, {"test_data/pucch_demodulator_format2_test_input_symbols34.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates34.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits34.dat"}},
  {{155, 14, 0.49, {{0}, 150, 3, 6, 2, 58859, 992}}, {"test_data/pucch_demodulator_format2_test_input_symbols36.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates36.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits36.dat"}},
  {{211, 14, 0.81, {{0}, 19, 4, 6, 2, 54757, 215}}, {"test_data/pucch_demodulator_format2_test_input_symbols38.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates38.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits38.dat"}},
  {{269, 14, 0.09, {{0}, 253, 5, 6, 2, 61554, 181}}, {"test_data/pucch_demodulator_format2_test_input_symbols40.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates40.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits40.dat"}},
  {{72, 14, 0.25, {{0}, 30, 6, 6, 2, 3372, 297}}, {"test_data/pucch_demodulator_format2_test_input_symbols42.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates42.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits42.dat"}},
  {{117, 14, 0.81, {{0}, 109, 7, 6, 2, 18067, 899}}, {"test_data/pucch_demodulator_format2_test_input_symbols44.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates44.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits44.dat"}},
  {{133, 14, 0.36, {{0}, 101, 8, 6, 2, 3583, 638}}, {"test_data/pucch_demodulator_format2_test_input_symbols46.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates46.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits46.dat"}},
  {{232, 14, 0.01, {{0}, 155, 9, 6, 2, 54712, 17}}, {"test_data/pucch_demodulator_format2_test_input_symbols48.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates48.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits48.dat"}},
  {{31, 14, 0.04, {{0}, 16, 10, 6, 2, 58295, 346}}, {"test_data/pucch_demodulator_format2_test_input_symbols50.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates50.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits50.dat"}},
  {{242, 14, 0.25, {{0}, 183, 11, 6, 2, 9902, 674}}, {"test_data/pucch_demodulator_format2_test_input_symbols52.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates52.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits52.dat"}},
  {{223, 14, 0.81, {{0}, 103, 12, 6, 2, 40666, 696}}, {"test_data/pucch_demodulator_format2_test_input_symbols54.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates54.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits54.dat"}},
  {{271, 14, 1, {{0}, 258, 13, 6, 2, 642, 60}}, {"test_data/pucch_demodulator_format2_test_input_symbols56.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates56.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits56.dat"}},
  {{268, 14, 0.36, {{0}, 247, 14, 6, 2, 53052, 185}}, {"test_data/pucch_demodulator_format2_test_input_symbols58.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates58.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits58.dat"}},
  {{184, 14, 0.16, {{0}, 95, 15, 6, 2, 44932, 610}}, {"test_data/pucch_demodulator_format2_test_input_symbols60.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates60.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits60.dat"}},
  {{190, 14, 0.01, {{0}, 162, 16, 6, 2, 32354, 541}}, {"test_data/pucch_demodulator_format2_test_input_symbols62.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates62.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits62.dat"}},
  {{256, 14, 0.64, {{0}, 193, 1, 12, 2, 11305, 53}}, {"test_data/pucch_demodulator_format2_test_input_symbols64.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates64.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits64.dat"}},
  {{211, 14, 0.49, {{0}, 159, 2, 12, 2, 25422, 522}}, {"test_data/pucch_demodulator_format2_test_input_symbols66.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates66.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits66.dat"}},
  {{273, 14, 0.36, {{0}, 268, 3, 12, 2, 20292, 20}}, {"test_data/pucch_demodulator_format2_test_input_symbols68.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates68.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits68.dat"}},
  {{131, 14, 1, {{0}, 121, 4, 12, 2, 45146, 320}}, {"test_data/pucch_demodulator_format2_test_input_symbols70.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates70.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits70.dat"}},
  {{169, 14, 0.04, {{0}, 97, 5, 12, 2, 26438, 848}}, {"test_data/pucch_demodulator_format2_test_input_symbols72.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates72.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits72.dat"}},
  {{150, 14, 0.49, {{0}, 144, 6, 12, 2, 56958, 285}}, {"test_data/pucch_demodulator_format2_test_input_symbols74.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates74.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits74.dat"}},
  {{274, 14, 0.64, {{0}, 267, 7, 12, 2, 41260, 714}}, {"test_data/pucch_demodulator_format2_test_input_symbols76.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates76.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits76.dat"}},
  {{266, 14, 0.16, {{0}, 240, 8, 12, 2, 47056, 189}}, {"test_data/pucch_demodulator_format2_test_input_symbols78.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates78.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits78.dat"}},
  {{138, 14, 0.36, {{0}, 55, 9, 12, 2, 41574, 205}}, {"test_data/pucch_demodulator_format2_test_input_symbols80.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates80.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits80.dat"}},
  {{269, 14, 0.04, {{0}, 254, 10, 12, 2, 45467, 311}}, {"test_data/pucch_demodulator_format2_test_input_symbols82.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates82.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits82.dat"}},
  {{260, 14, 0.25, {{0}, 189, 11, 12, 2, 17890, 687}}, {"test_data/pucch_demodulator_format2_test_input_symbols84.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates84.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits84.dat"}},
  {{179, 14, 0.64, {{0}, 88, 12, 12, 2, 57322, 599}}, {"test_data/pucch_demodulator_format2_test_input_symbols86.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates86.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits86.dat"}},
  {{68, 14, 0.36, {{0}, 28, 13, 12, 2, 24817, 538}}, {"test_data/pucch_demodulator_format2_test_input_symbols88.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates88.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits88.dat"}},
  {{102, 14, 0.04, {{0}, 44, 14, 12, 2, 1597, 428}}, {"test_data/pucch_demodulator_format2_test_input_symbols90.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates90.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits90.dat"}},
  {{254, 14, 0.25, {{0}, 235, 15, 12, 2, 8118, 404}}, {"test_data/pucch_demodulator_format2_test_input_symbols92.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates92.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits92.dat"}},
  {{223, 14, 0.04, {{0}, 186, 16, 12, 2, 51360, 269}}, {"test_data/pucch_demodulator_format2_test_input_symbols94.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates94.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits94.dat"}},
    // clang-format on
};

} // namespace srsran
