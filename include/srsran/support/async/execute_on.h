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

#include "srsran/support/async/async_task.h"
#include "srsran/support/async/detail/function_signature.h"
#include <thread>

namespace srsran {
namespace detail {

template <typename TaskExecutor, bool UseExecute>
struct try_execute_on_awaiter {
  explicit try_execute_on_awaiter(TaskExecutor& exec_) : exec(exec_) {}

  bool await_ready() noexcept { return false; }

  void await_suspend(coro_handle<> suspending_awaitable)
  {
    auto task = [this, suspending_awaitable]() mutable {
      success = true;
      suspending_awaitable.resume();
    };

    bool res = UseExecute ? exec.execute(task) : exec.defer(task);
    if (not res) {
      // Failed to dispatch task. Resume it from current thread, but with "success == false".
      success = false;
      suspending_awaitable.resume();
    }
  }

  [[nodiscard]] bool await_resume() { return success; }

  try_execute_on_awaiter& get_awaiter() { return *this; }

private:
  TaskExecutor& exec;
  bool          success = false;
};

template <typename TaskExecutor, bool UseExecute>
struct blocking_execute_on_awaiter {
  explicit blocking_execute_on_awaiter(TaskExecutor& exec_) : exec(exec_) {}

  bool await_ready() noexcept { return false; }

  void await_suspend(coro_handle<> suspending_awaitable)
  {
    auto task = [suspending_awaitable]() mutable { suspending_awaitable.resume(); };

    while (not(UseExecute ? exec.execute(task) : exec.defer(task))) {
      // Keep trying until it succeeds.
      std::this_thread::yield();
    }
  }

  void await_resume() {}

  blocking_execute_on_awaiter& get_awaiter() { return *this; }

private:
  TaskExecutor& exec;
};

} // namespace detail

/// Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call to execute
/// fails, the awaitable will return false.
template <typename TaskExecutor>
[[nodiscard]] auto try_execute_on(TaskExecutor& exec)
{
  return detail::try_execute_on_awaiter<TaskExecutor, true>{exec};
}

/// Returns an awaitable that resumes the suspended coroutine in a different execution context. If the call to defer
/// fails, the awaitable will return false.
template <typename TaskExecutor>
[[nodiscard]] auto try_defer_to(TaskExecutor& exec)
{
  return detail::try_execute_on_awaiter<TaskExecutor, false>{exec};
}

/// \brief Returns an awaitable that performs a task in a "dispatch_exec" executor, and resumes in "resume_exec"
/// executor.
///
/// Note: This is a specialization for tasks with non-void return values.
/// \param dispatch_exec Task executor where given task is going to run.
/// \param resume_exec   Task executor where coroutine will resume once the task is run.
/// \param callable      Task to run.
/// \return Awaitable.
template <typename DispatchTaskExecutor, typename ResumeTaskExecutor, typename Callable>
[[nodiscard]] auto
try_offload_to_executor(DispatchTaskExecutor& dispatch_exec, ResumeTaskExecutor& resume_exec, Callable&& callable)
{
  using result_type = detail::function_return_t<decltype(&std::decay_t<Callable>::operator())>;

  if constexpr (std::is_same_v<result_type, void>) {
    // Void return case.
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

  // Non-void return case.
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

    result_type await_resume() { return result; }

    task_executor_offloader& get_awaiter() { return *this; }

  private:
    DispatchTaskExecutor& dispatch_exec;
    ResumeTaskExecutor&   resume_exec;
    Callable              task;
    result_type           result;
    coro_handle<>         continuation;
  };

  return task_executor_offloader{dispatch_exec, resume_exec, std::forward<Callable>(callable)};
}

} // namespace srsran
