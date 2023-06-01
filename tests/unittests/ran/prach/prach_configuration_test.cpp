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

#include "srsran/adt/span.h"
#include "srsran/ran/prach/prach_configuration.h"
#include <gtest/gtest.h>
#include <set>

using namespace srsran;

TEST(PrachConfiguration, Fr1Paired)
{
  for (unsigned prach_config_index = 0; prach_config_index != UINT8_MAX; ++prach_config_index) {
    prach_configuration config = prach_configuration_get(frequency_range::FR1, duplex_mode::FDD, prach_config_index);

    // Assert format.
    if (prach_config_index < 28) {
      ASSERT_EQ(config.format, prach_format_type::zero);
    } else if (prach_config_index < 53) {
      ASSERT_EQ(config.format, prach_format_type::one);
    } else if (prach_config_index < 60) {
      ASSERT_EQ(config.format, prach_format_type::two);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(config.format, prach_format_type::three);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(config.format, prach_format_type::A1);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(config.format, PRACH_CONFIG_RESERVED.format);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(config.format, prach_format_type::B4);
    } else {
      ASSERT_EQ(config.format, PRACH_CONFIG_RESERVED.format);
    }

    // Assert x.
    if (prach_config_index < 28) {
      ASSERT_EQ(1U << (4U - std::min(4U, prach_config_index / 4U)), config.x);
    } else if (prach_config_index < 53) {
      ASSERT_EQ(1U << (4U - std::min(4U, (prach_config_index - 28U) / 4U)), config.x);
    } else if (prach_config_index < 56) {
      ASSERT_EQ(1U << (4U - (prach_config_index - 53U)), config.x);
    } else if (prach_config_index < 60) {
      ASSERT_EQ(1U << (1U - (prach_config_index - 56U) / 2), config.x);
    } else if (prach_config_index < 64) {
      ASSERT_EQ(16, config.x);
    } else if (prach_config_index < 67) {
      ASSERT_EQ(8, config.x);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(1U << (2U - std::min(2U, (prach_config_index - 67U) / 4U)), config.x);
    } else if (prach_config_index < 89) {
      ASSERT_EQ(16U, config.x);
    } else if (prach_config_index < 91) {
      ASSERT_EQ(8U, config.x);
    } else if (prach_config_index < 94) {
      ASSERT_EQ(4U, config.x);
    } else if (prach_config_index < 98) {
      ASSERT_EQ(2U, config.x);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(1U, config.x);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    } else if (prach_config_index < 200) {
      ASSERT_EQ(16U, config.x);
    } else if (prach_config_index < 202) {
      ASSERT_EQ(8U, config.x);
    } else if (prach_config_index < 205) {
      ASSERT_EQ(4U, config.x);
    } else if (prach_config_index < 209) {
      ASSERT_EQ(2U, config.x);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(1U, config.x);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert y.
    static const std::set<unsigned> config_B4_y_1 = {199, 201, 204};
    if (prach_config_index < 16) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 28) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 44) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 53) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 55) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 60) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 75) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 93) {
      ASSERT_EQ((prach_config_index - 87) % 2, config.y);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.y, config.y);
    } else if (config_B4_y_1.count(prach_config_index) > 0) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(0, config.y);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert subframe number.
    static const std::set<unsigned> config_subframe_4_9 = {87, 89, 91, 92, 94, 100, 198, 200, 202, 204, 205, 214};
    static const std::set<unsigned> config_subframe_4   = {
          88,
          90,
          93,
          96,
          98,
          199,
          201,
          203,
          207,
          210,
    };
    static const std::set<unsigned> config_subframe_1     = {95, 101, 206, 209};
    static const std::set<unsigned> config_subframe_7     = {97, 102, 208, 211};
    static const std::set<unsigned> config_subframe_1_6   = {99, 212};
    static const std::set<unsigned> config_subframe_2_7   = {103, 213};
    static const std::set<unsigned> config_subframe_1_4_7 = {104, 215};
    static const std::set<unsigned> config_subframe_even  = {105, 216};
    static const std::set<unsigned> config_subframe_all   = {106, 217};
    static const std::set<unsigned> config_subframe_odd   = {107, 218};

    static const std::array<std::array<uint8_t, 1>, 4> repeat_sequence = {{{1}, {4}, {7}, {9}}};
    if (prach_config_index < 19) {
      ASSERT_EQ(span<const uint8_t>(repeat_sequence[prach_config_index % repeat_sequence.size()]),
                span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 19 || prach_config_index == 47 || prach_config_index == 78) {
      ASSERT_EQ(span<const uint8_t>({1, 6}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 20 || prach_config_index == 48 || prach_config_index == 79) {
      ASSERT_EQ(span<const uint8_t>({2, 7}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 21 || prach_config_index == 49 || prach_config_index == 80) {
      ASSERT_EQ(span<const uint8_t>({3, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 22 || prach_config_index == 50 || prach_config_index == 81) {
      ASSERT_EQ(span<const uint8_t>({1, 4, 7}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 23 || prach_config_index == 51 || prach_config_index == 82) {
      ASSERT_EQ(span<const uint8_t>({2, 5, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 24 || prach_config_index == 52 || prach_config_index == 83) {
      ASSERT_EQ(span<const uint8_t>({3, 6, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 25 || prach_config_index == 84) {
      ASSERT_EQ(span<const uint8_t>({0, 2, 4, 6, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 26 || prach_config_index == 85) {
      ASSERT_EQ(span<const uint8_t>({1, 3, 5, 7, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 27 || prach_config_index == 86) {
      ASSERT_EQ(span<const uint8_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 47) {
      ASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 28) % repeat_sequence.size()]),
                span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 56) {
      ASSERT_EQ(span<const uint8_t>({1}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 60) {
      const std::array<uint8_t, 1> subframes = {static_cast<uint8_t>(4 * ((prach_config_index - 56) % 2) + 1)};
      ASSERT_EQ(span<const uint8_t>(subframes), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 67) {
      ASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 60) % repeat_sequence.size()]),
                span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 78) {
      ASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 67) % repeat_sequence.size()]),
                span<const uint8_t>(config.subframe));
    } else if (config_subframe_4_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({4, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({4}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_6.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 6}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_2_7.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({2, 7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_4_7.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 4, 7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_even.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({0, 2, 4, 6, 8}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_odd.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 3, 5, 7, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_all.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else {
      ASSERT_EQ(span<const uint8_t>(PRACH_CONFIG_RESERVED.subframe), span<const uint8_t>(config.subframe));
    }

    // Assert starting symbol.
    if (prach_config_index < 108) {
      ASSERT_EQ(0, config.starting_symbol);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.starting_symbol, config.starting_symbol);
    }

    // Assert number of PRACH slots within a subframe.
    static const std::set<unsigned> config_slots_1 = {87, 89, 91, 92, 94, 98, 99, 100};
    if (prach_config_index < 87) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    } else if (config_slots_1.count(prach_config_index)) {
      ASSERT_EQ(1, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(2, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(2, config.nof_prach_slots_within_subframe);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    }

    // Assert number of time-domain PRACH occasions within a PRACH slot.
    if (prach_config_index < 87) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(6, config.nof_occasions_within_slot);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(1, config.nof_occasions_within_slot);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    }

    // Assert PRACH duration.
    if (prach_config_index < 87) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    } else if (prach_config_index < 108) {
      ASSERT_EQ(2, config.duration);
    } else if (prach_config_index < 198) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    } else if (prach_config_index < 219) {
      ASSERT_EQ(12, config.duration);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    }
  }
}

TEST(PrachConfiguration, Fr1Unpaired)
{
  for (unsigned prach_config_index = 0; prach_config_index != UINT8_MAX; ++prach_config_index) {
    prach_configuration config = prach_configuration_get(frequency_range::FR1, duplex_mode::TDD, prach_config_index);

    // Assert format.
    if (prach_config_index < 28) {
      ASSERT_EQ(config.format, prach_format_type::zero);
    } else if (prach_config_index < 34) {
      ASSERT_EQ(config.format, prach_format_type::one);
    } else if (prach_config_index < 40) {
      ASSERT_EQ(config.format, prach_format_type::two);
    } else if (prach_config_index < 67) {
      ASSERT_EQ(config.format, prach_format_type::three);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(config.format, prach_format_type::A1);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(config.format, PRACH_CONFIG_RESERVED.format);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(config.format, prach_format_type::B4);
    } else {
      ASSERT_EQ(config.format, PRACH_CONFIG_RESERVED.format);
    }

    // Assert x.
    static const std::set<unsigned> config_x_16 = {0, 28, 34, 40, 67, 145};
    static const std::set<unsigned> config_x_8  = {1, 29, 35, 41, 68, 146};
    static const std::set<unsigned> config_x_4  = {2, 30, 36, 42, 69, 147};
    static const std::set<unsigned> config_x_2  = {3, 4, 5, 6, 31, 32, 37, 38, 43, 44, 45, 46};
    if (config_x_16.count(prach_config_index)) {
      ASSERT_EQ(16, config.x);
    } else if (config_x_8.count(prach_config_index)) {
      ASSERT_EQ(8, config.x);
    } else if (config_x_4.count(prach_config_index)) {
      ASSERT_EQ(4, config.x);
    } else if (config_x_2.count(prach_config_index)) {
      ASSERT_EQ(2, config.x);
    } else if (prach_config_index < 67) {
      ASSERT_EQ(1, config.x);
    } else if (prach_config_index < 77) {
      ASSERT_EQ(2, config.x);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(1, config.x);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    } else if (prach_config_index < 155) {
      ASSERT_EQ(2, config.x);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(1, config.x);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert y.
    if (prach_config_index < 3) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 7) {
      ASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 28) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 31) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 34) {
      ASSERT_EQ((prach_config_index - 1) % 2, config.y);
    } else if (prach_config_index < 36) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 40) {
      ASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 42) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 47) {
      ASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 67) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 77) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(0, config.y);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.y, config.y);
    } else if (prach_config_index < 155) {
      ASSERT_EQ(1, config.y);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(0, config.y);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.y, config.y);
    }

    // Assert subframe number.
    static const std::set<unsigned> config_subframe_1 = {155};
    static const std::set<unsigned> config_subframe_2 = {14, 54, 156};
    static const std::set<unsigned> config_subframe_3 = {13, 53};
    static const std::set<unsigned> config_subframe_4 = {5, 6, 12, 45, 46, 52, 157};
    static const std::set<unsigned> config_subframe_5 = {11, 51};
    static const std::set<unsigned> config_subframe_6 = {10, 34, 35, 36, 37, 38, 39, 50};
    static const std::set<unsigned> config_subframe_7 = {9, 28, 29, 30, 31, 32, 33, 49, 158};
    static const std::set<unsigned> config_subframe_8 = {8, 48};
    static const std::set<unsigned> config_subframe_9 = {0,  1,  2,  3,  4,  7,   40,  41,  42,  43,  44,  47,  67, 68,
                                                         69, 70, 77, 78, 79, 145, 146, 147, 148, 149, 159, 160, 161};
    static const std::set<unsigned> config_subframe_1_6         = {15, 16, 55, 56};
    static const std::set<unsigned> config_subframe_4_9         = {17, 57, 71, 75, 81, 151, 152, 162};
    static const std::set<unsigned> config_subframe_3_8         = {18, 58};
    static const std::set<unsigned> config_subframe_2_7         = {19, 59};
    static const std::set<unsigned> config_subframe_7_9         = {72, 73, 82, 150, 163};
    static const std::set<unsigned> config_subframe_8_9         = {20, 60, 74, 80, 153, 164};
    static const std::set<unsigned> config_subframe_4_8_9       = {21, 61};
    static const std::set<unsigned> config_subframe_3_4_9       = {22, 62};
    static const std::set<unsigned> config_subframe_7_8_9       = {23, 63};
    static const std::set<unsigned> config_subframe_3_4_8_9     = {24, 64, 83, 84, 165};
    static const std::set<unsigned> config_subframe_6_7_8_9     = {25};
    static const std::set<unsigned> config_subframe_1_4_6_9     = {26, 65};
    static const std::set<unsigned> config_subframe_2_3_4_7_8_9 = {76, 154};
    static const std::set<unsigned> config_subframe_odd         = {27, 66, 85, 166};
    static const std::set<unsigned> config_subframe_all         = {86, 167, 168};

    if (config_subframe_1.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_2.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({2}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({3}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({4}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_5.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({5}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_6.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({6}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_8.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({8}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_6.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 6}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({4, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_8.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({3, 8}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_2_7.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({2, 7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({7, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({4, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_4_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({3, 4, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_4_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({3, 4, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_6_7_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_4_6_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 4, 6, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_2_3_4_7_8_9.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({2, 3, 4, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_odd.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({1, 3, 5, 7, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_all.count(prach_config_index)) {
      ASSERT_EQ(span<const uint8_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else {
      ASSERT_EQ(span<const uint8_t>({}), span<const uint8_t>(config.subframe));
    }

    // Assert starting symbol.
    static const std::set<unsigned> config_starting_symbol_2 = {147, 149, 150, 151, 160, 162, 163, 165, 166, 168};
    static const std::set<unsigned> config_starting_symbol_7 = {16, 37, 38, 39, 56, 71, 72, 78, 82, 86};
    if (config_starting_symbol_2.count(prach_config_index)) {
      ASSERT_EQ(2, config.starting_symbol);
    } else if (config_starting_symbol_7.count(prach_config_index)) {
      ASSERT_EQ(7, config.starting_symbol);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(0, config.starting_symbol);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.starting_symbol, config.starting_symbol);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(0, config.starting_symbol);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.starting_symbol, config.starting_symbol);
    }

    // Assert number of PRACH slots within a subframe.
    static const std::set<unsigned> config_slots_2 = {67, 68, 74, 75, 77, 80, 84, 145, 146, 152, 153, 161, 164, 167};
    if (prach_config_index < 67) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    } else if (config_slots_2.count(prach_config_index)) {
      ASSERT_EQ(2, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(1, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(1, config.nof_prach_slots_within_subframe);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);
    }

    // Assert number of time-domain PRACH occasions within a PRACH slot.
    static const std::set<unsigned> config_occasions_3 = {71, 72, 78, 82, 86};
    if (prach_config_index < 67) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    } else if (config_occasions_3.count(prach_config_index)) {
      ASSERT_EQ(3, config.nof_occasions_within_slot);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(6, config.nof_occasions_within_slot);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(1, config.nof_occasions_within_slot);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);
    }

    // Assert PRACH duration.
    if (prach_config_index < 67) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    } else if (prach_config_index < 87) {
      ASSERT_EQ(2, config.duration);
    } else if (prach_config_index < 145) {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    } else if (prach_config_index < 169) {
      ASSERT_EQ(12, config.duration);
    } else {
      ASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
    }
  }
}
