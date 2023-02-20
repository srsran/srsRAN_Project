/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/span.h"
#include "srsgnb/ran/prach/prach_configuration.h"
#include "srsgnb/support/srsgnb_test.h"
#include <set>

using namespace srsran;

static void test_fr1_paired()
{
  for (unsigned prach_config_index = 0; prach_config_index != UINT8_MAX; ++prach_config_index) {
    prach_configuration config = prach_configuration_get(frequency_range::FR1, duplex_mode::FDD, prach_config_index);

    // Assert format.
    if (prach_config_index < 28) {
      TESTASSERT(config.format == preamble_format::FORMAT0);
    } else if (prach_config_index < 53) {
      TESTASSERT(config.format == preamble_format::FORMAT1);
    } else if (prach_config_index < 60) {
      TESTASSERT(config.format == preamble_format::FORMAT2);
    } else if (prach_config_index < 87) {
      TESTASSERT(config.format == preamble_format::FORMAT3);
    } else {
      TESTASSERT(config.format == PRACH_CONFIG_RESERVED.format);
    }

    // Assert x.
    if (prach_config_index < 28) {
      TESTASSERT_EQ(1U << (4U - std::min(4U, prach_config_index / 4U)), config.x);
    } else if (prach_config_index < 53) {
      TESTASSERT_EQ(1U << (4U - std::min(4U, (prach_config_index - 28U) / 4U)), config.x);
    } else if (prach_config_index < 56) {
      TESTASSERT_EQ(1U << (4U - (prach_config_index - 53U)), config.x);
    } else if (prach_config_index < 60) {
      TESTASSERT_EQ(1U << (1U - (prach_config_index - 56U) / 2), config.x);
    } else if (prach_config_index < 64) {
      TESTASSERT_EQ(16, config.x);
    } else if (prach_config_index < 67) {
      TESTASSERT_EQ(8, config.x);
    } else if (prach_config_index < 87) {
      TESTASSERT_EQ(1U << (2U - std::min(2U, (prach_config_index - 67U) / 4U)), config.x);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert y.
    if (prach_config_index < 16) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 28) {
      TESTASSERT_EQ(0, config.y);
    } else if (prach_config_index < 44) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 53) {
      TESTASSERT_EQ(0, config.y);
    } else if (prach_config_index < 55) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 60) {
      TESTASSERT_EQ(0, config.y);
    } else if (prach_config_index < 75) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 87) {
      TESTASSERT_EQ(0, config.y);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert subframe number.
    static const std::array<std::array<uint8_t, 1>, 4> repeat_sequence = {{{1}, {4}, {7}, {9}}};
    if (prach_config_index < 19) {
      TESTASSERT_EQ(span<const uint8_t>(repeat_sequence[prach_config_index % repeat_sequence.size()]),
                    span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 19 || prach_config_index == 47 || prach_config_index == 78) {
      TESTASSERT_EQ(span<const uint8_t>({1, 6}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 20 || prach_config_index == 48 || prach_config_index == 79) {
      TESTASSERT_EQ(span<const uint8_t>({2, 7}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 21 || prach_config_index == 49 || prach_config_index == 80) {
      TESTASSERT_EQ(span<const uint8_t>({3, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 22 || prach_config_index == 50 || prach_config_index == 81) {
      TESTASSERT_EQ(span<const uint8_t>({1, 4, 7}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 23 || prach_config_index == 51 || prach_config_index == 82) {
      TESTASSERT_EQ(span<const uint8_t>({2, 5, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 24 || prach_config_index == 52 || prach_config_index == 83) {
      TESTASSERT_EQ(span<const uint8_t>({3, 6, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 25 || prach_config_index == 84) {
      TESTASSERT_EQ(span<const uint8_t>({0, 2, 4, 6, 8}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 26 || prach_config_index == 85) {
      TESTASSERT_EQ(span<const uint8_t>({1, 3, 5, 7, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index == 27 || prach_config_index == 86) {
      TESTASSERT_EQ(span<const uint8_t>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 47) {
      TESTASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 28) % repeat_sequence.size()]),
                    span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 56) {
      TESTASSERT_EQ(span<const uint8_t>({1}), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 60) {
      const std::array<uint8_t, 1> subframes = {static_cast<uint8_t>(4 * ((prach_config_index - 56) % 2) + 1)};
      TESTASSERT_EQ(span<const uint8_t>(subframes), span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 67) {
      TESTASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 60) % repeat_sequence.size()]),
                    span<const uint8_t>(config.subframe));
    } else if (prach_config_index < 78) {
      TESTASSERT_EQ(span<const uint8_t>(repeat_sequence[(prach_config_index - 67) % repeat_sequence.size()]),
                    span<const uint8_t>(config.subframe));
    } else {
      TESTASSERT_EQ(span<const uint8_t>(PRACH_CONFIG_RESERVED.subframe), span<const uint8_t>(config.subframe));
    }

    // Assert starting symbol.
    if (prach_config_index < 78) {
      TESTASSERT_EQ(0, config.starting_symbol);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.starting_symbol, config.starting_symbol);
    }

    // Assert number of PRACH slots within a subframe.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);

    // Assert number of time-domain PRACH occasions within a PRACH slot.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);

    // Assert PRACH duration.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
  }
}

static void test_fr1_unpaired()
{
  for (unsigned prach_config_index = 0; prach_config_index != UINT8_MAX; ++prach_config_index) {
    prach_configuration config = prach_configuration_get(frequency_range::FR1, duplex_mode::TDD, prach_config_index);

    // Assert format.
    if (prach_config_index < 28) {
      TESTASSERT(config.format == preamble_format::FORMAT0);
    } else if (prach_config_index < 34) {
      TESTASSERT(config.format == preamble_format::FORMAT1);
    } else if (prach_config_index < 40) {
      TESTASSERT(config.format == preamble_format::FORMAT2);
    } else if (prach_config_index < 67) {
      TESTASSERT(config.format == preamble_format::FORMAT3);
    } else {
      TESTASSERT(config.format == PRACH_CONFIG_RESERVED.format);
    }

    // Assert x.
    static const std::set<unsigned> config_x_16 = {0, 28, 34, 40};
    static const std::set<unsigned> config_x_8  = {1, 29, 35, 41};
    static const std::set<unsigned> config_x_4  = {2, 30, 36, 42};
    static const std::set<unsigned> config_x_2  = {3, 4, 5, 6, 31, 32, 37, 38, 43, 44, 45, 46};
    if (config_x_16.count(prach_config_index)) {
      TESTASSERT_EQ(16, config.x);
    } else if (config_x_8.count(prach_config_index)) {
      TESTASSERT_EQ(8, config.x);
    } else if (config_x_4.count(prach_config_index)) {
      TESTASSERT_EQ(4, config.x);
    } else if (config_x_2.count(prach_config_index)) {
      TESTASSERT_EQ(2, config.x);
    } else if (prach_config_index < 67) {
      TESTASSERT_EQ(1, config.x);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert y.
    if (prach_config_index < 3) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 7) {
      TESTASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 28) {
      TESTASSERT_EQ(0, config.y);
    } else if (prach_config_index < 31) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 34) {
      TESTASSERT_EQ((prach_config_index - 1) % 2, config.y);
    } else if (prach_config_index < 36) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 40) {
      TESTASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 42) {
      TESTASSERT_EQ(1, config.y);
    } else if (prach_config_index < 47) {
      TESTASSERT_EQ((prach_config_index + 1) % 2, config.y);
    } else if (prach_config_index < 67) {
      TESTASSERT_EQ(0, config.y);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.x, config.x);
    }

    // Assert subframe number.
    static const std::set<unsigned> config_subframe_2 = {14, 54};
    static const std::set<unsigned> config_subframe_3 = {13, 53};
    static const std::set<unsigned> config_subframe_4 = {5, 6, 12, 45, 46, 52};
    static const std::set<unsigned> config_subframe_5 = {11, 51};
    static const std::set<unsigned> config_subframe_6 = {10, 34, 35, 36, 37, 38, 39, 50};
    static const std::set<unsigned> config_subframe_7 = {9, 28, 29, 30, 31, 32, 33, 49};
    static const std::set<unsigned> config_subframe_8 = {8, 48};
    static const std::set<unsigned> config_subframe_9 = {
        0,
        1,
        2,
        3,
        4,
        7,
        40,
        41,
        42,
        43,
        44,
        47,
    };
    static const std::set<unsigned> config_subframe_1_6       = {15, 16, 55, 56};
    static const std::set<unsigned> config_subframe_4_9       = {17, 57};
    static const std::set<unsigned> config_subframe_3_8       = {18, 58};
    static const std::set<unsigned> config_subframe_2_7       = {19, 59};
    static const std::set<unsigned> config_subframe_8_9       = {20, 60};
    static const std::set<unsigned> config_subframe_4_8_9     = {21, 61};
    static const std::set<unsigned> config_subframe_3_4_9     = {22, 62};
    static const std::set<unsigned> config_subframe_7_8_9     = {23, 63};
    static const std::set<unsigned> config_subframe_3_4_8_9   = {24, 64};
    static const std::set<unsigned> config_subframe_6_7_8_9   = {25};
    static const std::set<unsigned> config_subframe_1_4_6_9   = {26, 65};
    static const std::set<unsigned> config_subframe_1_3_5_7_9 = {27, 66};

    if (config_subframe_2.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({2}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({3}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({4}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_5.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({5}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_6.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({6}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_8.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({8}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_6.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({1, 6}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({4, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_8.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({3, 8}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_2_7.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({2, 7}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_8_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_4_8_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({4, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_4_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({3, 4, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_7_8_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_3_4_8_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({3, 4, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_6_7_8_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({6, 7, 8, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_4_6_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({1, 4, 6, 9}), span<const uint8_t>(config.subframe));
    } else if (config_subframe_1_3_5_7_9.count(prach_config_index)) {
      TESTASSERT_EQ(span<const uint8_t>({1, 3, 5, 7, 9}), span<const uint8_t>(config.subframe));
    } else {
      TESTASSERT_EQ(span<const uint8_t>({}), span<const uint8_t>(config.subframe));
    }

    // Assert starting symbol.
    static const std::set<unsigned> config_starting_symbol_7 = {16, 37, 38, 39, 56};
    if (config_starting_symbol_7.count(prach_config_index)) {
      TESTASSERT_EQ(7, config.starting_symbol);
    } else if (prach_config_index < 67) {
      TESTASSERT_EQ(0, config.starting_symbol);
    } else {
      TESTASSERT_EQ(PRACH_CONFIG_RESERVED.starting_symbol, config.starting_symbol);
    }

    // Assert number of PRACH slots within a subframe.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.nof_prach_slots_within_subframe, config.nof_prach_slots_within_subframe);

    // Assert number of time-domain PRACH occasions within a PRACH slot.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.nof_occasions_within_slot, config.nof_occasions_within_slot);

    // Assert PRACH duration.
    TESTASSERT_EQ(PRACH_CONFIG_RESERVED.duration, config.duration);
  }
}

int main()
{
  test_fr1_paired();
  test_fr1_unpaired();
  return 0;
}