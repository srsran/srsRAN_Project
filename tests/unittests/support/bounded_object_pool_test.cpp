/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class bounded_object_pool_test : public ::testing::Test
{
protected:
  static constexpr size_t  pool_capacity = 1024;
  bounded_object_pool<int> pool{pool_capacity};
};

TEST_F(bounded_object_pool_test, pool_initiated_with_provided_capacity)
{
  ASSERT_EQ(pool.capacity(), pool_capacity);
}

TEST_F(bounded_object_pool_test, pool_initiated_is_full)
{
  ASSERT_EQ(pool.size_approx(), pool.capacity());
}

TEST_F(bounded_object_pool_test, one_allocation_works)
{
  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(pool.size_approx(), pool.capacity() - 1);
  obj.reset();
}

TEST_F(bounded_object_pool_test, depleted_pool_returns_null)
{
  std::vector<bounded_object_pool<int>::ptr> objs;
  for (unsigned i = 0; i != pool.capacity(); ++i) {
    objs.push_back(pool.get());
    ASSERT_NE(objs.back(), nullptr);
    *objs.back() = i;
  }
  auto obj = pool.get();
  ASSERT_EQ(obj, nullptr);
  for (unsigned i = 0; i != pool.capacity(); ++i) {
    ASSERT_EQ(*objs[i], i);
  }

  objs.pop_back();
  obj = pool.get();
  ASSERT_NE(obj, nullptr);
}

TEST_F(bounded_object_pool_test, stress_pool)
{
  unsigned                                                 nof_workers = 8;
  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> worker_pool{"pool", nof_workers, 128};

  const unsigned                        nof_operations = 1000000;
  std::vector<std::chrono::nanoseconds> latencies(nof_workers);
  for (unsigned i = 0; i != nof_workers; ++i) {
    worker_pool.push_task_blocking([&, worker_id = i]() {
      auto start_tp = std::chrono::high_resolution_clock::now();
      for (unsigned j = 0; j != nof_operations; ++j) {
        auto obj = pool.get();
        if (obj != nullptr) {
          *obj = test_rgen::uniform_int(0, 100);
        }
      }
      auto end_tp          = std::chrono::high_resolution_clock::now();
      latencies[worker_id] = std::chrono::duration_cast<std::chrono::nanoseconds>(end_tp - start_tp);
    });
  }

  worker_pool.wait_pending_tasks();
  std::chrono::nanoseconds avg_latency{0};
  for (unsigned i = 0; i != nof_workers; ++i) {
    avg_latency += latencies[i];
  }
  avg_latency /= nof_workers;

  std::vector<bounded_object_pool<int>::ptr> objs;
  for (unsigned i = 0; i != pool.capacity(); ++i) {
    objs.push_back(pool.get());
    ASSERT_NE(objs.back(), nullptr);
    *objs.back() = i;
  }
  auto obj = pool.get();
  ASSERT_EQ(obj, nullptr);
  for (unsigned i = 0; i != pool.capacity(); ++i) {
    ASSERT_EQ(*objs[i], i);
  }

  double latency_secs = avg_latency.count() / 1.0e9;
  fmt::print("Time elapsed: {:.2} s\n", latency_secs);
  fmt::print("Rate: {:.2} calls/sec\n", nof_operations / latency_secs);
  fmt::print("Average get() latency: {} ns\n", avg_latency.count() / nof_operations);
}
