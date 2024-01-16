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

#include "srsran/support/units.h"
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
