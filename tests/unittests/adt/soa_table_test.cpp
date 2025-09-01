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
  soa::table<int, float> table;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_EQ(table.capacity(), 0);
  ASSERT_FALSE(table.has_row_id(soa::row_id{0}));

  auto col0 = table.column<0>();
  ASSERT_TRUE(col0.empty());
  ASSERT_EQ(col0.size(), 0);
  ASSERT_FALSE(col0.has_row_id(soa::row_id{0}));
}

TEST(soa_table_test, insert_one_row)
{
  soa::table<int, float> table;

  soa::row_id row_id = table.insert(42, 3.14f);
  ASSERT_EQ(row_id.value(), 0);
  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_GE(table.capacity(), 1);
  ASSERT_TRUE(table.has_row_id(soa::row_id{0}));
  ASSERT_EQ(table.at<0>(row_id), 42);
  ASSERT_EQ(table.at<1>(row_id), 3.14f);

  // Column test
  auto col0 = table.column<0>();
  auto col1 = table.column<1>();
  ASSERT_FALSE(col0.empty());
  ASSERT_FALSE(col1.empty());
  ASSERT_EQ(col0.size(), 1);
  ASSERT_EQ(col1.size(), 1);
  ASSERT_TRUE(col0.has_row_id(row_id));
  ASSERT_TRUE(col1.has_row_id(row_id));
  ASSERT_EQ(col0[row_id], 42);
  ASSERT_EQ(col1[row_id], 3.14f);

  // Row test
  auto row0 = table.row(row_id);
  ASSERT_EQ(row0.nof_columns(), 2);
  ASSERT_EQ(row0.id(), row_id);
  ASSERT_EQ(row0.at<0>(), 42);
  ASSERT_EQ(row0.at<1>(), 3.14f);

  auto col0_span = col0.to_span();
  ASSERT_EQ(col0_span.size(), 1);
  std::array<int, 1> expected_col0{42};
  ASSERT_TRUE(std::equal(col0_span.begin(), col0_span.end(), expected_col0.begin(), expected_col0.end()));
}

TEST(soa_table_test, insert_and_rem_one_row)
{
  soa::table<int, float> table;
  auto                   row_id = table.insert(42, 3.14f);
  ASSERT_TRUE(table.erase(row_id));
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_FALSE(table.has_row_id(row_id));

  // Column view test.
  auto col0 = table.column<0>();
  ASSERT_TRUE(col0.empty());
  auto col1 = table.column<1>();
  ASSERT_TRUE(col1.empty());
}

TEST(soa_table_test, insert_two)
{
  soa::table<int, float> table;
  auto                   row_id0 = table.insert(42, 3.14f);
  auto                   row_id1 = table.insert(43, 4.14f);
  ASSERT_NE(row_id0, row_id1);

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 2);
  ASSERT_TRUE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id1));

  // Column view test.
  auto col0 = table.column<0>();
  ASSERT_EQ(col0[row_id0], 42);
  ASSERT_EQ(col0[row_id1], 43);
  auto col1 = table.column<1>();
  ASSERT_EQ(col1[row_id0], 3.14f);
  ASSERT_EQ(col1[row_id1], 4.14f);

  // Row view test.
  auto row0 = table.row(row_id0);
  ASSERT_EQ(row0.at<0>(), 42);
  ASSERT_EQ(row0.at<1>(), 3.14f);
  auto row1 = table.row(row_id1);
  ASSERT_EQ(row1.at<0>(), 43);
  ASSERT_EQ(row1.at<1>(), 4.14f);
  ASSERT_NE(row0, row1);
}

TEST(soa_table_test, insert_and_rem_two)
{
  soa::table<int, float> table;
  auto                   row_id0 = table.insert(42, 3.14f);
  auto                   row_id1 = table.insert(43, 4.14f);
  ASSERT_TRUE(table.erase(row_id0));

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id1));
  auto col0 = table.column<0>();
  ASSERT_EQ(col0.size(), 1);
  ASSERT_EQ(col0[row_id1], 43);
  auto col1 = table.column<1>();
  ASSERT_EQ(col1.size(), 1);
  ASSERT_EQ(col1[row_id1], 4.14f);

  ASSERT_TRUE(table.erase(row_id1));
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_FALSE(table.has_row_id(row_id1));
}

TEST(soa_table_test, insert_two_rem_and_insert)
{
  soa::table<int, float> table;
  auto                   row_id0 = table.insert(42, 3.14f);
  auto                   row_id1 = table.insert(43, 4.14f);
  ASSERT_TRUE(table.erase(row_id0));
  ASSERT_FALSE(table.has_row_id(row_id0));
  auto row_id2 = table.insert(44, 5.14f);

  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_EQ(row_id2, row_id0) << "row_id should be reused";
  ASSERT_NE(row_id1, row_id2);
  ASSERT_EQ(table.size(), 2);
  ASSERT_EQ(table.column<0>().size(), 2);
  ASSERT_EQ(table.at<0>(row_id1), 43) << "row_id must stay stable";
  ASSERT_EQ(table.at<1>(row_id1), 4.14f);
  ASSERT_EQ(table.at<0>(row_id2), 44);
  ASSERT_EQ(table.at<1>(row_id2), 5.14f);
  ASSERT_NE(row_id1, row_id2);
}

TEST(soa_table_test, insert_three_rem_two_and_insert)
{
  soa::table<int, float> table;
  auto                   row_id0 = table.insert(42, 3.14f);
  auto                   row_id1 = table.insert(43, 4.14f);
  auto                   row_id2 = table.insert(44, 5.14f);
  ASSERT_TRUE(table.erase(row_id0));
  ASSERT_TRUE(table.erase(row_id1));
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_FALSE(table.column<0>().has_row_id(row_id0));
  ASSERT_FALSE(table.has_row_id(row_id1));
  ASSERT_FALSE(table.column<1>().has_row_id(row_id1));
  auto row_id3 = table.insert(45, 6.14f);

  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_TRUE(table.column<0>().has_row_id(row_id2));
  ASSERT_TRUE(table.has_row_id(row_id3));
  ASSERT_TRUE(table.column<1>().has_row_id(row_id3));
}

TEST(soa_table_test, iterator)
{
  soa::table<int, float> table;
  ASSERT_EQ(table.begin(), table.end());

  auto row0 = table.insert(42, 3.14f);
  auto row1 = table.insert(43, 4.14f);

  auto it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row0));
  ASSERT_EQ(it->at<0>(), 42);
  ++it;
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row1));
  ASSERT_EQ(it->at<0>(), 43);
  ++it;
  ASSERT_EQ(it, table.end());

  table.erase(table.begin());
  ASSERT_EQ(table.size(), 1);
  it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row1));
  ASSERT_EQ(it->at<0>(), 43);

  table.erase(it);
  ASSERT_EQ(table.end(), table.begin());
}

TEST(soa_table_test, destructuring)
{
  soa::table<int, float> table;
  auto                   rowid0 = table.insert(42, 3.14f);

  auto [i, f] = table.row(rowid0);
  ASSERT_EQ(i, 42);
  ASSERT_EQ(f, 3.14f);
  auto rowlv    = table.row(rowid0);
  auto [i2, f2] = rowlv;
  ASSERT_EQ(i2, 42);
  ASSERT_EQ(f2, 3.14f);
}
