
#include "srsgnb/support/async/async_queue.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

template <typename T>
struct read_queue_coroutine {
  async_queue<T>& q;
  read_queue_coroutine(async_queue<T>& q_) : q(q_) {}

  void operator()(coro_context<async_task<T> >& ctx)
  {
    CORO_BEGIN(ctx);
    CORO_AWAIT_VALUE(T v, q);
    CORO_RETURN(std::move(v));
  }
};

void test_async_queue()
{
  async_queue<int> q{64};
  async_task<int>  t  = launch_async<read_queue_coroutine<int> >(q);
  async_task<int>  t2 = launch_async<read_queue_coroutine<int> >(q);
  async_task<int>  t3 = launch_async<read_queue_coroutine<int> >(q);

  TESTASSERT(not t.ready());
  TESTASSERT(not t2.ready());
  TESTASSERT(not t3.ready());

  q.try_push(5);
  TESTASSERT(t.ready());
  TESTASSERT_EQ(5, t.get());
  TESTASSERT(not t2.ready());
  TESTASSERT(not t3.ready());

  q.try_push(6);
  TESTASSERT(t2.ready());
  TESTASSERT_EQ(6, t2.get());
  TESTASSERT(not t3.ready());

  q.try_push(7);
  TESTASSERT_EQ(7, t3.get());
}

void test_async_queue_dtor()
{
  {
    async_queue<int> q{64};
    async_task<int>  t  = launch_async<read_queue_coroutine<int> >(q);
    async_task<int>  t2 = launch_async<read_queue_coroutine<int> >(q);
    async_task<int>  t3 = launch_async<read_queue_coroutine<int> >(q);

    TESTASSERT(not t.ready());
    TESTASSERT(not t2.ready());
    TESTASSERT(not t3.ready());

    q.try_push(2);
    TESTASSERT(not t2.ready());
  }
}

void test_async_queue_moveonly()
{
  async_queue<moveonly_test_object> q(64);

  async_task<moveonly_test_object> t  = launch_async<read_queue_coroutine<moveonly_test_object> >(q);
  async_task<moveonly_test_object> t2 = launch_async<read_queue_coroutine<moveonly_test_object> >(q);

  q.try_push(moveonly_test_object(2));
  TESTASSERT(t.ready() and t.get().value() == 2);

  moveonly_test_object obj = std::move(t).get();
}

int main()
{
  test_async_queue();
  test_async_queue_dtor();
  test_async_queue_moveonly();
}