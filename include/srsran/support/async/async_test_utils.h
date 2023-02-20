/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "async_task.h"
#include "eager_async_task.h"
#include "manual_event.h"
#include "srsran/adt/optional.h"

namespace srsran {

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
    return launch_async([this](coro_context<async_task<Result>>& ctx) {
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
    return launch_async([this](coro_context<async_task<void>>& ctx) {
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
    *static_cast<eager_async_task<R>*>(this) = launch_async([this](coro_context<eager_async_task<R>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT_VALUE(result, t);
      CORO_RETURN(result.value());
    });
  }

  optional<R> result;

private:
  async_task<R>& t;
};

template <>
struct lazy_task_launcher<void> : public eager_async_task<void> {
  lazy_task_launcher(async_task<void>& t_) : t(t_)
  {
    *static_cast<eager_async_task<void>*>(this) = launch_async([this](coro_context<eager_async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_AWAIT(t);
      CORO_RETURN();
    });
  }

private:
  async_task<void>& t;
};

} // namespace srsran
