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

// This file was generated using the following MATLAB class on 10-11-2023 (seed 0):
//   + "srsPUCCHProcessorFormat1Unittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct pucch_entry {
  pucch_processor::format1_configuration config;
  std::vector<uint8_t>                   ack_bits;
};
struct test_case_t {
  std::vector<pucch_entry>                                entries;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
};

static const std::vector<test_case_t> pucch_processor_format1_test_data = {
    // clang-format off
  {{{{nullopt, {0, 1309}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 821, 0, {0,1,2,3,}, 10, 14, 0, 1}, {}}, {{nullopt, {0, 1309}, 51, 1, cyclic_prefix::NORMAL, 4, {}, 821, 0, {0,1,2,3,}, 7, 14, 0, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols0.dat"}},
  {{{{nullopt, {0, 5281}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 186, 1, {0,1,2,3,}, 6, 14, 0, 2}, {1}}, {{nullopt, {0, 5281}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 186, 1, {0,1,2,3,}, 8, 14, 0, 2}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols1.dat"}},
  {{{{nullopt, {0, 1319}, 51, 1, cyclic_prefix::NORMAL, 44, {}, 943, 2, {0,1,2,3,}, 0, 14, 0, 4}, {0, 0}}, {{nullopt, {0, 1319}, 51, 1, cyclic_prefix::NORMAL, 44, {}, 943, 2, {0,1,2,3,}, 4, 14, 0, 4}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols2.dat"}},
  {{{{nullopt, {0, 9826}, 51, 1, cyclic_prefix::NORMAL, 12, {}, 135, 0, {0,1,2,3,}, 2, 13, 1, 5}, {}}, {{nullopt, {0, 9826}, 51, 1, cyclic_prefix::NORMAL, 12, {}, 135, 0, {0,1,2,3,}, 5, 13, 1, 5}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols3.dat"}},
  {{{{nullopt, {0, 9264}, 51, 1, cyclic_prefix::NORMAL, 35, {}, 900, 1, {0,1,2,3,}, 11, 13, 1, 0}, {1}}, {{nullopt, {0, 9264}, 51, 1, cyclic_prefix::NORMAL, 35, {}, 900, 1, {0,1,2,3,}, 1, 13, 1, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols4.dat"}},
  {{{{nullopt, {0, 7075}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 537, 2, {0,1,2,3,}, 4, 13, 1, 3}, {0, 1}}, {{nullopt, {0, 7075}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 537, 2, {0,1,2,3,}, 7, 13, 1, 3}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols5.dat"}},
  {{{{nullopt, {0, 5176}, 51, 1, cyclic_prefix::NORMAL, 11, {}, 418, 0, {0,1,2,3,}, 7, 5, 5, 1}, {}}, {{nullopt, {0, 5176}, 51, 1, cyclic_prefix::NORMAL, 11, {}, 418, 0, {0,1,2,3,}, 1, 5, 5, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols6.dat"}},
  {{{{nullopt, {0, 5449}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 410, 1, {0,1,2,3,}, 11, 5, 5, 0}, {1}}, {{nullopt, {0, 5449}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 410, 1, {0,1,2,3,}, 7, 5, 5, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols7.dat"}},
  {{{{nullopt, {0, 147}, 51, 1, cyclic_prefix::NORMAL, 5, {}, 847, 2, {0,1,2,3,}, 4, 5, 5, 1}, {1, 0}}, {{nullopt, {0, 147}, 51, 1, cyclic_prefix::NORMAL, 5, {}, 847, 2, {0,1,2,3,}, 7, 5, 5, 1}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols8.dat"}},
  {{{{nullopt, {0, 1340}, 51, 1, cyclic_prefix::NORMAL, 14, {}, 123, 0, {0,1,2,3,}, 3, 4, 10, 1}, {}}, {{nullopt, {0, 1340}, 51, 1, cyclic_prefix::NORMAL, 14, {}, 123, 0, {0,1,2,3,}, 0, 4, 10, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols9.dat"}},
  {{{{nullopt, {0, 6781}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 59, 1, {0,1,2,3,}, 0, 4, 10, 1}, {1}}, {{nullopt, {0, 6781}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 59, 1, {0,1,2,3,}, 5, 4, 10, 1}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols10.dat"}},
  {{{{nullopt, {0, 3038}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 363, 2, {0,1,2,3,}, 6, 4, 10, 0}, {1, 1}}, {{nullopt, {0, 3038}, 51, 1, cyclic_prefix::NORMAL, 46, {}, 363, 2, {0,1,2,3,}, 5, 4, 10, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols11.dat"}},
  {{{{nullopt, {0, 4959}, 51, 1, cyclic_prefix::NORMAL, 6, {39}, 540, 0, {0,1,2,3,}, 6, 14, 0, 0}, {}}, {{nullopt, {0, 4959}, 51, 1, cyclic_prefix::NORMAL, 6, {39}, 540, 0, {0,1,2,3,}, 10, 14, 0, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols12.dat"}},
  {{{{nullopt, {0, 2149}, 51, 1, cyclic_prefix::NORMAL, 0, {12}, 405, 1, {0,1,2,3,}, 2, 14, 0, 2}, {0}}, {{nullopt, {0, 2149}, 51, 1, cyclic_prefix::NORMAL, 0, {12}, 405, 1, {0,1,2,3,}, 0, 14, 0, 2}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols13.dat"}},
  {{{{nullopt, {0, 9898}, 51, 1, cyclic_prefix::NORMAL, 45, {49}, 474, 2, {0,1,2,3,}, 0, 14, 0, 1}, {0, 0}}, {{nullopt, {0, 9898}, 51, 1, cyclic_prefix::NORMAL, 45, {49}, 474, 2, {0,1,2,3,}, 7, 14, 0, 1}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols14.dat"}},
  {{{{nullopt, {0, 1563}, 51, 1, cyclic_prefix::NORMAL, 32, {21}, 1005, 0, {0,1,2,3,}, 7, 13, 1, 2}, {}}, {{nullopt, {0, 1563}, 51, 1, cyclic_prefix::NORMAL, 32, {21}, 1005, 0, {0,1,2,3,}, 5, 13, 1, 2}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols15.dat"}},
  {{{{nullopt, {0, 5258}, 51, 1, cyclic_prefix::NORMAL, 15, {8}, 361, 1, {0,1,2,3,}, 2, 13, 1, 0}, {1}}, {{nullopt, {0, 5258}, 51, 1, cyclic_prefix::NORMAL, 15, {8}, 361, 1, {0,1,2,3,}, 9, 13, 1, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols16.dat"}},
  {{{{nullopt, {0, 1626}, 51, 1, cyclic_prefix::NORMAL, 10, {33}, 796, 2, {0,1,2,3,}, 10, 13, 1, 1}, {1, 1}}, {{nullopt, {0, 1626}, 51, 1, cyclic_prefix::NORMAL, 10, {33}, 796, 2, {0,1,2,3,}, 9, 13, 1, 1}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols17.dat"}},
  {{{{nullopt, {0, 7141}, 51, 1, cyclic_prefix::NORMAL, 45, {28}, 854, 0, {0,1,2,3,}, 2, 5, 5, 0}, {}}, {{nullopt, {0, 7141}, 51, 1, cyclic_prefix::NORMAL, 45, {28}, 854, 0, {0,1,2,3,}, 9, 5, 5, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols18.dat"}},
  {{{{nullopt, {0, 339}, 51, 1, cyclic_prefix::NORMAL, 10, {17}, 409, 1, {0,1,2,3,}, 7, 5, 5, 0}, {1}}, {{nullopt, {0, 339}, 51, 1, cyclic_prefix::NORMAL, 10, {17}, 409, 1, {0,1,2,3,}, 2, 5, 5, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols19.dat"}},
  {{{{nullopt, {0, 2785}, 51, 1, cyclic_prefix::NORMAL, 49, {4}, 374, 2, {0,1,2,3,}, 9, 5, 5, 0}, {1, 0}}, {{nullopt, {0, 2785}, 51, 1, cyclic_prefix::NORMAL, 49, {4}, 374, 2, {0,1,2,3,}, 11, 5, 5, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols20.dat"}},
  {{{{nullopt, {0, 6366}, 51, 1, cyclic_prefix::NORMAL, 45, {38}, 899, 0, {0,1,2,3,}, 0, 4, 10, 0}, {}}, {{nullopt, {0, 6366}, 51, 1, cyclic_prefix::NORMAL, 45, {38}, 899, 0, {0,1,2,3,}, 5, 4, 10, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols21.dat"}},
  {{{{nullopt, {0, 107}, 51, 1, cyclic_prefix::NORMAL, 20, {20}, 109, 1, {0,1,2,3,}, 6, 4, 10, 0}, {0}}, {{nullopt, {0, 107}, 51, 1, cyclic_prefix::NORMAL, 20, {20}, 109, 1, {0,1,2,3,}, 3, 4, 10, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols22.dat"}},
  {{{{nullopt, {0, 8584}, 51, 1, cyclic_prefix::NORMAL, 10, {19}, 56, 2, {0,1,2,3,}, 11, 4, 10, 0}, {1, 1}}, {{nullopt, {0, 8584}, 51, 1, cyclic_prefix::NORMAL, 10, {19}, 56, 2, {0,1,2,3,}, 10, 4, 10, 0}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols23.dat"}},
  {{{{nullopt, {1, 9913}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 452, 0, {0,1,2,3,}, 7, 14, 0, 3}, {}}, {{nullopt, {1, 9913}, 51, 1, cyclic_prefix::NORMAL, 17, {}, 452, 0, {0,1,2,3,}, 5, 14, 0, 3}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols24.dat"}},
  {{{{nullopt, {1, 5622}, 51, 1, cyclic_prefix::NORMAL, 28, {}, 284, 1, {0,1,2,3,}, 10, 14, 0, 5}, {1}}, {{nullopt, {1, 5622}, 51, 1, cyclic_prefix::NORMAL, 28, {}, 284, 1, {0,1,2,3,}, 3, 14, 0, 5}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols25.dat"}},
  {{{{nullopt, {1, 7037}, 51, 1, cyclic_prefix::NORMAL, 26, {}, 12, 2, {0,1,2,3,}, 11, 14, 0, 5}, {0, 0}}, {{nullopt, {1, 7037}, 51, 1, cyclic_prefix::NORMAL, 26, {}, 12, 2, {0,1,2,3,}, 1, 14, 0, 5}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols26.dat"}},
  {{{{nullopt, {1, 5395}, 51, 1, cyclic_prefix::NORMAL, 2, {}, 876, 0, {0,1,2,3,}, 7, 13, 1, 5}, {}}, {{nullopt, {1, 5395}, 51, 1, cyclic_prefix::NORMAL, 2, {}, 876, 0, {0,1,2,3,}, 11, 13, 1, 5}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols27.dat"}},
  {{{{nullopt, {1, 2966}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 183, 1, {0,1,2,3,}, 9, 13, 1, 4}, {0}}, {{nullopt, {1, 2966}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 183, 1, {0,1,2,3,}, 0, 13, 1, 4}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols28.dat"}},
  {{{{nullopt, {1, 2863}, 51, 1, cyclic_prefix::NORMAL, 27, {}, 503, 2, {0,1,2,3,}, 8, 13, 1, 2}, {0, 0}}, {{nullopt, {1, 2863}, 51, 1, cyclic_prefix::NORMAL, 27, {}, 503, 2, {0,1,2,3,}, 0, 13, 1, 2}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols29.dat"}},
  {{{{nullopt, {1, 15445}, 51, 1, cyclic_prefix::NORMAL, 19, {}, 278, 0, {0,1,2,3,}, 9, 5, 5, 1}, {}}, {{nullopt, {1, 15445}, 51, 1, cyclic_prefix::NORMAL, 19, {}, 278, 0, {0,1,2,3,}, 11, 5, 5, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols30.dat"}},
  {{{{nullopt, {1, 12933}, 51, 1, cyclic_prefix::NORMAL, 43, {}, 465, 1, {0,1,2,3,}, 1, 5, 5, 0}, {0}}, {{nullopt, {1, 12933}, 51, 1, cyclic_prefix::NORMAL, 43, {}, 465, 1, {0,1,2,3,}, 3, 5, 5, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols31.dat"}},
  {{{{nullopt, {1, 19355}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 373, 2, {0,1,2,3,}, 2, 5, 5, 1}, {0, 0}}, {{nullopt, {1, 19355}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 373, 2, {0,1,2,3,}, 11, 5, 5, 1}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols32.dat"}},
  {{{{nullopt, {1, 4048}, 51, 1, cyclic_prefix::NORMAL, 16, {}, 890, 0, {0,1,2,3,}, 1, 4, 10, 1}, {}}, {{nullopt, {1, 4048}, 51, 1, cyclic_prefix::NORMAL, 16, {}, 890, 0, {0,1,2,3,}, 6, 4, 10, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols33.dat"}},
  {{{{nullopt, {1, 15799}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 198, 1, {0,1,2,3,}, 5, 4, 10, 0}, {1}}, {{nullopt, {1, 15799}, 51, 1, cyclic_prefix::NORMAL, 41, {}, 198, 1, {0,1,2,3,}, 4, 4, 10, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols34.dat"}},
  {{{{nullopt, {1, 12017}, 51, 1, cyclic_prefix::NORMAL, 34, {}, 964, 2, {0,1,2,3,}, 10, 4, 10, 1}, {1, 1}}, {{nullopt, {1, 12017}, 51, 1, cyclic_prefix::NORMAL, 34, {}, 964, 2, {0,1,2,3,}, 6, 4, 10, 1}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols35.dat"}},
  {{{{nullopt, {1, 17242}, 51, 1, cyclic_prefix::NORMAL, 22, {8}, 991, 0, {0,1,2,3,}, 4, 14, 0, 1}, {}}, {{nullopt, {1, 17242}, 51, 1, cyclic_prefix::NORMAL, 22, {8}, 991, 0, {0,1,2,3,}, 11, 14, 0, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols36.dat"}},
  {{{{nullopt, {1, 12305}, 51, 1, cyclic_prefix::NORMAL, 23, {19}, 564, 1, {0,1,2,3,}, 10, 14, 0, 2}, {1}}, {{nullopt, {1, 12305}, 51, 1, cyclic_prefix::NORMAL, 23, {19}, 564, 1, {0,1,2,3,}, 6, 14, 0, 2}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols37.dat"}},
  {{{{nullopt, {1, 18641}, 51, 1, cyclic_prefix::NORMAL, 40, {45}, 449, 2, {0,1,2,3,}, 7, 14, 0, 0}, {0, 0}}, {{nullopt, {1, 18641}, 51, 1, cyclic_prefix::NORMAL, 40, {45}, 449, 2, {0,1,2,3,}, 5, 14, 0, 0}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols38.dat"}},
  {{{{nullopt, {1, 10429}, 51, 1, cyclic_prefix::NORMAL, 49, {21}, 842, 0, {0,1,2,3,}, 10, 13, 1, 2}, {}}, {{nullopt, {1, 10429}, 51, 1, cyclic_prefix::NORMAL, 49, {21}, 842, 0, {0,1,2,3,}, 8, 13, 1, 2}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols39.dat"}},
  {{{{nullopt, {1, 2158}, 51, 1, cyclic_prefix::NORMAL, 18, {43}, 750, 1, {0,1,2,3,}, 7, 13, 1, 1}, {0}}, {{nullopt, {1, 2158}, 51, 1, cyclic_prefix::NORMAL, 18, {43}, 750, 1, {0,1,2,3,}, 4, 13, 1, 1}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols40.dat"}},
  {{{{nullopt, {1, 17046}, 51, 1, cyclic_prefix::NORMAL, 2, {21}, 266, 2, {0,1,2,3,}, 6, 13, 1, 0}, {0, 1}}, {{nullopt, {1, 17046}, 51, 1, cyclic_prefix::NORMAL, 2, {21}, 266, 2, {0,1,2,3,}, 8, 13, 1, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols41.dat"}},
  {{{{nullopt, {1, 10351}, 51, 1, cyclic_prefix::NORMAL, 49, {38}, 908, 0, {0,1,2,3,}, 2, 5, 5, 0}, {}}, {{nullopt, {1, 10351}, 51, 1, cyclic_prefix::NORMAL, 49, {38}, 908, 0, {0,1,2,3,}, 8, 5, 5, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols42.dat"}},
  {{{{nullopt, {1, 2424}, 51, 1, cyclic_prefix::NORMAL, 13, {28}, 283, 1, {0,1,2,3,}, 0, 5, 5, 0}, {0}}, {{nullopt, {1, 2424}, 51, 1, cyclic_prefix::NORMAL, 13, {28}, 283, 1, {0,1,2,3,}, 3, 5, 5, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols43.dat"}},
  {{{{nullopt, {1, 6402}, 51, 1, cyclic_prefix::NORMAL, 9, {44}, 163, 2, {0,1,2,3,}, 5, 5, 5, 0}, {1, 1}}, {{nullopt, {1, 6402}, 51, 1, cyclic_prefix::NORMAL, 9, {44}, 163, 2, {0,1,2,3,}, 9, 5, 5, 0}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols44.dat"}},
  {{{{nullopt, {1, 13826}, 51, 1, cyclic_prefix::NORMAL, 27, {34}, 399, 0, {0,1,2,3,}, 11, 4, 10, 0}, {}}, {{nullopt, {1, 13826}, 51, 1, cyclic_prefix::NORMAL, 27, {34}, 399, 0, {0,1,2,3,}, 1, 4, 10, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols45.dat"}},
  {{{{nullopt, {1, 7857}, 51, 1, cyclic_prefix::NORMAL, 37, {17}, 869, 1, {0,1,2,3,}, 1, 4, 10, 0}, {1}}, {{nullopt, {1, 7857}, 51, 1, cyclic_prefix::NORMAL, 37, {17}, 869, 1, {0,1,2,3,}, 7, 4, 10, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols46.dat"}},
  {{{{nullopt, {1, 1545}, 51, 1, cyclic_prefix::NORMAL, 33, {50}, 594, 2, {0,1,2,3,}, 3, 4, 10, 0}, {1, 0}}, {{nullopt, {1, 1545}, 51, 1, cyclic_prefix::NORMAL, 33, {50}, 594, 2, {0,1,2,3,}, 0, 4, 10, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols47.dat"}},
    // clang-format on
};

} // namespace srsran
