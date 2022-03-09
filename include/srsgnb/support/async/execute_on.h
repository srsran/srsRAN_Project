
#ifndef SRSGNB_SUPPORT_ASYNC_EXECUTOR_ON_H
#define SRSGNB_SUPPORT_ASYNC_EXECUTOR_ON_H

#include "async_task.h"
#include "detail/function_signature.h"

namespace srsgnb {

namespace detail {

template <typename TaskExecutor>
struct task_executor_awaiter {
  task_executor_awaiter(TaskExecutor& exec_) : exec(exec_) {}

  bool await_ready() noexcept { return false; }

  bool await_suspend(coro_handle<> suspending_awaitable)
  {
    exec.execute([suspending_awaitable]() mutable { suspending_awaitable.resume(); });
    return false;
  }

  void await_resume() {}

  task_executor_awaiter& get_awaiter() { return *this; }

private:
  TaskExecutor& exec;
};

template <typename Task>
std::enable_if_t<not std::is_same<detail::function_return_t<Task>, void>::value> call_helper(Task&& t)
{
  return t();
}

template <typename Task>
std::enable_if_t<std::is_same<detail::function_return_t<Task>, bool>::value> call_helper(Task&& t)
{
  t();
  return true;
}

template <typename DispatchTaskExecutor,
          typename ResumeTaskExecutor,
          typename Callable,
          typename ResultType = detail::function_return_t<decltype(&std::decay_t<Callable>::operator())> >
struct task_executor_offloader {
  task_executor_offloader(DispatchTaskExecutor& dispatch_exec_, ResumeTaskExecutor& resume_exec_, Callable callable_) :
    dispatch_exec(dispatch_exec_), resume_exec(resume_exec_), task(std::forward<Callable>(callable_))
  {}

  bool await_ready() noexcept { return false; }

  bool await_suspend(coro_handle<> suspending_coro)
  {
    continuation = suspending_coro;
    dispatch_exec.execute([this]() mutable {
      result = task();
      resume_exec.execute([this]() { continuation.resume(); });
    });
    return false;
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

template <typename DispatchTaskExecutor, typename ResumeTaskExecutor, typename Callable>
struct task_executor_offloader<DispatchTaskExecutor, ResumeTaskExecutor, Callable, void> {
  task_executor_offloader(DispatchTaskExecutor& dispatch_exec_, ResumeTaskExecutor& resume_exec_, Callable callable_) :
    dispatch_exec(dispatch_exec_), resume_exec(resume_exec_), task(std::forward<Callable>(callable_))
  {}

  bool await_ready() noexcept { return false; }

  bool await_suspend(coro_handle<> suspending_coro)
  {
    continuation = suspending_coro;
    dispatch_exec.execute([this]() mutable {
      task();
      resume_exec.execute([this]() { continuation.resume(); });
    });
    return false;
  }

  void await_resume() {}

  task_executor_offloader& get_awaiter() { return *this; }

private:
  DispatchTaskExecutor& dispatch_exec;
  ResumeTaskExecutor&   resume_exec;
  Callable              task;
  coro_handle<>         continuation;
};

} // namespace detail

/// Awaiter that switches to execution context provided by given executor
template <typename TaskExecutor>
auto execute_on(TaskExecutor& exec) -> detail::task_executor_awaiter<TaskExecutor>
{
  return {exec};
}

template <typename DispatchTaskExecutor, typename CurrentTaskExecutor, typename Callable>
auto offload_to_executor(DispatchTaskExecutor& dispatch_exec, CurrentTaskExecutor& resume_exec, Callable&& callable)
    -> detail::task_executor_offloader<DispatchTaskExecutor, CurrentTaskExecutor, Callable>
{
  return {dispatch_exec, resume_exec, std::forward<Callable>(callable)};
}

template <typename DispatchTaskExecutor,
          typename CurrentTaskExecutor,
          typename Callable,
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())> >
std::enable_if_t<std::is_same<ReturnType, void>::value, async_task<void> >
dispatch_and_resume_on(DispatchTaskExecutor& dispatch_exec, CurrentTaskExecutor& return_exec, Callable&& callable)
{
  return launch_async(
      [&return_exec, &dispatch_exec, task = std::forward<Callable>(callable)](coro_context<async_task<void> >& ctx) {
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
          typename ReturnType = detail::function_return_t<decltype(&Callable::operator())> >
std::enable_if_t<not std::is_same<ReturnType, void>::value, async_task<ReturnType> >
dispatch_and_resume_on(DispatchTaskExecutor& dispatch_exec, CurrentTaskExecutor& return_exec, Callable&& callable)
{
  ReturnType ret{};
  return launch_async([&return_exec, &dispatch_exec, task = std::forward<Callable>(callable), ret](
                          coro_context<async_task<ReturnType> >& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(execute_on(dispatch_exec));
    ret = task();
    CORO_AWAIT(execute_on(return_exec));
    CORO_RETURN(ret);
  });
}

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_ASYNC_EXECUTOR_ON_H
