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

#include "srsran/adt/moodycamel_mpmc_queue.h"
#include "srsran/adt/mpmc_queue.h"
#include "srsran/adt/mutexed_mpmc_queue.h"
#include "srsran/adt/mutexed_mpsc_queue.h"
#include "srsran/adt/spsc_queue.h"
#include "srsran/support/detail/type_list.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

/// Metafunction to convert type_list into ::testing::Types.
template <typename T>
struct to_gtest_types;

template <typename... Args>
struct to_gtest_types<type_list<Args...>> {
  using type = ::testing::Types<Args...>;
};

template <typename T>
using to_gtest_type_list_t = typename to_gtest_types<T>::type;

template <typename QueueType>
class base_concurrent_queue_test
{
protected:
  using value_type = typename QueueType::value_type;

  base_concurrent_queue_test(std::true_type /**/, size_t qsize) : queue(qsize, std::chrono::microseconds{5}) {}
  base_concurrent_queue_test(std::false_type /**/, size_t qsize) : queue(qsize) {}

  base_concurrent_queue_test(size_t qsize = 128) :
    base_concurrent_queue_test(std::conditional_t<QueueType::wait_policy == concurrent_queue_wait_policy::sleep,
                                                  std::true_type,
                                                  std::false_type>{},
                               qsize)
  {
  }

  QueueType queue;
};

using qpolicy = concurrent_queue_policy;
using wpolicy = concurrent_queue_wait_policy;

// Types of concurrent queues.
using bounded_non_blocking_queue_types =
    type_list<concurrent_queue<int, qpolicy::locking_mpmc, wpolicy::non_blocking>,
              concurrent_queue<moveonly_test_object, qpolicy::locking_mpmc, wpolicy::non_blocking>,
              concurrent_queue<int, qpolicy::lockfree_spsc, wpolicy::non_blocking>,
              concurrent_queue<moveonly_test_object, qpolicy::lockfree_spsc, wpolicy::non_blocking>,
              concurrent_queue<int, qpolicy::lockfree_mpmc, wpolicy::non_blocking>,
              concurrent_queue<moveonly_test_object, qpolicy::lockfree_mpmc, wpolicy::non_blocking>,
              concurrent_queue<int, qpolicy::locking_mpsc, wpolicy::non_blocking>,
              concurrent_queue<moveonly_test_object, qpolicy::locking_mpsc, wpolicy::non_blocking>>;
using unbounded_non_blocking_queue_types =
    type_list<concurrent_queue<int, qpolicy::moodycamel_lockfree_mpmc, wpolicy::non_blocking>,
              concurrent_queue<moveonly_test_object, qpolicy::moodycamel_lockfree_mpmc, wpolicy::non_blocking>>;
using non_blocking_queue_types =
    type_list_helper::concat_t<bounded_non_blocking_queue_types, unbounded_non_blocking_queue_types>;

using bounded_blocking_queue_types =
    type_list<concurrent_queue<int, qpolicy::locking_mpmc, wpolicy::condition_variable>,
              concurrent_queue<moveonly_test_object, qpolicy::locking_mpmc, wpolicy::condition_variable>,
              concurrent_queue<int, qpolicy::lockfree_spsc, wpolicy::sleep>,
              concurrent_queue<moveonly_test_object, qpolicy::lockfree_spsc, wpolicy::sleep>,
              concurrent_queue<int, qpolicy::lockfree_mpmc, wpolicy::sleep>,
              concurrent_queue<moveonly_test_object, qpolicy::lockfree_mpmc, wpolicy::sleep>,
              concurrent_queue<int, qpolicy::locking_mpsc, wpolicy::sleep>,
              concurrent_queue<moveonly_test_object, qpolicy::locking_mpsc, wpolicy::sleep>,
              concurrent_queue<int, qpolicy::locking_mpsc, wpolicy::condition_variable>,
              concurrent_queue<moveonly_test_object, qpolicy::locking_mpsc, wpolicy::condition_variable>>;
using unbounded_blocking_queue_types =
    type_list<concurrent_queue<int, qpolicy::moodycamel_lockfree_mpmc, wpolicy::sleep>,
              concurrent_queue<moveonly_test_object, qpolicy::moodycamel_lockfree_mpmc, wpolicy::sleep>>;
using blocking_queue_types = type_list_helper::concat_t<bounded_blocking_queue_types, unbounded_blocking_queue_types>;

using all_queue_types = type_list_helper::concat_t<non_blocking_queue_types, blocking_queue_types>;

