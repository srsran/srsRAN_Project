/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/units.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(bit_units, basic)
{
  using namespace units::literals;

  units::bits a = 2_bits;
  ASSERT_EQ(a.value(), 2);
}

TEST(bit_units, conversion)
{
  using namespace units::literals;

  units::bits a = 10_bits;
  ASSERT_EQ(a.value(), 10);
  ASSERT_EQ(a.truncate_to_bytes().value(), 1);
  ASSERT_EQ(a.round_up_to_bytes().value(), 2);

  units::bits b = 8_bits;
  ASSERT_TRUE(b.is_byte_exact());
}

TEST(byte_units, basic)
{
  using namespace units::literals;

  units::bytes a = 2_bytes;

  // To bit translation methods.
  ASSERT_EQ(a.to_bits(), 16_bits);
  units::bits c = static_cast<units::bits>(a);
  ASSERT_EQ(c, 16_bits);
}
