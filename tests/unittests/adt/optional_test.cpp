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

#include "srsran/adt/optional.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_int()
{
  return std::uniform_int_distribution<int>{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}(g);
}

template <typename T>
class any_type_optional_tester : public ::testing::Test
{
public:
  using value_type = T;

  ~any_type_optional_tester() { TESTASSERT(moveonly_test_object::object_count() == 0); }
};

using any_types = ::testing::Types<int, moveonly_test_object>;
TYPED_TEST_SUITE(any_type_optional_tester, any_types);

TYPED_TEST(any_type_optional_tester, optional_has_same_triviality_traits_as_value_type)
{
  using T = typename TestFixture::value_type;

  static_assert(std::is_trivially_destructible<optional<T>>::value == std::is_trivially_destructible<T>::value,
                "optional<T> trait is incorrect");
  static_assert(std::is_trivially_copy_constructible<optional<T>>::value ==
                    std::is_trivially_copy_constructible<T>::value,
                "optional<T> trait is incorrect");
  static_assert(std::is_trivially_move_constructible<optional<T>>::value ==
                    std::is_trivially_move_constructible<T>::value,
                "optional<T> trait is incorrect");
  static_assert(std::is_trivially_copy_assignable<optional<T>>::value == std::is_trivially_copy_assignable<T>::value,
                "optional<T> trait is incorrect");
  static_assert(std::is_trivially_move_assignable<optional<T>>::value == std::is_trivially_move_assignable<T>::value,
                "optional<T> trait is incorrect");
}

TYPED_TEST(any_type_optional_tester, default_ctor_creates_empty_optional)
{
  using T = typename TestFixture::value_type;
  optional<T> val;
  ASSERT_FALSE(val.has_value());
}

TYPED_TEST(any_type_optional_tester, value_ctor_sets_optional_value)
{
  using T       = typename TestFixture::value_type;
  int         v = get_random_int();
  optional<T> val(v);
  ASSERT_TRUE(val.has_value());
  ASSERT_TRUE(static_cast<bool>(val));
  ASSERT_EQ(val.value(), v);
  ASSERT_EQ(*val, v);
}

TYPED_TEST(any_type_optional_tester, value_assign_sets_optional_value)
{
  using T = typename TestFixture::value_type;
  optional<T> val;

  int v = get_random_int();
  val   = v;
  ASSERT_TRUE(val.has_value());
  ASSERT_EQ(val.value(), v);
}

TYPED_TEST(any_type_optional_tester, optional_operator_eq_compares_internal_values)
{
  using T = typename TestFixture::value_type;
  optional<T> opt1, opt2;

  // two empty optionals
  ASSERT_EQ(opt1, opt2);

  // one empty and the other not empty.
  int v = get_random_int();
  T   val(v);
  opt1 = v;
  ASSERT_EQ(opt1, val);
  ASSERT_EQ(val, opt1);
  ASSERT_NE(opt2, val);
  ASSERT_NE(val, opt2);
  ASSERT_NE(opt1, opt2);
  ASSERT_NE(opt2, opt1);

  // two equal optionals with values.
  opt2 = v;
  ASSERT_EQ(opt1, opt2);
  ASSERT_EQ(opt2, opt1);

  // two unequal optional with values.
  opt2 = v == std::numeric_limits<int>::max() ? 0 : v + 1;
  ASSERT_NE(opt1, opt2);
  ASSERT_NE(opt2, opt1);
  ASSERT_NE(opt2, val);
  ASSERT_NE(val, opt2);
}

TYPED_TEST(any_type_optional_tester, optional_move_calls_value_move)
{
  using T       = typename TestFixture::value_type;
  int         v = get_random_int();
  T           val(v);
  optional<T> opt1(v), opt2;

  opt2 = std::move(opt1);
  ASSERT_TRUE(opt1.has_value()) << "Moving from non-empty optional does not delete optional value";
  ASSERT_TRUE(opt2.has_value());
  ASSERT_EQ(opt2, val);
}

TYPED_TEST(any_type_optional_tester, optional_reset_sets_optional_empty)
{
  using T = typename TestFixture::value_type;
  optional<T> opt(get_random_int());

  opt.reset();
  ASSERT_FALSE(opt.has_value());

  opt.reset();
  ASSERT_FALSE(opt.has_value()) << "Failed to reset empty optional";
}

TYPED_TEST(any_type_optional_tester, optional_emplace_value)
{
  using T = typename TestFixture::value_type;
  optional<T> opt;
  int         v = get_random_int(), v2 = get_random_int();
  T           val(v), val2(v2);

  // write on empty optional.
  opt.emplace(v);
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(opt, val);

  // overwrite.
  opt.emplace(v2);
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(opt, val2);
}
