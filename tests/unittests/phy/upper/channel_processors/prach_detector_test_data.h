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

// This file was generated using the following MATLAB class on 17-03-2023:
//   + "srsPRACHDetectorUnittest.m"

#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct context_t {
  prach_detector::configuration config;
  prach_detection_result        result;
};
struct test_case_t {
  context_t         context;
  file_vector<cf_t> symbols;
};

static const std::vector<test_case_t> prach_detector_test_data = {
    // clang-format off
  {{{834, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{57, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output0.dat"}},
  {{{130, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{58, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output1.dat"}},
  {{{647, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{6, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output2.dat"}},
  {{{285, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{35, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output3.dat"}},
  {{{980, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{61, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output4.dat"}},
  {{{161, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{62, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output5.dat"}},
  {{{980, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{31, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output6.dat"}},
  {{{819, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{9, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output7.dat"}},
  {{{431, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{58, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output8.dat"}},
  {{{811, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{61, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output9.dat"}},
  {{{671, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{2, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output10.dat"}},
  {{{869, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{59, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output11.dat"}},
  {{{695, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{48, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output12.dat"}},
  {{{760, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{25, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output13.dat"}},
  {{{671, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{10, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output14.dat"}},
  {{{722, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{2, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output15.dat"}},
  {{{283, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{2, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output16.dat"}},
  {{{99, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{52, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output17.dat"}},
  {{{711, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{20, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output18.dat"}},
  {{{973, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{2, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output19.dat"}},
  {{{449, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{24, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output20.dat"}},
  {{{783, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{50, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output21.dat"}},
  {{{191, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{31, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output22.dat"}},
  {{{456, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{41, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output23.dat"}},
  {{{726, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{48, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output24.dat"}},
  {{{282, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{43, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output25.dat"}},
  {{{670, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{10, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output26.dat"}},
  {{{121, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{31, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output27.dat"}},
  {{{982, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{21, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output28.dat"}},
  {{{599, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{14, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output29.dat"}},
  {{{769, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{16, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output30.dat"}},
  {{{518, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{44, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output31.dat"}},
  {{{912, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{61, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output32.dat"}},
  {{{560, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{8, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output33.dat"}},
  {{{152, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{16, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output34.dat"}},
  {{{860, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{16, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output35.dat"}},
  {{{833, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{15, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output36.dat"}},
  {{{951, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{22, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output37.dat"}},
  {{{201, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{16, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output38.dat"}},
  {{{630, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{30, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output39.dat"}},
  {{{360, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{53, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output40.dat"}},
  {{{599, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{35, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output41.dat"}},
  {{{939, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{18, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output42.dat"}},
  {{{775, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{48, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output43.dat"}},
  {{{389, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{36, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output44.dat"}},
  {{{77, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{3, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output45.dat"}},
  {{{543, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{49, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output46.dat"}},
  {{{956, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{8, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output47.dat"}},
  {{{582, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{30, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output48.dat"}},
  {{{12, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{21, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output49.dat"}},
  {{{166, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{50, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output50.dat"}},
  {{{318, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{33, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output51.dat"}},
  {{{169, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{38, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output52.dat"}},
  {{{269, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{41, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output53.dat"}},
  {{{705, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{47, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output54.dat"}},
  {{{461, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{5, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output55.dat"}},
  {{{234, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{58, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output56.dat"}},
  {{{156, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{52, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output57.dat"}},
  {{{551, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{63, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output58.dat"}},
  {{{80, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{28, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output59.dat"}},
  {{{109, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{61, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output60.dat"}},
  {{{4, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{49, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output61.dat"}},
  {{{836, to_prach_format_type("0"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{55, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output62.dat"}},
  {{{86, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{25, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output63.dat"}},
  {{{266, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{51, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output64.dat"}},
  {{{441, to_prach_format_type("1"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{58, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output65.dat"}},
  {{{186, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{16, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output66.dat"}},
  {{{149, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{8, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output67.dat"}},
  {{{890, to_prach_format_type("2"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{37, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output68.dat"}},
  {{{563, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{9, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output69.dat"}},
  {{{873, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{39, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output70.dat"}},
  {{{359, to_prach_format_type("3"), restricted_set_config::UNRESTRICTED, 0, 0, 64}, {0, phy_time_unit::from_seconds(0.0), phy_time_unit::from_seconds(0.0), {{32, phy_time_unit::from_seconds(0.0), 0, 0}}}}, {"test_data/prach_detector_test_output71.dat"}},
    // clang-format on
};

} // namespace srsran
