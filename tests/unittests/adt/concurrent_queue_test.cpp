/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/unique_thread.h"
#include <gtest/gtest.h>

// Disable GCC 5's -Wsuggest-override warnings in gtest.
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else // __clang__
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif // __clang__

using namespace srsran;

template <typename QueueType>
class concurrent_queue_test : public ::testing::Test
{
protected:
  using value_type = typename QueueType::value_type;

  concurrent_queue_test(std::true_type /**/, size_t qsize) : queue(qsize, std::chrono::microseconds{5}) {}
  concurrent_queue_test(std::false_type /**/, size_t qsize) : queue(qsize) {}

  concurrent_queue_test(size_t qsize = 128) :
    concurrent_queue_test<QueueType>(std::conditional_t<QueueType::wait_policy == concurrent_queue_wait_policy::sleep,
                                                        std::true_type,
                                                        std::false_type>{},
                                     qsize)
  {
  }

  QueueType queue;
};

using concurrent_queue_types = ::testing::Types<
    concurrent_queue<int, concurrent_queue_policy::locking_mpmc, concurrent_queue_wait_policy::condition_variable>,
    concurrent_queue<moveonly_test_object,
                     concurrent_queue_policy::locking_mpmc,
                     concurrent_queue_wait_policy::condition_variable>,
    concurrent_queue<int, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>,
    concurrent_queue<moveonly_test_object, concurrent_queue_policy::lockfree_spsc, concurrent_queue_wait_policy::sleep>,
    concurrent_queue<int, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>,
    concurrent_queue<moveonly_test_object, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>,
    concurrent_queue<int, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::condition_variable>,
    concurrent_queue<moveonly_test_object,
                     concurrent_queue_policy::locking_mpsc,
                     concurrent_queue_wait_policy::condition_variable>>;
TYPED_TEST_SUITE(concurrent_queue_test, concurrent_queue_types);

TYPED_TEST(concurrent_queue_test, checks_for_empty_queue)
{
  ASSERT_EQ(this->queue.capacity(), 128);
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

  auto val = this->queue.try_pop();
  ASSERT_FALSE(val.has_value());
  ASSERT_TRUE(this->queue.empty());

  this->queue.clear();
  ASSERT_TRUE(this->queue.empty());
}

TYPED_TEST(concurrent_queue_test, checks_for_non_empty_queue)
{
  ASSERT_TRUE(this->queue.try_push(5));
  ASSERT_FALSE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 1);

  auto val = this->queue.try_pop();
  ASSERT_TRUE(val.has_value());
  ASSERT_EQ(*val, 5);
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

  val = this->queue.try_pop();
  ASSERT_FALSE(val.has_value());
}

TYPED_TEST(concurrent_queue_test, clear_of_non_empty_queue)
{
  ASSERT_TRUE(this->queue.try_push(5));
  ASSERT_TRUE(this->queue.try_push(6));

  this->queue.clear();
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

  this->queue.clear();
  ASSERT_TRUE(this->queue.empty());
}

TYPED_TEST(concurrent_queue_test, try_push_to_full_queue_fails)
{
  for (unsigned i = 0; i != this->queue.capacity(); ++i) {
    ASSERT_EQ(this->queue.size(), i);
    ASSERT_TRUE(this->queue.try_push(i));
  }
  ASSERT_FALSE(this->queue.try_push(5));
  ASSERT_EQ(this->queue.size(), this->queue.capacity());

  auto val = this->queue.try_pop();
  ASSERT_TRUE(val.has_value());
  ASSERT_EQ(*val, 0);
  ASSERT_EQ(this->queue.size(), this->queue.capacity() - 1);
}

TYPED_TEST(concurrent_queue_test, blocking_push_and_pop)
{
  static const int NOF_PUSHES = 1000;

  unique_thread t1("my_thread", [&]() {
    for (int i = 0; i < NOF_PUSHES; ++i) {
      this->queue.push_blocking(i);
    }
  });

  for (int i = 0; i != NOF_PUSHES; ++i) {
    auto val = this->queue.pop_blocking();
    ASSERT_TRUE(val.has_value());
    ASSERT_EQ(*val, i);
  }
}

TYPED_TEST(concurrent_queue_test, request_stop_unblocks_pop)
{
  static const int NOF_PUSHES = 10;

  std::atomic<bool> finished{false};
  unique_thread     t1("my_thread", [&]() {
    while (this->queue.pop_blocking().has_value()) {
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

TYPED_TEST(concurrent_queue_test, request_stop_unblocks_push)
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
