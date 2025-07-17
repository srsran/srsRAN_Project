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

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

struct big_int {
  big_int& operator=(int v)
  {
    val = v;
    return *this;
  }
  bool operator==(const big_int& other) const { return val == other.val; }
  bool operator==(int other) const { return val == other; }

  int                                 val;
  std::array<char, 128 - sizeof(int)> padding;
};

template <typename Pool>
class common_bounded_object_pool_test : public ::testing::Test
{
protected:
  using pool_type                       = Pool;
  static constexpr size_t pool_capacity = 1024;
  Pool                    pool{pool_capacity};
};
using test_value_types = ::testing::Types<bounded_unique_object_pool<int>, bounded_object_pool<int>>;
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
      auto start_tp = std::chrono::high_resolution_clock::now();
      for (unsigned j = 0; j != nof_operations; ++j) {
        auto obj = this->pool.get();
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
  constexpr bool is_ptr_pool  = std::is_same_v<TypeParam, bounded_unique_object_pool<typename TypeParam::value_type>>;
  fmt::print("Result for \"{}\":\n", is_ptr_pool ? "bounded_object_ptr_pool" : "bounded_object_pool");
  fmt::print("Time elapsed: {:.2} s\n", latency_secs);
  fmt::print("Rate: {:.2} calls/sec\n", nof_operations / latency_secs);
  fmt::print("Average get() latency: {} ns\n", avg_latency.count() / nof_operations);
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
