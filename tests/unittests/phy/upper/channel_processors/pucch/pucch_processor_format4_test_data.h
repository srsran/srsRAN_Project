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

// This file was generated using the following MATLAB class on 18-11-2024 (seed 0):
//   + "srsPUCCHProcessorFormat4Unittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct context_t {
  unsigned                               grid_nof_prb;
  unsigned                               grid_nof_symbols;
  pucch_processor::format4_configuration config;
  std::vector<uint8_t>                   harq_ack;
  std::vector<uint8_t>                   sr;
  std::vector<uint8_t>                   csi_part_1;
  std::vector<uint8_t>                   csi_part_2;
};

struct test_case_t {
  context_t                                               context;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
};

static const std::vector<test_case_t> pucch_processor_format4_test_data = {
    // clang-format off
  {{96, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 70, 26, 38, {}, 0, 14, 8323, 821, 927, 6, 0, 0, 0, true, false, 3, 4}, {1, 0, 1, 1, 0, 1}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols0.dat"}},
  {{257, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 42, 215, 38, {}, 0, 14, 31358, 462, 262, 10, 0, 0, 0, false, false, 3, 4}, {0, 0, 0, 1, 0, 1, 0, 0, 0, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols1.dat"}},
  {{254, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 25, 229, 16, {}, 0, 14, 6228, 36, 198, 17, 0, 0, 0, false, true, 1, 2}, {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols2.dat"}},
  {{240, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 21, 219, 19, 10, 0, 14, 35718, 501, 961, 10, 0, 0, 0, false, true, 0, 2}, {0, 1, 0, 0, 1, 0, 0, 0, 1, 1}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols3.dat"}},
  {{219, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 34, 185, 6, 26, 0, 14, 52256, 830, 151, 9, 0, 0, 0, true, false, 0, 4}, {1, 1, 1, 0, 0, 0, 1, 1, 1}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols4.dat"}},
  {{229, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 112, 117, 79, 32, 0, 14, 24030, 516, 182, 6, 0, 0, 0, true, false, 2, 4}, {0, 0, 0, 1, 1, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols5.dat"}},
  {{169, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 73, 96, 1, {}, 7, 7, 47469, 892, 732, 3, 0, 0, 0, false, false, 0, 4}, {0, 1, 1}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols6.dat"}},
  {{256, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 59, 197, 49, {}, 7, 7, 5547, 373, 452, 4, 0, 0, 0, false, false, 1, 4}, {0, 1, 0, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols7.dat"}},
  {{232, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 201, 31, 129, {}, 7, 7, 55529, 849, 752, 6, 0, 0, 0, false, false, 3, 4}, {0, 1, 0, 0, 1, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols8.dat"}},
  {{268, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 36, 232, 0, 6, 7, 7, 612, 230, 208, 7, 0, 0, 0, true, false, 0, 2}, {0, 1, 0, 1, 0, 0, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols9.dat"}},
  {{269, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 81, 188, 67, 34, 7, 7, 64683, 188, 711, 12, 0, 0, 0, true, false, 1, 2}, {1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols10.dat"}},
  {{217, 14, {std::nullopt, {0, 0}, cyclic_prefix::NORMAL, {0,1,2,3,}, 72, 145, 69, 59, 7, 7, 59825, 354, 776, 6, 0, 0, 0, false, true, 0, 2}, {0, 0, 1, 1, 1, 0}, {}, {}, {}}, {"test_data/pucch_processor_format4_test_input_symbols11.dat"}},
    // clang-format on
};

} // namespace srsran
