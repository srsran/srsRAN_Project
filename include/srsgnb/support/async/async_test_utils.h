
#ifndef SRSGNB_ASYNC_TEST_UTILS_H
#define SRSGNB_ASYNC_TEST_UTILS_H

#include "async_task.h"
#include "manual_event.h"

namespace srsgnb {

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

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TEST_UTILS_H
