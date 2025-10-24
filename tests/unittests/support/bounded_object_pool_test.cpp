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

#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

/// Colllect metrics on the bounded object pool operations.
static constexpr bool EnableMetrics = false;

template <typename Pool>
class common_bounded_object_pool_test : public ::testing::Test
{
protected:
  using pool_type                       = Pool;
  static constexpr size_t pool_capacity = 1024;
  Pool                    pool{pool_capacity};
};
using test_value_types = ::testing::Types<bounded_unique_object_pool<int, EnableMetrics>,
                                          bounded_object_pool<int, noop_operation, EnableMetrics>>;
TYPED_TEST_SUITE(common_bounded_object_pool_test, test_value_types);

TYPED_TEST(common_bounded_object_pool_test, pool_initiated_with_provided_capacity)
{
  ASSERT_EQ(this->pool.capacity(), this->pool_capacity);
}

TYPED_TEST(common_bounded_object_pool_test, pool_initiated_is_full)
{
  ASSERT_EQ(this->pool.size_approx(), this->pool.capacity());
}

TYPED_TEST(common_bounded_object_pool_test, one_allocation_works)
{
  auto obj = this->pool.get();
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(this->pool.size_approx(), this->pool.capacity() - 1);
  obj.reset();
}

TYPED_TEST(common_bounded_object_pool_test, depleted_pool_returns_null)
{
  std::vector<typename decltype(this->pool)::ptr> objs;
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    objs.push_back(this->pool.get());
    ASSERT_NE(objs.back(), nullptr);
    *objs.back() = i;
  }
  auto obj = this->pool.get();
  ASSERT_EQ(obj, nullptr);
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    ASSERT_EQ(*objs[i], i);
  }

  objs.pop_back();
  obj = this->pool.get();
  ASSERT_NE(obj, nullptr);
}

TYPED_TEST(common_bounded_object_pool_test, stress_pool)
{
  unsigned                                                 nof_workers = 8;
  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> worker_pool{"pool", nof_workers, 128};

  const unsigned                        nof_operations = 1000000;
  std::vector<std::chrono::nanoseconds> latencies(nof_workers);
  for (unsigned i = 0; i != nof_workers; ++i) {
    worker_pool.push_task_blocking([&, worker_id = i]() {
      auto                                 start_tp = std::chrono::high_resolution_clock::now();
      std::vector<typename TypeParam::ptr> allocated_objs;
      allocated_objs.reserve(nof_operations);
      for (unsigned j = 0; j != nof_operations; ++j) {
        int randn = test_rgen::uniform_int(0, 100);
        if (randn < 50) {
          auto obj = this->pool.get();
          if (obj != nullptr) {
            *obj = randn;
          }
          allocated_objs.push_back(std::move(obj));
        } else {
          if (not allocated_objs.empty()) {
            allocated_objs.pop_back();
          }
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
  uint64_t nof_alloc_reattempts = this->pool.nof_alloc_reattempts();
  uint64_t nof_scanned_segments = this->pool.nof_scanned_segments();

  std::vector<typename decltype(this->pool)::ptr> objs;
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    objs.push_back(this->pool.get());
    ASSERT_NE(objs.back(), nullptr);
    *objs.back() = i;
  }
  auto obj = this->pool.get();
  ASSERT_EQ(obj, nullptr);
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    ASSERT_EQ(*objs[i], i);
  }

  double         latency_secs = avg_latency.count() / 1.0e9;
  constexpr bool is_ptr_pool =
      std::is_same_v<TypeParam, bounded_unique_object_pool<typename TypeParam::value_type, EnableMetrics>>;
  fmt::print("Result for \"{}\":\n", is_ptr_pool ? "bounded_object_ptr_pool" : "bounded_object_pool");
  fmt::print("Time elapsed: {:.2} s\n", latency_secs);
  fmt::print("Rate: {:.2} calls/sec\n", nof_operations / latency_secs);
  fmt::print("Average get() latency: {} ns\n", avg_latency.count() / nof_operations);
  fmt::print("Nof scanned elements: {}\n", nof_scanned_segments);
  fmt::print("Nof alloc reattempts: {}\n", nof_alloc_reattempts);
}

template <typename Pool>
class bounded_object_pool_non_pwr2_test : public ::testing::Test
{
protected:
  static constexpr size_t pool_capacity = 1000;
  Pool                    pool{pool_capacity};
};
TYPED_TEST_SUITE(bounded_object_pool_non_pwr2_test, test_value_types);

TYPED_TEST(bounded_object_pool_non_pwr2_test, non_power2_capacity_is_respected)
{
  ASSERT_EQ(this->pool.capacity(), this->pool_capacity);
  ASSERT_EQ(this->pool.size_approx(), this->pool.capacity());

  std::vector<typename decltype(this->pool)::ptr> objs;
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    objs.push_back(this->pool.get());
    ASSERT_NE(objs.back(), nullptr);
    *objs.back() = i;
  }
  auto obj = this->pool.get();
  ASSERT_EQ(obj, nullptr);
  for (unsigned i = 0; i != this->pool.capacity(); ++i) {
    ASSERT_EQ(*objs[i], i);
  }

  objs.pop_back();
  obj = this->pool.get();
  ASSERT_NE(obj, nullptr);
}
