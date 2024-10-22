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
//   + "srsPUCCHProcessorFormat1Unittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
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
  {{{{std::nullopt, {0, 901}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 744, 0, {0,1,2,3,}, 0, 14, 0, 3}, {}}, {{std::nullopt, {0, 901}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 744, 0, {0,1,2,3,}, 11, 14, 0, 3}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols0.dat"}},
  {{{{std::nullopt, {0, 8535}, 51, 1, cyclic_prefix::NORMAL, 45, {}, 837, 1, {0,1,2,3,}, 0, 14, 0, 6}, {0}}, {{std::nullopt, {0, 8535}, 51, 1, cyclic_prefix::NORMAL, 45, {}, 837, 1, {0,1,2,3,}, 11, 14, 0, 6}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols1.dat"}},
  {{{{std::nullopt, {0, 960}, 51, 1, cyclic_prefix::NORMAL, 6, {}, 966, 2, {0,1,2,3,}, 0, 14, 0, 1}, {0, 0}}, {{std::nullopt, {0, 960}, 51, 1, cyclic_prefix::NORMAL, 6, {}, 966, 2, {0,1,2,3,}, 4, 14, 0, 1}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols2.dat"}},
  {{{{std::nullopt, {0, 8558}, 51, 1, cyclic_prefix::NORMAL, 8, {}, 900, 0, {0,1,2,3,}, 1, 13, 1, 1}, {}}, {{std::nullopt, {0, 8558}, 51, 1, cyclic_prefix::NORMAL, 8, {}, 900, 0, {0,1,2,3,}, 9, 13, 1, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols3.dat"}},
  {{{{std::nullopt, {0, 6069}, 51, 1, cyclic_prefix::NORMAL, 11, {}, 671, 1, {0,1,2,3,}, 11, 13, 1, 4}, {0}}, {{std::nullopt, {0, 6069}, 51, 1, cyclic_prefix::NORMAL, 11, {}, 671, 1, {0,1,2,3,}, 6, 13, 1, 4}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols4.dat"}},
  {{{{std::nullopt, {0, 1325}, 51, 1, cyclic_prefix::NORMAL, 5, {}, 762, 2, {0,1,2,3,}, 1, 13, 1, 5}, {0, 1}}, {{std::nullopt, {0, 1325}, 51, 1, cyclic_prefix::NORMAL, 5, {}, 762, 2, {0,1,2,3,}, 0, 13, 1, 5}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols5.dat"}},
  {{{{std::nullopt, {0, 7855}, 51, 1, cyclic_prefix::NORMAL, 25, {}, 564, 0, {0,1,2,3,}, 2, 5, 5, 0}, {}}, {{std::nullopt, {0, 7855}, 51, 1, cyclic_prefix::NORMAL, 25, {}, 564, 0, {0,1,2,3,}, 4, 5, 5, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols6.dat"}},
  {{{{std::nullopt, {0, 292}, 51, 1, cyclic_prefix::NORMAL, 19, {}, 912, 1, {0,1,2,3,}, 4, 5, 5, 0}, {0}}, {{std::nullopt, {0, 292}, 51, 1, cyclic_prefix::NORMAL, 19, {}, 912, 1, {0,1,2,3,}, 11, 5, 5, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols7.dat"}},
  {{{{std::nullopt, {0, 10205}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 42, 2, {0,1,2,3,}, 10, 5, 5, 0}, {0, 0}}, {{std::nullopt, {0, 10205}, 51, 1, cyclic_prefix::NORMAL, 36, {}, 42, 2, {0,1,2,3,}, 1, 5, 5, 0}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols8.dat"}},
  {{{{std::nullopt, {0, 3024}, 51, 1, cyclic_prefix::NORMAL, 47, {}, 615, 0, {0,1,2,3,}, 7, 4, 10, 1}, {}}, {{std::nullopt, {0, 3024}, 51, 1, cyclic_prefix::NORMAL, 47, {}, 615, 0, {0,1,2,3,}, 1, 4, 10, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols9.dat"}},
  {{{{std::nullopt, {0, 8527}, 51, 1, cyclic_prefix::NORMAL, 20, {}, 77, 1, {0,1,2,3,}, 9, 4, 10, 1}, {1}}, {{std::nullopt, {0, 8527}, 51, 1, cyclic_prefix::NORMAL, 20, {}, 77, 1, {0,1,2,3,}, 7, 4, 10, 1}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols10.dat"}},
  {{{{std::nullopt, {0, 680}, 51, 1, cyclic_prefix::NORMAL, 34, {}, 173, 2, {0,1,2,3,}, 0, 4, 10, 1}, {1, 0}}, {{std::nullopt, {0, 680}, 51, 1, cyclic_prefix::NORMAL, 34, {}, 173, 2, {0,1,2,3,}, 1, 4, 10, 1}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols11.dat"}},
  {{{{std::nullopt, {0, 586}, 51, 1, cyclic_prefix::NORMAL, 34, {32}, 959, 0, {0,1,2,3,}, 1, 14, 0, 0}, {}}, {{std::nullopt, {0, 586}, 51, 1, cyclic_prefix::NORMAL, 34, {32}, 959, 0, {0,1,2,3,}, 4, 14, 0, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols12.dat"}},
  {{{{std::nullopt, {0, 5144}, 51, 1, cyclic_prefix::NORMAL, 40, {14}, 513, 1, {0,1,2,3,}, 5, 14, 0, 1}, {0}}, {{std::nullopt, {0, 5144}, 51, 1, cyclic_prefix::NORMAL, 40, {14}, 513, 1, {0,1,2,3,}, 4, 14, 0, 1}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols13.dat"}},
  {{{{std::nullopt, {0, 1231}, 51, 1, cyclic_prefix::NORMAL, 31, {14}, 341, 2, {0,1,2,3,}, 9, 14, 0, 2}, {1, 1}}, {{std::nullopt, {0, 1231}, 51, 1, cyclic_prefix::NORMAL, 31, {14}, 341, 2, {0,1,2,3,}, 7, 14, 0, 2}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols14.dat"}},
  {{{{std::nullopt, {0, 8138}, 51, 1, cyclic_prefix::NORMAL, 0, {44}, 525, 0, {0,1,2,3,}, 1, 13, 1, 2}, {}}, {{std::nullopt, {0, 8138}, 51, 1, cyclic_prefix::NORMAL, 0, {44}, 525, 0, {0,1,2,3,}, 4, 13, 1, 2}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols15.dat"}},
  {{{{std::nullopt, {0, 8365}, 51, 1, cyclic_prefix::NORMAL, 27, {6}, 312, 1, {0,1,2,3,}, 10, 13, 1, 1}, {1}}, {{std::nullopt, {0, 8365}, 51, 1, cyclic_prefix::NORMAL, 27, {6}, 312, 1, {0,1,2,3,}, 8, 13, 1, 1}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols16.dat"}},
  {{{{std::nullopt, {0, 6627}, 51, 1, cyclic_prefix::NORMAL, 22, {21}, 882, 2, {0,1,2,3,}, 0, 13, 1, 2}, {1, 0}}, {{std::nullopt, {0, 6627}, 51, 1, cyclic_prefix::NORMAL, 22, {21}, 882, 2, {0,1,2,3,}, 2, 13, 1, 2}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols17.dat"}},
  {{{{std::nullopt, {0, 6754}, 51, 1, cyclic_prefix::NORMAL, 36, {12}, 772, 0, {0,1,2,3,}, 8, 5, 5, 0}, {}}, {{std::nullopt, {0, 6754}, 51, 1, cyclic_prefix::NORMAL, 36, {12}, 772, 0, {0,1,2,3,}, 4, 5, 5, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols18.dat"}},
  {{{{std::nullopt, {0, 4406}, 51, 1, cyclic_prefix::NORMAL, 26, {48}, 404, 1, {0,1,2,3,}, 9, 5, 5, 0}, {1}}, {{std::nullopt, {0, 4406}, 51, 1, cyclic_prefix::NORMAL, 26, {48}, 404, 1, {0,1,2,3,}, 6, 5, 5, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols19.dat"}},
  {{{{std::nullopt, {0, 3583}, 51, 1, cyclic_prefix::NORMAL, 11, {5}, 550, 2, {0,1,2,3,}, 5, 5, 5, 0}, {1, 0}}, {{std::nullopt, {0, 3583}, 51, 1, cyclic_prefix::NORMAL, 11, {5}, 550, 2, {0,1,2,3,}, 11, 5, 5, 0}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols20.dat"}},
  {{{{std::nullopt, {0, 6394}, 51, 1, cyclic_prefix::NORMAL, 44, {31}, 53, 0, {0,1,2,3,}, 3, 4, 10, 0}, {}}, {{std::nullopt, {0, 6394}, 51, 1, cyclic_prefix::NORMAL, 44, {31}, 53, 0, {0,1,2,3,}, 7, 4, 10, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols21.dat"}},
  {{{{std::nullopt, {0, 9749}, 51, 1, cyclic_prefix::NORMAL, 14, {14}, 485, 1, {0,1,2,3,}, 2, 4, 10, 0}, {0}}, {{std::nullopt, {0, 9749}, 51, 1, cyclic_prefix::NORMAL, 14, {14}, 485, 1, {0,1,2,3,}, 4, 4, 10, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols22.dat"}},
  {{{{std::nullopt, {0, 3708}, 51, 1, cyclic_prefix::NORMAL, 42, {48}, 632, 2, {0,1,2,3,}, 4, 4, 10, 0}, {0, 0}}, {{std::nullopt, {0, 3708}, 51, 1, cyclic_prefix::NORMAL, 42, {48}, 632, 2, {0,1,2,3,}, 1, 4, 10, 0}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols23.dat"}},
  {{{{std::nullopt, {1, 15738}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 323, 0, {0,1,2,3,}, 4, 14, 0, 1}, {}}, {{std::nullopt, {1, 15738}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 323, 0, {0,1,2,3,}, 6, 14, 0, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols24.dat"}},
  {{{{std::nullopt, {1, 6110}, 51, 1, cyclic_prefix::NORMAL, 23, {}, 773, 1, {0,1,2,3,}, 9, 14, 0, 5}, {1}}, {{std::nullopt, {1, 6110}, 51, 1, cyclic_prefix::NORMAL, 23, {}, 773, 1, {0,1,2,3,}, 1, 14, 0, 5}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols25.dat"}},
  {{{{std::nullopt, {1, 404}, 51, 1, cyclic_prefix::NORMAL, 18, {}, 394, 2, {0,1,2,3,}, 8, 14, 0, 3}, {1, 0}}, {{std::nullopt, {1, 404}, 51, 1, cyclic_prefix::NORMAL, 18, {}, 394, 2, {0,1,2,3,}, 3, 14, 0, 3}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols26.dat"}},
  {{{{std::nullopt, {1, 13056}, 51, 1, cyclic_prefix::NORMAL, 45, {}, 258, 0, {0,1,2,3,}, 8, 13, 1, 5}, {}}, {{std::nullopt, {1, 13056}, 51, 1, cyclic_prefix::NORMAL, 45, {}, 258, 0, {0,1,2,3,}, 1, 13, 1, 5}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols27.dat"}},
  {{{{std::nullopt, {1, 13304}, 51, 1, cyclic_prefix::NORMAL, 2, {}, 963, 1, {0,1,2,3,}, 3, 13, 1, 5}, {1}}, {{std::nullopt, {1, 13304}, 51, 1, cyclic_prefix::NORMAL, 2, {}, 963, 1, {0,1,2,3,}, 0, 13, 1, 5}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols28.dat"}},
  {{{{std::nullopt, {1, 15889}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 665, 2, {0,1,2,3,}, 4, 13, 1, 5}, {0, 0}}, {{std::nullopt, {1, 15889}, 51, 1, cyclic_prefix::NORMAL, 49, {}, 665, 2, {0,1,2,3,}, 8, 13, 1, 5}, {0, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols29.dat"}},
  {{{{std::nullopt, {1, 5962}, 51, 1, cyclic_prefix::NORMAL, 44, {}, 55, 0, {0,1,2,3,}, 6, 5, 5, 1}, {}}, {{std::nullopt, {1, 5962}, 51, 1, cyclic_prefix::NORMAL, 44, {}, 55, 0, {0,1,2,3,}, 5, 5, 5, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols30.dat"}},
  {{{{std::nullopt, {1, 15450}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 659, 1, {0,1,2,3,}, 8, 5, 5, 0}, {0}}, {{std::nullopt, {1, 15450}, 51, 1, cyclic_prefix::NORMAL, 32, {}, 659, 1, {0,1,2,3,}, 10, 5, 5, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols31.dat"}},
  {{{{std::nullopt, {1, 4650}, 51, 1, cyclic_prefix::NORMAL, 50, {}, 198, 2, {0,1,2,3,}, 8, 5, 5, 1}, {1, 1}}, {{std::nullopt, {1, 4650}, 51, 1, cyclic_prefix::NORMAL, 50, {}, 198, 2, {0,1,2,3,}, 3, 5, 5, 1}, {1, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols32.dat"}},
  {{{{std::nullopt, {1, 14663}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 361, 0, {0,1,2,3,}, 11, 4, 10, 0}, {}}, {{std::nullopt, {1, 14663}, 51, 1, cyclic_prefix::NORMAL, 48, {}, 361, 0, {0,1,2,3,}, 3, 4, 10, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols33.dat"}},
  {{{{std::nullopt, {1, 16259}, 51, 1, cyclic_prefix::NORMAL, 24, {}, 888, 1, {0,1,2,3,}, 9, 4, 10, 1}, {1}}, {{std::nullopt, {1, 16259}, 51, 1, cyclic_prefix::NORMAL, 24, {}, 888, 1, {0,1,2,3,}, 1, 4, 10, 1}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols34.dat"}},
  {{{{std::nullopt, {1, 14395}, 51, 1, cyclic_prefix::NORMAL, 39, {}, 394, 2, {0,1,2,3,}, 3, 4, 10, 1}, {1, 1}}, {{std::nullopt, {1, 14395}, 51, 1, cyclic_prefix::NORMAL, 39, {}, 394, 2, {0,1,2,3,}, 1, 4, 10, 1}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols35.dat"}},
  {{{{std::nullopt, {1, 1997}, 51, 1, cyclic_prefix::NORMAL, 21, {29}, 878, 0, {0,1,2,3,}, 4, 14, 0, 1}, {}}, {{std::nullopt, {1, 1997}, 51, 1, cyclic_prefix::NORMAL, 21, {29}, 878, 0, {0,1,2,3,}, 8, 14, 0, 1}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols36.dat"}},
  {{{{std::nullopt, {1, 2649}, 51, 1, cyclic_prefix::NORMAL, 42, {48}, 222, 1, {0,1,2,3,}, 6, 14, 0, 1}, {1}}, {{std::nullopt, {1, 2649}, 51, 1, cyclic_prefix::NORMAL, 42, {48}, 222, 1, {0,1,2,3,}, 4, 14, 0, 1}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols37.dat"}},
  {{{{std::nullopt, {1, 14725}, 51, 1, cyclic_prefix::NORMAL, 20, {6}, 119, 2, {0,1,2,3,}, 7, 14, 0, 1}, {1, 1}}, {{std::nullopt, {1, 14725}, 51, 1, cyclic_prefix::NORMAL, 20, {6}, 119, 2, {0,1,2,3,}, 1, 14, 0, 1}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols38.dat"}},
  {{{{std::nullopt, {1, 6201}, 51, 1, cyclic_prefix::NORMAL, 30, {28}, 80, 0, {0,1,2,3,}, 6, 13, 1, 0}, {}}, {{std::nullopt, {1, 6201}, 51, 1, cyclic_prefix::NORMAL, 30, {28}, 80, 0, {0,1,2,3,}, 10, 13, 1, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols39.dat"}},
  {{{{std::nullopt, {1, 8613}, 51, 1, cyclic_prefix::NORMAL, 23, {8}, 806, 1, {0,1,2,3,}, 7, 13, 1, 2}, {1}}, {{std::nullopt, {1, 8613}, 51, 1, cyclic_prefix::NORMAL, 23, {8}, 806, 1, {0,1,2,3,}, 2, 13, 1, 2}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols40.dat"}},
  {{{{std::nullopt, {1, 11951}, 51, 1, cyclic_prefix::NORMAL, 27, {45}, 377, 2, {0,1,2,3,}, 1, 13, 1, 0}, {1, 1}}, {{std::nullopt, {1, 11951}, 51, 1, cyclic_prefix::NORMAL, 27, {45}, 377, 2, {0,1,2,3,}, 6, 13, 1, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols41.dat"}},
  {{{{std::nullopt, {1, 16380}, 51, 1, cyclic_prefix::NORMAL, 14, {1}, 997, 0, {0,1,2,3,}, 1, 5, 5, 0}, {}}, {{std::nullopt, {1, 16380}, 51, 1, cyclic_prefix::NORMAL, 14, {1}, 997, 0, {0,1,2,3,}, 5, 5, 5, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols42.dat"}},
  {{{{std::nullopt, {1, 13182}, 51, 1, cyclic_prefix::NORMAL, 37, {21}, 537, 1, {0,1,2,3,}, 8, 5, 5, 0}, {0}}, {{std::nullopt, {1, 13182}, 51, 1, cyclic_prefix::NORMAL, 37, {21}, 537, 1, {0,1,2,3,}, 1, 5, 5, 0}, {0}}}, {"test_data/pucch_processor_format1_test_input_symbols43.dat"}},
  {{{{std::nullopt, {1, 3100}, 51, 1, cyclic_prefix::NORMAL, 42, {29}, 316, 2, {0,1,2,3,}, 7, 5, 5, 0}, {1, 1}}, {{std::nullopt, {1, 3100}, 51, 1, cyclic_prefix::NORMAL, 42, {29}, 316, 2, {0,1,2,3,}, 4, 5, 5, 0}, {1, 0}}}, {"test_data/pucch_processor_format1_test_input_symbols44.dat"}},
  {{{{std::nullopt, {1, 6364}, 51, 1, cyclic_prefix::NORMAL, 2, {39}, 480, 0, {0,1,2,3,}, 4, 4, 10, 0}, {}}, {{std::nullopt, {1, 6364}, 51, 1, cyclic_prefix::NORMAL, 2, {39}, 480, 0, {0,1,2,3,}, 5, 4, 10, 0}, {}}}, {"test_data/pucch_processor_format1_test_input_symbols45.dat"}},
  {{{{std::nullopt, {1, 6121}, 51, 1, cyclic_prefix::NORMAL, 16, {6}, 639, 1, {0,1,2,3,}, 3, 4, 10, 0}, {1}}, {{std::nullopt, {1, 6121}, 51, 1, cyclic_prefix::NORMAL, 16, {6}, 639, 1, {0,1,2,3,}, 8, 4, 10, 0}, {1}}}, {"test_data/pucch_processor_format1_test_input_symbols46.dat"}},
  {{{{std::nullopt, {1, 2042}, 51, 1, cyclic_prefix::NORMAL, 16, {49}, 364, 2, {0,1,2,3,}, 1, 4, 10, 0}, {1, 1}}, {{std::nullopt, {1, 2042}, 51, 1, cyclic_prefix::NORMAL, 16, {49}, 364, 2, {0,1,2,3,}, 9, 4, 10, 0}, {0, 1}}}, {"test_data/pucch_processor_format1_test_input_symbols47.dat"}},
    // clang-format on
};

} // namespace srsran
