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

// This file was generated using the following MATLAB class on 23-Sep-2022:
//   + "srsPUCCHProcessorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pucch_processor.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  pucch_processor::format1_configuration                  config;
  file_vector<resource_grid_reader_spy::expected_entry_t> data_symbols;
  file_vector<resource_grid_reader_spy::expected_entry_t> dmrs_symbols;
};

static const std::vector<test_case_t> pucch_processor_format1_test_data = {
    // clang-format off
  {{{{0, 8}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 450, 450, 0, 1, 0, 0, {7}}, 1, 14, 0, 4}, {"test_data/pucch_processor_format1_test_input_data0.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs0.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 189, 189, 0, 2, 0, 0, {4}}, 11, 14, 0, 1}, {"test_data/pucch_processor_format1_test_input_data2.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs2.dat"}},
  {{{{0, 4}, 51, 1, cyclic_prefix::NORMAL, 21, {}, 583, 583, 0, 1, 0, 0, {6}}, 1, 13, 1, 5}, {"test_data/pucch_processor_format1_test_input_data4.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs4.dat"}},
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 43, {}, 314, 314, 0, 2, 0, 0, {5}}, 0, 13, 1, 5}, {"test_data/pucch_processor_format1_test_input_data6.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs6.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 8, {}, 513, 513, 0, 1, 0, 0, {3}}, 7, 5, 5, 1}, {"test_data/pucch_processor_format1_test_input_data8.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs8.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 951, 951, 0, 2, 0, 0, {0}}, 1, 5, 5, 1}, {"test_data/pucch_processor_format1_test_input_data10.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs10.dat"}},
  {{{{0, 0}, 51, 1, cyclic_prefix::NORMAL, 39, {}, 676, 676, 0, 1, 0, 0, {3}}, 4, 4, 10, 1}, {"test_data/pucch_processor_format1_test_input_data12.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs12.dat"}},
  {{{{0, 4}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 392, 392, 0, 2, 0, 0, {3}}, 7, 4, 10, 1}, {"test_data/pucch_processor_format1_test_input_data14.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs14.dat"}},
  {{{{0, 6}, 51, 1, cyclic_prefix::NORMAL, 25, {17}, 46, 46, 0, 1, 0, 0, {1}}, 1, 14, 0, 2}, {"test_data/pucch_processor_format1_test_input_data16.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs16.dat"}},
  {{{{0, 2}, 51, 1, cyclic_prefix::NORMAL, 25, {45}, 861, 861, 0, 2, 0, 0, {6}}, 3, 14, 0, 2}, {"test_data/pucch_processor_format1_test_input_data18.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs18.dat"}},
  {{{{0, 1}, 51, 1, cyclic_prefix::NORMAL, 42, {41}, 944, 944, 0, 1, 0, 0, {1}}, 3, 13, 1, 0}, {"test_data/pucch_processor_format1_test_input_data20.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs20.dat"}},
  {{{{0, 9}, 51, 1, cyclic_prefix::NORMAL, 12, {24}, 923, 923, 0, 2, 0, 0, {2}}, 2, 13, 1, 1}, {"test_data/pucch_processor_format1_test_input_data22.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs22.dat"}},
  {{{{0, 5}, 51, 1, cyclic_prefix::NORMAL, 14, {38}, 784, 784, 0, 1, 0, 0, {4}}, 11, 5, 5, 0}, {"test_data/pucch_processor_format1_test_input_data24.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs24.dat"}},
  {{{{0, 3}, 51, 1, cyclic_prefix::NORMAL, 2, {27}, 936, 936, 0, 2, 0, 0, {4}}, 0, 5, 5, 0}, {"test_data/pucch_processor_format1_test_input_data26.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs26.dat"}},
  {{{{0, 1}, 51, 1, cyclic_prefix::NORMAL, 0, {17}, 435, 435, 0, 1, 0, 0, {4}}, 5, 4, 10, 0}, {"test_data/pucch_processor_format1_test_input_data28.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs28.dat"}},
  {{{{0, 7}, 51, 1, cyclic_prefix::NORMAL, 8, {30}, 806, 806, 0, 2, 0, 0, {2}}, 6, 4, 10, 0}, {"test_data/pucch_processor_format1_test_input_data30.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs30.dat"}},
  {{{{1, 13}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 358, 358, 0, 1, 0, 0, {5}}, 5, 14, 0, 1}, {"test_data/pucch_processor_format1_test_input_data32.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs32.dat"}},
  {{{{1, 3}, 51, 1, cyclic_prefix::NORMAL, 50, {}, 148, 148, 0, 2, 0, 0, {6}}, 6, 14, 0, 0}, {"test_data/pucch_processor_format1_test_input_data34.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs34.dat"}},
  {{{{1, 19}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 613, 613, 0, 1, 0, 0, {0}}, 9, 13, 1, 5}, {"test_data/pucch_processor_format1_test_input_data36.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs36.dat"}},
  {{{{1, 7}, 51, 1, cyclic_prefix::NORMAL, 22, {}, 801, 801, 0, 2, 0, 0, {2}}, 9, 13, 1, 5}, {"test_data/pucch_processor_format1_test_input_data38.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs38.dat"}},
  {{{{1, 2}, 51, 1, cyclic_prefix::NORMAL, 29, {}, 384, 384, 0, 1, 0, 0, {1}}, 10, 5, 5, 1}, {"test_data/pucch_processor_format1_test_input_data40.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs40.dat"}},
  {{{{1, 17}, 51, 1, cyclic_prefix::NORMAL, 26, {}, 743, 743, 0, 2, 0, 0, {4}}, 4, 5, 5, 0}, {"test_data/pucch_processor_format1_test_input_data42.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs42.dat"}},
  {{{{1, 2}, 51, 1, cyclic_prefix::NORMAL, 21, {}, 117, 117, 0, 1, 0, 0, {1}}, 2, 4, 10, 0}, {"test_data/pucch_processor_format1_test_input_data44.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs44.dat"}},
  {{{{1, 18}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 978, 978, 0, 2, 0, 0, {3}}, 5, 4, 10, 1}, {"test_data/pucch_processor_format1_test_input_data46.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs46.dat"}},
  {{{{1, 15}, 51, 1, cyclic_prefix::NORMAL, 20, {6}, 58, 58, 0, 1, 0, 0, {3}}, 2, 14, 0, 0}, {"test_data/pucch_processor_format1_test_input_data48.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs48.dat"}},
  {{{{1, 19}, 51, 1, cyclic_prefix::NORMAL, 11, {41}, 775, 775, 0, 2, 0, 0, {4}}, 0, 14, 0, 1}, {"test_data/pucch_processor_format1_test_input_data50.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs50.dat"}},
  {{{{1, 3}, 51, 1, cyclic_prefix::NORMAL, 33, {27}, 864, 864, 0, 1, 0, 0, {5}}, 8, 13, 1, 1}, {"test_data/pucch_processor_format1_test_input_data52.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs52.dat"}},
  {{{{1, 14}, 51, 1, cyclic_prefix::NORMAL, 9, {31}, 97, 97, 0, 2, 0, 0, {1}}, 8, 13, 1, 1}, {"test_data/pucch_processor_format1_test_input_data54.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs54.dat"}},
  {{{{1, 18}, 51, 1, cyclic_prefix::NORMAL, 22, {22}, 903, 903, 0, 1, 0, 0, {6}}, 5, 5, 5, 0}, {"test_data/pucch_processor_format1_test_input_data56.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs56.dat"}},
  {{{{1, 10}, 51, 1, cyclic_prefix::NORMAL, 40, {32}, 855, 855, 0, 2, 0, 0, {4}}, 9, 5, 5, 0}, {"test_data/pucch_processor_format1_test_input_data58.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs58.dat"}},
  {{{{1, 10}, 51, 1, cyclic_prefix::NORMAL, 44, {28}, 821, 821, 0, 1, 0, 0, {2}}, 11, 4, 10, 0}, {"test_data/pucch_processor_format1_test_input_data60.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs60.dat"}},
  {{{{1, 11}, 51, 1, cyclic_prefix::NORMAL, 24, {11}, 259, 259, 0, 2, 0, 0, {1}}, 3, 4, 10, 0}, {"test_data/pucch_processor_format1_test_input_data62.dat"}, {"test_data/pucch_processor_format1_test_input_dmrs62.dat"}},
    // clang-format on
};

} // namespace srsgnb
