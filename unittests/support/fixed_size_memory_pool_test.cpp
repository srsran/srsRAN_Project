/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/memory_pool/fixed_size_memory_block_pool.h"
#include <gtest/gtest.h>

using namespace srsgnb;

TEST(fixed_memory_block_pool_test, correct_instantiation)
{
  struct test_tag_id {};

  auto& pool = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(256, 32);
  ASSERT_EQ(pool.nof_memory_blocks(), 32);
  ASSERT_EQ(pool.memory_block_size(), 256);
  pool.print_all_buffers();
}

TEST(fixed_memory_block_pool_test, allocated_block_is_valid)
{
  struct test_tag_id {};

  auto& pool  = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(256, 32);
  void* block = pool.allocate_node(256);
  ASSERT_NE(block, nullptr);
  pool.deallocate_node(block);
}

TEST(fixed_memory_block_pool_test, number_of_alloc_blocks_matches_pool_size)
{
  struct test_tag_id {};
  auto& pool = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(256, 32);

  std::vector<void*> blocks;
  for (unsigned i = 0; i != pool.nof_memory_blocks(); ++i) {
    void* b = pool.allocate_node(256);
    ASSERT_NE(b, nullptr);
    blocks.push_back(b);
  }
  for (void* b : blocks) {
    pool.deallocate_node(b);
  }
  pool.print_all_buffers();
}
