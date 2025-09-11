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

#include "srsran/support/executors/strand_executor.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <numeric>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

static_assert(is_task_executor<priority_task_strand_executor<task_executor*>>::value, "is_task_executor failed");
static_assert(is_task_executor_ptr<priority_task_strand_executor<task_executor*>*>::value,
              "is_task_executor_ptr failed");
static_assert(is_task_executor_ptr<priority_task_strand_executor<task_executor*>*&>::value,
              "is_task_executor_ptr failed");

// Test helper function that verifies that a given number of tasks are executed in order.
template <typename WaitCondition>
static void run_count_test(task_executor&       strand,
                           unsigned             total_increments,
                           unsigned             nof_producers,
                           const WaitCondition& wait_tasks_to_run)
{
  srslog::init();

  ASSERT_EQ(total_increments % nof_producers, 0);
  const unsigned increments_per_producer = total_increments / nof_producers;

  std::vector<unsigned> result;

  unsigned              unsync_count = 0;
  std::atomic<unsigned> sync_finished{0};
  auto                  inc_count_task      = [&unsync_count, &result]() { result.push_back(unsync_count++); };
  auto                  last_inc_count_task = [&unsync_count, &result, &sync_finished]() {
    result.push_back(unsync_count++);
    sync_finished++;
    fmt::print("Strand {} finished\n", unsync_count - 1);
  };

  auto push_increments = [&strand, &inc_count_task, increments_per_producer, last_inc_count_task]() {
    // Random sleep to shuffle the order of pushers.
    std::this_thread::sleep_for(std::chrono::microseconds{test_rgen::uniform_int(0, 100)});

    for (unsigned i = 0; i != increments_per_producer - 1; ++i) {
      ASSERT_TRUE(strand.defer(inc_count_task));
    }
    ASSERT_TRUE(strand.defer(last_inc_count_task));
  };

  // Create threads to push tasks.
  std::vector<unique_thread> pushers;
  for (unsigned i = 0; i != nof_producers; ++i) {
    pushers.emplace_back(fmt::format("pusher{}", i), push_increments);
  }

  for (unique_thread& t : pushers) {
    t.join();
  }

  while (sync_finished.load() != nof_producers) {
    wait_tasks_to_run();
  }

  std::vector<unsigned> expected(total_increments);
  std::iota(expected.begin(), expected.end(), 0);

  // Verify that the pushes to the (not mutexed) vector "result" were all in order.
  ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin(), expected.end())) << fmt::format(
      "Sizes={}, {}. Result: {}", result.size(), expected.size(), fmt::join(result.begin(), result.end(), ", "));

  wait_tasks_to_run();
}

template <typename StrandType>
class single_prio_strand_test : public testing::Test
{
protected:
  template <typename ExecType>
  void setup_strand(ExecType&& exec, unsigned queue_size)
  {
    strand_exec = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc, StrandType>(std::forward<ExecType>(exec),
                                                                                           queue_size);
  }

  std::unique_ptr<task_executor> strand_exec;
};
using test_strand_types = ::testing::Types<basic_strand_lock, force_dispatch_strand_lock<enqueue_priority::max>>;
TYPED_TEST_SUITE(single_prio_strand_test, test_strand_types);

TYPED_TEST(single_prio_strand_test, dispatch_to_single_worker_causes_no_race_conditions)
{
  static const unsigned nof_increments = 2048;
  static const unsigned nof_pushers    = 4;

  task_worker w{"WORKER", nof_increments};
  this->setup_strand(make_task_executor(w), nof_increments);

  run_count_test(*this->strand_exec, nof_increments, nof_pushers, [&w]() { w.wait_pending_tasks(); });
}

TYPED_TEST(single_prio_strand_test, dispatch_to_worker_pool_causes_no_race_conditions)
{
  static const unsigned nof_workers    = 6;
  static const unsigned nof_increments = 4096 * 4;
  static const unsigned nof_pushers    = 4;

  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{
      "POOL", nof_workers, nof_increments, std::chrono::microseconds{100}};
  this->setup_strand(task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc>(pool), nof_increments);

  run_count_test(*this->strand_exec, nof_increments, nof_pushers, [&pool]() { pool.wait_pending_tasks(); });
}

template <typename StrandType>
class multi_prio_strand_test : public testing::Test
{
protected:
  void setup_strand(task_executor& exec)
  {
    const std::array<unsigned, 2>        qsizes  = {16, 16};
    std::vector<concurrent_queue_params> qparams = {{concurrent_queue_policy::lockfree_mpmc, qsizes[0]},
                                                    {concurrent_queue_policy::lockfree_mpmc, qsizes[1]}};
    strand_ptr                                   = make_priority_task_strand_ptr<StrandType>(&exec, qparams);
    strand_execs                                 = make_priority_task_executor_ptrs(strand_ptr.get());
  }

  std::unique_ptr<priority_task_strand<task_executor*, StrandType>> strand_ptr;
  std::vector<std::unique_ptr<task_executor>>                       strand_execs;
};
TYPED_TEST_SUITE(multi_prio_strand_test, test_strand_types);

TYPED_TEST(multi_prio_strand_test, multi_priorities_in_strand)
{
  static const unsigned nof_workers = 3;
  static const unsigned qsize       = 16;

  // Instantiate worker pool.
  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> pool{
      "POOL", nof_workers, qsize, std::chrono::microseconds{100}};
  auto pool_exec = task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc>(pool);

  // Instantiate strands.
  this->setup_strand(pool_exec);

  concurrent_queue<enqueue_priority, concurrent_queue_policy::lockfree_mpmc, concurrent_queue_wait_policy::sleep>
      order_of_tasks(10);

  ASSERT_TRUE(this->strand_execs[0]->execute([&]() {
    ASSERT_TRUE(order_of_tasks.try_push(enqueue_priority::max));

    // Strand tasks are being pushed from within worker pool, but strand is already locked.
    // So, they will run right after this lambda returns, respecting the strand task priorities.
    ASSERT_TRUE(this->strand_execs[1]->execute(
        [&order_of_tasks]() { EXPECT_TRUE(order_of_tasks.try_push(enqueue_priority::max - 1)); }));
    ASSERT_TRUE(this->strand_execs[0]->execute(
        [&order_of_tasks]() { EXPECT_TRUE(order_of_tasks.try_push(enqueue_priority::max)); }));
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
