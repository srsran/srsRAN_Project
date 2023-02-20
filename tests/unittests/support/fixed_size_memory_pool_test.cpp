/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker.h"
#include "srsran/support/memory_pool/fixed_size_memory_block_pool.h"
#include "srsran/support/memory_pool/unsync_fixed_size_memory_block_pool.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(fixed_memory_block_pool_test, correct_instantiation)
{
  struct test_tag_id {};

  auto& pool = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(32, 256);
  ASSERT_EQ(pool.nof_memory_blocks(), 32);
  ASSERT_EQ(pool.memory_block_size(), 256);
  pool.print_all_buffers();
}

TEST(fixed_memory_block_pool_test, allocated_block_is_valid)
{
  struct test_tag_id {};
  auto& pool  = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(32, 256);
  void* block = pool.allocate_node(256);
  ASSERT_NE(block, nullptr);
  pool.deallocate_node(block);
}

TEST(fixed_memory_block_pool_test, number_of_alloc_blocks_matches_pool_size)
{
  struct test_tag_id {};
  auto& pool = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(32, 256);

  std::vector<void*> blocks;

  // Deplete pool with allocations.
  for (unsigned i = 0; i != pool.nof_memory_blocks(); ++i) {
    void* b = pool.allocate_node(256);
    ASSERT_NE(b, nullptr) << "Pool got depleted before expected";
    blocks.push_back(b);
  }
  ASSERT_EQ(pool.allocate_node(256), nullptr) << "Pool should be depleted at this point";

  // Deallocate blocks.
  for (void* b : blocks) {
    pool.deallocate_node(b);
  }

  pool.print_all_buffers();
}

TEST(fixed_memory_block_pool_test, when_worker_local_cache_exceeds_threshold_returns_block_to_central_cache)
{
  const unsigned nof_blocks = 128, max_local_cache_size = nof_blocks / 16;
  struct test_tag_id {};
  auto&       pool = fixed_size_memory_block_pool<test_tag_id, true>::get_instance(nof_blocks, 256);
  task_worker w("WorkerDealloc", nof_blocks * 4);

  for (unsigned i = 0; i != nof_blocks; ++i) {
    void* b = pool.allocate_node(256);
    ASSERT_NE(b, nullptr) << "Pool got depleted before expected";

    // Delegate deallocation to another thread.
    w.push_task([b, &pool]() { pool.deallocate_node(b); });
  }

  // Place a barrier to wait for other thread to deallocate some blocks.
  w.wait_pending_tasks();

  for (unsigned i = 0; i != nof_blocks - max_local_cache_size; ++i) {
    void* b = pool.allocate_node(256);
    ASSERT_NE(b, nullptr) << "Some blocks were not moved back to central cache";
    w.push_task([b, &pool]() { pool.deallocate_node(b); });
  }

  // Place a barrier.
  w.wait_pending_tasks();

  pool.print_all_buffers();
}

TEST(unsync_fixed_memory_block_pool_test, correct_instantiation)
{
  const unsigned                      nof_blocks = 128, mblock_size = 16;
  unsync_fixed_size_memory_block_pool pool{nof_blocks, mblock_size};
  ASSERT_EQ(pool.nof_memory_blocks(), nof_blocks);
  ASSERT_EQ(pool.memory_block_size(), mblock_size);
  ASSERT_EQ(pool.nof_blocks_available(), nof_blocks);
  ASSERT_FALSE(pool.empty());
}

TEST(unsync_fixed_memory_block_pool_test, allocated_block_is_valid)
{
  const unsigned                      nof_blocks = 128, mblock_size = 16;
  unsync_fixed_size_memory_block_pool pool{nof_blocks, mblock_size};
  void*                               block = pool.allocate_node(mblock_size);
  ASSERT_NE(block, nullptr);
  ASSERT_EQ(pool.nof_memory_blocks() - 1, pool.nof_blocks_available());
  ASSERT_FALSE(pool.empty());
  pool.deallocate_node(block);
  ASSERT_EQ(pool.nof_memory_blocks(), pool.nof_blocks_available());
}

TEST(unsync_fixed_memory_block_pool_test, number_of_alloc_blocks_matches_pool_size)
{
  const unsigned                      nof_blocks = 128, mblock_size = 16;
  unsync_fixed_size_memory_block_pool pool{nof_blocks, mblock_size};
  std::vector<void*>                  blocks;
  for (unsigned i = 0; i != nof_blocks; ++i) {
    ASSERT_EQ(pool.nof_memory_blocks() - i, pool.nof_blocks_available());
    void* block = pool.allocate_node(mblock_size);
    ASSERT_NE(block, nullptr);
    blocks.push_back(block);
  }
  ASSERT_TRUE(pool.empty());
  ASSERT_EQ(0, pool.nof_blocks_available());
  void* empty_block = pool.allocate_node(mblock_size);
  ASSERT_EQ(empty_block, nullptr);
  for (void* block : blocks) {
    pool.deallocate_node(block);
  }
}

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::debug);

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
