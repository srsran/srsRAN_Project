/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/test_utils.h"
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
  std::tuple<size_t>                            get_args(std::true_type /**/) { return std::make_tuple(128); }
  std::tuple<size_t, std::chrono::microseconds> get_args(std::false_type /**/)
  {
    return std::make_tuple(128, std::chrono::microseconds{10});
  }

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
    concurrent_queue<moveonly_test_object, concurrent_queue_policy::locking_mpsc, concurrent_queue_wait_policy::sleep>>;
TYPED_TEST_SUITE(concurrent_queue_test, concurrent_queue_types);

TYPED_TEST(concurrent_queue_test, basic_queue_interface)
{
  ASSERT_EQ(this->queue.capacity(), 128);
  ASSERT_TRUE(this->queue.empty());
  ASSERT_EQ(this->queue.size(), 0);

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
