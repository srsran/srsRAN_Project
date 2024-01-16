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

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsPDCCHEncoderUnittest.m"

#include "srsran/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  pdcch_encoder::config_t config;
  file_vector<uint8_t>    message;
  file_vector<uint8_t>    encoded;
};

static const std::vector<test_case_t> pdcch_encoder_test_data = {
    // clang-format off
  {{108, 53393}, {"test_data/pdcch_encoder_test_input0.dat"}, {"test_data/pdcch_encoder_test_output0.dat"}},
  {{108, 31809}, {"test_data/pdcch_encoder_test_input1.dat"}, {"test_data/pdcch_encoder_test_output1.dat"}},
  {{108, 50168}, {"test_data/pdcch_encoder_test_input2.dat"}, {"test_data/pdcch_encoder_test_output2.dat"}},
  {{108, 38355}, {"test_data/pdcch_encoder_test_input3.dat"}, {"test_data/pdcch_encoder_test_output3.dat"}},
  {{108, 53562}, {"test_data/pdcch_encoder_test_input4.dat"}, {"test_data/pdcch_encoder_test_output4.dat"}},
  {{216, 24149}, {"test_data/pdcch_encoder_test_input5.dat"}, {"test_data/pdcch_encoder_test_output5.dat"}},
  {{216, 52090}, {"test_data/pdcch_encoder_test_input6.dat"}, {"test_data/pdcch_encoder_test_output6.dat"}},
  {{216, 16912}, {"test_data/pdcch_encoder_test_input7.dat"}, {"test_data/pdcch_encoder_test_output7.dat"}},
  {{216, 8949}, {"test_data/pdcch_encoder_test_input8.dat"}, {"test_data/pdcch_encoder_test_output8.dat"}},
  {{216, 43227}, {"test_data/pdcch_encoder_test_input9.dat"}, {"test_data/pdcch_encoder_test_output9.dat"}},
  {{216, 24659}, {"test_data/pdcch_encoder_test_input10.dat"}, {"test_data/pdcch_encoder_test_output10.dat"}},
  {{432, 38158}, {"test_data/pdcch_encoder_test_input11.dat"}, {"test_data/pdcch_encoder_test_output11.dat"}},
  {{432, 47254}, {"test_data/pdcch_encoder_test_input12.dat"}, {"test_data/pdcch_encoder_test_output12.dat"}},
  {{432, 38410}, {"test_data/pdcch_encoder_test_input13.dat"}, {"test_data/pdcch_encoder_test_output13.dat"}},
  {{432, 38536}, {"test_data/pdcch_encoder_test_input14.dat"}, {"test_data/pdcch_encoder_test_output14.dat"}},
  {{432, 5471}, {"test_data/pdcch_encoder_test_input15.dat"}, {"test_data/pdcch_encoder_test_output15.dat"}},
  {{432, 56291}, {"test_data/pdcch_encoder_test_input16.dat"}, {"test_data/pdcch_encoder_test_output16.dat"}},
  {{864, 56815}, {"test_data/pdcch_encoder_test_input17.dat"}, {"test_data/pdcch_encoder_test_output17.dat"}},
  {{864, 36161}, {"test_data/pdcch_encoder_test_input18.dat"}, {"test_data/pdcch_encoder_test_output18.dat"}},
  {{864, 50394}, {"test_data/pdcch_encoder_test_input19.dat"}, {"test_data/pdcch_encoder_test_output19.dat"}},
  {{864, 36555}, {"test_data/pdcch_encoder_test_input20.dat"}, {"test_data/pdcch_encoder_test_output20.dat"}},
  {{864, 15759}, {"test_data/pdcch_encoder_test_input21.dat"}, {"test_data/pdcch_encoder_test_output21.dat"}},
  {{864, 56803}, {"test_data/pdcch_encoder_test_input22.dat"}, {"test_data/pdcch_encoder_test_output22.dat"}},
  {{1728, 36522}, {"test_data/pdcch_encoder_test_input23.dat"}, {"test_data/pdcch_encoder_test_output23.dat"}},
  {{1728, 28293}, {"test_data/pdcch_encoder_test_input24.dat"}, {"test_data/pdcch_encoder_test_output24.dat"}},
  {{1728, 32266}, {"test_data/pdcch_encoder_test_input25.dat"}, {"test_data/pdcch_encoder_test_output25.dat"}},
  {{1728, 38432}, {"test_data/pdcch_encoder_test_input26.dat"}, {"test_data/pdcch_encoder_test_output26.dat"}},
  {{1728, 64538}, {"test_data/pdcch_encoder_test_input27.dat"}, {"test_data/pdcch_encoder_test_output27.dat"}},
  {{1728, 51407}, {"test_data/pdcch_encoder_test_input28.dat"}, {"test_data/pdcch_encoder_test_output28.dat"}},
    // clang-format on
};

} // namespace srsran
