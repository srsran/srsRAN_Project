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

#include "srsran/support/async/async_event_source.h"
#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class async_single_event_observer_test : public ::testing::Test
{
protected:
  timer_manager                    timers{1};
  manual_task_worker               worker{64};
  async_event_source<int>          event_source{timer_factory{timers, worker}};
  async_single_event_observer<int> tr;
};

TEST_F(async_single_event_observer_test, when_transaction_sink_is_created_then_it_starts_unregistered)
{
  ASSERT_FALSE(tr.connected());
  ASSERT_FALSE(tr.complete());
}

TEST_F(async_single_event_observer_test, when_no_events_have_been_set_then_sink_is_not_complete)
{
  tr.subscribe_to(event_source);
  ASSERT_FALSE(tr.complete());
}

TEST_F(async_single_event_observer_test, when_event_source_is_triggered_then_sink_receives_result)
{
  tr.subscribe_to(event_source);

  event_source.set(2);
  ASSERT_TRUE(tr.complete());
  ASSERT_EQ(tr.result(), 2);
}

TEST_F(async_single_event_observer_test, when_sink_subscribes_multiple_times_then_previous_result_auto_resets)
{
  tr.subscribe_to(event_source);
  event_source.set(2);

  tr.subscribe_to(event_source);
  event_source.set(3);

  ASSERT_TRUE(tr.complete());
  ASSERT_EQ(tr.result(), 3);
}

#ifdef ASSERTS_ENABLED
TEST_F(async_single_event_observer_test, only_one_sink_per_event_source_allowed)
{
  async_single_event_observer<int> tr2;
  tr.subscribe_to(event_source);

  eager_async_task<int> t = launch_async([this](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT(tr);
    CORO_RETURN(tr.result());
  });

  ASSERT_FALSE(t.ready());
  ASSERT_DEATH(tr2.subscribe_to(event_source), ".*");
}
#endif
