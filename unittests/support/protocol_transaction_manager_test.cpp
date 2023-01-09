/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/async/protocol_transaction_manager.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

/// In this test, the transaction is completed successfully after the coroutine suspends.
void test_protocol_transaction_success_trigger_after_suspend()
{
  timer_manager                          timers;
  protocol_transaction_manager<int, 128> transaction_manager(timers, -1);

  protocol_transaction<int> tr = transaction_manager.create_transaction();
  TESTASSERT(not tr.complete());
  TESTASSERT_EQ(0, tr.id());

  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    TESTASSERT(not tr.complete());
    CORO_AWAIT(tr);
    TESTASSERT(tr.complete());
    CORO_RETURN(tr.result());
  });

  // Test: Setting the result should resume the coroutine.
  TESTASSERT(not t.ready());
  transaction_manager.set(tr.id(), 1);
  TESTASSERT(t.ready());
  TESTASSERT_EQ(1, tr.result());
}

/// In this test, the transaction is completed successfully before the coroutine suspends.
void test_protocol_transaction_success_trigger_before_suspend()
{
  timer_manager                          timers;
  protocol_transaction_manager<int, 128> transaction_manager(timers, -1);

  protocol_transaction<int> tr = transaction_manager.create_transaction();
  TESTASSERT(not tr.complete());
  TESTASSERT_EQ(0, tr.id());

  eager_async_task<int> t = launch_async([&tr, &transaction_manager](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    TESTASSERT(not tr.complete());
    transaction_manager.set(tr.id(), 2);
    CORO_AWAIT(tr);
    TESTASSERT(tr.complete());
    CORO_RETURN(tr.result());
  });

  // Test: Setting the result should resume the coroutine.
  TESTASSERT(t.ready());
  TESTASSERT_EQ(2, tr.result());
}

void test_protocol_transaction_different_id()
{
  timer_manager                          timers;
  protocol_transaction_manager<int, 128> transaction_manager(timers, -1);

  protocol_transaction<int> tr  = transaction_manager.create_transaction();
  protocol_transaction<int> tr2 = transaction_manager.create_transaction();
  TESTASSERT(not tr.complete());
  TESTASSERT(not tr2.complete());
  TESTASSERT_EQ(0, tr.id());
  TESTASSERT_EQ(1, tr2.id());

  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test: Setting the result for a different transaction id should not resume the coroutine.
  TESTASSERT(not t.ready());
  transaction_manager.set(tr2.id(), 1);
  TESTASSERT(not t.ready());
  transaction_manager.set(tr.id(), 2);
  TESTASSERT(t.ready());
}

void test_protocol_transaction_timeout()
{
  const unsigned                         timeout = 10;
  timer_manager                          timers;
  protocol_transaction_manager<int, 128> transaction_manager(timers, -1);

  protocol_transaction<int> tr = transaction_manager.create_transaction(timeout);
  TESTASSERT(not tr.complete());
  TESTASSERT_EQ(0, tr.id());

  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test: Coroutine is suspended until transaction timeout.
  for (unsigned i = 0; i < timeout; ++i) {
    TESTASSERT(not t.ready());
    timers.tick_all();
  }
  TESTASSERT(t.ready());
  TESTASSERT_EQ(-1, t.get());
}

int main()
{
  test_protocol_transaction_success_trigger_after_suspend();
  test_protocol_transaction_success_trigger_before_suspend();
  test_protocol_transaction_different_id();
  test_protocol_transaction_timeout();
}