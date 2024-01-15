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

#include "srsran/adt/interval.h"
#include <gtest/gtest.h>

using namespace srsran;

static_assert(not interval<int>::is_real::value, "Invalid type trait");
static_assert(not interval<unsigned>::is_real::value, "Invalid type trait");
static_assert(interval<float>::is_real::value, "Invalid type trait");
static_assert(interval<double>::is_real::value, "Invalid type trait");

TEST(right_open_integer_interval_test, default_ctor)
{
  interval<int> interv;
  EXPECT_EQ(interv.length(), 0);
  EXPECT_EQ(interv.start(), interv.stop());
  EXPECT_FALSE(interv.contains(0));
}

TEST(right_closed_integer_interval_test, default_ctor)
{
  interval<int, true> interv;
  EXPECT_EQ(interv.length(), 1);
  EXPECT_EQ(interv.start(), interv.stop());
}

TEST(right_open_integer_interval_test, interval_ctor)
{
  interval<int> interv{5, 10};
  EXPECT_EQ(interv.length(), 5);
  EXPECT_EQ(interv.start(), 5);
  EXPECT_EQ(interv.stop(), 10);
  EXPECT_FALSE(interv.contains(0));
  EXPECT_TRUE(interv.contains(5));
  EXPECT_TRUE(interv.contains(9));
  EXPECT_FALSE(interv.contains(10));
}

TEST(right_closed_integer_interval_test, interval_ctor)
{
  interval<int, true> interv{5, 10};
  EXPECT_EQ(interv.length(), 6);
  EXPECT_EQ(interv.start(), 5);
  EXPECT_EQ(interv.stop(), 10);
  EXPECT_FALSE(interv.contains(0));
  EXPECT_TRUE(interv.contains(5));
  EXPECT_TRUE(interv.contains(9));
  EXPECT_TRUE(interv.contains(10));
}

TEST(right_open_integer_interval_test, overlaps)
{
  interval<int> interv{5, 10};
  interval<int> interv2{6, 9}, interv3{10, 11}, interv4{4, 5}, interv5{9, 12}, interv6{2, 4};

  ASSERT_TRUE(interv.overlaps(interv2));
  ASSERT_FALSE(interv.overlaps(interv3));
  ASSERT_FALSE(interv.overlaps(interv4));
  ASSERT_TRUE(interv.overlaps(interv5));
  ASSERT_FALSE(interv.overlaps(interv6));
}

TEST(right_closed_integer_interval_test, overlaps)
{
  interval<int, true> interv{5, 10};
  interval<int, true> interv2{6, 9}, interv3{10, 11}, interv4{4, 5}, interv5{9, 12}, interv6{2, 4};

  ASSERT_TRUE(interv.overlaps(interv2));
  ASSERT_TRUE(interv.overlaps(interv3));
  ASSERT_TRUE(interv.overlaps(interv4));
  ASSERT_TRUE(interv.overlaps(interv5));
  ASSERT_FALSE(interv.overlaps(interv6));
}

TEST(right_open_integer_interval_test, formatter)
{
  interval<int> interv{5, 10};
  std::string   s = fmt::format("{}", interv);

  ASSERT_EQ(s, "[5..10)");
}

TEST(right_closed_integer_interval_test, formatter)
{
  interval<int, true> interv{5, 10};
  std::string         s = fmt::format("{}", interv);

  ASSERT_EQ(s, "[5..10]");
}

TEST(right_open_real_interval_test, formatter)
{
  interval<float> interv{5, 10};
  std::string     s = fmt::format("{}", interv);

  ASSERT_EQ(s, "[5.0, 10.0)");
}

TEST(right_closed_real_interval_test, formatter)
{
  interval<float, true> interv{5, 10};
  std::string           s = fmt::format("{}", interv);

  ASSERT_EQ(s, "[5.0, 10.0]");
}
