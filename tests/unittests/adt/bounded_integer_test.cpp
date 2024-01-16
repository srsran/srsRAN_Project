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

#include "srsran/adt/bounded_integer.h"
#include "srsran/support/test_utils.h"
#include <cstdint>
#include <gtest/gtest.h>

using namespace srsran;

// Constexpr checks.
static_assert(bounded_integer<int, 2, 5>(3) == 3, "Constexpr ctor failed");
static_assert(bounded_integer<int, 2, 5>::min() == 2, "Constexpr max() failed");
static_assert(bounded_integer<int, 2, 5>::max() == 5, "Constexpr max() failed");
static_assert(bounded_integer<int, 2, 5>(2) == bounded_integer<int, 2, 5>(2), "Constexpr comparison failed");

TEST(bounded_integer_test, valid_method)
{
  bounded_integer<unsigned, 5, 1000> val(test_rgen::uniform_int<unsigned>(5, 1000));
  ASSERT_TRUE(val.valid());
}

TEST(bounded_integer_test, value_comparison)
{
  unsigned                         num = test_rgen::uniform_int<unsigned>(2, 10);
  bounded_integer<unsigned, 2, 10> val = num;
  ASSERT_EQ(val, num);
  ASSERT_EQ(num, val);
  unsigned num2 = num + 1;
  ASSERT_LT(val, num2);
  ASSERT_GT(num2, val);
}

TEST(bounded_integer_test, cast_to_integer)
{
  unsigned                        num = test_rgen::uniform_int<uint8_t>(2, 10);
  bounded_integer<uint8_t, 2, 10> val = num;

  TESTASSERT_EQ(num, static_cast<uint8_t>(val));
  TESTASSERT_EQ(num, val.to_uint());
}

TEST(bounded_integer_test, copy_ctor)
{
  bounded_integer<unsigned, 5, 1000> val(test_rgen::uniform_int<unsigned>(5, 1000));
  bounded_integer<unsigned, 5, 1000> val2{val};

  TESTASSERT_EQ(val, val2);
}

TEST(bounded_integer_test, copy_assignment)
{
  bounded_integer<int, -5, 1000> val(test_rgen::uniform_int<int>(-5, 1000));
  bounded_integer<int, -5, 1000> val2(test_rgen::uniform_int<int>(-5, 1000));

  val2 = val;
  TESTASSERT_EQ(val, val2);
}

TEST(bounded_integer_test, increment_decrement_operator)
{
  int                            num = test_rgen::uniform_int<int>(-4, 999);
  bounded_integer<int, -5, 1000> val(num);

  ASSERT_EQ(val, num);
  ASSERT_EQ(++val, num + 1);
  ASSERT_EQ(--val, num);
  ASSERT_EQ(val++, num);
  ASSERT_EQ(val--, num + 1);
  ASSERT_EQ(val, num);
}

TEST(bounded_integer_test, addition_subtraction_with_raw_integers)
{
  int                             num = test_rgen::uniform_int<int>(-5, 995);
  bounded_integer<int, -10, 1000> val(num);

  ASSERT_EQ(val, num);
  ASSERT_EQ(val + 5, num + 5);
  ASSERT_EQ(val - 5, num - 5);
  ASSERT_EQ(val, num);

  val += 3;
  num += 3;
  ASSERT_EQ(val, num);
  val -= 2;
  num -= 2;
  ASSERT_EQ(val, num);
}

TEST(bounded_integer_test, addition_subtraction_with_other_bounded_integers)
{
  int                               num  = test_rgen::uniform_int<int>(-500, 500);
  int                               num2 = test_rgen::uniform_int<int>(-500, 500);
  bounded_integer<int, -1000, 1000> val(num);
  bounded_integer<int, -1000, 1000> val2(num2);

  ASSERT_EQ(val + val2, num + num2);
  ASSERT_EQ(val - val2, num - num2);

  val += val2;
  ASSERT_EQ(val, num + num2);
  ASSERT_EQ(val2, num2);
  val -= val2;
  ASSERT_EQ(val, num);
  ASSERT_EQ(val2, num2);
}

TEST(bounded_integer_test, fmt_format)
{
  int                            num = test_rgen::uniform_int<int>(-5, 1000);
  bounded_integer<int, -5, 1000> val(num);

  std::string s = fmt::format("{}", val);
  ASSERT_EQ(s, std::to_string(num));
}

TEST(bounded_integer_test, fmt_format_when_out_of_bounds)
{
  bounded_integer<int, 4, 10> val{bounded_integer_invalid_tag{}};
  std::string                 s = fmt::format("{}", val);
  TESTASSERT_EQ("INVALID", s);
}
