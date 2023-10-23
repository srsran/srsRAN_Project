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

// This file was generated using the following MATLAB class on 02-10-2023 (seed 0):
//   + "srsPUCCHProcessorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  pucch_processor::format1_configuration                  config;
  std::vector<uint8_t>                                    ack_bits;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
};

static const std::vector<test_case_t> pucch_processor_format1_test_data = {
    // clang-format off
  {{nullopt, {0, 1309}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 821, 0, {0,1,2,3,}, 10, 14, 0, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols0.dat"}},
  {{nullopt, {0, 1961}, 51, 1, cyclic_prefix::NORMAL, 29, {}, 646, 1, {0,1,2,3,}, 6, 14, 0, 4}, {0}, {"test_data/pucch_processor_format1_test_input_symbols1.dat"}},
  {{nullopt, {0, 7404}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 803, 2, {0,1,2,3,}, 11, 14, 0, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols2.dat"}},
  {{nullopt, {0, 7367}, 51, 1, cyclic_prefix::NORMAL, 45, {}, 270, 0, {0,1,2,3,}, 8, 13, 1, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols3.dat"}},
  {{nullopt, {0, 4829}, 51, 1, cyclic_prefix::NORMAL, 25, {}, 530, 1, {0,1,2,3,}, 7, 13, 1, 4}, {0}, {"test_data/pucch_processor_format1_test_input_symbols4.dat"}},
  {{nullopt, {0, 4997}, 51, 1, cyclic_prefix::NORMAL, 13, {}, 436, 2, {0,1,2,3,}, 10, 13, 1, 1}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols5.dat"}},
  {{nullopt, {0, 1932}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 44, 0, {0,1,2,3,}, 11, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols6.dat"}},
  {{nullopt, {0, 3489}, 51, 1, cyclic_prefix::NORMAL, 39, {}, 685, 1, {0,1,2,3,}, 3, 5, 5, 1}, {0}, {"test_data/pucch_processor_format1_test_input_symbols7.dat"}},
  {{nullopt, {0, 4353}, 51, 1, cyclic_prefix::NORMAL, 2, {}, 570, 2, {0,1,2,3,}, 7, 5, 5, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_symbols8.dat"}},
  {{nullopt, {0, 6512}, 51, 1, cyclic_prefix::NORMAL, 43, {}, 223, 0, {0,1,2,3,}, 2, 4, 10, 1}, {}, {"test_data/pucch_processor_format1_test_input_symbols9.dat"}},
  {{nullopt, {0, 1970}, 51, 1, cyclic_prefix::NORMAL, 12, {}, 150, 1, {0,1,2,3,}, 3, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_symbols10.dat"}},
  {{nullopt, {0, 2814}, 51, 1, cyclic_prefix::NORMAL, 8, {}, 640, 2, {0,1,2,3,}, 7, 4, 10, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_symbols11.dat"}},
  {{nullopt, {0, 525}, 51, 1, cyclic_prefix::NORMAL, 47, {19}, 605, 0, {0,1,2,3,}, 7, 14, 0, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols12.dat"}},
  {{nullopt, {0, 6431}, 51, 1, cyclic_prefix::NORMAL, 32, {12}, 260, 1, {0,1,2,3,}, 2, 14, 0, 2}, {0}, {"test_data/pucch_processor_format1_test_input_symbols13.dat"}},
  {{nullopt, {0, 522}, 51, 1, cyclic_prefix::NORMAL, 16, {0}, 877, 2, {0,1,2,3,}, 3, 14, 0, 2}, {1, 1}, {"test_data/pucch_processor_format1_test_input_symbols14.dat"}},
  {{nullopt, {0, 1381}, 51, 1, cyclic_prefix::NORMAL, 12, {26}, 251, 0, {0,1,2,3,}, 4, 13, 1, 2}, {}, {"test_data/pucch_processor_format1_test_input_symbols15.dat"}},
  {{nullopt, {0, 10059}, 51, 1, cyclic_prefix::NORMAL, 2, {22}, 976, 1, {0,1,2,3,}, 8, 13, 1, 1}, {0}, {"test_data/pucch_processor_format1_test_input_symbols16.dat"}},
  {{nullopt, {0, 3028}, 51, 1, cyclic_prefix::NORMAL, 12, {4}, 411, 2, {0,1,2,3,}, 3, 13, 1, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols17.dat"}},
  {{nullopt, {0, 2832}, 51, 1, cyclic_prefix::NORMAL, 17, {10}, 598, 0, {0,1,2,3,}, 1, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols18.dat"}},
  {{nullopt, {0, 4025}, 51, 1, cyclic_prefix::NORMAL, 43, {7}, 575, 1, {0,1,2,3,}, 5, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols19.dat"}},
  {{nullopt, {0, 8202}, 51, 1, cyclic_prefix::NORMAL, 13, {31}, 94, 2, {0,1,2,3,}, 7, 5, 5, 0}, {1, 1}, {"test_data/pucch_processor_format1_test_input_symbols20.dat"}},
  {{nullopt, {0, 4061}, 51, 1, cyclic_prefix::NORMAL, 10, {12}, 510, 0, {0,1,2,3,}, 9, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols21.dat"}},
  {{nullopt, {0, 138}, 51, 1, cyclic_prefix::NORMAL, 9, {22}, 80, 1, {0,1,2,3,}, 4, 4, 10, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols22.dat"}},
  {{nullopt, {0, 9809}, 51, 1, cyclic_prefix::NORMAL, 41, {46}, 516, 2, {0,1,2,3,}, 4, 4, 10, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_symbols23.dat"}},
  {{nullopt, {1, 15065}, 51, 1, cyclic_prefix::NORMAL, 20, {}, 512, 0, {0,1,2,3,}, 9, 14, 0, 1}, {}, {"test_data/pucch_processor_format1_test_input_symbols24.dat"}},
  {{nullopt, {1, 3746}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 81, 1, {0,1,2,3,}, 0, 14, 0, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols25.dat"}},
  {{nullopt, {1, 4988}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 147, 2, {0,1,2,3,}, 9, 14, 0, 6}, {1, 1}, {"test_data/pucch_processor_format1_test_input_symbols26.dat"}},
  {{nullopt, {1, 5453}, 51, 1, cyclic_prefix::NORMAL, 43, {}, 99, 0, {0,1,2,3,}, 9, 13, 1, 5}, {}, {"test_data/pucch_processor_format1_test_input_symbols27.dat"}},
  {{nullopt, {1, 18883}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 853, 1, {0,1,2,3,}, 4, 13, 1, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols28.dat"}},
  {{nullopt, {1, 3571}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 748, 2, {0,1,2,3,}, 6, 13, 1, 1}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols29.dat"}},
  {{nullopt, {1, 11264}, 51, 1, cyclic_prefix::NORMAL, 37, {}, 85, 0, {0,1,2,3,}, 1, 5, 5, 1}, {}, {"test_data/pucch_processor_format1_test_input_symbols30.dat"}},
  {{nullopt, {1, 6768}, 51, 1, cyclic_prefix::NORMAL, 22, {}, 380, 1, {0,1,2,3,}, 8, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols31.dat"}},
  {{nullopt, {1, 20064}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 353, 2, {0,1,2,3,}, 11, 5, 5, 1}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols32.dat"}},
  {{nullopt, {1, 19546}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 337, 0, {0,1,2,3,}, 9, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols33.dat"}},
  {{nullopt, {1, 8563}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 506, 1, {0,1,2,3,}, 2, 4, 10, 0}, {0}, {"test_data/pucch_processor_format1_test_input_symbols34.dat"}},
  {{nullopt, {1, 3719}, 51, 1, cyclic_prefix::NORMAL, 44, {}, 1004, 2, {0,1,2,3,}, 1, 4, 10, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_symbols35.dat"}},
  {{nullopt, {1, 4469}, 51, 1, cyclic_prefix::NORMAL, 50, {14}, 56, 0, {0,1,2,3,}, 11, 14, 0, 1}, {}, {"test_data/pucch_processor_format1_test_input_symbols36.dat"}},
  {{nullopt, {1, 759}, 51, 1, cyclic_prefix::NORMAL, 39, {46}, 55, 1, {0,1,2,3,}, 9, 14, 0, 1}, {0}, {"test_data/pucch_processor_format1_test_input_symbols37.dat"}},
  {{nullopt, {1, 17925}, 51, 1, cyclic_prefix::NORMAL, 47, {28}, 482, 2, {0,1,2,3,}, 7, 14, 0, 1}, {1, 0}, {"test_data/pucch_processor_format1_test_input_symbols38.dat"}},
  {{nullopt, {1, 5020}, 51, 1, cyclic_prefix::NORMAL, 27, {3}, 107, 0, {0,1,2,3,}, 1, 13, 1, 1}, {}, {"test_data/pucch_processor_format1_test_input_symbols39.dat"}},
  {{nullopt, {1, 14020}, 51, 1, cyclic_prefix::NORMAL, 8, {31}, 278, 1, {0,1,2,3,}, 2, 13, 1, 2}, {0}, {"test_data/pucch_processor_format1_test_input_symbols40.dat"}},
  {{nullopt, {1, 9687}, 51, 1, cyclic_prefix::NORMAL, 19, {26}, 927, 2, {0,1,2,3,}, 9, 13, 1, 0}, {0, 1}, {"test_data/pucch_processor_format1_test_input_symbols41.dat"}},
  {{nullopt, {1, 7167}, 51, 1, cyclic_prefix::NORMAL, 13, {50}, 311, 0, {0,1,2,3,}, 3, 5, 5, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols42.dat"}},
  {{nullopt, {1, 15566}, 51, 1, cyclic_prefix::NORMAL, 43, {31}, 0, 1, {0,1,2,3,}, 7, 5, 5, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols43.dat"}},
  {{nullopt, {1, 8442}, 51, 1, cyclic_prefix::NORMAL, 17, {18}, 483, 2, {0,1,2,3,}, 4, 5, 5, 0}, {0, 0}, {"test_data/pucch_processor_format1_test_input_symbols44.dat"}},
  {{nullopt, {1, 8077}, 51, 1, cyclic_prefix::NORMAL, 9, {34}, 814, 0, {0,1,2,3,}, 9, 4, 10, 0}, {}, {"test_data/pucch_processor_format1_test_input_symbols45.dat"}},
  {{nullopt, {1, 6534}, 51, 1, cyclic_prefix::NORMAL, 45, {39}, 131, 1, {0,1,2,3,}, 6, 4, 10, 0}, {1}, {"test_data/pucch_processor_format1_test_input_symbols46.dat"}},
  {{nullopt, {1, 19409}, 51, 1, cyclic_prefix::NORMAL, 7, {0}, 225, 2, {0,1,2,3,}, 2, 4, 10, 0}, {1, 0}, {"test_data/pucch_processor_format1_test_input_symbols47.dat"}},
    // clang-format on
};

} // namespace srsran
