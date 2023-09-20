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
#include "srsran/support/test_utils.h"
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

  timer_manager                     timers;
  manual_task_worker                ctrl_worker{64};
  protocol_transaction_manager<int> transaction_manager{NOF_TRANSACTIONS, timer_factory{timers, ctrl_worker}};
};

TEST_F(protocol_transaction_test,
       when_transactions_are_created_then_they_are_not_complete_and_have_consecutive_transaction_ids)
{
  // new transaction objects.
  std::vector<protocol_transaction<int>> transactions;
  for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
    transactions.push_back(transaction_manager.create_transaction());
    auto& tr = transactions.back();
    ASSERT_TRUE(tr.valid());
    ASSERT_FALSE(tr.complete());
    ASSERT_EQ(i, tr.id());
  }
}

TEST_F(protocol_transaction_test, when_transaction_ids_are_all_occupied_then_transaction_creation_should_fail)
{
  // deplete transaction ids.
  std::vector<protocol_transaction<int>> transactions;
  for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
    transactions.push_back(transaction_manager.create_transaction());
    ASSERT_TRUE(transactions.back().valid());
  }

  protocol_transaction<int> tr = transaction_manager.create_transaction();
  ASSERT_FALSE(tr.valid());

  // Delete one running transaction.
  auto id = test_rgen::uniform_int<protocol_transaction_id_t>(0, NOF_TRANSACTIONS - 1);
  transactions.erase(transactions.begin() + id);

  // It should be possible to create a new one now.
  tr = transaction_manager.create_transaction();
  ASSERT_TRUE(tr.valid());
  ASSERT_EQ(tr.id(), id);
  ASSERT_FALSE(tr.complete());
}

TEST_F(protocol_transaction_test, when_transaction_goes_out_of_scope_then_transaction_id_can_be_reused)
{
  // deplete transaction ids.
  {
    std::vector<protocol_transaction<int>> transactions;
    for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
      transactions.push_back(transaction_manager.create_transaction());
      ASSERT_TRUE(transactions.back().valid());
    }
  }

  std::vector<protocol_transaction<int>> transactions;
  for (unsigned i = 0; i != NOF_TRANSACTIONS; ++i) {
    transactions.push_back(transaction_manager.create_transaction());
    ASSERT_TRUE(transactions.back().valid());
  }
}

TEST_F(protocol_transaction_test, when_transaction_is_released_then_next_transaction_id_is_not_same_as_released)
{
  protocol_transaction<int> tr    = transaction_manager.create_transaction();
  protocol_transaction_id_t tr_id = tr.id();

  tr.release();

  protocol_transaction<int> tr2 = transaction_manager.create_transaction();
  ASSERT_NE(tr_id, tr2.id());
}

TEST_F(protocol_transaction_test, when_transaction_is_set_after_suspend_then_awaiting_coroutine_is_resumed)
{
  protocol_transaction<int> tr = transaction_manager.create_transaction();
  auto                      t  = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    EXPECT_FALSE(tr.complete());
    CORO_AWAIT(tr);
    EXPECT_TRUE(tr.complete());
    CORO_RETURN(tr.response());
  });

  // Test Section.
  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(transaction_manager.set_response(tr.id(), 1));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), 1);
  EXPECT_TRUE(tr.has_response());
  EXPECT_FALSE(tr.aborted());
  ASSERT_EQ(1, tr.response());
}

TEST_F(protocol_transaction_test, when_transaction_is_set_before_suspend_then_awaiting_coroutine_skips_suspension)
{
  protocol_transaction<int> tr = transaction_manager.create_transaction();
  auto                      t  = launch_async([this, &tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    EXPECT_FALSE(tr.complete());
    // transaction set before await.
    ASSERT_TRUE(this->transaction_manager.set_response(tr.id(), 2));
    CORO_AWAIT(tr);
    EXPECT_TRUE(tr.complete());
    CORO_RETURN(tr.response());
  });

  // Test Section.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(tr.has_response());
  ASSERT_EQ(2, tr.response());
}

TEST_F(protocol_transaction_test,
       when_concurrent_transactions_are_created_then_coroutine_only_gets_resumed_when_respective_transaction_is_set)
{
  protocol_transaction<int> tr  = transaction_manager.create_transaction();
  protocol_transaction<int> tr2 = transaction_manager.create_transaction();
  auto                      t   = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.response());
  });

  // Test Section.
  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(transaction_manager.set_response(tr2.id(), 1));
  ASSERT_FALSE(t.ready());
  ASSERT_TRUE(transaction_manager.set_response(tr.id(), 2));
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), 2);
}

TEST_F(protocol_transaction_test,
       when_transaction_is_created_with_timeout_then_transaction_is_automatically_cancelled_on_timeout)
{
  const std::chrono::milliseconds timeout{10};

  protocol_transaction<int> tr = transaction_manager.create_transaction(timeout);
  ASSERT_FALSE(tr.complete());
  auto t = launch_async([&tr](coro_context<eager_async_task<expected<int, protocol_transaction_failure>>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.outcome());
  });
  for (unsigned i = 0; i < timeout.count(); ++i) {
    ASSERT_FALSE(t.ready());
    tick();
  }

  // Test Section.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().is_error());
  ASSERT_EQ(t.get().error(), protocol_transaction_failure::timeout);
}

TEST_F(
    protocol_transaction_test,
    when_transaction_is_created_with_transaction_id_and_timeout_then_transaction_id_is_used_and_transaction_is_automatically_cancelled_on_timeout)
{
  const std::chrono::milliseconds timeout{10};
  protocol_transaction_id_t       transaction_id = 10;

  protocol_transaction<int> tr = transaction_manager.create_transaction(transaction_id, timeout);
  ASSERT_EQ(tr.id(), transaction_id);
  ASSERT_FALSE(tr.complete());
  auto t = launch_async([&tr](coro_context<eager_async_task<protocol_transaction<int>::outcome_type>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.outcome());
  });

  // Test Section.
  for (unsigned i = 0; i < timeout.count(); ++i) {
    ASSERT_FALSE(t.ready());
    tick();
  }
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().is_error());
  ASSERT_EQ(t.get().error(), protocol_transaction_failure::timeout);
}
