
#ifndef SRSGNB_ASYNC_TEST_UTILS_H
#define SRSGNB_ASYNC_TEST_UTILS_H

#include "async_task.h"
#include "eager_async_task.h"
#include "manual_event.h"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// Factory of async tasks that await on an external event.
template <typename Result>
class wait_manual_event_tester
{
public:
  wait_manual_event_tester() {}
  wait_manual_event_tester(const Result& r) : result(r) {}
  wait_manual_event_tester(Result&& r) : result(std::move(r)) {}

  async_task<Result> launch()
  {
    return launch_async([this](coro_context<async_task<Result> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(ready_ev);
      CORO_RETURN(result);
    });
  }

  manual_event_flag ready_ev;
  Result            result;
};

/// Specialization for result of type void.
template <>
class wait_manual_event_tester<void>
{
public:
  async_task<void> launch()
  {
    return launch_async([this](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(ready_ev);
      CORO_RETURN();
    });
  }
  manual_event_flag ready_ev;
};

template <typename R>
struct lazy_task_launcher : public eager_async_task<R> {
  lazy_task_launcher(async_task<R>& t_) : t(t_)
  {
    *static_cast<eager_async_task<R>*>(this) = launch_async([this](coro_context<eager_async_task<R> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(result, t);
      CORO_RETURN(result.value());
    });
  }

  optional<R> result;

private:
  async_task<R>& t;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TEST_UTILS_H
