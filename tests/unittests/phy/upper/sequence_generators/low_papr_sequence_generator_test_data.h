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

// This file was generated using the following MATLAB class on 15-03-2024 (seed 0):
//   + "srsLowPAPRSequenceUnittest.m"

#include "srsran/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_context {
  unsigned u;
  unsigned v;
  unsigned n_cs;
  unsigned n_cs_max;
  unsigned M_zc;
};

struct test_case_t {
  test_context      context;
  file_vector<cf_t> sequence;
};

static const std::vector<test_case_t> low_papr_sequence_generator_test_data = {
    // clang-format off
  {{24, 0, 7, 8, 12}, {"test_data/low_papr_sequence_generator_test_output0.dat"}},
  {{3, 0, 10, 12, 12}, {"test_data/low_papr_sequence_generator_test_output1.dat"}},
  {{18, 0, 0, 8, 24}, {"test_data/low_papr_sequence_generator_test_output2.dat"}},
  {{8, 0, 6, 12, 24}, {"test_data/low_papr_sequence_generator_test_output3.dat"}},
  {{28, 0, 7, 8, 36}, {"test_data/low_papr_sequence_generator_test_output4.dat"}},
  {{4, 0, 11, 12, 36}, {"test_data/low_papr_sequence_generator_test_output5.dat"}},
  {{28, 0, 3, 8, 48}, {"test_data/low_papr_sequence_generator_test_output6.dat"}},
  {{24, 0, 1, 12, 48}, {"test_data/low_papr_sequence_generator_test_output7.dat"}},
  {{12, 0, 7, 8, 60}, {"test_data/low_papr_sequence_generator_test_output8.dat"}},
  {{23, 0, 11, 12, 60}, {"test_data/low_papr_sequence_generator_test_output9.dat"}},
  {{19, 0, 6, 8, 72}, {"test_data/low_papr_sequence_generator_test_output10.dat"}},
  {{28, 1, 9, 12, 72}, {"test_data/low_papr_sequence_generator_test_output11.dat"}},
  {{22, 0, 5, 8, 84}, {"test_data/low_papr_sequence_generator_test_output12.dat"}},
  {{5, 1, 0, 12, 84}, {"test_data/low_papr_sequence_generator_test_output13.dat"}},
  {{8, 0, 0, 8, 96}, {"test_data/low_papr_sequence_generator_test_output14.dat"}},
  {{24, 1, 3, 12, 96}, {"test_data/low_papr_sequence_generator_test_output15.dat"}},
  {{28, 0, 3, 8, 108}, {"test_data/low_papr_sequence_generator_test_output16.dat"}},
  {{11, 1, 9, 12, 108}, {"test_data/low_papr_sequence_generator_test_output17.dat"}},
  {{5, 0, 3, 8, 120}, {"test_data/low_papr_sequence_generator_test_output18.dat"}},
  {{19, 1, 9, 12, 120}, {"test_data/low_papr_sequence_generator_test_output19.dat"}},
  {{8, 1, 5, 8, 132}, {"test_data/low_papr_sequence_generator_test_output20.dat"}},
  {{4, 0, 5, 12, 132}, {"test_data/low_papr_sequence_generator_test_output21.dat"}},
  {{28, 0, 4, 8, 144}, {"test_data/low_papr_sequence_generator_test_output22.dat"}},
  {{6, 1, 3, 12, 144}, {"test_data/low_papr_sequence_generator_test_output23.dat"}},
  {{15, 1, 7, 8, 156}, {"test_data/low_papr_sequence_generator_test_output24.dat"}},
  {{28, 1, 1, 12, 156}, {"test_data/low_papr_sequence_generator_test_output25.dat"}},
  {{4, 0, 6, 8, 168}, {"test_data/low_papr_sequence_generator_test_output26.dat"}},
  {{7, 1, 2, 12, 168}, {"test_data/low_papr_sequence_generator_test_output27.dat"}},
  {{27, 0, 1, 8, 180}, {"test_data/low_papr_sequence_generator_test_output28.dat"}},
  {{7, 1, 5, 12, 180}, {"test_data/low_papr_sequence_generator_test_output29.dat"}},
  {{10, 1, 4, 8, 192}, {"test_data/low_papr_sequence_generator_test_output30.dat"}},
  {{16, 1, 3, 12, 192}, {"test_data/low_papr_sequence_generator_test_output31.dat"}},
  {{22, 1, 3, 8, 204}, {"test_data/low_papr_sequence_generator_test_output32.dat"}},
  {{17, 0, 0, 12, 204}, {"test_data/low_papr_sequence_generator_test_output33.dat"}},
  {{15, 1, 7, 8, 216}, {"test_data/low_papr_sequence_generator_test_output34.dat"}},
  {{3, 1, 5, 12, 216}, {"test_data/low_papr_sequence_generator_test_output35.dat"}},
  {{0, 0, 1, 8, 228}, {"test_data/low_papr_sequence_generator_test_output36.dat"}},
  {{23, 0, 6, 12, 228}, {"test_data/low_papr_sequence_generator_test_output37.dat"}},
  {{4, 1, 2, 8, 240}, {"test_data/low_papr_sequence_generator_test_output38.dat"}},
  {{19, 1, 8, 12, 240}, {"test_data/low_papr_sequence_generator_test_output39.dat"}},
  {{13, 0, 1, 8, 252}, {"test_data/low_papr_sequence_generator_test_output40.dat"}},
  {{27, 0, 9, 12, 252}, {"test_data/low_papr_sequence_generator_test_output41.dat"}},
  {{16, 1, 0, 8, 264}, {"test_data/low_papr_sequence_generator_test_output42.dat"}},
  {{13, 0, 11, 12, 264}, {"test_data/low_papr_sequence_generator_test_output43.dat"}},
  {{0, 1, 6, 8, 276}, {"test_data/low_papr_sequence_generator_test_output44.dat"}},
  {{26, 0, 4, 12, 276}, {"test_data/low_papr_sequence_generator_test_output45.dat"}},
  {{7, 1, 3, 8, 288}, {"test_data/low_papr_sequence_generator_test_output46.dat"}},
  {{27, 0, 3, 12, 288}, {"test_data/low_papr_sequence_generator_test_output47.dat"}},
  {{4, 0, 6, 8, 312}, {"test_data/low_papr_sequence_generator_test_output48.dat"}},
  {{17, 1, 1, 12, 312}, {"test_data/low_papr_sequence_generator_test_output49.dat"}},
  {{25, 1, 2, 8, 324}, {"test_data/low_papr_sequence_generator_test_output50.dat"}},
  {{15, 0, 0, 12, 324}, {"test_data/low_papr_sequence_generator_test_output51.dat"}},
  {{7, 0, 1, 8, 336}, {"test_data/low_papr_sequence_generator_test_output52.dat"}},
  {{7, 0, 0, 12, 336}, {"test_data/low_papr_sequence_generator_test_output53.dat"}},
  {{27, 1, 3, 8, 360}, {"test_data/low_papr_sequence_generator_test_output54.dat"}},
  {{14, 0, 10, 12, 360}, {"test_data/low_papr_sequence_generator_test_output55.dat"}},
  {{11, 0, 6, 8, 384}, {"test_data/low_papr_sequence_generator_test_output56.dat"}},
  {{11, 0, 4, 12, 384}, {"test_data/low_papr_sequence_generator_test_output57.dat"}},
  {{2, 0, 7, 8, 396}, {"test_data/low_papr_sequence_generator_test_output58.dat"}},
  {{28, 1, 0, 12, 396}, {"test_data/low_papr_sequence_generator_test_output59.dat"}},
  {{7, 0, 6, 8, 408}, {"test_data/low_papr_sequence_generator_test_output60.dat"}},
  {{0, 0, 2, 12, 408}, {"test_data/low_papr_sequence_generator_test_output61.dat"}},
  {{19, 1, 5, 8, 432}, {"test_data/low_papr_sequence_generator_test_output62.dat"}},
  {{13, 1, 3, 12, 432}, {"test_data/low_papr_sequence_generator_test_output63.dat"}},
  {{22, 0, 5, 8, 456}, {"test_data/low_papr_sequence_generator_test_output64.dat"}},
  {{5, 0, 7, 12, 456}, {"test_data/low_papr_sequence_generator_test_output65.dat"}},
  {{23, 0, 7, 8, 480}, {"test_data/low_papr_sequence_generator_test_output66.dat"}},
  {{23, 0, 5, 12, 480}, {"test_data/low_papr_sequence_generator_test_output67.dat"}},
  {{13, 0, 4, 8, 504}, {"test_data/low_papr_sequence_generator_test_output68.dat"}},
  {{15, 1, 9, 12, 504}, {"test_data/low_papr_sequence_generator_test_output69.dat"}},
  {{19, 0, 6, 8, 528}, {"test_data/low_papr_sequence_generator_test_output70.dat"}},
  {{15, 0, 11, 12, 528}, {"test_data/low_papr_sequence_generator_test_output71.dat"}},
  {{26, 1, 4, 8, 552}, {"test_data/low_papr_sequence_generator_test_output72.dat"}},
  {{17, 0, 3, 12, 552}, {"test_data/low_papr_sequence_generator_test_output73.dat"}},
  {{14, 0, 6, 8, 576}, {"test_data/low_papr_sequence_generator_test_output74.dat"}},
  {{5, 0, 2, 12, 576}, {"test_data/low_papr_sequence_generator_test_output75.dat"}},
  {{6, 0, 2, 8, 624}, {"test_data/low_papr_sequence_generator_test_output76.dat"}},
  {{27, 0, 2, 12, 624}, {"test_data/low_papr_sequence_generator_test_output77.dat"}},
  {{27, 1, 3, 8, 648}, {"test_data/low_papr_sequence_generator_test_output78.dat"}},
  {{3, 0, 4, 12, 648}, {"test_data/low_papr_sequence_generator_test_output79.dat"}},
  {{17, 0, 4, 8, 672}, {"test_data/low_papr_sequence_generator_test_output80.dat"}},
  {{21, 0, 1, 12, 672}, {"test_data/low_papr_sequence_generator_test_output81.dat"}},
  {{8, 0, 3, 8, 720}, {"test_data/low_papr_sequence_generator_test_output82.dat"}},
  {{15, 0, 3, 12, 720}, {"test_data/low_papr_sequence_generator_test_output83.dat"}},
  {{24, 0, 7, 8, 768}, {"test_data/low_papr_sequence_generator_test_output84.dat"}},
  {{21, 0, 6, 12, 768}, {"test_data/low_papr_sequence_generator_test_output85.dat"}},
  {{7, 0, 7, 8, 792}, {"test_data/low_papr_sequence_generator_test_output86.dat"}},
  {{16, 1, 2, 12, 792}, {"test_data/low_papr_sequence_generator_test_output87.dat"}},
  {{14, 1, 5, 8, 816}, {"test_data/low_papr_sequence_generator_test_output88.dat"}},
  {{11, 0, 11, 12, 816}, {"test_data/low_papr_sequence_generator_test_output89.dat"}},
  {{1, 1, 7, 8, 864}, {"test_data/low_papr_sequence_generator_test_output90.dat"}},
  {{23, 0, 3, 12, 864}, {"test_data/low_papr_sequence_generator_test_output91.dat"}},
  {{10, 1, 1, 8, 912}, {"test_data/low_papr_sequence_generator_test_output92.dat"}},
  {{21, 0, 7, 12, 912}, {"test_data/low_papr_sequence_generator_test_output93.dat"}},
  {{14, 1, 5, 8, 960}, {"test_data/low_papr_sequence_generator_test_output94.dat"}},
  {{27, 1, 4, 12, 960}, {"test_data/low_papr_sequence_generator_test_output95.dat"}},
  {{20, 0, 0, 8, 1008}, {"test_data/low_papr_sequence_generator_test_output96.dat"}},
  {{22, 1, 5, 12, 1008}, {"test_data/low_papr_sequence_generator_test_output97.dat"}},
  {{27, 1, 4, 8, 1056}, {"test_data/low_papr_sequence_generator_test_output98.dat"}},
  {{25, 1, 6, 12, 1056}, {"test_data/low_papr_sequence_generator_test_output99.dat"}},
  {{5, 0, 7, 8, 1104}, {"test_data/low_papr_sequence_generator_test_output100.dat"}},
  {{0, 0, 2, 12, 1104}, {"test_data/low_papr_sequence_generator_test_output101.dat"}},
  {{29, 1, 4, 8, 1152}, {"test_data/low_papr_sequence_generator_test_output102.dat"}},
  {{14, 0, 8, 12, 1152}, {"test_data/low_papr_sequence_generator_test_output103.dat"}},
  {{1, 0, 4, 8, 1248}, {"test_data/low_papr_sequence_generator_test_output104.dat"}},
  {{2, 1, 9, 12, 1248}, {"test_data/low_papr_sequence_generator_test_output105.dat"}},
  {{21, 0, 5, 8, 1296}, {"test_data/low_papr_sequence_generator_test_output106.dat"}},
  {{15, 1, 7, 12, 1296}, {"test_data/low_papr_sequence_generator_test_output107.dat"}},
  {{24, 0, 3, 8, 1344}, {"test_data/low_papr_sequence_generator_test_output108.dat"}},
  {{24, 0, 1, 12, 1344}, {"test_data/low_papr_sequence_generator_test_output109.dat"}},
  {{5, 0, 6, 8, 1440}, {"test_data/low_papr_sequence_generator_test_output110.dat"}},
  {{24, 0, 4, 12, 1440}, {"test_data/low_papr_sequence_generator_test_output111.dat"}},
  {{15, 0, 5, 8, 1536}, {"test_data/low_papr_sequence_generator_test_output112.dat"}},
  {{18, 0, 5, 12, 1536}, {"test_data/low_papr_sequence_generator_test_output113.dat"}},
  {{0, 1, 1, 8, 1584}, {"test_data/low_papr_sequence_generator_test_output114.dat"}},
  {{3, 0, 2, 12, 1584}, {"test_data/low_papr_sequence_generator_test_output115.dat"}},
  {{14, 0, 7, 8, 1632}, {"test_data/low_papr_sequence_generator_test_output116.dat"}},
  {{27, 0, 8, 12, 1632}, {"test_data/low_papr_sequence_generator_test_output117.dat"}},
    // clang-format on
};

} // namespace srsran
