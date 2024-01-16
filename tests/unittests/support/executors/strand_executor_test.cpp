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

#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace srsran;

// Test helper function that verifies that a given number of tasks are executed in order.
template <typename WaitCondition>
void run_count_test(task_executor&       strand,
                    unsigned             total_increments,
                    unsigned             nof_producers,
                    const WaitCondition& wait_tasks_to_run)
{
  ASSERT_EQ(total_increments % nof_producers, 0);
  const unsigned increments_per_producer = total_increments / nof_producers;

  std::vector<unsigned> result;

  unsigned unsync_count   = 0;
  auto     inc_count_task = [&unsync_count, &result]() { result.push_back(unsync_count++); };

  auto push_increments = [&strand, &inc_count_task, increments_per_producer]() {
    for (unsigned i = 0; i != increments_per_producer; ++i) {
      ASSERT_TRUE(strand.defer(inc_count_task));
    }
  };

  std::vector<unique_thread> pushers;
  for (unsigned i = 0; i != nof_producers; ++i) {
    pushers.emplace_back(fmt::format("pusher{}", i), push_increments);
  }

  for (unique_thread& t : pushers) {
    t.join();
  }

  wait_tasks_to_run();

  std::vector<unsigned> expected(total_increments);
  std::iota(expected.begin(), expected.end(), 0);

  ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin(), expected.end())) << fmt::format(
      "Sizes={}, {}. Result: {}", result.size(), expected.size(), fmt::join(result.begin(), result.end(), ", "));
}

TEST(strand_executor_test, dispatch_to_single_worker_causes_no_race_conditions)
{
  static const unsigned nof_increments = 2048;
  static const unsigned nof_pushers    = 4;

  task_worker w{"WORKER", nof_increments};
  auto        worker_exec = make_task_executor(w);
  auto        strand_exec =
      make_strand_executor_ptr<concurrent_queue_policy::lockfree_mpmc>(std::move(worker_exec), nof_increments);

  run_count_test(*strand_exec, nof_increments, nof_pushers, [&w]() { w.wait_pending_tasks(); });
}

TEST(strand_executor_test, dispatch_to_worker_pool_causes_no_race_conditions)
{
  static const unsigned nof_workers    = 6;
  static const unsigned nof_increments = 4096 * 4;
  static const unsigned nof_pushers    = 4;

  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{
      nof_workers, nof_increments, "POOL", std::chrono::microseconds{100}};
  auto pool_exec   = task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc>(pool);
  auto strand_exec = make_strand_executor_ptr<concurrent_queue_policy::lockfree_mpmc>(pool_exec, nof_increments);

  run_count_test(*strand_exec, nof_increments, nof_pushers, [&pool]() { pool.wait_pending_tasks(); });
}

TEST(strand_executor_test, execute_inside_worker_runs_inline)
{
  static const unsigned nof_increments = 4096;

  task_worker w{"WORKER", 256};
  auto        worker_exec = make_task_executor(w);
  auto        strand_exec = make_strand_executor_ptr<concurrent_queue_policy::lockfree_mpmc>(worker_exec, 4);

  unsigned count = 0;
  w.push_task_blocking([&strand_exec, &count]() {
    // Running from inside the task_worker. Execute calls should be run inline.
    for (unsigned i = 0; i != nof_increments; ++i) {
      ASSERT_TRUE(strand_exec->execute([&count]() { count++; }));
    }
  });

  w.wait_pending_tasks();

  // Even though the worker queue size is smaller than the number of tasks, the fact that we run the task inline
  // should ensure no task is dropped.
  ASSERT_EQ(count, nof_increments);
}

TEST(strand_executor_test, multi_priorities_in_strand)
{
  static const unsigned                nof_workers = 3;
  static const unsigned                qsize       = 16;
  static const std::array<unsigned, 2> qsizes      = {16, 16};

  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{
      nof_workers, qsize, "POOL", std::chrono::microseconds{100}};
  auto pool_exec = task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc>(pool);
  std::vector<std::unique_ptr<task_executor>> strand_execs =
      make_strand_executor_ptrs<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::lockfree_spsc>(
          pool_exec, qsizes);

  concurrent_queue<enqueue_priority, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>
      order_of_tasks(10);

  ASSERT_TRUE(strand_execs[0]->execute([&]() {
    order_of_tasks.try_push(enqueue_priority::max);

    // Strand tasks are being pushed from within worker pool, but strand is already locked.
    // So, they will run right after this lambda returns, respecting the strand task priorities.
    ASSERT_TRUE(strand_execs[1]->execute(
        [&order_of_tasks]() { EXPECT_TRUE(order_of_tasks.try_push(enqueue_priority::max - 1)); }));
    ASSERT_TRUE(
        strand_execs[0]->execute([&order_of_tasks]() { EXPECT_TRUE(order_of_tasks.try_push(enqueue_priority::max)); }));
  }));

  std::array<enqueue_priority, 3> expected_values{
      enqueue_priority::max, enqueue_priority::max, enqueue_priority::max - 1};
  for (unsigned i = 0; i != 3; ++i) {
    enqueue_priority p;
    ASSERT_TRUE(order_of_tasks.pop_blocking(p));
    ASSERT_EQ(p, expected_values[i]);
  }

  pool.stop();
}
