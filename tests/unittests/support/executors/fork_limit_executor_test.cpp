/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_fork_limiter.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class fork_limit_executor_test : public ::testing::Test
{
protected:
  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{"POOL", 4, 1024, std::chrono::microseconds{50}};
};

TEST_F(fork_limit_executor_test, max_forks_limit_are_respected)
{
  static const unsigned max_forks = 4;
  auto                  limiter   = make_task_fork_limiter_ptr<concurrent_queue_policy::lockfree_mpmc>(
      make_task_worker_pool_executor(pool), max_forks, 1024);

  const unsigned nof_tasks = 1000;

  std::atomic<unsigned> active_forks{0};
  std::atomic<unsigned> max_active_forks{0};
  for (unsigned i = 0; i < nof_tasks; ++i) {
    ASSERT_TRUE(limiter->defer([&max_active_forks, &active_forks]() {
      auto cur_val = active_forks.fetch_add(1, std::memory_order_relaxed) + 1;
      // Compute max concurrent forks.
      unsigned prev_max = max_active_forks.load(std::memory_order_relaxed);
      while (prev_max < cur_val and not max_active_forks.compare_exchange_weak(
                                        prev_max, cur_val, std::memory_order_relaxed, std::memory_order_relaxed)) {
        // Try to update max_active_forks until it succeeds.
      }
      active_forks.fetch_sub(1, std::memory_order_relaxed);
    }));
  }

  pool.wait_pending_tasks();

  EXPECT_LE(max_active_forks, max_forks) << "Fork limit executor should not allow more than " << max_forks
                                         << " concurrent tasks. Value: " << max_active_forks;
  ASSERT_GT(max_active_forks, 0);
  fmt::print("Max active forks: {}\n", max_active_forks.load());
}
