/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/support/tracing/resource_usage.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(resource_usage_test, calculate_resource_usage_diff)
{
  resource_usage::snapshot point1 = resource_usage::now().value();
  resource_usage::snapshot point2 = resource_usage::now().value();

  resource_usage::diff d = point2 - point1;
  ASSERT_GE(d.vol_ctxt_switch_count, 0);
  ASSERT_GE(d.invol_ctxt_switch_count, 0);
}

TEST(resource_usage_test, snapshot_diff_is_correct)
{
  resource_usage::snapshot snap1{5, 10, std::chrono::microseconds{100}, std::chrono::microseconds{200}};
  resource_usage::snapshot snap2{7, 12, std::chrono::microseconds{150}, std::chrono::microseconds{250}};
  resource_usage::diff     d = snap2 - snap1;

  ASSERT_EQ(d.vol_ctxt_switch_count, snap2.vol_ctxt_switch_count - snap1.vol_ctxt_switch_count);
  ASSERT_EQ(d.invol_ctxt_switch_count, snap2.invol_ctxt_switch_count - snap1.invol_ctxt_switch_count);
  ASSERT_EQ(d.user_time, snap2.user_time - snap1.user_time);
  ASSERT_EQ(d.sys_time, snap2.sys_time - snap1.sys_time);
}

TEST(resource_usage_test, format_resource_usage_diff)
{
  resource_usage::snapshot point1 = resource_usage::now().value();
  ::usleep(10);
  resource_usage::snapshot point2 = resource_usage::now().value();
  resource_usage::diff     d      = point2 - point1;

  std::string str = fmt::format("{}", d);
  ASSERT_GT(str.size(), 0);
  fmt::print("> result: {}\n", str);
}
