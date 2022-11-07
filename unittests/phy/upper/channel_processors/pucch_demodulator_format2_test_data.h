/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 16-Nov-2022:
//   + "srsPUCCHDemodulatorFormat2Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pucch_demodulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

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
  {{269, 14, 0.81, {{0}, 91, 1, 0, 1, 18211, 721}}, {"test_data/pucch_demodulator_format2_test_input_symbols0.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates0.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits0.dat"}},
  {{185, 14, 1, {{0}, 97, 2, 0, 1, 27076, 37}}, {"test_data/pucch_demodulator_format2_test_input_symbols2.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates2.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits2.dat"}},
  {{18, 14, 0.64, {{0}, 15, 3, 0, 1, 12888, 375}}, {"test_data/pucch_demodulator_format2_test_input_symbols4.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates4.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits4.dat"}},
  {{274, 14, 0.81, {{0}, 117, 4, 0, 1, 58231, 582}}, {"test_data/pucch_demodulator_format2_test_input_symbols6.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates6.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits6.dat"}},
  {{233, 14, 0.16, {{0}, 219, 5, 0, 1, 7829, 248}}, {"test_data/pucch_demodulator_format2_test_input_symbols8.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates8.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits8.dat"}},
  {{245, 14, 0.01, {{0}, 217, 6, 0, 1, 25090, 838}}, {"test_data/pucch_demodulator_format2_test_input_symbols10.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates10.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits10.dat"}},
  {{243, 14, 0.04, {{0}, 156, 7, 0, 1, 8552, 221}}, {"test_data/pucch_demodulator_format2_test_input_symbols12.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates12.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits12.dat"}},
  {{260, 14, 0.09, {{0}, 217, 8, 0, 1, 9126, 603}}, {"test_data/pucch_demodulator_format2_test_input_symbols14.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates14.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits14.dat"}},
  {{251, 14, 0.25, {{0}, 208, 9, 0, 1, 22673, 729}}, {"test_data/pucch_demodulator_format2_test_input_symbols16.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates16.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits16.dat"}},
  {{239, 14, 0.25, {{0}, 229, 10, 0, 1, 15414, 382}}, {"test_data/pucch_demodulator_format2_test_input_symbols18.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates18.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits18.dat"}},
  {{274, 14, 0.16, {{0}, 261, 11, 0, 1, 30484, 121}}, {"test_data/pucch_demodulator_format2_test_input_symbols20.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates20.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits20.dat"}},
  {{58, 14, 0.04, {{0}, 44, 12, 0, 1, 33352, 539}}, {"test_data/pucch_demodulator_format2_test_input_symbols22.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates22.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits22.dat"}},
  {{260, 14, 0.09, {{0}, 226, 13, 0, 1, 32166, 67}}, {"test_data/pucch_demodulator_format2_test_input_symbols24.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates24.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits24.dat"}},
  {{191, 14, 0.04, {{0}, 135, 14, 0, 1, 62128, 762}}, {"test_data/pucch_demodulator_format2_test_input_symbols26.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates26.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits26.dat"}},
  {{266, 14, 0.16, {{0}, 240, 15, 0, 1, 8736, 270}}, {"test_data/pucch_demodulator_format2_test_input_symbols28.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates28.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits28.dat"}},
  {{262, 14, 0.16, {{0}, 215, 16, 0, 1, 29346, 288}}, {"test_data/pucch_demodulator_format2_test_input_symbols30.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates30.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits30.dat"}},
  {{135, 14, 0.04, {{0}, 131, 1, 6, 2, 45724, 224}}, {"test_data/pucch_demodulator_format2_test_input_symbols32.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates32.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits32.dat"}},
  {{225, 14, 0.49, {{0}, 206, 2, 6, 2, 12617, 337}}, {"test_data/pucch_demodulator_format2_test_input_symbols34.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates34.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits34.dat"}},
  {{257, 14, 0.49, {{0}, 252, 3, 6, 2, 54181, 918}}, {"test_data/pucch_demodulator_format2_test_input_symbols36.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates36.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits36.dat"}},
  {{228, 14, 0.36, {{0}, 195, 4, 6, 2, 10745, 528}}, {"test_data/pucch_demodulator_format2_test_input_symbols38.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates38.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits38.dat"}},
  {{272, 14, 0.25, {{0}, 264, 5, 6, 2, 24128, 697}}, {"test_data/pucch_demodulator_format2_test_input_symbols40.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates40.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits40.dat"}},
  {{272, 14, 0.04, {{0}, 239, 6, 6, 2, 9772, 842}}, {"test_data/pucch_demodulator_format2_test_input_symbols42.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates42.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits42.dat"}},
  {{88, 14, 0.81, {{0}, 70, 7, 6, 2, 15920, 836}}, {"test_data/pucch_demodulator_format2_test_input_symbols44.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates44.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits44.dat"}},
  {{213, 14, 1, {{0}, 126, 8, 6, 2, 43440, 663}}, {"test_data/pucch_demodulator_format2_test_input_symbols46.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates46.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits46.dat"}},
  {{275, 14, 0.49, {{0}, 266, 9, 6, 2, 37696, 294}}, {"test_data/pucch_demodulator_format2_test_input_symbols48.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates48.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits48.dat"}},
  {{268, 14, 0.64, {{0}, 158, 10, 6, 2, 2423, 892}}, {"test_data/pucch_demodulator_format2_test_input_symbols50.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates50.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits50.dat"}},
  {{56, 14, 0.25, {{0}, 35, 11, 6, 2, 23655, 906}}, {"test_data/pucch_demodulator_format2_test_input_symbols52.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates52.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits52.dat"}},
  {{184, 14, 0.16, {{0}, 147, 12, 6, 2, 54897, 599}}, {"test_data/pucch_demodulator_format2_test_input_symbols54.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates54.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits54.dat"}},
  {{221, 14, 0.49, {{0}, 80, 13, 6, 2, 31593, 332}}, {"test_data/pucch_demodulator_format2_test_input_symbols56.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates56.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits56.dat"}},
  {{256, 14, 0.64, {{0}, 241, 14, 6, 2, 13996, 885}}, {"test_data/pucch_demodulator_format2_test_input_symbols58.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates58.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits58.dat"}},
  {{267, 14, 0.01, {{0}, 156, 15, 6, 2, 13155, 634}}, {"test_data/pucch_demodulator_format2_test_input_symbols60.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates60.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits60.dat"}},
  {{241, 14, 0.81, {{0}, 202, 16, 6, 2, 24038, 1023}}, {"test_data/pucch_demodulator_format2_test_input_symbols62.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates62.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits62.dat"}},
  {{271, 14, 0.16, {{0}, 207, 1, 12, 2, 25544, 219}}, {"test_data/pucch_demodulator_format2_test_input_symbols64.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates64.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits64.dat"}},
  {{180, 14, 0.81, {{0}, 173, 2, 12, 2, 53364, 797}}, {"test_data/pucch_demodulator_format2_test_input_symbols66.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates66.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits66.dat"}},
  {{95, 14, 0.09, {{0}, 31, 3, 12, 2, 23593, 295}}, {"test_data/pucch_demodulator_format2_test_input_symbols68.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates68.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits68.dat"}},
  {{270, 14, 0.04, {{0}, 199, 4, 12, 2, 46037, 670}}, {"test_data/pucch_demodulator_format2_test_input_symbols70.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates70.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits70.dat"}},
  {{168, 14, 0.64, {{0}, 163, 5, 12, 2, 41324, 508}}, {"test_data/pucch_demodulator_format2_test_input_symbols72.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates72.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits72.dat"}},
  {{258, 14, 0.04, {{0}, 247, 6, 12, 2, 2397, 735}}, {"test_data/pucch_demodulator_format2_test_input_symbols74.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates74.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits74.dat"}},
  {{176, 14, 0.25, {{0}, 137, 7, 12, 2, 57697, 321}}, {"test_data/pucch_demodulator_format2_test_input_symbols76.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates76.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits76.dat"}},
  {{244, 14, 0.09, {{0}, 134, 8, 12, 2, 22700, 469}}, {"test_data/pucch_demodulator_format2_test_input_symbols78.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates78.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits78.dat"}},
  {{191, 14, 1, {{0}, 127, 9, 12, 2, 3090, 724}}, {"test_data/pucch_demodulator_format2_test_input_symbols80.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates80.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits80.dat"}},
  {{271, 14, 0.09, {{0}, 235, 10, 12, 2, 46187, 854}}, {"test_data/pucch_demodulator_format2_test_input_symbols82.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates82.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits82.dat"}},
  {{221, 14, 0.36, {{0}, 185, 11, 12, 2, 54986, 849}}, {"test_data/pucch_demodulator_format2_test_input_symbols84.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates84.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits84.dat"}},
  {{253, 14, 0.49, {{0}, 238, 12, 12, 2, 31786, 774}}, {"test_data/pucch_demodulator_format2_test_input_symbols86.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates86.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits86.dat"}},
  {{223, 14, 0.01, {{0}, 117, 13, 12, 2, 40962, 937}}, {"test_data/pucch_demodulator_format2_test_input_symbols88.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates88.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits88.dat"}},
  {{218, 14, 0.25, {{0}, 88, 14, 12, 2, 29660, 741}}, {"test_data/pucch_demodulator_format2_test_input_symbols90.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates90.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits90.dat"}},
  {{184, 14, 0.01, {{0}, 144, 15, 12, 2, 54433, 432}}, {"test_data/pucch_demodulator_format2_test_input_symbols92.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates92.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits92.dat"}},
  {{260, 14, 0.04, {{0}, 244, 16, 12, 2, 30325, 337}}, {"test_data/pucch_demodulator_format2_test_input_symbols94.dat"}, {"test_data/pucch_demodulator_format2_test_input_estimates94.dat"}, {"test_data/pucch_demodulator_format2_test_output_sch_soft_bits94.dat"}},
    // clang-format on
};

} // namespace srsgnb
