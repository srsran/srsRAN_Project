/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/memory_pool/lockfree_object_pool.h"
#include "srsran/support/test_utils.h"
#include "srsran/support/unique_thread.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(lockfree_index_stack_test, concurrent_push_pop)
{
  std::vector<std::unique_ptr<std::thread>> workers;
  unsigned                                  nof_workers = 8;
  unsigned                                  nof_oper    = 1000;

  unsigned                     stack_cap = 10;
  detail::lockfree_index_stack stack{stack_cap, true};

  std::atomic<bool> start{false};

  for (unsigned i = 0; i != nof_workers; ++i) {
    workers.emplace_back(std::make_unique<std::thread>([&]() {
      std::vector<int> vals;
      vals.reserve(stack_cap);

      while (not start) {
      }

      for (unsigned j = 0; j != nof_oper; ++j) {
        if (test_rgen::uniform_int(0, 1) == 0) {
          if (not vals.empty()) {
            stack.push(vals.back());
            vals.pop_back();
          }
        } else {
          uint32_t val = stack.try_pop();
          if (val != detail::lockfree_index_stack::npos()) {
            EXPECT_TRUE(vals.size() < stack_cap);
            vals.push_back(val);
          }
        }
      }

      while (not vals.empty()) {
        stack.push(vals.back());
        vals.pop_back();
      }
    }));
  }

  start = true;

  for (auto& w : workers) {
    w->join();
  }

  std::vector<int8_t> vals;
  std::vector<int8_t> expected;
  for (unsigned i = 0; i != stack_cap; ++i) {
    uint32_t val = stack.try_pop();
    EXPECT_NE(val, detail::lockfree_index_stack::npos());
    vals.push_back(val);
    expected.push_back(i);
  }
  std::sort(vals.begin(), vals.end());
  ASSERT_EQ(vals, expected);
}

TEST(lockfree_object_pool_test, pool_created_with_requested_capacity)
{
  unsigned                  nof_objs = 10;
  int                       val      = 3;
  lockfree_object_pool<int> pool(nof_objs, val);

  ASSERT_EQ(pool.capacity(), nof_objs);
  ASSERT_EQ(pool.estimated_size(), pool.capacity());
}

TEST(lockfree_object_pool_test, pool_allocates_elements_with_initialized_value)
{
  unsigned nof_objs = 10;
  int      val      = 5;

  lockfree_object_pool<int> pool(nof_objs, val);

  std::vector<lockfree_object_pool<int>::ptr> allocated_objs;
  for (unsigned i = 0; i != nof_objs; ++i) {
    ASSERT_EQ(pool.estimated_size(), pool.capacity() - i);
    allocated_objs.push_back(pool.get());
    ASSERT_EQ(*allocated_objs.back(), val);
  }
  ASSERT_EQ(pool.estimated_size(), 0);
  ASSERT_EQ(pool.get(), nullptr);

  allocated_objs.clear();
  ASSERT_EQ(pool.estimated_size(), nof_objs);

  auto ptr = pool.get();
  ASSERT_EQ(*ptr, val);
  ASSERT_EQ(pool.estimated_size(), nof_objs - 1);
}

TEST(lockfree_bounded_stack_test, test_initialization)
{
  lockfree_bounded_stack<int> stack{10};
  ASSERT_EQ(stack.capacity(), 10);
  ASSERT_EQ(stack.size(), 0);
}

TEST(lockfree_bounded_stack_test, test_push_pop_one_element)
{
  lockfree_bounded_stack<int> stack{10};

  stack.push(5);
  ASSERT_EQ(stack.size(), 1);
  ASSERT_EQ(stack.capacity(), 10);

  int val;
  ASSERT_TRUE(stack.pop(val));
  ASSERT_EQ(val, 5);
  ASSERT_EQ(stack.size(), 0);
  ASSERT_EQ(stack.capacity(), 10);
}

TEST(lockfree_bounded_stack_test, test_pop_empty)
{
  lockfree_bounded_stack<int> stack{10};

  int val = 3;
  ASSERT_FALSE(stack.pop(val));
  stack.push(val);
  ASSERT_TRUE(stack.pop(val));
  ASSERT_EQ(val, 3);
  ASSERT_FALSE(stack.pop(val));
}

TEST(lockfree_bounded_stack_test, concurrent_push_pop)
{
  std::vector<std::unique_ptr<unique_thread>> workers;
  unsigned                                    nof_workers = 10;
  unsigned                                    nof_oper    = 1000;

  unsigned                       stack_cap = 100;
  lockfree_bounded_stack<int8_t> stack{stack_cap};
  for (unsigned i = 0; i != stack_cap; ++i) {
    stack.push(i);
  }

  std::atomic<bool> start{false};

  for (unsigned i = 0; i != nof_workers; ++i) {
    workers.emplace_back(std::make_unique<unique_thread>(fmt::format("worker{}", i), [&]() {
      std::vector<int> vals;
      vals.reserve(stack_cap);

      while (not start) {
      }
      for (unsigned j = 0; j != nof_oper; ++j) {
        if (test_rgen::uniform_int(0, 1) == 0) {
          if (not vals.empty()) {
            EXPECT_TRUE(stack.push(vals.back()));
            vals.pop_back();
          }
        } else {
          int8_t val;
          if (stack.pop(val)) {
            EXPECT_TRUE(vals.size() <= stack_cap);
            vals.push_back(val);
          }
        }
      }

      while (not vals.empty()) {
        EXPECT_TRUE(stack.push(vals.back()));
        vals.pop_back();
      }
    }));
  }

  start = true;

  for (auto& w : workers) {
    w->join();
  }

  std::vector<int8_t> vals;
  std::vector<int8_t> expected;
  for (unsigned i = 0; i != stack_cap; ++i) {
    int8_t val;
    EXPECT_TRUE(stack.pop(val));
    vals.push_back(val);
    expected.push_back(i);
  }
  std::sort(vals.begin(), vals.end());
  ASSERT_EQ(vals, expected);
}
