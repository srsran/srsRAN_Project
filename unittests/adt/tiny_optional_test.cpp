/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/tiny_optional.h"
#include <gtest/gtest.h>

using namespace srsgnb;

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
