/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

// This file was generated using the following MATLAB class on 24-07-2024 (seed 0):
//   + "srsPUCCHProcessorFormat0Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/support/file_vector.h"
#include <optional>

namespace srsran {

struct pucch_entry {
  pucch_processor::format0_configuration config;
  std::vector<uint8_t>                   ack_bits;
  std::optional<uint8_t>                 sr;
};
struct test_case_t {
  pucch_entry                                             entry;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
};

static const std::vector<test_case_t> pucch_processor_format0_test_data = {
    // clang-format off
  {{{std::nullopt, {0, 999}, cyclic_prefix::NORMAL, 51, 1, 6, {}, 12, 1, 7, 821, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols0.dat"}},
  {{{std::nullopt, {0, 4307}, cyclic_prefix::NORMAL, 51, 1, 32, {}, 13, 1, 3, 256, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols1.dat"}},
  {{{std::nullopt, {0, 4717}, cyclic_prefix::NORMAL, 51, 1, 35, {}, 10, 1, 3, 5, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols2.dat"}},
  {{{std::nullopt, {0, 7750}, cyclic_prefix::NORMAL, 51, 1, 16, {}, 3, 1, 8, 214, 1, true, {0,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols3.dat"}},
  {{{std::nullopt, {0, 1757}, cyclic_prefix::NORMAL, 51, 1, 44, {}, 7, 1, 0, 993, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols4.dat"}},
  {{{std::nullopt, {0, 4302}, cyclic_prefix::NORMAL, 51, 1, 22, {}, 12, 1, 1, 596, 1, true, {0,1,2,3,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols5.dat"}},
  {{{std::nullopt, {0, 4847}, cyclic_prefix::NORMAL, 51, 1, 0, {}, 11, 1, 8, 379, 2, true, {0,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols6.dat"}},
  {{{std::nullopt, {0, 6635}, cyclic_prefix::NORMAL, 51, 1, 16, {}, 8, 1, 7, 627, 2, true, {0,1,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols7.dat"}},
  {{{std::nullopt, {0, 4521}, cyclic_prefix::NORMAL, 51, 1, 27, {}, 1, 1, 5, 836, 2, true, {0,1,2,3,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols8.dat"}},
  {{{std::nullopt, {0, 9573}, cyclic_prefix::NORMAL, 51, 1, 11, {}, 3, 1, 1, 727, 1, false, {0,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols9.dat"}},
  {{{std::nullopt, {0, 3370}, cyclic_prefix::NORMAL, 51, 1, 25, {}, 12, 1, 0, 425, 1, false, {0,1,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols10.dat"}},
  {{{std::nullopt, {0, 154}, cyclic_prefix::NORMAL, 51, 1, 7, {}, 4, 1, 3, 272, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols11.dat"}},
  {{{std::nullopt, {0, 7636}, cyclic_prefix::NORMAL, 51, 1, 45, {}, 0, 1, 0, 152, 2, false, {0,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols12.dat"}},
  {{{std::nullopt, {0, 2333}, cyclic_prefix::NORMAL, 51, 1, 18, {}, 10, 1, 9, 816, 2, false, {0,1,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols13.dat"}},
  {{{std::nullopt, {0, 7914}, cyclic_prefix::NORMAL, 51, 1, 8, {}, 8, 1, 6, 646, 2, false, {0,1,2,3,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols14.dat"}},
  {{{std::nullopt, {0, 9634}, cyclic_prefix::NORMAL, 51, 1, 32, {}, 6, 2, 9, 631, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols15.dat"}},
  {{{std::nullopt, {0, 1489}, cyclic_prefix::NORMAL, 51, 1, 50, {}, 11, 2, 8, 527, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols16.dat"}},
  {{{std::nullopt, {0, 8182}, cyclic_prefix::NORMAL, 51, 1, 40, {}, 3, 2, 2, 1007, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols17.dat"}},
  {{{std::nullopt, {0, 4745}, cyclic_prefix::NORMAL, 51, 1, 1, {}, 0, 2, 9, 525, 1, true, {0,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols18.dat"}},
  {{{std::nullopt, {0, 3781}, cyclic_prefix::NORMAL, 51, 1, 2, {}, 8, 2, 4, 38, 1, true, {0,1,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols19.dat"}},
  {{{std::nullopt, {0, 2014}, cyclic_prefix::NORMAL, 51, 1, 47, {}, 8, 2, 10, 938, 1, true, {0,1,2,3,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols20.dat"}},
  {{{std::nullopt, {0, 2900}, cyclic_prefix::NORMAL, 51, 1, 12, {}, 2, 2, 9, 624, 2, true, {0,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols21.dat"}},
  {{{std::nullopt, {0, 5526}, cyclic_prefix::NORMAL, 51, 1, 27, {}, 7, 2, 10, 46, 2, true, {0,1,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols22.dat"}},
  {{{std::nullopt, {0, 5982}, cyclic_prefix::NORMAL, 51, 1, 28, {}, 3, 2, 4, 226, 2, true, {0,1,2,3,}}, {1, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols23.dat"}},
  {{{std::nullopt, {0, 967}, cyclic_prefix::NORMAL, 51, 1, 20, {}, 10, 2, 4, 746, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols24.dat"}},
  {{{std::nullopt, {0, 3223}, cyclic_prefix::NORMAL, 51, 1, 15, {}, 4, 2, 5, 887, 1, false, {0,1,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols25.dat"}},
  {{{std::nullopt, {0, 5384}, cyclic_prefix::NORMAL, 51, 1, 3, {}, 12, 2, 0, 63, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols26.dat"}},
  {{{std::nullopt, {0, 3801}, cyclic_prefix::NORMAL, 51, 1, 50, {}, 10, 2, 9, 84, 2, false, {0,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols27.dat"}},
  {{{std::nullopt, {0, 1080}, cyclic_prefix::NORMAL, 51, 1, 43, {}, 0, 2, 7, 447, 2, false, {0,1,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols28.dat"}},
  {{{std::nullopt, {0, 1704}, cyclic_prefix::NORMAL, 51, 1, 37, {}, 3, 2, 7, 691, 2, false, {0,1,2,3,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols29.dat"}},
  {{{std::nullopt, {0, 2838}, cyclic_prefix::NORMAL, 51, 1, 38, {41}, 7, 2, 9, 195, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols30.dat"}},
  {{{std::nullopt, {0, 4643}, cyclic_prefix::NORMAL, 51, 1, 20, {28}, 5, 2, 3, 984, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols31.dat"}},
  {{{std::nullopt, {0, 7840}, cyclic_prefix::NORMAL, 51, 1, 0, {47}, 10, 2, 1, 719, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols32.dat"}},
  {{{std::nullopt, {0, 379}, cyclic_prefix::NORMAL, 51, 1, 3, {50}, 9, 2, 2, 394, 1, true, {0,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols33.dat"}},
  {{{std::nullopt, {0, 9807}, cyclic_prefix::NORMAL, 51, 1, 22, {13}, 3, 2, 5, 811, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols34.dat"}},
  {{{std::nullopt, {0, 8793}, cyclic_prefix::NORMAL, 51, 1, 15, {35}, 8, 2, 4, 638, 1, true, {0,1,2,3,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols35.dat"}},
  {{{std::nullopt, {0, 2375}, cyclic_prefix::NORMAL, 51, 1, 49, {25}, 8, 2, 4, 390, 2, true, {0,}}, {0, 1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols36.dat"}},
  {{{std::nullopt, {0, 3654}, cyclic_prefix::NORMAL, 51, 1, 18, {37}, 9, 2, 11, 200, 2, true, {0,1,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols37.dat"}},
  {{{std::nullopt, {0, 2807}, cyclic_prefix::NORMAL, 51, 1, 44, {43}, 11, 2, 3, 379, 2, true, {0,1,2,3,}}, {0, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols38.dat"}},
  {{{std::nullopt, {0, 1443}, cyclic_prefix::NORMAL, 51, 1, 45, {21}, 0, 2, 10, 105, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols39.dat"}},
  {{{std::nullopt, {0, 9501}, cyclic_prefix::NORMAL, 51, 1, 11, {9}, 1, 2, 10, 188, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols40.dat"}},
  {{{std::nullopt, {0, 2234}, cyclic_prefix::NORMAL, 51, 1, 31, {18}, 9, 2, 1, 483, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols41.dat"}},
  {{{std::nullopt, {0, 6443}, cyclic_prefix::NORMAL, 51, 1, 35, {8}, 10, 2, 8, 768, 2, false, {0,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols42.dat"}},
  {{{std::nullopt, {0, 6690}, cyclic_prefix::NORMAL, 51, 1, 49, {14}, 10, 2, 10, 747, 2, false, {0,1,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols43.dat"}},
  {{{std::nullopt, {0, 6257}, cyclic_prefix::NORMAL, 51, 1, 28, {36}, 7, 2, 5, 385, 2, false, {0,1,2,3,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols44.dat"}},
  {{{std::nullopt, {1, 17146}, cyclic_prefix::NORMAL, 51, 1, 49, {}, 4, 1, 0, 798, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols45.dat"}},
  {{{std::nullopt, {1, 18241}, cyclic_prefix::NORMAL, 51, 1, 2, {}, 12, 1, 10, 709, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols46.dat"}},
  {{{std::nullopt, {1, 861}, cyclic_prefix::NORMAL, 51, 1, 8, {}, 9, 1, 2, 847, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols47.dat"}},
  {{{std::nullopt, {1, 12746}, cyclic_prefix::NORMAL, 51, 1, 49, {}, 11, 1, 8, 622, 1, true, {0,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols48.dat"}},
  {{{std::nullopt, {1, 15747}, cyclic_prefix::NORMAL, 51, 1, 2, {}, 2, 1, 0, 641, 1, true, {0,1,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols49.dat"}},
  {{{std::nullopt, {1, 3093}, cyclic_prefix::NORMAL, 51, 1, 4, {}, 4, 1, 1, 209, 1, true, {0,1,2,3,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols50.dat"}},
  {{{std::nullopt, {1, 10210}, cyclic_prefix::NORMAL, 51, 1, 41, {}, 0, 1, 7, 91, 2, true, {0,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols51.dat"}},
  {{{std::nullopt, {1, 19008}, cyclic_prefix::NORMAL, 51, 1, 49, {}, 9, 1, 6, 977, 2, true, {0,1,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols52.dat"}},
  {{{std::nullopt, {1, 10774}, cyclic_prefix::NORMAL, 51, 1, 44, {}, 9, 1, 0, 655, 2, true, {0,1,2,3,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols53.dat"}},
  {{{std::nullopt, {1, 2319}, cyclic_prefix::NORMAL, 51, 1, 42, {}, 8, 1, 4, 2, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols54.dat"}},
  {{{std::nullopt, {1, 5068}, cyclic_prefix::NORMAL, 51, 1, 40, {}, 12, 1, 7, 643, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols55.dat"}},
  {{{std::nullopt, {1, 2865}, cyclic_prefix::NORMAL, 51, 1, 3, {}, 1, 1, 4, 653, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols56.dat"}},
  {{{std::nullopt, {1, 10312}, cyclic_prefix::NORMAL, 51, 1, 16, {}, 9, 1, 6, 450, 2, false, {0,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols57.dat"}},
  {{{std::nullopt, {1, 16333}, cyclic_prefix::NORMAL, 51, 1, 49, {}, 1, 1, 10, 217, 2, false, {0,1,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols58.dat"}},
  {{{std::nullopt, {1, 7225}, cyclic_prefix::NORMAL, 51, 1, 16, {}, 10, 1, 10, 451, 2, false, {0,1,2,3,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols59.dat"}},
  {{{std::nullopt, {1, 4932}, cyclic_prefix::NORMAL, 51, 1, 17, {}, 2, 2, 1, 526, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols60.dat"}},
  {{{std::nullopt, {1, 6922}, cyclic_prefix::NORMAL, 51, 1, 31, {}, 5, 2, 5, 676, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols61.dat"}},
  {{{std::nullopt, {1, 6630}, cyclic_prefix::NORMAL, 51, 1, 31, {}, 3, 2, 11, 447, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols62.dat"}},
  {{{std::nullopt, {1, 18420}, cyclic_prefix::NORMAL, 51, 1, 40, {}, 6, 2, 7, 381, 1, true, {0,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols63.dat"}},
  {{{std::nullopt, {1, 9510}, cyclic_prefix::NORMAL, 51, 1, 27, {}, 9, 2, 7, 498, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols64.dat"}},
  {{{std::nullopt, {1, 19269}, cyclic_prefix::NORMAL, 51, 1, 28, {}, 3, 2, 0, 819, 1, true, {0,1,2,3,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols65.dat"}},
  {{{std::nullopt, {1, 11524}, cyclic_prefix::NORMAL, 51, 1, 23, {}, 10, 2, 9, 474, 2, true, {0,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols66.dat"}},
  {{{std::nullopt, {1, 3786}, cyclic_prefix::NORMAL, 51, 1, 32, {}, 6, 2, 3, 570, 2, true, {0,1,}}, {0, 1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols67.dat"}},
  {{{std::nullopt, {1, 8986}, cyclic_prefix::NORMAL, 51, 1, 26, {}, 1, 2, 3, 862, 2, true, {0,1,2,3,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols68.dat"}},
  {{{std::nullopt, {1, 15387}, cyclic_prefix::NORMAL, 51, 1, 46, {}, 8, 2, 2, 822, 1, false, {0,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols69.dat"}},
  {{{std::nullopt, {1, 17097}, cyclic_prefix::NORMAL, 51, 1, 5, {}, 4, 2, 5, 662, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols70.dat"}},
  {{{std::nullopt, {1, 4473}, cyclic_prefix::NORMAL, 51, 1, 46, {}, 7, 2, 9, 35, 1, false, {0,1,2,3,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols71.dat"}},
  {{{std::nullopt, {1, 14792}, cyclic_prefix::NORMAL, 51, 1, 22, {}, 6, 2, 0, 451, 2, false, {0,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols72.dat"}},
  {{{std::nullopt, {1, 16115}, cyclic_prefix::NORMAL, 51, 1, 4, {}, 12, 2, 6, 31, 2, false, {0,1,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols73.dat"}},
  {{{std::nullopt, {1, 4329}, cyclic_prefix::NORMAL, 51, 1, 44, {}, 0, 2, 9, 795, 2, false, {0,1,2,3,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols74.dat"}},
  {{{std::nullopt, {1, 12202}, cyclic_prefix::NORMAL, 51, 1, 28, {11}, 1, 2, 7, 344, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols75.dat"}},
  {{{std::nullopt, {1, 5694}, cyclic_prefix::NORMAL, 51, 1, 7, {18}, 9, 2, 10, 261, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols76.dat"}},
  {{{std::nullopt, {1, 19064}, cyclic_prefix::NORMAL, 51, 1, 20, {30}, 1, 2, 5, 456, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols77.dat"}},
  {{{std::nullopt, {1, 19137}, cyclic_prefix::NORMAL, 51, 1, 28, {49}, 0, 2, 11, 23, 1, true, {0,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols78.dat"}},
  {{{std::nullopt, {1, 18417}, cyclic_prefix::NORMAL, 51, 1, 11, {37}, 0, 2, 2, 603, 1, true, {0,1,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols79.dat"}},
  {{{std::nullopt, {1, 8608}, cyclic_prefix::NORMAL, 51, 1, 10, {32}, 1, 2, 11, 943, 1, true, {0,1,2,3,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols80.dat"}},
  {{{std::nullopt, {1, 136}, cyclic_prefix::NORMAL, 51, 1, 9, {10}, 4, 2, 4, 82, 2, true, {0,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols81.dat"}},
  {{{std::nullopt, {1, 11273}, cyclic_prefix::NORMAL, 51, 1, 32, {12}, 5, 2, 1, 806, 2, true, {0,1,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols82.dat"}},
  {{{std::nullopt, {1, 17130}, cyclic_prefix::NORMAL, 51, 1, 31, {23}, 1, 2, 2, 376, 2, true, {0,1,2,3,}}, {0, 1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols83.dat"}},
  {{{std::nullopt, {1, 3841}, cyclic_prefix::NORMAL, 51, 1, 36, {33}, 11, 2, 3, 359, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols84.dat"}},
  {{{std::nullopt, {1, 15557}, cyclic_prefix::NORMAL, 51, 1, 42, {23}, 1, 2, 5, 79, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols85.dat"}},
  {{{std::nullopt, {1, 19928}, cyclic_prefix::NORMAL, 51, 1, 0, {36}, 8, 2, 3, 98, 1, false, {0,1,2,3,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols86.dat"}},
  {{{std::nullopt, {1, 13669}, cyclic_prefix::NORMAL, 51, 1, 29, {16}, 9, 2, 10, 96, 2, false, {0,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols87.dat"}},
  {{{std::nullopt, {1, 14472}, cyclic_prefix::NORMAL, 51, 1, 19, {9}, 10, 2, 5, 237, 2, false, {0,1,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols88.dat"}},
  {{{std::nullopt, {1, 3181}, cyclic_prefix::NORMAL, 51, 1, 33, {32}, 4, 2, 1, 21, 2, false, {0,1,2,3,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols89.dat"}},
    // clang-format on
};

} // namespace srsran
