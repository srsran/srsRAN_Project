
#ifndef SRSGNB_TASK_SCHEDULER_H
#define SRSGNB_TASK_SCHEDULER_H

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
    exec.execute([this, suspending_awaitable]() mutable { suspending_awaitable.resume(); });
    return false;
  }

  void await_resume() {}

  task_executor_awaiter& get_awaiter() { return *this; }

private:
  TaskExecutor& exec;
};

} // namespace detail

template <typename TaskExecutor>
auto schedule_on(TaskExecutor& exec) -> detail::task_executor_awaiter<TaskExecutor>
{
  return {exec};
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
        CORO_AWAIT(schedule_on(dispatch_exec));
        task();
        CORO_AWAIT(schedule_on(return_exec));
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
  ReturnType ret;
  return launch_async([&return_exec, &dispatch_exec, task = std::forward<Callable>(callable), ret](
                          coro_context<async_task<ReturnType> >& ctx) mutable {
    CORO_BEGIN(ctx);
    CORO_AWAIT(schedule_on(dispatch_exec));
    ret = task();
    CORO_AWAIT(schedule_on(return_exec));
    CORO_RETURN(ret);
  });
}

} // namespace srsgnb

#endif // SRSGNB_TASK_SCHEDULER_H
