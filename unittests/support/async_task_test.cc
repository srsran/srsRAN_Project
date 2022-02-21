
#include "srsgnb/support/async/coroutine.h"
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
}

/// Test manual event
void test_manual_event()
{
  manual_event<int> event2;
  TESTASSERT(not event2.is_set());
  event2.set(5);
  TESTASSERT(event2.is_set());
  TESTASSERT_EQ(5, event2.get());
  event2.reset();
  TESTASSERT(not event2.is_set());
}

int main()
{
  test_manual_event_flag();
  test_manual_event();
}