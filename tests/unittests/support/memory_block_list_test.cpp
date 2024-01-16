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

#include "srsran/support/memory_pool/memory_block_list.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class memory_block_list_tester : public ::testing::Test
{
protected:
  void create_and_push_blocks(unsigned n)
  {
    this->blocks.resize(n);
    for (unsigned i = 0; i != this->blocks.size(); ++i) {
      list.push(&blocks[i]);
    }
  }

  std::vector<std::array<uint8_t, 128>> blocks;
  free_memory_block_list                list;
};

TEST_F(memory_block_list_tester, free_list_starts_empty)
{
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.size(), 0);
  ASSERT_EQ(list.try_pop(), nullptr);

  list.clear(); // no-op
  ASSERT_TRUE(list.empty());
}

TEST_F(memory_block_list_tester, push_blocks)
{
  create_and_push_blocks(test_rgen::uniform_int<unsigned>(1, 10));

  ASSERT_FALSE(list.empty());
  ASSERT_EQ(list.size(), this->blocks.size());
}

TEST_F(memory_block_list_tester, pop_blocks)
{
  create_and_push_blocks(test_rgen::uniform_int<unsigned>(1, 10));

  std::vector<void*> popped_blocks;
  for (unsigned i = 0; i != this->blocks.size(); ++i) {
    void* node = list.try_pop();
    ASSERT_NE(node, nullptr);
    popped_blocks.push_back(node);
  }
  std::sort(popped_blocks.begin(), popped_blocks.end());
  for (unsigned i = 0; i != this->blocks.size(); ++i) {
    ASSERT_EQ(&this->blocks[i], popped_blocks[i]);
  }
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, clear)
{
  create_and_push_blocks(test_rgen::uniform_int<unsigned>(1, 10));

  ASSERT_FALSE(list.empty());
  list.clear();
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, try_pop_batch)
{
  create_and_push_blocks(test_rgen::uniform_int<unsigned>(1, 10));

  unsigned               n     = test_rgen::uniform_int<unsigned>(0, blocks.size());
  free_memory_block_list list2 = list.try_pop_batch(n);

  ASSERT_EQ(list2.size(), n);
  ASSERT_EQ(list.size(), blocks.size() - n);
  for (unsigned i = 0; i != n; ++i) {
    ASSERT_NE(list2.try_pop(), nullptr);
  }
  ASSERT_EQ(list2.try_pop(), nullptr);
  for (unsigned i = 0; i != blocks.size() - n; ++i) {
    ASSERT_NE(list.try_pop(), nullptr);
  }
  ASSERT_EQ(list.try_pop(), nullptr);
}

TEST_F(memory_block_list_tester, steal_blocks)
{
  create_and_push_blocks(test_rgen::uniform_int<unsigned>(1, 10));
  std::vector<std::array<uint8_t, 128>> blocks2(test_rgen::uniform_int<unsigned>(0, blocks.size()));
  free_memory_block_list                list2;
  for (unsigned i = 0; i != blocks2.size(); ++i) {
    list2.push(&blocks2[i]);
  }

  list2.steal_blocks(list);
  ASSERT_EQ(list2.size(), blocks2.size() + blocks.size());
  ASSERT_EQ(list.size(), 0);

  for (unsigned i = 0; i != blocks2.size() + blocks.size(); ++i) {
    ASSERT_NE(list2.try_pop(), nullptr);
  }
  ASSERT_EQ(list2.try_pop(), nullptr);
}
