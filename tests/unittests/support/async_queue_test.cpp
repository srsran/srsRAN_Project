/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/async/async_queue.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

template <typename T>
struct read_queue_coroutine {
  async_queue<T>& q;
  read_queue_coroutine(async_queue<T>& q_) : q(q_) {}

  void operator()(coro_context<eager_async_task<T>>& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(T v, q);
    CORO_RETURN(std::move(v));
  }
};

TEST(async_queue_test, queue_starts_empty)
{
  async_queue<int> q{64};
  ASSERT_EQ(q.size(), 0);
}

TEST(async_queue_test, when_we_push_to_async_queue_awaiting_coroutine_is_resumed)
{
  async_queue<int>      q{5};
  eager_async_task<int> t   = launch_async<read_queue_coroutine<int>>(q);
  int                   val = test_rgen::uniform_int<int>();

  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(q.try_push(val));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), val);
  ASSERT_EQ(q.size(), 0);
}

TEST(async_queue_test, when_coroutine_awaits_non_empty_queue_it_does_not_suspend)
{
  async_queue<int> q{5};
  int              val  = test_rgen::uniform_int<int>();
  int              val2 = test_rgen::uniform_int<int>();

  ASSERT_TRUE(q.try_push(val));
  ASSERT_TRUE(q.try_push(val2));
  ASSERT_EQ(q.size(), 2);

  eager_async_task<int> t = launch_async<read_queue_coroutine<int>>(q);
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), val);
  ASSERT_EQ(q.size(), 1);
  eager_async_task<int> t2 = launch_async<read_queue_coroutine<int>>(q);
  ASSERT_TRUE(t2.ready());
  ASSERT_EQ(t2.get(), val2);
  ASSERT_EQ(q.size(), 0);
}

TEST(async_queue_test, async_queue_works_for_consecutive_push_pops)
{
  async_queue<int> q{5};
  int              val = test_rgen::uniform_int<int>();

  for (unsigned i = 0; i != 5; ++i) {
    eager_async_task<int> t = launch_async<read_queue_coroutine<int>>(q);
    ASSERT_FALSE(t.ready());
    ASSERT_TRUE(q.try_push(val));
    ASSERT_TRUE(t.ready()) << fmt::format("Failure at iteration: {}", i);
    ASSERT_EQ(t.get(), val);
  }
}

TEST(async_queue_test, queue_pushes_resumes_awaiting_coroutines_in_fifo_order)
{
  async_queue<int>                     q{64};
  std::array<eager_async_task<int>, 3> awaiting_tasks = {launch_async<read_queue_coroutine<int>>(q),
                                                         launch_async<read_queue_coroutine<int>>(q),
                                                         launch_async<read_queue_coroutine<int>>(q)};

  ASSERT_TRUE(std::none_of(awaiting_tasks.begin(), awaiting_tasks.end(), [](const auto& c) { return c.ready(); }));
  for (unsigned i = 0; i != awaiting_tasks.size(); ++i) {
    int val = test_rgen::uniform_int<int>();
    ASSERT_FALSE(awaiting_tasks[i].ready());
    ASSERT_TRUE(q.try_push(val));
    ASSERT_TRUE(awaiting_tasks[i].ready());
    ASSERT_EQ(awaiting_tasks[i].get(), val);
  }
  ASSERT_TRUE(std::all_of(awaiting_tasks.begin(), awaiting_tasks.end(), [](const auto& c) { return c.ready(); }));
  ASSERT_EQ(q.size(), 0);
}

TEST(async_queue_test, harmonious_destruction_of_async_queue_and_awaiting_tasks)
{
  {
    async_queue<int>      q{64};
    eager_async_task<int> t  = launch_async<read_queue_coroutine<int>>(q);
    eager_async_task<int> t2 = launch_async<read_queue_coroutine<int>>(q);
    eager_async_task<int> t3 = launch_async<read_queue_coroutine<int>>(q);

    ASSERT_TRUE(q.try_push(2));
    ASSERT_FALSE(t2.ready());
  }
  // sanitizer should not catch any access to dangling pointer during dtor calls.
}

TEST(async_queue_test, async_queue_supports_move_only_objects)
{
  async_queue<moveonly_test_object>      q(64);
  eager_async_task<moveonly_test_object> t   = launch_async<read_queue_coroutine<moveonly_test_object>>(q);
  eager_async_task<moveonly_test_object> t2  = launch_async<read_queue_coroutine<moveonly_test_object>>(q);
  int                                    val = test_rgen::uniform_int<int>();

  ASSERT_TRUE(q.try_push(moveonly_test_object(val)));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), val);

  moveonly_test_object obj = std::move(t).get();
  ASSERT_EQ(obj, val);
  ASSERT_FALSE(t2.ready());
}

class simple_task
{
public:
  void operator()(coro_context<async_task<void>>& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  }
};

TEST(async_queue_test, async_queue_supports_reentrant_push)
{
  async_queue<int> q(64);

  int                   val = 0, val2 = 0;
  eager_async_task<int> t = launch_async([&q, &val, &val2](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(val, q);
    CORO_AWAIT(launch_async<simple_task>());
    CORO_AWAIT_VALUE(val2, q);
    CORO_RETURN(val + val2);
  });
  ASSERT_TRUE(q.try_push(5));
  ASSERT_TRUE(q.try_push(10));

  ASSERT_EQ(t.get(), 15);
}