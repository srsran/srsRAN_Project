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

// This file was generated using the following MATLAB class:
//   + "srsPBCHdmrsUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  dmrs_pbch_processor::config_t                           config;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> dmrs_pbch_processor_test_data = {
    // clang-format off
  {{300, 0, 4, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output0.dat"}},
  {{896, 0, 4, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output1.dat"}},
  {{60, 0, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output2.dat"}},
  {{238, 0, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output3.dat"}},
  {{667, 0, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output4.dat"}},
  {{759, 0, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output5.dat"}},
  {{879, 1, 4, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output6.dat"}},
  {{303, 1, 4, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output7.dat"}},
  {{211, 1, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output8.dat"}},
  {{427, 1, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output9.dat"}},
  {{849, 1, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output10.dat"}},
  {{28, 1, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output11.dat"}},
  {{297, 2, 4, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output12.dat"}},
  {{54, 2, 4, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output13.dat"}},
  {{334, 2, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output14.dat"}},
  {{411, 2, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output15.dat"}},
  {{796, 2, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output16.dat"}},
  {{607, 2, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output17.dat"}},
  {{371, 3, 4, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output18.dat"}},
  {{794, 3, 4, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output19.dat"}},
  {{540, 3, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output20.dat"}},
  {{688, 3, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output21.dat"}},
  {{30, 3, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output22.dat"}},
  {{578, 3, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output23.dat"}},
  {{419, 4, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output24.dat"}},
  {{125, 4, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output25.dat"}},
  {{691, 4, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output26.dat"}},
  {{410, 4, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output27.dat"}},
  {{856, 6, 8, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output28.dat"}},
  {{71, 6, 8, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output29.dat"}},
  {{347, 6, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output30.dat"}},
  {{67, 6, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output31.dat"}},
  {{851, 16, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output32.dat"}},
  {{17, 16, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output33.dat"}},
  {{931, 32, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output34.dat"}},
  {{913, 32, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output35.dat"}},
  {{473, 48, 64, 0, 0, 0, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output36.dat"}},
  {{553, 48, 64, 0, 0, 1, 1, {0}}, {"test_data/dmrs_pbch_processor_test_output37.dat"}},
    // clang-format on
};

} // namespace srsran
