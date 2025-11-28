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

#include "srsran/adt/soa_table.h"
#include <gtest/gtest.h>

using namespace srsran;

enum class ColId { COL0, COL1 };

TEST(soa_table_test, empty_table)
{
  soa::table<ColId, int, float> table;

  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_EQ(table.capacity(), 0);
  ASSERT_FALSE(table.has_row_id(soa::row_id{0}));

  auto col0 = table.column<ColId::COL0>();
  ASSERT_TRUE(col0.empty());
  ASSERT_EQ(col0.size(), 0);
  ASSERT_FALSE(col0.has_row_id(soa::row_id{0}));
}

TEST(soa_table_test, insert_one_row)
{
  soa::table<ColId, int, float> table;

  soa::row_id row_id = table.insert(42, 3.14f);
  ASSERT_EQ(row_id.value(), 0);
  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_GE(table.capacity(), 1);
  ASSERT_TRUE(table.has_row_id(soa::row_id{0}));
  ASSERT_EQ(table.at<ColId::COL0>(row_id), 42);
  ASSERT_EQ(table.at<ColId::COL1>(row_id), 3.14f);

  // Column test.
  auto col0 = table.column<ColId::COL0>();
  auto col1 = table.column<ColId::COL1>();
  ASSERT_FALSE(col0.empty());
  ASSERT_FALSE(col1.empty());
  ASSERT_EQ(col0.size(), 1);
  ASSERT_EQ(col1.size(), 1);
  ASSERT_TRUE(col0.has_row_id(row_id));
  ASSERT_TRUE(col1.has_row_id(row_id));
  ASSERT_EQ(col0[row_id], 42);
  ASSERT_EQ(col1[row_id], 3.14f);

  // Row test.
  auto row0 = table.row(row_id);
  ASSERT_EQ(row0.nof_columns(), 2);
  ASSERT_EQ(row0.id(), row_id);
  ASSERT_EQ(row0.at<ColId::COL0>(), 42);
  ASSERT_EQ(row0.at<ColId::COL1>(), 3.14f);

  auto col0_span = col0.to_span();
  ASSERT_EQ(col0_span.size(), 1);
  std::array<int, 1> expected_col0{42};
  ASSERT_TRUE(std::equal(col0_span.begin(), col0_span.end(), expected_col0.begin(), expected_col0.end()));
}

TEST(soa_table_test, insert_and_rem_one_row)
{
  soa::table<ColId, int, float> table;
  auto                          row_id = table.insert(42, 3.14f);
  ASSERT_TRUE(table.erase(row_id));
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_FALSE(table.has_row_id(row_id));

  // Column view test.
  auto col0 = table.column<ColId::COL0>();
  ASSERT_TRUE(col0.empty());
  auto col1 = table.column<ColId::COL1>();
  ASSERT_TRUE(col1.empty());
}

TEST(soa_table_test, insert_two)
{
  soa::table<ColId, int, float> table;
  auto                          row_id0 = table.insert(42, 3.14f);
  auto                          row_id1 = table.insert(43, 4.14f);
  ASSERT_NE(row_id0, row_id1);

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 2);
  ASSERT_TRUE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id1));

  // Column view test.
  auto col0 = table.column<ColId::COL0>();
  ASSERT_EQ(col0[row_id0], 42);
  ASSERT_EQ(col0[row_id1], 43);
  auto col1 = table.column<ColId::COL1>();
  ASSERT_EQ(col1[row_id0], 3.14f);
  ASSERT_EQ(col1[row_id1], 4.14f);

  // Row view test.
  auto row0 = table.row(row_id0);
  ASSERT_EQ(row0.at<ColId::COL0>(), 42);
  ASSERT_EQ(row0.at<ColId::COL1>(), 3.14f);
  auto row1 = table.row(row_id1);
  ASSERT_EQ(row1.at<ColId::COL0>(), 43);
  ASSERT_EQ(row1.at<ColId::COL1>(), 4.14f);
  ASSERT_NE(row0, row1);
}

TEST(soa_table_test, insert_and_rem_two)
{
  soa::table<ColId, int, float> table;
  auto                          row_id0 = table.insert(42, 3.14f);
  auto                          row_id1 = table.insert(43, 4.14f);
  ASSERT_TRUE(table.erase(row_id0));

  ASSERT_FALSE(table.empty());
  ASSERT_EQ(table.size(), 1);
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id1));
  auto col0 = table.column<ColId::COL0>();
  ASSERT_EQ(col0.size(), 1);
  ASSERT_EQ(col0[row_id1], 43);
  auto col1 = table.column<ColId::COL1>();
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
  soa::table<ColId, int, float> table;
  auto                          row_id0 = table.insert(42, 3.14f);
  auto                          row_id1 = table.insert(43, 4.14f);
  ASSERT_TRUE(table.erase(row_id0));
  ASSERT_FALSE(table.has_row_id(row_id0));
  auto row_id2 = table.insert(44, 5.14f);

  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_EQ(row_id2, row_id0) << "row_id should be reused";
  ASSERT_NE(row_id1, row_id2);
  ASSERT_EQ(table.size(), 2);
  ASSERT_EQ(table.column<ColId::COL0>().size(), 2);
  ASSERT_EQ(table.at<ColId::COL0>(row_id1), 43) << "row_id must stay stable";
  ASSERT_EQ(table.at<ColId::COL1>(row_id1), 4.14f);
  ASSERT_EQ(table.at<ColId::COL0>(row_id2), 44);
  ASSERT_EQ(table.at<ColId::COL1>(row_id2), 5.14f);
  ASSERT_NE(row_id1, row_id2);
}

