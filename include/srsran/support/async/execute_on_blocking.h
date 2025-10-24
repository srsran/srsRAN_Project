/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/noop_functor.h"
#include "srsran/support/async/execute_on.h"
#include "srsran/support/timers.h"

namespace srsran {
namespace detail {

template <bool IsExecute, typename TaskExecutor, typename OnFailureToDispatch>
[[nodiscard]] auto dispatch_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure)
{
  struct blocking_dispatch_on_awaiter {
    blocking_dispatch_on_awaiter(TaskExecutor& exec_, timer_manager& timers_, OnFailureToDispatch&& on_failure_) :
      exec(exec_), timers(timers_), on_failure(std::forward<OnFailureToDispatch>(on_failure_))
    {
    }

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
      on_failure();

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
    TaskExecutor&       exec;
    timer_manager&      timers;
    OnFailureToDispatch on_failure;
    unique_timer        retry_timer;
  };

  return blocking_dispatch_on_awaiter{exec, timers, std::forward<OnFailureToDispatch>(on_failure)};
}

template <bool IsExecute,
          typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename OnFailureToDispatch = noop_operation>
[[nodiscard]] auto dispatch_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                                     CurrentTaskExecutor&  return_exec,
                                                     timer_manager&        timers,
                                                     Callable&&            callable,
                                                     OnFailureToDispatch&& on_failure = noop_operation{})
{
  if constexpr (std::is_invocable_v<Callable>) {
    // The task is a callable object.
    using return_type = std::invoke_result_t<Callable>;

    if constexpr (std::is_same_v<return_type, void>) {
      // CASE: callable has the signature void().
      return launch_async([&return_exec,
                           &dispatch_exec,
                           task       = std::forward<Callable>(callable),
                           on_failure = std::forward<OnFailureToDispatch>(on_failure),
                           &timers](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Dispatch execution context switch.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(dispatch_exec, timers, on_failure));

        // Run task.
        task();

        // Continuation in the original executor.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(return_exec, timers, on_failure));

        CORO_RETURN();
      });
    } else {
      // CASE: callable has the signature R(), where R is the non-void "return_type".
      return launch_async([&return_exec,
                           &dispatch_exec,
                           task       = std::forward<Callable>(callable),
                           on_failure = std::forward<OnFailureToDispatch>(on_failure),
                           &timers,
                           ret = return_type{}](coro_context<async_task<return_type>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Dispatch execution context switch.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(dispatch_exec, timers, on_failure));

        // Run task.
        ret = task();

        // Continuation in the original executor.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(return_exec, timers, on_failure));

        CORO_RETURN(ret);
      });
    }
  } else {
    // The task is an awaitable.
    using return_type = awaitable_result_t<Callable>;

    if constexpr (std::is_same_v<return_type, void>) {
      // CASE: callback is an awaitable with "return_type" as void.
      return launch_async([&return_exec,
                           &dispatch_exec,
                           task       = std::forward<Callable>(callable),
                           on_failure = std::forward<OnFailureToDispatch>(on_failure),
                           &timers](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Dispatch execution context switch.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(dispatch_exec, timers, on_failure));

        // Run task.
        CORO_AWAIT(task);

        // Continuation in the original executor.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(return_exec, timers, on_failure));

        CORO_RETURN();
      });
    } else {
      // CASE: callback is an awaitable with "return_type" non-void.
      return launch_async([&return_exec,
                           &dispatch_exec,
                           task       = std::forward<Callable>(callable),
                           on_failure = std::forward<OnFailureToDispatch>(on_failure),
                           &timers,
                           ret = return_type{}](coro_context<async_task<return_type>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Dispatch execution context switch.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(dispatch_exec, timers, on_failure));

        // Run task.
        CORO_AWAIT_VALUE(ret, task);

        // Continuation in the original executor.
        CORO_AWAIT(dispatch_on_blocking<IsExecute>(return_exec, timers, on_failure));

        CORO_RETURN(ret);
      });
    }
  }
}

} // namespace detail

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call
/// to execute fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
///
/// \param[in] exec Executor used to dispatch coroutine to a new execution context.
/// \param[in] timers Timer service used to handle reattempts to dispatch task to new execution context.
/// \param[in] on_failure Callback invoked in case the dispatch to executor fails at first attempt.
template <typename TaskExecutor, typename OnFailureToDispatch = noop_operation>
[[nodiscard]] auto
execute_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_on_blocking<true, TaskExecutor, OnFailureToDispatch>(
      exec, timers, std::forward<OnFailureToDispatch>(on_failure));
}

/// \brief Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call to
/// defer fails, the awaitable yields and will retry the dispatch at a later point, until it succeeds.
///
/// \param[in] exec Executor used to dispatch coroutine to a new execution context.
/// \param[in] timers Timer service used to handle reattempts to dispatch task to new execution context.
/// \param[in] on_failure Callback invoked in case the dispatch to executor fails at first attempt.
template <typename TaskExecutor, typename OnFailureToDispatch = noop_operation>
[[nodiscard]] auto
defer_on_blocking(TaskExecutor& exec, timer_manager& timers, OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_on_blocking<false, TaskExecutor, OnFailureToDispatch>(
      exec, timers, std::forward<OnFailureToDispatch>(on_failure));
}

/// Returns an async_task<ReturnType> that runs a given invocable task in a \c dispatch_exec executor, and once the task
/// is complete, it resumes the suspended coroutine in a \c return_exec executor.
template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename OnFailureToDispatch = noop_operation>
[[nodiscard]] auto execute_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                                    CurrentTaskExecutor&  return_exec,
                                                    timer_manager&        timers,
                                                    Callable&&            callable,
                                                    OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_and_continue_on_blocking<true>(dispatch_exec,
                                                         return_exec,
                                                         timers,
                                                         std::forward<Callable>(callable),
                                                         std::forward<OnFailureToDispatch>(on_failure));
}

template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename OnFailureToDispatch = noop_operation>
[[nodiscard]] auto defer_and_continue_on_blocking(DispatchTaskExecutor& dispatch_exec,
                                                  CurrentTaskExecutor&  return_exec,
                                                  timer_manager&        timers,
                                                  Callable&&            callable,
                                                  OnFailureToDispatch&& on_failure = noop_operation{})
{
  return detail::dispatch_and_continue_on_blocking<false>(dispatch_exec,
                                                          return_exec,
                                                          timers,
                                                          std::forward<Callable>(callable),
                                                          std::forward<OnFailureToDispatch>(on_failure));
}

} // namespace srsran
