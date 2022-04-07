
#ifndef SRSGNB_ASYNC_TASK_H
#define SRSGNB_ASYNC_TASK_H

#include "coroutine.h"
#include "detail/base_task.h"

namespace srsgnb {

/// Non-lazy coroutine type that outputs a result R when finished.
/// \tparam R Result of the task
template <typename R>
class async_task : public detail::task_crtp<async_task<R>, R>
{
  using base_type = detail::task_crtp<async_task<R>, R>;

public:
  using result_type = R;

  struct promise_type : public detail::promise_data<result_type, detail::task_promise_base<suspend_never> > {
    async_task<R> get_return_object()
    {
      auto corohandle = coro_handle<promise_type>::from_promise(this);
      corohandle.resume();
      return async_task<R>{corohandle};
    }
  };

  async_task() = default;
  explicit async_task(coro_handle<promise_type> cb) : handle(cb) {}

  /// awaiter interface.
  async_task<R>& get_awaiter() { return *this; }
  bool           await_suspend(coro_handle<> cb)
  {
    srsran_sanity_check(not this->empty(), "Awaiting an empty base_task");
    return handle.promise().set_continuation(cb);
  }

private:
  friend class detail::task_crtp<async_task<R>, R>;

  unique_coroutine<promise_type> handle;
};

/// Base class of non-coroutine resumable tasks
template <typename R>
using async_procedure = base_resumable_procedure<async_task<R> >;

/// Lazy coroutine type that outputs a result R when finished.
/// \tparam R Result of the task
template <typename R>
class lazy_task : public detail::task_crtp<lazy_task<R>, R>
{
  using base_type = detail::task_crtp<lazy_task<R>, R>;

public:
  using result_type = R;

  struct promise_type : public detail::promise_data<result_type, detail::task_promise_base<suspend_always> > {
    lazy_task<R> get_return_object()
    {
      auto corohandle = coro_handle<promise_type>::from_promise(this);
      corohandle.resume();
      return lazy_task<R>{corohandle};
    }
  };

  lazy_task() = default;
  explicit lazy_task(coro_handle<promise_type> cb) : handle(cb) {}

  /// awaiter interface.
  lazy_task<R>& get_awaiter() { return *this; }
  bool          await_suspend(coro_handle<> cb)
  {
    srsran_sanity_check(not this->empty(), "Awaiting an empty base_task");
    bool c = handle.promise().set_continuation(cb);
    handle.resume();
    return c;
  }

private:
  friend class detail::task_crtp<lazy_task<R>, R>;

  unique_coroutine<promise_type> handle;
};

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TASK_H