TEST(soa_table_test, insert_three_rem_two_and_insert)
{
  soa::table<ColId, int, float> table;
  auto                          row_id0 = table.insert(42, 3.14f);
  auto                          row_id1 = table.insert(43, 4.14f);
  auto                          row_id2 = table.insert(44, 5.14f);
  ASSERT_TRUE(table.erase(row_id0));
  ASSERT_TRUE(table.erase(row_id1));
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_FALSE(table.column<ColId::COL0>().has_row_id(row_id0));
  ASSERT_FALSE(table.has_row_id(row_id1));
  ASSERT_FALSE(table.column<ColId::COL1>().has_row_id(row_id1));
  auto row_id3 = table.insert(45, 6.14f);

  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_TRUE(table.column<ColId::COL0>().has_row_id(row_id2));
  ASSERT_TRUE(table.has_row_id(row_id3));
  ASSERT_TRUE(table.column<ColId::COL1>().has_row_id(row_id3));
}

TEST(soa_table_test, iterator)
{
  soa::table<ColId, int, float> table;
  ASSERT_EQ(table.begin(), table.end());

  auto row0 = table.insert(42, 3.14f);
  auto row1 = table.insert(43, 4.14f);

  auto it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row0));
  ASSERT_EQ(it->at<ColId::COL0>(), 42);
  ++it;
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row1));
  ASSERT_EQ(it->at<ColId::COL0>(), 43);
  ++it;
  ASSERT_EQ(it, table.end());

  table.erase(table.begin());
  ASSERT_EQ(table.size(), 1);
  it = table.begin();
  ASSERT_NE(it, table.end());
  ASSERT_EQ(*it, table.row(row1));
  ASSERT_EQ(it->at<ColId::COL0>(), 43);

  table.erase(it);
  ASSERT_EQ(table.end(), table.begin());
}

TEST(soa_table_test, destructuring)
{
  soa::table<ColId, int, float> table;
  auto                          rowid0 = table.insert(42, 3.14f);

  auto [i, f] = table.row(rowid0);
  ASSERT_EQ(i, 42);
  ASSERT_EQ(f, 3.14f);
  auto rowlv    = table.row(rowid0);
  auto [i2, f2] = rowlv;
  ASSERT_EQ(i2, 42);
  ASSERT_EQ(f2, 3.14f);
}

TEST(soa_table_test, reserve_and_clear_resets_state)
{
  // This test guards the reset logic after bulk cleanup.
  soa::table<ColId, int, float> table;
  table.reserve(10);
  ASSERT_GE(table.capacity(), 10);

  auto rowid0 = table.insert(42, 3.14f);
  table.insert(43, 4.14f);
  ASSERT_EQ(table.size(), 2);

  table.clear();
  ASSERT_TRUE(table.empty());
  ASSERT_EQ(table.size(), 0);
  ASSERT_GE(table.capacity(), 10);

  auto rowid2 = table.insert(44, 5.14f);
  ASSERT_EQ(rowid2, rowid0) << "row_id should be reused";
  ASSERT_EQ(table.size(), 1);
  ASSERT_EQ(table.at<ColId::COL0>(rowid2), 44);
  ASSERT_EQ(table.at<ColId::COL1>(rowid2), 5.14f);
  ASSERT_FALSE(false);
}

TEST(soa_table_test, erase_invalid_row_id_is_noop)
{
  soa::table<ColId, int, float> table;
  auto                          row_id0 = table.insert(42, 3.14f);
  table.insert(43, 3.15f);
  auto row_id2 = table.insert(44, 3.16f);

  // Erasing invalid row should be no-op.
  ASSERT_EQ(table.size(), 3);
  ASSERT_FALSE(table.erase(soa::row_id{row_id2.value() + 1}));
  ASSERT_EQ(table.size(), 3);
  ASSERT_TRUE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id2));

  // Erasing again should be a noop.
  ASSERT_TRUE(table.erase(soa::row_id{row_id0}));
  ASSERT_EQ(table.size(), 2);
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_FALSE(table.erase(soa::row_id{row_id0}));
  ASSERT_EQ(table.size(), 2);
  ASSERT_TRUE(table.has_row_id(row_id2));
  ASSERT_FALSE(table.has_row_id(row_id0));
}

TEST(soa_table_test, move_only_columns)
{
  soa::table<ColId, std::unique_ptr<int>, std::string> table;
  auto                                                 ptr1     = std::make_unique<int>(42);
  auto*                                                ptr1_raw = ptr1.get();
  auto                                                 ptr2     = std::make_unique<int>(43);
  auto*                                                ptr2_raw = ptr2.get();
  auto                                                 row_id0  = table.insert(std::move(ptr1), "foo");
  auto                                                 row_id1  = table.insert(std::move(ptr2), "bar");
  ASSERT_EQ(table.at<ColId::COL0>(row_id0).get(), ptr1_raw);
  ASSERT_EQ(table.at<ColId::COL0>(row_id1).get(), ptr2_raw);
  ASSERT_EQ(*table.at<ColId::COL0>(row_id0), 42);
  ASSERT_EQ(*table.at<ColId::COL0>(row_id1), 43);
  ASSERT_EQ(ptr1, nullptr);
  ASSERT_EQ(ptr2, nullptr);
  table.erase(row_id0);
  ASSERT_FALSE(table.has_row_id(row_id0));
  ASSERT_EQ(*table.at<ColId::COL0>(row_id1), 43);
}
