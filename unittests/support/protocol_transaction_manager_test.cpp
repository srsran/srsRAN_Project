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

class protocol_transaction_test : public ::testing::Test
{
protected:
  static constexpr size_t NOF_TRANSACTIONS = 128;

  timer_manager                                       timers;
  protocol_transaction_manager<int, NOF_TRANSACTIONS> transaction_manager{timers, -1};
};

TEST_F(protocol_transaction_test,
       when_transactions_are_created_then_they_are_not_complete_and_have_consecutive_transaction_ids)
{
  // new transaction objects.
  for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
    protocol_transaction<int> tr = transaction_manager.create_transaction();
    ASSERT_FALSE(tr.complete());
    ASSERT_EQ(i, tr.id());
  }

  // reused transaction objects should start incomplete.
  for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
    protocol_transaction<int> tr = transaction_manager.create_transaction();
    ASSERT_FALSE(tr.complete());
    ASSERT_EQ(i, tr.id());
  }
}

TEST_F(protocol_transaction_test, when_transaction_is_set_after_suspend_then_awaiting_coroutine_is_resumed)
{
  protocol_transaction<int> tr = transaction_manager.create_transaction();
  eager_async_task<int>     t  = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    EXPECT_FALSE(tr.complete());
    CORO_AWAIT(tr);
    EXPECT_TRUE(tr.complete());
    CORO_RETURN(tr.result());
  });

  // Test Section.
  ASSERT_FALSE(t.ready());
  transaction_manager.set(tr.id(), 1);
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(1, tr.result());
}

TEST_F(protocol_transaction_test, when_transaction_is_set_before_suspend_then_awaiting_coroutine_skips_suspension)
{
  protocol_transaction<int> tr = transaction_manager.create_transaction();
  eager_async_task<int>     t  = launch_async([this, &tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    EXPECT_FALSE(tr.complete());
    // transaction set before await.
    this->transaction_manager.set(tr.id(), 2);
    CORO_AWAIT(tr);
    EXPECT_TRUE(tr.complete());
    CORO_RETURN(tr.result());
  });

  // Test Section.
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(2, tr.result());
}

TEST_F(protocol_transaction_test,
       when_concurrent_transactions_are_created_then_coroutine_only_gets_resumed_when_respective_transaction_id_is_set)
{
  protocol_transaction<int> tr  = transaction_manager.create_transaction();
  protocol_transaction<int> tr2 = transaction_manager.create_transaction();
  eager_async_task<int>     t   = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test Section.
  ASSERT_FALSE(t.ready());
  transaction_manager.set(tr2.id(), 1);
  ASSERT_FALSE(t.ready());
  transaction_manager.set(tr.id(), 2);
  ASSERT_TRUE(t.ready());
}

TEST_F(protocol_transaction_test,
       when_transaction_is_created_with_timeout_then_transaction_is_automatically_cancelled_on_timeout)
{
  const unsigned timeout = 10;

  protocol_transaction<int> tr = transaction_manager.create_transaction(timeout);
  ASSERT_FALSE(tr.complete());
  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test Section.
  for (unsigned i = 0; i < timeout; ++i) {
    ASSERT_FALSE(t.ready());
    timers.tick_all();
  }
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(-1, t.get());
}