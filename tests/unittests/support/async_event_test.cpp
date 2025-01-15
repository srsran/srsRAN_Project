/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/async/event_sender_receiver.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test manual event flag
TEST(manual_event_flag_test, test_all)
{
  manual_event_flag event;
  TESTASSERT(not event.is_set());
  event.set();
  TESTASSERT(event.is_set());
  event.set();
  TESTASSERT(event.is_set());
  event.reset();
  TESTASSERT(not event.is_set());
  event.reset();
  TESTASSERT(not event.is_set());

  // launch task that awaits on event flag
  eager_async_task<void> t = launch_async([&event](coro_context<eager_async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(event);
    CORO_RETURN();
  });
  TESTASSERT(not t.ready());

  // confirm setting event resumes the task
  event.set();
  TESTASSERT(t.ready());
}

/// Test manual event
TEST(manual_event_test, test_all)
{
  manual_event<int> event;
  TESTASSERT(not event.is_set());
  event.set(5);
  TESTASSERT(event.is_set());
  TESTASSERT_EQ(5, event.get());
  event.reset();
  TESTASSERT(not event.is_set());

  // launch task that awaits on event
  eager_async_task<int> t = launch_async([&event](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(int received_value, event);
    CORO_RETURN(received_value);
  });
  TESTASSERT(not t.ready());

  // confirm setting event resumes the task and value is passed
  event.set(5);
  TESTASSERT(t.ready());
  TESTASSERT_EQ(5, t.get());
}

TEST(event_sender_receiver_test, test_receiver_no_sender)
{
  event_receiver<int> rx;
  ASSERT_FALSE(rx.completed());
  ASSERT_FALSE(rx.successful());
  ASSERT_FALSE(rx.aborted());
}

TEST(event_sender_receiver_test, test_sender_set)
{
  event_receiver<int> rx;
  event_sender<int>   tx = rx.get_sender();

  ASSERT_FALSE(rx.completed());
  ASSERT_FALSE(rx.successful());
  ASSERT_FALSE(rx.aborted());

  tx.set(5);

  ASSERT_TRUE(rx.completed());
  ASSERT_TRUE(rx.successful());
  ASSERT_FALSE(rx.aborted());
  ASSERT_EQ(rx.result(), 5);
}

TEST(event_sender_receiver_test, test_sender_cancel)
{
  event_receiver<int> rx;

  {
    event_sender<int> tx = rx.get_sender();
    ASSERT_FALSE(rx.aborted());
  }
  ASSERT_TRUE(rx.completed());
  ASSERT_TRUE(rx.aborted());
  ASSERT_FALSE(rx.successful());
}

TEST(event_sender_receiver_test, await_event_receiver)
{
  manual_event_flag   finished_ev;
  event_receiver<int> rx;
  event_sender<int>   tx = rx.get_sender();

  eager_async_task<void> t =
      launch_async([&finished_ev, tx = std::move(tx)](coro_context<eager_async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT(finished_ev);
        tx.set(5);
        CORO_RETURN();
      });

  ASSERT_FALSE(t.ready());
  ASSERT_FALSE(rx.completed());

  finished_ev.set();
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(rx.completed());
  ASSERT_TRUE(rx.successful());
  ASSERT_EQ(rx.result(), 5);
}

TEST(event_sender_receiver_test, await_event_receiver_and_sender_cancelled)
{
  manual_event_flag   finished_ev;
  event_receiver<int> rx;
  event_sender<int>   tx = rx.get_sender();

  {
    eager_async_task<void> t =
        launch_async([&finished_ev, tx = std::move(tx)](coro_context<eager_async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          CORO_AWAIT(finished_ev);
          tx.set(5);
          CORO_RETURN();
        });
  }

  ASSERT_FALSE(finished_ev.is_set());
  ASSERT_TRUE(rx.completed());
  ASSERT_FALSE(rx.successful());
  ASSERT_TRUE(rx.aborted());
}
