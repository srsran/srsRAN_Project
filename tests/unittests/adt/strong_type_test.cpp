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

#include "srsran/adt/strong_type.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(strong_type_test, construction)
{
  using strong_int = strong_type<int, struct strong_int_tag>;

  strong_int a{3};
  ASSERT_EQ(a.value(), 3);
}

TEST(strong_type_test, equality)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_equality>;

  strong_int a{3};
  strong_int b{4};
  ASSERT_NE(a, b);

  strong_int c{3};
  strong_int d{3};
  ASSERT_EQ(c, d);
}

TEST(strong_type_test, equality_with_type)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_equality_with<int>>;

  strong_int a{3};
  ASSERT_EQ(a, 3);

  strong_int b{3};
  ASSERT_NE(b, 4);
}

TEST(strong_type_test, comparison)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_comparison>;

  strong_int a{3};
  strong_int b{4};
  ASSERT_LT(a, b);
  ASSERT_GT(b, a);
  ASSERT_LE(a, b);
  ASSERT_GE(b, a);
}

TEST(strong_type_test, comparison_with_type)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_comparison_with<int>>;

  strong_int a{3};
  ASSERT_LT(a, 4);
  ASSERT_LT(2, a);
  ASSERT_GT(a, 2);
  ASSERT_GT(4, a);
  ASSERT_LE(a, 3);
  ASSERT_LE(3, a);
  ASSERT_GE(a, 3);
  ASSERT_GE(3, a);
}

TEST(strong_type_test, incr_decr)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_increment_decrement>;

  strong_int a{3};
  ++a;
  ASSERT_EQ(a.value(), 4);
  ASSERT_EQ(a++.value(), 4);
  ASSERT_EQ(a.value(), 5);
  --a;
  ASSERT_EQ(a.value(), 4);
  ASSERT_EQ(a--.value(), 4);
  ASSERT_EQ(a.value(), 3);
}

TEST(strong_type_test, arithmetic)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_arithmetic>;

  strong_int a{3};
  strong_int b{4};
  ASSERT_EQ((a + b).value(), 7);
  ASSERT_EQ((a - b).value(), -1);
  ASSERT_EQ((a * b).value(), 12);
  ASSERT_EQ((a / b).value(), 0);
}

TEST(strong_type_test, arithmetic_with_underlying_type)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_arithmetic_with_underlying_type>;

  strong_int a{3};
  ASSERT_EQ((a + 1).value(), 4);
  ASSERT_EQ((a - 1).value(), 2);
  ASSERT_EQ((a * 3).value(), 9);
  ASSERT_EQ((a / 3).value(), 1);
}

TEST(strong_type_test, bitwise)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_bitwise>;

  strong_int a{2};
  strong_int b{1};
  ASSERT_EQ((a | b).value(), 3);
  ASSERT_EQ((a & b).value(), 0);
  ASSERT_EQ((a ^ b).value(), 3);
  ASSERT_EQ((a >> 1).value(), 1);
  ASSERT_EQ((a << 1).value(), 4);
}

TEST(strong_type_test, conversion)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_conversion_to<unsigned>>;

  strong_int a{2};
  ASSERT_EQ(static_cast<unsigned>(a), 2);
}

TEST(strong_type_test, multiplication_with_type)
{
  using strong_int = strong_type<int, struct strong_int_tag, strong_multiplication_with<int>>;

  strong_int a{3};
  ASSERT_EQ((a * 3).value(), 9);
  ASSERT_EQ((3 * a).value(), 9);
  a *= 2;
  ASSERT_EQ(a.value(), 6);
}
