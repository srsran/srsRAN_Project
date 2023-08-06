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

#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/async/protocol_transaction_manager.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class protocol_transaction_test : public ::testing::Test
{
protected:
  static constexpr size_t NOF_TRANSACTIONS = 128;

  void tick()
  {
    timers.tick();
    ctrl_worker.run_pending_tasks();
  }

  timer_manager                                       timers;
  manual_task_worker                                  ctrl_worker{64};
  protocol_transaction_manager<int, NOF_TRANSACTIONS> transaction_manager{timer_factory{timers, ctrl_worker}, -1};
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
  ASSERT_TRUE(transaction_manager.set(tr.id(), 1));
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
    ASSERT_TRUE(this->transaction_manager.set(tr.id(), 2));
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
  ASSERT_TRUE(transaction_manager.set(tr2.id(), 1));
  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(transaction_manager.set(tr.id(), 2));
  ASSERT_TRUE(t.ready());
}

TEST_F(protocol_transaction_test,
       when_transaction_is_created_with_timeout_then_transaction_is_automatically_cancelled_on_timeout)
{
  const std::chrono::milliseconds timeout{10};

  protocol_transaction<int> tr = transaction_manager.create_transaction(timeout);
  ASSERT_FALSE(tr.complete());
  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test Section.
  for (unsigned i = 0; i < timeout.count(); ++i) {
    ASSERT_FALSE(t.ready());
    tick();
  }
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(-1, t.get());
}

TEST_F(
    protocol_transaction_test,
    when_transaction_is_created_with_transaction_id_and_timeout_then_transaction_id_is_used_and_transaction_is_automatically_cancelled_on_timeout)
{
  const std::chrono::milliseconds timeout{10};
  unsigned                        transaction_id = 10;

  protocol_transaction<int> tr = transaction_manager.create_transaction(transaction_id, timeout);
  ASSERT_EQ(tr.id(), transaction_id);
  ASSERT_FALSE(tr.complete());
  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  // Test Section.
  for (unsigned i = 0; i < timeout.count(); ++i) {
    ASSERT_FALSE(t.ready());
    tick();
  }
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(-1, t.get());
}
