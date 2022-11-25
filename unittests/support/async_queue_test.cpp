/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/async_queue.h"
#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

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

TEST(async_queue_test, push_resumes_awaiting_coroutine)
{
  async_queue<int>      q{5};
  eager_async_task<int> t   = launch_async<read_queue_coroutine<int>>(q);
  int                   val = test_rgen::uniform_int<int>();

  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(q.try_push(val));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), val);
}

TEST(async_queue_test, many)
{
  async_queue<int>      q{64};
  eager_async_task<int> t  = launch_async<read_queue_coroutine<int>>(q);
  eager_async_task<int> t2 = launch_async<read_queue_coroutine<int>>(q);
  eager_async_task<int> t3 = launch_async<read_queue_coroutine<int>>(q);

  TESTASSERT(not t.ready());
  TESTASSERT(not t2.ready());
  TESTASSERT(not t3.ready());

  q.try_push(5);
  TESTASSERT(t.ready());
  TESTASSERT_EQ(5, t.get());
  TESTASSERT(not t2.ready());
  TESTASSERT(not t3.ready());

  q.try_push(6);
  TESTASSERT(t2.ready());
  TESTASSERT_EQ(6, t2.get());
  TESTASSERT(not t3.ready());

  q.try_push(7);
  TESTASSERT_EQ(7, t3.get());
}

TEST(async_queue_test, dtor)
{
  {
    async_queue<int>      q{64};
    eager_async_task<int> t  = launch_async<read_queue_coroutine<int>>(q);
    eager_async_task<int> t2 = launch_async<read_queue_coroutine<int>>(q);
    eager_async_task<int> t3 = launch_async<read_queue_coroutine<int>>(q);

    TESTASSERT(not t.ready());
    TESTASSERT(not t2.ready());
    TESTASSERT(not t3.ready());

    q.try_push(2);
    TESTASSERT(not t2.ready());
  }
}

TEST(async_queue_test, moveonly)
{
  async_queue<moveonly_test_object> q(64);

  eager_async_task<moveonly_test_object> t  = launch_async<read_queue_coroutine<moveonly_test_object>>(q);
  eager_async_task<moveonly_test_object> t2 = launch_async<read_queue_coroutine<moveonly_test_object>>(q);

  q.try_push(moveonly_test_object(2));
  TESTASSERT(t.ready() and t.get().value() == 2);

  moveonly_test_object obj = std::move(t).get();
}