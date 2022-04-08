
#ifndef SRSGNB_ASYNC_TEST_UTILS_H
#define SRSGNB_ASYNC_TEST_UTILS_H

#include "async_task.h"
#include "lazy_task.h"
#include "manual_event.h"

namespace srsgnb {

/// Factory of async tasks that await on an external event.
template <typename Result>
class wait_manual_event_tester
{
public:
  wait_manual_event_tester() {}
  wait_manual_event_tester(const Result& r) : result(r) {}
  wait_manual_event_tester(Result&& r) : result(std::move(r)) {}

  lazy_task<Result> launch()
  {
    return launch_async([this](coro_context<lazy_task<Result> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(ready_ev);
      CORO_RETURN(result);
    });
  }

  manual_event_flag ready_ev;
  Result            result;
};

template <typename R>
struct lazy_task_launcher : public async_task<R> {
  lazy_task_launcher(lazy_task<R>& t_) : t(t_)
  {
    *static_cast<async_task<R>*>(this) = launch_async([this](coro_context<async_task<R> >& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(result, t);
      CORO_RETURN(result.value());
    });
  }

  optional<R> result;

private:
  lazy_task<R>& t;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TEST_UTILS_H
