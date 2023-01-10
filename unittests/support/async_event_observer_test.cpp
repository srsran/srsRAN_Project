/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/async_event_source.h"
#include "srsgnb/support/async/eager_async_task.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class async_single_event_observer_test : public ::testing::Test
{
protected:
  timer_manager           timers{1};
  async_event_source<int> transaction_channel{timers, -1};
};

TEST_F(async_single_event_observer_test, when_transaction_subscriber_is_created_then_it_starts_unregistered)
{
  async_single_event_observer<int> tr;
  ASSERT_FALSE(tr.connected());
}

TEST_F(async_single_event_observer_test, when_no_events_have_been_set_then_subscriber_is_not_complete)
{
  async_single_event_observer<int> tr;
  tr.subscribe_to(transaction_channel);
  ASSERT_FALSE(tr.complete());
}

TEST_F(async_single_event_observer_test, when_publisher_is_triggered_then_subscriber_receives_result)
{
  async_single_event_observer<int> tr;
  tr.subscribe_to(transaction_channel);

  transaction_channel.set(2);
  ASSERT_TRUE(tr.complete());
  ASSERT_EQ(tr.result(), 2);
}

#ifdef ASSERTS_ENABLED
TEST_F(async_single_event_observer_test, only_one_subscriber_per_publisher_allowed)
{
  async_single_event_observer<int> tr, tr2;
  tr.subscribe_to(transaction_channel);

  eager_async_task<int> t = launch_async([&tr](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  ASSERT_FALSE(t.ready());
  ASSERT_DEATH(tr2.subscribe_to(transaction_channel), ".*");
}
#endif
