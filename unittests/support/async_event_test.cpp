
#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// Test manual event flag
void test_manual_event_flag()
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
  eager_async_task<void> t = launch_async([&event](coro_context<eager_async_task<void> >& ctx) {
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
void test_manual_event()
{
  manual_event<int> event;
  TESTASSERT(not event.is_set());
  event.set(5);
  TESTASSERT(event.is_set());
  TESTASSERT_EQ(5, event.get());
  event.reset();
  TESTASSERT(not event.is_set());

  // launch task that awaits on event
  eager_async_task<int> t = launch_async([&event](coro_context<eager_async_task<int> >& ctx) {
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

int main()
{
  test_manual_event_flag();
  test_manual_event();
}