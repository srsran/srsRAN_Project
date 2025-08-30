/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/soa_table.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(soa_table_test, empty_table)
{
  soa_table::table<int, float> table;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_EQ(table.capacity(), 0);
  ASSERT_FALSE(table.has_row_id(0));
}
