/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/priority_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/executors/task_worker_pool.h"
#include <future>
#include <gtest/gtest.h>

using namespace srsran;

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

TEST(task_worker, correct_initialization)
{
  task_worker worker{"WORKER", 1024};
  ASSERT_EQ(std::string(worker.worker_name()), "WORKER");
  ASSERT_EQ(worker.max_pending_tasks(), 1024);
  ASSERT_EQ(worker.nof_pending_tasks(), 0);
}

TEST(task_worker, multiple_stops_can_be_called)
{
  task_worker worker{"WORKER", 1024};
  worker.stop();
  worker.stop();
}

TEST(task_worker, single_pushed_task_is_run)
{
  task_worker           worker{"WORKER", 1024};
  std::atomic<uint32_t> count{0};
  ASSERT_TRUE(worker.push_task([&count]() { count++; }));
  worker.wait_pending_tasks();
  ASSERT_EQ(count, 1);
}

template <typename TaskWorkerPool>
class task_worker_pool_test : public ::testing::Test
{
protected:
  using pool_type = TaskWorkerPool;

  template <typename T = TaskWorkerPool, std::enable_if_t<std::is_same<T, task_worker_pool<true>>::value, int> = 0>
  task_worker_pool_test() : pool{4, 128, "POOL", std::chrono::microseconds{100}}
  {
  }
  template <typename T = TaskWorkerPool, std::enable_if_t<not std::is_same<T, task_worker_pool<true>>::value, int> = 0>
  task_worker_pool_test() : pool{4, 128, "POOL"}
  {
  }

  pool_type pool;
};
using worker_pool_types = ::testing::Types<task_worker_pool<false>, task_worker_pool<true>>;
TYPED_TEST_SUITE(task_worker_pool_test, worker_pool_types);

TYPED_TEST(task_worker_pool_test, correct_initialization)
{
  ASSERT_EQ(this->pool.nof_workers(), 4);
  ASSERT_EQ(this->pool.nof_pending_tasks(), 0);
}

TYPED_TEST(task_worker_pool_test, worker_pool_runs_single_task)
{
  std::promise<void> p;
  std::future<void>  f = p.get_future();
  ASSERT_TRUE(this->pool.push_task([&p]() {
    p.set_value();
    fmt::print("Finished in {}\n", this_thread_name());
  }));
  f.get();
}

TYPED_TEST(task_worker_pool_test, worker_pool_runs_tasks_in_all_workers)
{
  std::atomic<unsigned> count{0};
  for (unsigned i = 0; i != 10000 and count < this->pool.nof_workers(); ++i) {
    for (unsigned j = 0; j != this->pool.nof_workers(); ++j) {
      ASSERT_TRUE(this->pool.push_task([&count]() {
        thread_local bool first = true;
        std::this_thread::sleep_for(std::chrono::microseconds{100});
        if (first) {
          count++;
          first = false;
          fmt::print("Finished in {}\n", this_thread_name());
        }
      }));
    }
    std::this_thread::sleep_for(std::chrono::microseconds{100});
  }
  ASSERT_EQ(this->pool.nof_workers(), count);
}

TEST(spsc_task_worker_test, correct_initialization)
{
  general_task_worker<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep> worker{
      "WORKER", 128, std::chrono::microseconds{100}};

  ASSERT_EQ(std::string(worker.worker_name()), "WORKER");
  ASSERT_EQ(worker.max_pending_tasks(), 128);
  ASSERT_EQ(worker.nof_pending_tasks(), 0);
}
TEST(spsc_task_worker_test, single_pushed_task_is_run)
{
  general_task_worker<concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep> worker{
      "WORKER", 128, std::chrono::microseconds{100}};

  std::atomic<uint32_t> count{0};
  ASSERT_TRUE(worker.push_task([&count]() { count++; }));
  worker.wait_pending_tasks();
  ASSERT_EQ(count, 1);
}

TEST(priority_task_worker_test, priorities_respected_on_queue)
{
  priority_task_worker<concurrent_queue_policy::lockfree_mpmc, concurrent_queue_policy::lockfree_mpmc> worker{
      "WORKER", {16, 16}, std::chrono::microseconds{100}};
  std::atomic<uint32_t> result{0};

  ASSERT_TRUE(worker.push_task<enqueue_priority::min>([&]() mutable {
    // This task should be executed last.
    for (unsigned i = 0; i != 16; ++i) {
      ASSERT_TRUE(worker.push_task<enqueue_priority::max - 1>([&, i]() mutable { ASSERT_EQ(result++, 16 + i); }));
    }
    // This task should be executed first.
    for (unsigned i = 0; i != 16; ++i) {
      ASSERT_TRUE(worker.push_task<enqueue_priority::max>([&, i]() mutable { ASSERT_EQ(result++, i); }));
    }
  }));

  while (result != 32) {
    std::this_thread::sleep_for(std::chrono::microseconds{100});
  }
}