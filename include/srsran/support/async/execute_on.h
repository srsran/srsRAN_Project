/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "async_task.h"
#include "detail/function_signature.h"

namespace srsran {

/// Awaiter that switches to execution context provided by given executor.
template <typename TaskExecutor>
auto execute_on(TaskExecutor& exec)
{
  struct task_executor_awaiter {
    task_executor_awaiter(TaskExecutor& exec_) : exec(exec_) {}

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      bool res = exec.execute([this, suspending_awaitable]() mutable {
        success = true;
        suspending_awaitable.resume();
      });
      if (not res) {
        // Failed to dispatch task. Resume it from current thread, but with "success == false".
        success = false;
        suspending_awaitable.resume();
      }
    }

    bool await_resume() { return success; }

    task_executor_awaiter& get_awaiter() { return *this; }

  private:
    TaskExecutor& exec;
    bool          success;
  };

  return task_executor_awaiter{exec};
}

/// Awaiter that defers continuation to execution context provided by given executor.
template <typename TaskExecutor>
auto defer_to(TaskExecutor& exec)
{
  struct task_executor_awaiter {
    task_executor_awaiter(TaskExecutor& exec_) : exec(exec_) {}

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      bool res = exec.defer([this, suspending_awaitable]() mutable {
        success = true;
        suspending_awaitable.resume();
      });
      if (not res) {
        // Failed to dispatch task. Resume it from current thread, but with "success == false".
        success = false;
        suspending_awaitable.resume();
      }
    }

    bool await_resume() { return success; }

    task_executor_awaiter& get_awaiter() { return *this; }

  private:
    TaskExecutor& exec;
    bool          success;
  };

  return task_executor_awaiter{exec};
}

/// Returns an awaitable that performs a task in a "dispatch_exec" executor, and resumes in "resume_exec" executor.
/// Note: This is a specialization for tasks with non-void return values.
/// \param dispatch_exec task executor where given task is going to run.
/// \param resume_exec task executor where coroutine will resume once the task is run.
/// \param callable task to run.
/// \return awaitable.
template <typename DispatchTaskExecutor,
          typename ResumeTaskExecutor,
          typename Callable,
          typename ResultType = detail::function_return_t<decltype(&std::decay_t<Callable>::operator())>,
          std::enable_if_t<not std::is_same<ResultType, void>::value, int> = 0>
auto offload_to_executor(DispatchTaskExecutor& dispatch_exec, ResumeTaskExecutor& resume_exec, Callable&& callable)
{
  struct task_executor_offloader {
    task_executor_offloader(DispatchTaskExecutor& dispatch_exec_,
                            ResumeTaskExecutor&   resume_exec_,
                            Callable              callable_) :
      dispatch_exec(dispatch_exec_), resume_exec(resume_exec_), task(std::forward<Callable>(callable_))
    {
    }

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_coro)
    {
      continuation     = suspending_coro;
      bool dispatched1 = dispatch_exec.execute([this]() mutable {
        result           = task();
        bool dispatched2 = resume_exec.execute([this]() { continuation.resume(); });
        srsran_assert(dispatched2, "Failed to dispatch task");
      });
      srsran_assert(dispatched1, "Failed to dispatch task");
    }

    ResultType await_resume() { return result; }

    task_executor_offloader& get_awaiter() { return *this; }

  private:
    DispatchTaskExecutor& dispatch_exec;
    ResumeTaskExecutor&   resume_exec;
    Callable              task;
    ResultType            result;
    coro_handle<>         continuation;
  };
  return task_executor_offloader{dispatch_exec, resume_exec, std::forward<Callable>(callable)};
}

template <typename DispatchTaskExecutor,
          typename ResumeTaskExecutor,
          typename Callable,
          typename ResultType = detail::function_return_t<decltype(&std::decay_t<Callable>::operator())>,
          std::enable_if_t<std::is_same<ResultType, void>::value, int> = 0>
auto offload_to_executor(DispatchTaskExecutor& dispatch_exec, ResumeTaskExecutor& resume_exec, Callable&& callable)
{
  struct task_executor_offloader {
    task_executor_offloader(DispatchTaskExecutor& dispatch_exec_,
                            ResumeTaskExecutor&   resume_exec_,
                            Callable              callable_) :
      dispatch_exec(dispatch_exec_), resume_exec(resume_exec_), task(std::forward<Callable>(callable_))
    {
    }

    bool await_ready() noexcept { return false; }

    void await_suspend(coro_handle<> suspending_coro)
    {
      continuation = suspending_coro;
      dispatch_exec.execute([this]() mutable {
        task();
        resume_exec.execute([this]() { continuation.resume(); });
      });
    }

    void await_resume() {}

    task_executor_offloader& get_awaiter() { return *this; }

  private:
    DispatchTaskExecutor& dispatch_exec;
    ResumeTaskExecutor&   resume_exec;
    Callable              task;
    coro_handle<>         continuation;
  };

  return task_executor_offloader{dispatch_exec, resume_exec, std::forward<Callable>(callable)};
}

template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())>>
std::enable_if_t<std::is_same<ReturnType, void>::value, async_task<void>>
dispatch_and_resume_on(DispatchTaskExecutor& dispatch_exec, CurrentTaskExecutor& return_exec, Callable&& callable)
{
  return launch_async(
      [&return_exec, &dispatch_exec, task = std::forward<Callable>(callable)](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(execute_on(dispatch_exec));
        task();
        CORO_AWAIT(execute_on(return_exec));
        CORO_RETURN();
      });
}

template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())>>
std::enable_if_t<not std::is_same<ReturnType, void>::value, async_task<ReturnType>>
dispatch_and_resume_on(DispatchTaskExecutor& dispatch_exec, CurrentTaskExecutor& return_exec, Callable&& callable)
{
  ReturnType ret{};
  return launch_async([&return_exec, &dispatch_exec, task = std::forward<Callable>(callable), ret](
                          coro_context<async_task<ReturnType>>& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(execute_on(dispatch_exec));
    ret = task();
    CORO_AWAIT(execute_on(return_exec));
    CORO_RETURN(ret);
  });
}

} // namespace srsran
