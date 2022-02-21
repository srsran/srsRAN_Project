
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

namespace task_chaining_test {

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

class passthrough_async_procedure final : public async_procedure<int>
{
public:
  explicit passthrough_async_procedure(async_task<int>& t_) : t(t_) {}

  void start() override { async_await(t, &passthrough_async_procedure::handle_result); }

private:
  void handle_result(int received_event_value) { async_return(received_event_value); }

  async_task<int>& t;
};

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
    return launch_async_lambda([&prev](coro_context<async_task<int> >& ctx) {
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

int main()
{
  task_chaining_test::run();
}