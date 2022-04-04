
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

struct wait_event_coroutine {
  manual_event<int>& event;

public:
  explicit wait_event_coroutine(manual_event<int>& ev) : event(ev) {}

  void operator()(coro_context<async_task<int> >& ctx)
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

//////////////////////////////////////////////// TESTS ///////////////////////////////////////////////////////////

/// Tests for chaining of multiple async tasks
namespace task_chaining_test {

/// Coroutine implementation that just awaits another tasks and forwards its result
class passthrough_coroutine
{
  async_task<int>& t;

public:
  explicit passthrough_coroutine(async_task<int>& t_) : t(t_) {}
  void operator()(coro_context<async_task<int> >& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(int v, t);
    CORO_RETURN(v);
  }
};

/// Unit Test that verifies the correct forwarding of events across awaiters
template <typename TaskFactory>
void run_impl(TaskFactory&& launch_passthrough_task)
{
  manual_event<int> event;
  async_task<int>   task  = launch_async<wait_event_coroutine>(event);
  async_task<int>   task2 = launch_passthrough_task(task);
  async_task<int>   task3 = launch_passthrough_task(task2);
  TESTASSERT(not task3.ready());

  event.set(3);
  TESTASSERT(task3.ready());
  TESTASSERT_EQ(3, task3.get());
}

void run()
{
  // Run test with coroutine object
  run_impl([](async_task<int>& prev) { return launch_async<passthrough_coroutine>(prev); });

  // Run test with lambda coroutine object
  auto lambda_coro_factory = [](async_task<int>& prev) {
    return launch_async([&prev](coro_context<async_task<int> >& ctx) {
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
    moveonly_test_object to_destroy(5);
    manual_event<int>    ev;
    async_task<int>      t  = launch_async([&ev, u = std::move(to_destroy)](coro_context<async_task<int> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(int obj, ev);
      CORO_RETURN(obj);
    });
    async_task<int>      t2 = launch_async([&t](coro_context<async_task<int> >& ctx) {
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
    moveonly_test_object to_destroy(5);
    manual_event<int>    ev;
    async_task<int>      t  = launch_async<proc_cleanup_first>(ev, std::move(to_destroy));
    async_task<int>      t2 = launch_async<passthrough_async_procedure>(t);
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
  task_chaining_test::run();
  task_cleanup::run();
}