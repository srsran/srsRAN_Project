/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 23-10-2024 (seed 0):
//   + "srsPUCCHProcessorFormat0Unittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
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
  {{{std::nullopt, {0, 5139}, cyclic_prefix::NORMAL, 51, 1, 18, {}, 5, 1, 10, 304, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols0.dat"}},
  {{{std::nullopt, {0, 4902}, cyclic_prefix::NORMAL, 51, 1, 41, {}, 6, 1, 10, 204, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols1.dat"}},
  {{{std::nullopt, {0, 1159}, cyclic_prefix::NORMAL, 51, 1, 48, {}, 12, 1, 10, 686, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols2.dat"}},
  {{{std::nullopt, {0, 2341}, cyclic_prefix::NORMAL, 51, 1, 8, {}, 6, 1, 9, 695, 1, true, {0,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols3.dat"}},
  {{{std::nullopt, {0, 1557}, cyclic_prefix::NORMAL, 51, 1, 32, {}, 0, 1, 11, 98, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols4.dat"}},
  {{{std::nullopt, {0, 8490}, cyclic_prefix::NORMAL, 51, 1, 31, {}, 1, 1, 1, 56, 1, true, {0,1,2,3,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols5.dat"}},
  {{{std::nullopt, {0, 3036}, cyclic_prefix::NORMAL, 51, 1, 13, {}, 6, 1, 2, 947, 2, true, {0,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols6.dat"}},
  {{{std::nullopt, {0, 2745}, cyclic_prefix::NORMAL, 51, 1, 40, {}, 9, 1, 11, 238, 2, true, {0,1,}}, {1, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols7.dat"}},
  {{{std::nullopt, {0, 6373}, cyclic_prefix::NORMAL, 51, 1, 43, {}, 1, 1, 2, 506, 2, true, {0,1,2,3,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols8.dat"}},
  {{{std::nullopt, {0, 2719}, cyclic_prefix::NORMAL, 51, 1, 12, {}, 4, 1, 6, 751, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols9.dat"}},
  {{{std::nullopt, {0, 7703}, cyclic_prefix::NORMAL, 51, 1, 14, {}, 12, 1, 5, 666, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols10.dat"}},
  {{{std::nullopt, {0, 9227}, cyclic_prefix::NORMAL, 51, 1, 16, {}, 7, 1, 0, 455, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols11.dat"}},
  {{{std::nullopt, {0, 3466}, cyclic_prefix::NORMAL, 51, 1, 45, {}, 1, 1, 8, 249, 2, false, {0,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols12.dat"}},
  {{{std::nullopt, {0, 7023}, cyclic_prefix::NORMAL, 51, 1, 48, {}, 7, 1, 9, 731, 2, false, {0,1,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols13.dat"}},
  {{{std::nullopt, {0, 835}, cyclic_prefix::NORMAL, 51, 1, 37, {}, 9, 1, 5, 893, 2, false, {0,1,2,3,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols14.dat"}},
  {{{std::nullopt, {0, 163}, cyclic_prefix::NORMAL, 51, 1, 24, {}, 2, 2, 11, 258, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols15.dat"}},
  {{{std::nullopt, {0, 6915}, cyclic_prefix::NORMAL, 51, 1, 38, {}, 3, 2, 5, 867, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols16.dat"}},
  {{{std::nullopt, {0, 5642}, cyclic_prefix::NORMAL, 51, 1, 4, {}, 7, 2, 0, 468, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols17.dat"}},
  {{{std::nullopt, {0, 6540}, cyclic_prefix::NORMAL, 51, 1, 13, {}, 3, 2, 6, 537, 1, true, {0,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols18.dat"}},
  {{{std::nullopt, {0, 4382}, cyclic_prefix::NORMAL, 51, 1, 26, {}, 2, 2, 7, 348, 1, true, {0,1,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols19.dat"}},
  {{{std::nullopt, {0, 4194}, cyclic_prefix::NORMAL, 51, 1, 6, {}, 10, 2, 4, 457, 1, true, {0,1,2,3,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols20.dat"}},
  {{{std::nullopt, {0, 9848}, cyclic_prefix::NORMAL, 51, 1, 50, {}, 10, 2, 11, 380, 2, true, {0,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols21.dat"}},
  {{{std::nullopt, {0, 4931}, cyclic_prefix::NORMAL, 51, 1, 48, {}, 10, 2, 10, 618, 2, true, {0,1,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols22.dat"}},
  {{{std::nullopt, {0, 199}, cyclic_prefix::NORMAL, 51, 1, 34, {}, 1, 2, 3, 278, 2, true, {0,1,2,3,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols23.dat"}},
  {{{std::nullopt, {0, 8468}, cyclic_prefix::NORMAL, 51, 1, 37, {}, 0, 2, 6, 907, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols24.dat"}},
  {{{std::nullopt, {0, 74}, cyclic_prefix::NORMAL, 51, 1, 29, {}, 11, 2, 5, 699, 1, false, {0,1,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols25.dat"}},
  {{{std::nullopt, {0, 9740}, cyclic_prefix::NORMAL, 51, 1, 3, {}, 4, 2, 4, 780, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols26.dat"}},
  {{{std::nullopt, {0, 8601}, cyclic_prefix::NORMAL, 51, 1, 9, {}, 10, 2, 4, 304, 2, false, {0,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols27.dat"}},
  {{{std::nullopt, {0, 5658}, cyclic_prefix::NORMAL, 51, 1, 26, {}, 3, 2, 9, 451, 2, false, {0,1,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols28.dat"}},
  {{{std::nullopt, {0, 4953}, cyclic_prefix::NORMAL, 51, 1, 28, {}, 4, 2, 0, 613, 2, false, {0,1,2,3,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols29.dat"}},
  {{{std::nullopt, {0, 9573}, cyclic_prefix::NORMAL, 51, 1, 3, {47}, 4, 2, 1, 249, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols30.dat"}},
  {{{std::nullopt, {0, 6902}, cyclic_prefix::NORMAL, 51, 1, 23, {50}, 3, 2, 1, 456, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols31.dat"}},
  {{{std::nullopt, {0, 3431}, cyclic_prefix::NORMAL, 51, 1, 5, {6}, 7, 2, 4, 924, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols32.dat"}},
  {{{std::nullopt, {0, 8740}, cyclic_prefix::NORMAL, 51, 1, 41, {18}, 0, 2, 11, 849, 1, true, {0,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols33.dat"}},
  {{{std::nullopt, {0, 5744}, cyclic_prefix::NORMAL, 51, 1, 24, {30}, 11, 2, 5, 371, 1, true, {0,1,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols34.dat"}},
  {{{std::nullopt, {0, 160}, cyclic_prefix::NORMAL, 51, 1, 19, {49}, 8, 2, 11, 580, 1, true, {0,1,2,3,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols35.dat"}},
  {{{std::nullopt, {0, 4788}, cyclic_prefix::NORMAL, 51, 1, 18, {3}, 10, 2, 10, 176, 2, true, {0,}}, {1, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols36.dat"}},
  {{{std::nullopt, {0, 5545}, cyclic_prefix::NORMAL, 51, 1, 34, {16}, 8, 2, 2, 389, 2, true, {0,1,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols37.dat"}},
  {{{std::nullopt, {0, 2471}, cyclic_prefix::NORMAL, 51, 1, 9, {6}, 9, 2, 6, 768, 2, true, {0,1,2,3,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols38.dat"}},
  {{{std::nullopt, {0, 4893}, cyclic_prefix::NORMAL, 51, 1, 45, {27}, 12, 2, 2, 311, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols39.dat"}},
  {{{std::nullopt, {0, 8322}, cyclic_prefix::NORMAL, 51, 1, 13, {45}, 4, 2, 2, 152, 1, false, {0,1,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols40.dat"}},
  {{{std::nullopt, {0, 8980}, cyclic_prefix::NORMAL, 51, 1, 38, {50}, 11, 2, 7, 935, 1, false, {0,1,2,3,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols41.dat"}},
  {{{std::nullopt, {0, 9194}, cyclic_prefix::NORMAL, 51, 1, 34, {44}, 12, 2, 0, 832, 2, false, {0,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols42.dat"}},
  {{{std::nullopt, {0, 7358}, cyclic_prefix::NORMAL, 51, 1, 38, {11}, 1, 2, 3, 40, 2, false, {0,1,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols43.dat"}},
  {{{std::nullopt, {0, 3818}, cyclic_prefix::NORMAL, 51, 1, 40, {10}, 12, 2, 8, 910, 2, false, {0,1,2,3,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols44.dat"}},
  {{{std::nullopt, {1, 19845}, cyclic_prefix::NORMAL, 51, 1, 34, {}, 6, 1, 10, 235, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols45.dat"}},
  {{{std::nullopt, {1, 18821}, cyclic_prefix::NORMAL, 51, 1, 18, {}, 12, 1, 11, 25, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols46.dat"}},
  {{{std::nullopt, {1, 17042}, cyclic_prefix::NORMAL, 51, 1, 38, {}, 6, 1, 1, 956, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols47.dat"}},
  {{{std::nullopt, {1, 12220}, cyclic_prefix::NORMAL, 51, 1, 50, {}, 11, 1, 4, 790, 1, true, {0,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols48.dat"}},
  {{{std::nullopt, {1, 1028}, cyclic_prefix::NORMAL, 51, 1, 21, {}, 8, 1, 8, 91, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols49.dat"}},
  {{{std::nullopt, {1, 4456}, cyclic_prefix::NORMAL, 51, 1, 22, {}, 2, 1, 7, 876, 1, true, {0,1,2,3,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols50.dat"}},
  {{{std::nullopt, {1, 6398}, cyclic_prefix::NORMAL, 51, 1, 47, {}, 5, 1, 3, 507, 2, true, {0,}}, {0, 0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols51.dat"}},
  {{{std::nullopt, {1, 19590}, cyclic_prefix::NORMAL, 51, 1, 43, {}, 7, 1, 2, 104, 2, true, {0,1,}}, {1, 1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols52.dat"}},
  {{{std::nullopt, {1, 17253}, cyclic_prefix::NORMAL, 51, 1, 11, {}, 0, 1, 1, 819, 2, true, {0,1,2,3,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols53.dat"}},
  {{{std::nullopt, {1, 7310}, cyclic_prefix::NORMAL, 51, 1, 12, {}, 1, 1, 0, 999, 1, false, {0,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols54.dat"}},
  {{{std::nullopt, {1, 10058}, cyclic_prefix::NORMAL, 51, 1, 33, {}, 0, 1, 5, 486, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols55.dat"}},
  {{{std::nullopt, {1, 1273}, cyclic_prefix::NORMAL, 51, 1, 43, {}, 2, 1, 10, 294, 1, false, {0,1,2,3,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols56.dat"}},
  {{{std::nullopt, {1, 1079}, cyclic_prefix::NORMAL, 51, 1, 5, {}, 11, 1, 7, 475, 2, false, {0,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols57.dat"}},
  {{{std::nullopt, {1, 19007}, cyclic_prefix::NORMAL, 51, 1, 2, {}, 8, 1, 1, 212, 2, false, {0,1,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols58.dat"}},
  {{{std::nullopt, {1, 19070}, cyclic_prefix::NORMAL, 51, 1, 48, {}, 8, 1, 8, 259, 2, false, {0,1,2,3,}}, {1, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols59.dat"}},
  {{{std::nullopt, {1, 1847}, cyclic_prefix::NORMAL, 51, 1, 26, {}, 11, 2, 4, 842, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols60.dat"}},
  {{{std::nullopt, {1, 19914}, cyclic_prefix::NORMAL, 51, 1, 37, {}, 1, 2, 9, 104, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols61.dat"}},
  {{{std::nullopt, {1, 18390}, cyclic_prefix::NORMAL, 51, 1, 11, {}, 10, 2, 7, 542, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols62.dat"}},
  {{{std::nullopt, {1, 18993}, cyclic_prefix::NORMAL, 51, 1, 21, {}, 12, 2, 11, 364, 1, true, {0,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols63.dat"}},
  {{{std::nullopt, {1, 18239}, cyclic_prefix::NORMAL, 51, 1, 23, {}, 5, 2, 1, 42, 1, true, {0,1,}}, {1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols64.dat"}},
  {{{std::nullopt, {1, 18001}, cyclic_prefix::NORMAL, 51, 1, 23, {}, 1, 2, 6, 203, 1, true, {0,1,2,3,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols65.dat"}},
  {{{std::nullopt, {1, 8462}, cyclic_prefix::NORMAL, 51, 1, 1, {}, 3, 2, 4, 92, 2, true, {0,}}, {1, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols66.dat"}},
  {{{std::nullopt, {1, 13473}, cyclic_prefix::NORMAL, 51, 1, 6, {}, 9, 2, 10, 22, 2, true, {0,1,}}, {1, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols67.dat"}},
  {{{std::nullopt, {1, 15721}, cyclic_prefix::NORMAL, 51, 1, 50, {}, 12, 2, 11, 394, 2, true, {0,1,2,3,}}, {0, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols68.dat"}},
  {{{std::nullopt, {1, 901}, cyclic_prefix::NORMAL, 51, 1, 49, {}, 3, 2, 9, 16, 1, false, {0,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols69.dat"}},
  {{{std::nullopt, {1, 10869}, cyclic_prefix::NORMAL, 51, 1, 28, {}, 2, 2, 4, 320, 1, false, {0,1,}}, {1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols70.dat"}},
  {{{std::nullopt, {1, 18247}, cyclic_prefix::NORMAL, 51, 1, 45, {}, 8, 2, 1, 61, 1, false, {0,1,2,3,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols71.dat"}},
  {{{std::nullopt, {1, 13313}, cyclic_prefix::NORMAL, 51, 1, 22, {}, 6, 2, 9, 527, 2, false, {0,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols72.dat"}},
  {{{std::nullopt, {1, 18641}, cyclic_prefix::NORMAL, 51, 1, 9, {}, 6, 2, 7, 876, 2, false, {0,1,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols73.dat"}},
  {{{std::nullopt, {1, 10927}, cyclic_prefix::NORMAL, 51, 1, 41, {}, 1, 2, 0, 954, 2, false, {0,1,2,3,}}, {0, 1}, {}}, {"test_data/pucch_processor_format0_test_input_symbols74.dat"}},
  {{{std::nullopt, {1, 4395}, cyclic_prefix::NORMAL, 51, 1, 15, {48}, 5, 2, 9, 427, 0, true, {0,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols75.dat"}},
  {{{std::nullopt, {1, 20258}, cyclic_prefix::NORMAL, 51, 1, 44, {33}, 7, 2, 4, 206, 0, true, {0,1,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols76.dat"}},
  {{{std::nullopt, {1, 3389}, cyclic_prefix::NORMAL, 51, 1, 39, {1}, 10, 2, 3, 909, 0, true, {0,1,2,3,}}, {}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols77.dat"}},
  {{{std::nullopt, {1, 431}, cyclic_prefix::NORMAL, 51, 1, 48, {29}, 3, 2, 3, 552, 1, true, {0,}}, {0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols78.dat"}},
  {{{std::nullopt, {1, 12391}, cyclic_prefix::NORMAL, 51, 1, 4, {6}, 6, 2, 1, 679, 1, true, {0,1,}}, {0}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols79.dat"}},
  {{{std::nullopt, {1, 6804}, cyclic_prefix::NORMAL, 51, 1, 18, {9}, 12, 2, 0, 1003, 1, true, {0,1,2,3,}}, {1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols80.dat"}},
  {{{std::nullopt, {1, 12726}, cyclic_prefix::NORMAL, 51, 1, 21, {38}, 5, 2, 0, 767, 2, true, {0,}}, {0, 1}, {0}}, {"test_data/pucch_processor_format0_test_input_symbols81.dat"}},
  {{{std::nullopt, {1, 6551}, cyclic_prefix::NORMAL, 51, 1, 22, {15}, 1, 2, 1, 771, 2, true, {0,1,}}, {0, 1}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols82.dat"}},
  {{{std::nullopt, {1, 5254}, cyclic_prefix::NORMAL, 51, 1, 1, {34}, 4, 2, 9, 591, 2, true, {0,1,2,3,}}, {1, 0}, {1}}, {"test_data/pucch_processor_format0_test_input_symbols83.dat"}},
  {{{std::nullopt, {1, 17655}, cyclic_prefix::NORMAL, 51, 1, 2, {45}, 1, 2, 10, 914, 1, false, {0,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols84.dat"}},
  {{{std::nullopt, {1, 19943}, cyclic_prefix::NORMAL, 51, 1, 7, {50}, 5, 2, 8, 1004, 1, false, {0,1,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols85.dat"}},
  {{{std::nullopt, {1, 9697}, cyclic_prefix::NORMAL, 51, 1, 28, {31}, 1, 2, 0, 189, 1, false, {0,1,2,3,}}, {0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols86.dat"}},
  {{{std::nullopt, {1, 19900}, cyclic_prefix::NORMAL, 51, 1, 4, {29}, 0, 2, 2, 612, 2, false, {0,}}, {1, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols87.dat"}},
  {{{std::nullopt, {1, 11479}, cyclic_prefix::NORMAL, 51, 1, 41, {32}, 4, 2, 7, 623, 2, false, {0,1,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols88.dat"}},
  {{{std::nullopt, {1, 13933}, cyclic_prefix::NORMAL, 51, 1, 22, {39}, 4, 2, 3, 260, 2, false, {0,1,2,3,}}, {0, 0}, {}}, {"test_data/pucch_processor_format0_test_input_symbols89.dat"}},
    // clang-format on
};

} // namespace srsran
