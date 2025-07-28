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

#include "srsran/support/executors/task_fork_limiter.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class fork_limit_executor_test : public ::testing::Test
{
protected:
  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{"POOL", 8, 1024, std::chrono::microseconds{50}};
};

TEST_F(fork_limit_executor_test, max_forks_limit_are_respected)
{
  srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::warning);
  srslog::init();

  static const unsigned max_forks = 4;
  const unsigned        nof_tasks = 5000;
  auto limiter = make_task_fork_limiter_ptr(make_task_worker_pool_executor(pool), max_forks, nof_tasks + 1000);

  std::atomic<unsigned> active_forks{0};
  std::atomic<unsigned> max_active_forks{0};
  std::atomic<unsigned> tasks_completed{0};
  for (unsigned i = 0; i < nof_tasks; ++i) {
    ASSERT_TRUE(limiter->defer([&max_active_forks, &active_forks, &tasks_completed]() {
      auto cur_val = active_forks.fetch_add(1, std::memory_order_acq_rel) + 1;
      // Compute max concurrent forks.
      unsigned prev_max = max_active_forks.load(std::memory_order_relaxed);
      while (prev_max < cur_val and not max_active_forks.compare_exchange_weak(
                                        prev_max, cur_val, std::memory_order_relaxed, std::memory_order_relaxed)) {
        // Try to update max_active_forks until it succeeds.
      }
      active_forks.fetch_sub(1, std::memory_order_acq_rel);
      tasks_completed.fetch_add(1, std::memory_order_relaxed);
    }));
  }

  pool.wait_pending_tasks();

  EXPECT_LE(max_active_forks, max_forks) << "Fork limit executor should not allow more than " << max_forks
                                         << " concurrent tasks. Value: " << max_active_forks;
  ASSERT_GT(max_active_forks, 0);
  ASSERT_EQ(tasks_completed.load(), nof_tasks);
  fmt::print("Max active forks: {}\n", max_active_forks.load());
}
