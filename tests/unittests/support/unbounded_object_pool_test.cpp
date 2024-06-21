/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/memory_pool/unbounded_object_pool.h"
#include <gtest/gtest.h>

using namespace srsran;

class unbounded_object_pool_test : public ::testing::Test
{
protected:
  constexpr static size_t    init_capacity = 8;
  unbounded_object_pool<int> pool{init_capacity};
};

TEST_F(unbounded_object_pool_test, pool_initiated_with_provided_capacity)
{
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity);
}

TEST_F(unbounded_object_pool_test, pool_allocates_objects_within_capacity)
{
  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity - 1);
  *obj = 42;
  obj.reset();
  ASSERT_EQ(pool.current_capacity_approx(), init_capacity);
}

TEST_F(unbounded_object_pool_test, pool_allocates_objects_beyond_initial_capacity)
{
  std::vector<unbounded_object_pool<int>::ptr> objs;
  for (unsigned i = 0; i != init_capacity; ++i) {
    objs.push_back(pool.get());
    ASSERT_NE(objs.back(), nullptr);
  }
  auto obj = pool.get();
  ASSERT_NE(obj, nullptr);

  // pool capacity grew.
  objs.clear();
  obj.reset();
  ASSERT_GT(pool.current_capacity_approx(), init_capacity);
}