template <typename QueueType>
class all_concurrent_queue_test : public base_concurrent_queue_test<QueueType>, public ::testing::Test
{};
TYPED_TEST_SUITE(all_concurrent_queue_test, to_gtest_type_list_t<all_queue_types>);

template <typename QueueType>
class blocking_concurrent_queue_test : public base_concurrent_queue_test<QueueType>, public ::testing::Test
{};
TYPED_TEST_SUITE(blocking_concurrent_queue_test, to_gtest_type_list_t<blocking_queue_types>);

template <typename QueueType>
class bounded_concurrent_queue_test : public base_concurrent_queue_test<QueueType>, public ::testing::Test
{};
TYPED_TEST_SUITE(bounded_concurrent_queue_test, to_gtest_type_list_t<bounded_non_blocking_queue_types>);

template <typename QueueType>
class bounded_blocking_concurrent_queue_test : public base_concurrent_queue_test<QueueType>, public ::testing::Test
{};
TYPED_TEST_SUITE(bounded_blocking_concurrent_queue_test, to_gtest_type_list_t<bounded_blocking_queue_types>);

TYPED_TEST(all_concurrent_queue_test, checks_for_empty_queue)
{
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

  typename TestFixture::value_type val;
  ASSERT_FALSE(this->queue.try_pop(val));
  ASSERT_TRUE(this->queue.empty());
}

TYPED_TEST(all_concurrent_queue_test, checks_for_non_empty_queue)
{
  ASSERT_TRUE(this->queue.try_push(5));
  ASSERT_FALSE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 1);

  typename TestFixture::value_type val;
  ASSERT_TRUE(this->queue.try_pop(val));
  ASSERT_EQ(val, 5);
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

  ASSERT_FALSE(this->queue.try_pop(val));
}

TYPED_TEST(all_concurrent_queue_test, test_consumer_api)
{
  auto consumer = this->queue.create_consumer();
  ASSERT_TRUE(this->queue.try_push(5));

  typename TestFixture::value_type val;
  ASSERT_TRUE(consumer.try_pop(val));
  ASSERT_EQ(val, 5);
  ASSERT_FALSE(this->queue.try_pop(val));
}

TYPED_TEST(bounded_concurrent_queue_test, try_push_to_full_queue_fails)
{
  ASSERT_EQ(this->queue.capacity(), 128);
  for (unsigned i = 0; i != this->queue.capacity(); ++i) {
    ASSERT_EQ(this->queue.size(), i);
    ASSERT_TRUE(this->queue.try_push(i));
  }
  ASSERT_FALSE(this->queue.try_push(5));
  ASSERT_EQ(this->queue.size(), this->queue.capacity());

  typename decltype(this->queue)::value_type val;
  ASSERT_TRUE(this->queue.try_pop(val));
  ASSERT_EQ(val, 0);
  ASSERT_EQ(this->queue.size(), this->queue.capacity() - 1);
}

TYPED_TEST(blocking_concurrent_queue_test, blocking_push_and_pop)
{
  static const int NOF_PUSHES = 1000;

  unique_thread t1("my_thread", [&]() {
    for (int i = 0; i < NOF_PUSHES; ++i) {
      bool res = this->queue.push_blocking(i);
      EXPECT_TRUE(res);
    }
  });

  for (int i = 0; i != NOF_PUSHES; ++i) {
    typename TestFixture::value_type val;
    ASSERT_TRUE(this->queue.pop_blocking(val));
    ASSERT_EQ(val, i);
  }
}

TYPED_TEST(blocking_concurrent_queue_test, request_stop_unblocks_pop)
{
  static const int NOF_PUSHES = 10;

  std::atomic<bool> finished{false};
  unique_thread     t1("my_thread", [&]() {
    typename TestFixture::value_type val;
    while (this->queue.pop_blocking(val)) {
    }
    finished = true;
  });

  for (int i = 0; i < NOF_PUSHES; ++i) {
    this->queue.push_blocking(i);
  }

  ASSERT_TRUE(not finished);
  this->queue.request_stop();

  while (not finished) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

TYPED_TEST(bounded_blocking_concurrent_queue_test, request_stop_unblocks_push)
{
  std::atomic<bool> finished{false};
  unique_thread     t1("my_thread", [&]() {
    int i = 0;
    while (this->queue.push_blocking(i++)) {
    }
    finished = true;
  });

  while (this->queue.size() < this->queue.capacity()) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }

  ASSERT_TRUE(not finished);
  this->queue.request_stop();
  t1.join();
  ASSERT_TRUE(finished);
}
