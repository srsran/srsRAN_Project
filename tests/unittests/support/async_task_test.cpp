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

#include "srsran/support/async/async_procedure.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

/// Tester coroutine that just awaits an event to complete.
struct wait_event_coroutine {
  manual_event<int>& event;

public:
  explicit wait_event_coroutine(manual_event<int>& ev) : event(ev) {}

  void operator()(coro_context<async_task<int>>& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(int v, event);
    CORO_RETURN(v);
  }
};
static_assert(std::is_same<async_task<int>,
                           decltype(launch_async<wait_event_coroutine>(std::declval<manual_event<int>&>()))>::value,
              "Invalid async_task<> type deduction");

/// Async procedure implementation that just awaits another tasks and forwards its result
class passthrough_async_procedure final : public async_procedure<int>
{
public:
  explicit passthrough_async_procedure(async_task<int>& t_) : t(t_) {}

  void start() override { async_await(t, &passthrough_async_procedure::handle_result); }

private:
  void handle_result(int received_event_value) { async_return(received_event_value); }

  async_task<int>& t;
};

/// Coroutine implementation that just awaits another tasks and forwards its result
class passthrough_coroutine
{
  async_task<int>& t;

public:
  explicit passthrough_coroutine(async_task<int>& t_) : t(t_) {}
  void operator()(coro_context<async_task<int>>& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(int v, t);
    CORO_RETURN(v);
  }
};

//////////////////////////////////////////////// TESTS ///////////////////////////////////////////////////////////

/// Test that an eager_async_task starts automatically.
void test_eager_task_start()
{
  int                   value = 2;
  eager_async_task<int> t     = launch_async([&value](coro_context<eager_async_task<int>>& ctx) mutable {
    CORO_BEGIN(ctx);
    value += 4;
    CORO_RETURN(value * 2);
  });
  TESTASSERT_EQ(6, value);
  TESTASSERT(t.ready());
}

/// Test that a async_task starts lazily.
void test_lazy_task_start()
{
  int             value = 2;
  async_task<int> t     = launch_async([&value](coro_context<async_task<int>>& ctx) mutable {
    CORO_BEGIN(ctx);
    value += 4;
    CORO_RETURN(value * 2);
  });

  // STATUS: The lazy coroutine is not started because is still not being awaited.
  TESTASSERT_EQ(2, value);

  // Action: Start an eager passthrough coroutine that awaits on the lazy coroutine.
  eager_async_task<int> t2 = launch_async([&t](coro_context<eager_async_task<int>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(int res, t);
    CORO_RETURN(res + 1);
  });

  // STATUS: The lazy coroutine runs to completion.
  TESTASSERT_EQ(6, value);
  TESTASSERT(t.ready());
  TESTASSERT_EQ(12, t.get());
  TESTASSERT(t2.ready());
  TESTASSERT_EQ(13, t2.get());
}

/// Tests for chaining of multiple async tasks
namespace task_chaining_test {

/// Unit Test that verifies the correct forwarding of events across awaiters.
template <typename TaskFactory>
void run_impl(TaskFactory&& launch_passthrough_task)
{
  manual_event<int>       event;
  async_task<int>         task  = launch_async<wait_event_coroutine>(event);
  async_task<int>         task2 = launch_passthrough_task(task);
  async_task<int>         task3 = launch_passthrough_task(task2);
  lazy_task_launcher<int> launcher(task3);

  // Status: While event is not set, the result is not propagated in the chain.
  TESTASSERT(not task3.ready());
  TESTASSERT(not task.ready());

  event.set(3);
  TESTASSERT(task.ready());
  TESTASSERT(task3.ready());
  TESTASSERT_EQ(3, task3.get());
}

/// Runs test with object-based, lambda-based async tasks and procedures.
void run()
{
  // Run test with coroutine object
  run_impl([](async_task<int>& prev) { return launch_async<passthrough_coroutine>(prev); });

  // Run test with lambda coroutine object
  auto lambda_coro_factory = [](async_task<int>& prev) {
    return launch_async([&prev](coro_context<async_task<int>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(int v, prev);
      CORO_RETURN(v);
    });
  };
  run_impl(lambda_coro_factory);

  // Run test with async_procedure
  run_impl([](async_task<int>& prev) { return launch_async<passthrough_async_procedure>(prev); });
}

} // namespace task_chaining_test

namespace task_cleanup {

void run_lambda()
{
  {
    moveonly_test_object  to_destroy(5);
    manual_event<int>     ev;
    async_task<int>       t  = launch_async([&ev, u = std::move(to_destroy)](coro_context<async_task<int>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(int obj, ev);
      CORO_RETURN(obj);
    });
    eager_async_task<int> t2 = launch_async([&t](coro_context<eager_async_task<int>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(int obj, t);
      CORO_RETURN(obj);
    });
    TESTASSERT(not ev.is_set());
    TESTASSERT(not t.ready());
    // tasks are suspended

    // Event and tasks get cancelled and destroyed here.
  }
  TESTASSERT_EQ(0, moveonly_test_object::object_count());
}

class proc_cleanup_first final : public async_procedure<int>
{
  manual_event<int>&   ev;
  moveonly_test_object o;

  void forward_ev(int v) { async_return(v); }

public:
  proc_cleanup_first(manual_event<int>& ev_, moveonly_test_object o_) : ev(ev_), o(std::move(o_)) {}

  void start() override { async_await(ev, &proc_cleanup_first::forward_ev); }
};

void run_async_procedure()
{
  {
    moveonly_test_object    to_destroy(5);
    manual_event<int>       ev;
    async_task<int>         t  = launch_async<proc_cleanup_first>(ev, std::move(to_destroy));
    async_task<int>         t2 = launch_async<passthrough_async_procedure>(t);
    lazy_task_launcher<int> t3(t2);
    TESTASSERT(not ev.is_set());
    TESTASSERT(not t.ready());
    // tasks are suspended

    // Event and tasks get cancelled and destroyed here.
  }
  TESTASSERT_EQ(0, moveonly_test_object::object_count());
}

void run()
{
  run_lambda();
  run_async_procedure();
}

} // namespace task_cleanup

int main()
{
  test_eager_task_start();
  test_lazy_task_start();
  task_chaining_test::run();
  task_cleanup::run();
}