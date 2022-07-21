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

using namespace srsgnb;

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
  return 0;
}