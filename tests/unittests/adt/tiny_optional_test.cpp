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

#include "srsran/adt/tiny_optional.h"
#include <gtest/gtest.h>

using namespace srsran;

static_assert(sizeof(tiny_optional<int, 5>) == sizeof(int), "tiny_optional sizeof is invalid");
static_assert(alignof(tiny_optional<int, 5>) == alignof(int), "tiny_optional alignof is invalid");
static_assert(std::is_trivially_destructible<tiny_optional<int, 5>>::value, "tiny_optional trait is invalid");
static_assert(std::is_trivially_copyable<tiny_optional<int, 5>>::value, "tiny_optional trait is invalid");
static_assert(std::is_trivially_move_constructible<tiny_optional<int, 5>>::value, "tiny_optional trait is invalid");
static_assert(std::is_trivially_move_assignable<tiny_optional<int, 5>>::value, "tiny_optional trait is invalid");

static_assert(sizeof(tiny_optional<int>) > sizeof(int), "tiny_optional sizeof is invalid");
static_assert(std::is_trivially_destructible<tiny_optional<int>>::value, "tiny_optional trait is invalid");

static_assert(sizeof(tiny_optional<std::unique_ptr<int>>) == sizeof(std::unique_ptr<int>),
              "tiny_optional sizeof is invalid");
static_assert(alignof(tiny_optional<std::unique_ptr<int>>) == alignof(std::unique_ptr<int>),
              "tiny_optional alignof is invalid");
static_assert(not std::is_trivially_destructible<std::unique_ptr<int>>::value, "tiny_optional trait is invalid");

TEST(tiny_optional_int, default_constructed_tiny_optional_has_no_value)
{
  tiny_optional<int, 2> opt;
  ASSERT_FALSE(opt.has_value());
}

TEST(tiny_optional_int_no_absent_value, default_constructed_tiny_optional_has_no_value)
{
  tiny_optional<int> opt;
  ASSERT_FALSE(opt.has_value());
}

TEST(tiny_optional_ptr, default_constructed_tiny_optional_has_no_value)
{
  tiny_optional<std::unique_ptr<int>> opt;
  ASSERT_FALSE(opt.has_value());
}

TEST(tiny_optional_int, assignment_sets_value)
{
  tiny_optional<int, 2> opt;
  opt = 1;
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(opt.value(), 1);
  ASSERT_EQ(*opt, 1);
}

TEST(tiny_optional_int_with_no_absent_value, assignment_sets_value)
{
  tiny_optional<int> opt;
  opt = 1;
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(opt.value(), 1);
  ASSERT_EQ(*opt, 1);
}

TEST(tiny_optional_ptr, assignment_sets_value)
{
  tiny_optional<std::unique_ptr<int>> opt;
  opt = std::make_unique<int>(1);
  ASSERT_TRUE(opt.has_value());
  ASSERT_EQ(*opt.value(), 1);
  ASSERT_EQ(**opt, 1);
}

TEST(tiny_optional_int, reset_erases_value)
{
  tiny_optional<int, 5> opt = 2;
  ASSERT_TRUE(opt.has_value());
  opt.reset();
  ASSERT_FALSE(opt.has_value());
}

TEST(tiny_optional_ptr, reset_erases_value)
{
  tiny_optional<std::unique_ptr<int>> opt = std::make_unique<int>(6);
  ASSERT_TRUE(opt.has_value());
  opt.reset();
  ASSERT_FALSE(opt.has_value());
}
