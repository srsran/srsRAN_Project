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

// This file was generated using the following MATLAB class on 26-Oct-2022:
//   + "srsPUCCHProcessorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pucch_processor.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pucch_processor::format1_configuration                  config;
  std::vector<uint8_t>                                    ack_bits;
  file_vector<resource_grid_reader_spy::expected_entry_t> data_symbols;
  file_vector<resource_grid_reader_spy::expected_entry_t> dmrs_symbols;
};

static const std::vector<test_case_t> pucch_processor_format1_test_data = {
    // clang-format off
  {{{0, 1309}, 52, 0, cyclic_prefix::NORMAL, 32, {}, 821, 0, {0}, 10, 14, 0, 0}, {}, {"test_data/pucch_processor_format1_test_input_data0.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs0.dat"}},
  {{{0, 9805}, 52, 0, cyclic_prefix::NORMAL, 8, {}, 280, 1, {0}, 11, 14, 0, 6}, {1}, {"test_data/pucch_processor_format1_test_input_data2.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs2.dat"}},
  {{{0, 1458}, 52, 0, cyclic_prefix::NORMAL, 47, {}, 489, 2, {0}, 5, 14, 0, 5}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data4.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs4.dat"}},
  {{{0, 9568}, 52, 0, cyclic_prefix::NORMAL, 39, {}, 35, 0, {0}, 8, 13, 1, 4}, {}, {"test_data/pucch_processor_format1_test_input_data6.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs6.dat"}},
  {{{0, 1756}, 52, 0, cyclic_prefix::NORMAL, 1, {}, 395, 1, {0}, 8, 13, 1, 1}, {0}, {"test_data/pucch_processor_format1_test_input_data8.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs8.dat"}},
  {{{0, 7118}, 52, 0, cyclic_prefix::NORMAL, 49, {}, 97, 2, {0}, 3, 13, 1, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_data10.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs10.dat"}},
  {{{0, 1917}, 52, 0, cyclic_prefix::NORMAL, 23, {}, 771, 0, {0}, 5, 5, 5, 1}, {}, {"test_data/pucch_processor_format1_test_input_data12.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs12.dat"}},
  {{{0, 2827}, 52, 0, cyclic_prefix::NORMAL, 34, {}, 715, 1, {0}, 8, 5, 5, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data14.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs14.dat"}},
  {{{0, 3489}, 52, 0, cyclic_prefix::NORMAL, 11, {}, 502, 2, {0}, 7, 5, 5, 1}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data16.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs16.dat"}},
  {{{0, 9828}, 52, 0, cyclic_prefix::NORMAL, 7, {}, 704, 0, {0}, 6, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_data18.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs18.dat"}},
  {{{0, 2608}, 52, 0, cyclic_prefix::NORMAL, 12, {}, 259, 1, {0}, 9, 4, 10, 1}, {0}, {"test_data/pucch_processor_format1_test_input_data20.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs20.dat"}},
  {{{0, 6302}, 52, 0, cyclic_prefix::NORMAL, 18, {}, 198, 2, {0}, 5, 4, 10, 1}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data22.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs22.dat"}},
  {{{0, 7752}, 52, 0, cyclic_prefix::NORMAL, 19, {3}, 924, 0, {0}, 9, 14, 0, 1}, {}, {"test_data/pucch_processor_format1_test_input_data24.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs24.dat"}},
  {{{0, 7975}, 52, 0, cyclic_prefix::NORMAL, 6, {24}, 54, 1, {0}, 11, 14, 0, 1}, {0}, {"test_data/pucch_processor_format1_test_input_data26.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs26.dat"}},
  {{{0, 8131}, 52, 0, cyclic_prefix::NORMAL, 27, {31}, 339, 2, {0}, 3, 14, 0, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data28.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs28.dat"}},
  {{{0, 4617}, 52, 0, cyclic_prefix::NORMAL, 11, {7}, 694, 0, {0}, 1, 13, 1, 2}, {}, {"test_data/pucch_processor_format1_test_input_data30.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs30.dat"}},
  {{{0, 10205}, 52, 0, cyclic_prefix::NORMAL, 23, {50}, 832, 1, {0}, 0, 13, 1, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data32.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs32.dat"}},
  {{{0, 8898}, 52, 0, cyclic_prefix::NORMAL, 20, {41}, 781, 2, {0}, 1, 13, 1, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data34.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs34.dat"}},
  {{{0, 1492}, 52, 0, cyclic_prefix::NORMAL, 45, {30}, 183, 0, {0}, 1, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_data36.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs36.dat"}},
  {{{0, 8731}, 52, 0, cyclic_prefix::NORMAL, 18, {26}, 554, 1, {0}, 7, 5, 5, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data38.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs38.dat"}},
  {{{0, 1262}, 52, 0, cyclic_prefix::NORMAL, 12, {21}, 76, 2, {0}, 2, 5, 5, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data40.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs40.dat"}},
  {{{0, 5004}, 52, 0, cyclic_prefix::NORMAL, 46, {19}, 952, 0, {0}, 4, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_data42.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs42.dat"}},
  {{{0, 3997}, 52, 0, cyclic_prefix::NORMAL, 21, {5}, 112, 1, {0}, 2, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data44.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs44.dat"}},
  {{{0, 5899}, 52, 0, cyclic_prefix::NORMAL, 12, {18}, 949, 2, {0}, 0, 4, 10, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data46.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs46.dat"}},
  {{{1, 13283}, 52, 0, cyclic_prefix::NORMAL, 33, {}, 43, 0, {0}, 8, 14, 0, 3}, {}, {"test_data/pucch_processor_format1_test_input_data48.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs48.dat"}},
  {{{1, 15245}, 52, 0, cyclic_prefix::NORMAL, 35, {}, 551, 1, {0}, 2, 14, 0, 1}, {0}, {"test_data/pucch_processor_format1_test_input_data50.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs50.dat"}},
  {{{1, 1675}, 52, 0, cyclic_prefix::NORMAL, 40, {}, 630, 2, {0}, 11, 14, 0, 3}, {0, 0}, {"test_data/pucch_processor_format1_test_input_data52.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs52.dat"}},
  {{{1, 10470}, 52, 0, cyclic_prefix::NORMAL, 41, {}, 308, 0, {0}, 9, 13, 1, 3}, {}, {"test_data/pucch_processor_format1_test_input_data54.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs54.dat"}},
  {{{1, 10916}, 52, 0, cyclic_prefix::NORMAL, 48, {}, 381, 1, {0}, 4, 13, 1, 5}, {1}, {"test_data/pucch_processor_format1_test_input_data56.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs56.dat"}},
  {{{1, 4251}, 52, 0, cyclic_prefix::NORMAL, 24, {}, 627, 2, {0}, 3, 13, 1, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data58.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs58.dat"}},
  {{{1, 4663}, 52, 0, cyclic_prefix::NORMAL, 16, {}, 227, 0, {0}, 5, 5, 5, 1}, {}, {"test_data/pucch_processor_format1_test_input_data60.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs60.dat"}},
  {{{1, 18523}, 52, 0, cyclic_prefix::NORMAL, 22, {}, 433, 1, {0}, 11, 5, 5, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data62.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs62.dat"}},
  {{{1, 5371}, 52, 0, cyclic_prefix::NORMAL, 36, {}, 411, 2, {0}, 7, 5, 5, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_data64.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs64.dat"}},
  {{{1, 10408}, 52, 0, cyclic_prefix::NORMAL, 13, {}, 321, 0, {0}, 1, 4, 10, 1}, {}, {"test_data/pucch_processor_format1_test_input_data66.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs66.dat"}},
  {{{1, 14958}, 52, 0, cyclic_prefix::NORMAL, 30, {}, 29, 1, {0}, 5, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data68.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs68.dat"}},
  {{{1, 10670}, 52, 0, cyclic_prefix::NORMAL, 25, {}, 970, 2, {0}, 2, 4, 10, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data70.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs70.dat"}},
  {{{1, 779}, 52, 0, cyclic_prefix::NORMAL, 47, {5}, 370, 0, {0}, 10, 14, 0, 2}, {}, {"test_data/pucch_processor_format1_test_input_data72.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs72.dat"}},
  {{{1, 13926}, 52, 0, cyclic_prefix::NORMAL, 37, {33}, 263, 1, {0}, 1, 14, 0, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data74.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs74.dat"}},
  {{{1, 18514}, 52, 0, cyclic_prefix::NORMAL, 17, {10}, 785, 2, {0}, 10, 14, 0, 2}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data76.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs76.dat"}},
  {{{1, 18529}, 52, 0, cyclic_prefix::NORMAL, 32, {41}, 504, 0, {0}, 7, 13, 1, 2}, {}, {"test_data/pucch_processor_format1_test_input_data78.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs78.dat"}},
  {{{1, 4903}, 52, 0, cyclic_prefix::NORMAL, 1, {8}, 581, 1, {0}, 10, 13, 1, 1}, {1}, {"test_data/pucch_processor_format1_test_input_data80.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs80.dat"}},
  {{{1, 9650}, 52, 0, cyclic_prefix::NORMAL, 35, {3}, 718, 2, {0}, 0, 13, 1, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data82.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs82.dat"}},
  {{{1, 14796}, 52, 0, cyclic_prefix::NORMAL, 34, {26}, 824, 0, {0}, 1, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_data84.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs84.dat"}},
  {{{1, 16392}, 52, 0, cyclic_prefix::NORMAL, 22, {42}, 980, 1, {0}, 5, 5, 5, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data86.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs86.dat"}},
  {{{1, 8003}, 52, 0, cyclic_prefix::NORMAL, 41, {3}, 134, 2, {0}, 9, 5, 5, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data88.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs88.dat"}},
  {{{1, 12873}, 52, 0, cyclic_prefix::NORMAL, 22, {0}, 420, 0, {0}, 3, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_data90.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs90.dat"}},
  {{{1, 2163}, 52, 0, cyclic_prefix::NORMAL, 10, {25}, 991, 1, {0}, 4, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data92.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs92.dat"}},
  {{{1, 1078}, 52, 0, cyclic_prefix::NORMAL, 13, {21}, 959, 2, {0}, 8, 4, 10, 0}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data94.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs94.dat"}},
    // clang-format on
};

} // namespace srsgnb
