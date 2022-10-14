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

// This file was generated using the following MATLAB class on 14-Oct-2022:
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
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 821, 821, 0, 0, 0, 0, {0}}, 1, 14, 0, 4}, {}, {"test_data/pucch_processor_format1_test_input_data0.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs0.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 98, 98, 0, 1, 0, 0, {0}}, 6, 14, 0, 6}, {0}, {"test_data/pucch_processor_format1_test_input_data2.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs2.dat"}},
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 40, {}, 978, 978, 0, 2, 0, 0, {0}}, 5, 14, 0, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data4.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs4.dat"}},
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 1, {}, 798, 798, 0, 0, 0, 0, {0}}, 7, 13, 1, 5}, {}, {"test_data/pucch_processor_format1_test_input_data6.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs6.dat"}},
  {{{{0, 6}, 51, 1, cyclic_prefix::NORMAL, 37, {}, 941, 941, 0, 1, 0, 0, {0}}, 9, 13, 1, 2}, {1}, {"test_data/pucch_processor_format1_test_input_data8.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs8.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 14, {}, 172, 172, 0, 2, 0, 0, {0}}, 0, 13, 1, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data10.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs10.dat"}},
  {{{{0, 3}, 51, 1, cyclic_prefix::NORMAL, 1, {}, 700, 700, 0, 0, 0, 0, {0}}, 11, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_data12.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs12.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 9, {}, 384, 384, 0, 1, 0, 0, {0}}, 9, 5, 5, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data14.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs14.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 14, {}, 651, 651, 0, 2, 0, 0, {0}}, 9, 5, 5, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data16.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs16.dat"}},
  {{{{0, 4}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 119, 119, 0, 0, 0, 0, {0}}, 11, 4, 10, 1}, {}, {"test_data/pucch_processor_format1_test_input_data18.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs18.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 25, {}, 225, 225, 0, 1, 0, 0, {0}}, 3, 4, 10, 1}, {1}, {"test_data/pucch_processor_format1_test_input_data20.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs20.dat"}},
  {{{{0, 5}, 51, 1, cyclic_prefix::NORMAL, 7, {}, 966, 966, 0, 2, 0, 0, {0}}, 1, 4, 10, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data22.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs22.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 17, {12}, 820, 820, 0, 0, 0, 0, {0}}, 11, 14, 0, 0}, {}, {"test_data/pucch_processor_format1_test_input_data24.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs24.dat"}},
  {{{{0, 4}, 51, 1, cyclic_prefix::NORMAL, 42, {28}, 620, 620, 0, 1, 0, 0, {0}}, 4, 14, 0, 1}, {1}, {"test_data/pucch_processor_format1_test_input_data26.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs26.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 19, {3}, 288, 288, 0, 2, 0, 0, {0}}, 9, 14, 0, 1}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data28.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs28.dat"}},
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 29, {0}, 785, 785, 0, 0, 0, 0, {0}}, 1, 13, 1, 1}, {}, {"test_data/pucch_processor_format1_test_input_data30.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs30.dat"}},
  {{{{0, 1}, 51, 1, cyclic_prefix::NORMAL, 15, {8}, 339, 339, 0, 1, 0, 0, {0}}, 9, 13, 1, 1}, {1}, {"test_data/pucch_processor_format1_test_input_data32.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs32.dat"}},
  {{{{0, 6}, 51, 1, cyclic_prefix::NORMAL, 38, {4}, 265, 265, 0, 2, 0, 0, {0}}, 8, 13, 1, 1}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data34.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs34.dat"}},
  {{{{0, 8}, 51, 1, cyclic_prefix::NORMAL, 50, {3}, 153, 153, 0, 0, 0, 0, {0}}, 6, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_data36.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs36.dat"}},
  {{{{0, 1}, 51, 1, cyclic_prefix::NORMAL, 0, {39}, 446, 446, 0, 1, 0, 0, {0}}, 11, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data38.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs38.dat"}},
  {{{{0, 0}, 51, 1, cyclic_prefix::NORMAL, 13, {40}, 875, 875, 0, 2, 0, 0, {0}}, 4, 5, 5, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data40.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs40.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 6, {44}, 183, 183, 0, 0, 0, 0, {0}}, 1, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_data42.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs42.dat"}},
  {{{{0, 5}, 51, 1, cyclic_prefix::NORMAL, 43, {31}, 584, 584, 0, 1, 0, 0, {0}}, 1, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_data44.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs44.dat"}},
  {{{{0, 4}, 51, 1, cyclic_prefix::NORMAL, 12, {6}, 517, 517, 0, 2, 0, 0, {0}}, 0, 4, 10, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_data46.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs46.dat"}},
  {{{{1, 0}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 420, 420, 0, 0, 0, 0, {0}}, 10, 14, 0, 3}, {}, {"test_data/pucch_processor_format1_test_input_data48.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs48.dat"}},
  {{{{1, 6}, 51, 1, cyclic_prefix::NORMAL, 18, {}, 493, 493, 0, 1, 0, 0, {0}}, 10, 14, 0, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data50.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs50.dat"}},
  {{{{1, 4}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 392, 392, 0, 2, 0, 0, {0}}, 4, 14, 0, 0}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data52.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs52.dat"}},
  {{{{1, 1}, 51, 1, cyclic_prefix::NORMAL, 18, {}, 579, 579, 0, 0, 0, 0, {0}}, 2, 13, 1, 4}, {}, {"test_data/pucch_processor_format1_test_input_data54.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs54.dat"}},
  {{{{1, 0}, 51, 1, cyclic_prefix::NORMAL, 33, {}, 15, 15, 0, 1, 0, 0, {0}}, 2, 13, 1, 4}, {1}, {"test_data/pucch_processor_format1_test_input_data56.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs56.dat"}},
  {{{{1, 10}, 51, 1, cyclic_prefix::NORMAL, 37, {}, 454, 454, 0, 2, 0, 0, {0}}, 3, 13, 1, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data58.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs58.dat"}},
  {{{{1, 12}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 371, 371, 0, 0, 0, 0, {0}}, 9, 5, 5, 1}, {}, {"test_data/pucch_processor_format1_test_input_data60.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs60.dat"}},
  {{{{1, 9}, 51, 1, cyclic_prefix::NORMAL, 22, {}, 781, 781, 0, 1, 0, 0, {0}}, 5, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data62.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs62.dat"}},
  {{{{1, 16}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 514, 514, 0, 2, 0, 0, {0}}, 9, 5, 5, 0}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data64.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs64.dat"}},
  {{{{1, 18}, 51, 1, cyclic_prefix::NORMAL, 28, {}, 353, 353, 0, 0, 0, 0, {0}}, 10, 4, 10, 1}, {}, {"test_data/pucch_processor_format1_test_input_data66.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs66.dat"}},
  {{{{1, 4}, 51, 1, cyclic_prefix::NORMAL, 24, {}, 591, 591, 0, 1, 0, 0, {0}}, 3, 4, 10, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data68.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs68.dat"}},
  {{{{1, 4}, 51, 1, cyclic_prefix::NORMAL, 11, {}, 196, 196, 0, 2, 0, 0, {0}}, 2, 4, 10, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data70.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs70.dat"}},
  {{{{1, 3}, 51, 1, cyclic_prefix::NORMAL, 49, {5}, 433, 433, 0, 0, 0, 0, {0}}, 10, 14, 0, 1}, {}, {"test_data/pucch_processor_format1_test_input_data72.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs72.dat"}},
  {{{{1, 8}, 51, 1, cyclic_prefix::NORMAL, 13, {36}, 260, 260, 0, 1, 0, 0, {0}}, 7, 14, 0, 1}, {0}, {"test_data/pucch_processor_format1_test_input_data74.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs74.dat"}},
  {{{{1, 5}, 51, 1, cyclic_prefix::NORMAL, 21, {4}, 118, 118, 0, 2, 0, 0, {0}}, 3, 14, 0, 1}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data76.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs76.dat"}},
  {{{{1, 18}, 51, 1, cyclic_prefix::NORMAL, 24, {12}, 29, 29, 0, 0, 0, 0, {0}}, 8, 13, 1, 1}, {}, {"test_data/pucch_processor_format1_test_input_data78.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs78.dat"}},
  {{{{1, 19}, 51, 1, cyclic_prefix::NORMAL, 26, {24}, 462, 462, 0, 1, 0, 0, {0}}, 6, 13, 1, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data80.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs80.dat"}},
  {{{{1, 7}, 51, 1, cyclic_prefix::NORMAL, 50, {45}, 684, 684, 0, 2, 0, 0, {0}}, 4, 13, 1, 0}, {1, 1}, {"test_data/pucch_processor_format1_test_input_data82.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs82.dat"}},
  {{{{1, 5}, 51, 1, cyclic_prefix::NORMAL, 34, {6}, 99, 99, 0, 0, 0, 0, {0}}, 4, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_data84.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs84.dat"}},
  {{{{1, 2}, 51, 1, cyclic_prefix::NORMAL, 25, {39}, 726, 726, 0, 1, 0, 0, {0}}, 7, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data86.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs86.dat"}},
  {{{{1, 17}, 51, 1, cyclic_prefix::NORMAL, 35, {10}, 910, 910, 0, 2, 0, 0, {0}}, 4, 5, 5, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_data88.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs88.dat"}},
  {{{{1, 9}, 51, 1, cyclic_prefix::NORMAL, 31, {31}, 504, 504, 0, 0, 0, 0, {0}}, 10, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_data90.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs90.dat"}},
  {{{{1, 16}, 51, 1, cyclic_prefix::NORMAL, 9, {12}, 866, 866, 0, 1, 0, 0, {0}}, 6, 4, 10, 0}, {1}, {"test_data/pucch_processor_format1_test_input_data92.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs92.dat"}},
  {{{{1, 9}, 51, 1, cyclic_prefix::NORMAL, 49, {36}, 28, 28, 0, 2, 0, 0, {0}}, 2, 4, 10, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_data94.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs94.dat"}},
    // clang-format on
};

} // namespace srsgnb
