/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/async/execute_on.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace detail {

template <typename TaskExecutor, bool IsExecute>
auto dispatch_on_blocking(TaskExecutor& exec, timer_manager& timers)
{
  struct blocking_dispatch_on_awaiter {
    blocking_dispatch_on_awaiter(TaskExecutor& exec_, timer_manager& timers_) : exec(exec_), timers(timers_) {}

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      auto task = [suspending_awaitable]() mutable { suspending_awaitable.resume(); };

      // Try to dispatch task.
      if constexpr (IsExecute) {
        if (exec.execute(task)) {
          return;
        }
      } else {
        if (exec.defer(task)) {
          return;
        }
      }

      // Task execute/defer failed (potentially because task executor queue is full).
      // Leverage timer infrastructure to run task.
      // Note: Even if the timer expiry fails to invoke task in executor, it keeps trying on every tick.
      retry_timer = timers.create_unique_timer(exec);
      retry_timer.set(std::chrono::milliseconds{1},
                      [suspending_awaitable](timer_id_t tid) mutable { suspending_awaitable.resume(); });
      retry_timer.run();
    }

    void await_resume() {}

    blocking_dispatch_on_awaiter& get_awaiter() { return *this; }

  private:
    TaskExecutor&  exec;
    timer_manager& timers;
    unique_timer   retry_timer;
  };

  return blocking_dispatch_on_awaiter{exec, timers};
}

} // namespace detail

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call
/// to execute fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
template <typename TaskExecutor>
auto execute_on_blocking(TaskExecutor& exec, timer_manager& timers)
{
  return detail::dispatch_on_blocking<TaskExecutor, true>(exec, timers);
}

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call
/// to defer fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
template <typename TaskExecutor>
auto defer_on_blocking(TaskExecutor& exec, timer_manager& timers)
{
  return detail::dispatch_on_blocking<TaskExecutor, false>(exec, timers);
}

/// \brief Returns an async_task<void> that runs a given invocable task in a \c dispatch_exec executor, and once the
/// task is complete, it resumes the suspended coroutine in a \c return_exec executor.
template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())>>
std::enable_if_t<std::is_same_v<ReturnType, void>, async_task<void>>
execute_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                 CurrentTaskExecutor&  return_exec,
                                 timer_manager&        timers,
                                 Callable&&            callable)
{
  return launch_async([&return_exec, &dispatch_exec, task = std::forward<Callable>(callable), &timers](
                          coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Dispatch execution context switch.
    CORO_AWAIT(execute_on_blocking(dispatch_exec, timers));

    // Run task.
    task();

    // Continuation in the original executor.
    CORO_AWAIT(execute_on_blocking(return_exec, timers));

    CORO_RETURN();
  });
}

/// \brief Returns an async_task<ReturnType> that runs a given invocable task in a \c dispatch_exec executor, and once
/// the task is complete, it resumes the suspended coroutine in a \c return_exec executor.
template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())>>
std::enable_if_t<not std::is_same_v<ReturnType, void>, async_task<ReturnType>>
execute_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                 CurrentTaskExecutor&  return_exec,
                                 timer_manager&        timers,
                                 Callable&&            callable)
{
  ReturnType ret{};
  return launch_async([&return_exec, &dispatch_exec, task = std::forward<Callable>(callable), &timers, ret](
                          coro_context<async_task<ReturnType>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // Dispatch execution context switch.
    CORO_AWAIT(execute_on_blocking(dispatch_exec, timers));

    // Run task.
    ret = task();

    // Continuation in the original executor.
    CORO_AWAIT(execute_on_blocking(return_exec, timers));

    CORO_RETURN(ret);
  });
}

} // namespace srsran