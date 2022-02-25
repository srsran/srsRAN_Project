
#ifndef SRSGNB_ASYNC_TASK_H
#define SRSGNB_ASYNC_TASK_H

#include "coroutine.h"

namespace srsgnb {

namespace detail {

/// Class to hold task promise Result value
template <typename R, typename Base>
struct promise_data : public Base {
  /// Set Return value
  template <typename U>
  void return_value(U&& u)
  {
    mem             = std::forward<U>(u);
    Base::has_value = true;
  }

  /// Get Return value
  const R& get() const&
  {
    srsran_assert(Base::has_value, "Trying to extract result from unset Promise");
    return mem;
  }
  R get() &&
  {
    srsran_assert(Base::has_value, "Trying to extract result from unset Promise");
    return std::move(mem);
  }

private:
  R mem; ///< stored result.
};

/// Specialization for when Return type is void
template <typename Base>
struct promise_data<void, Base> : public Base {
  void return_value() { Base::has_value = true; }
};

/// Base class of async_task promises
struct async_task_promise_base {
  bool          has_value = false; ///< If result of task has been stored
  coro_handle<> continuation;      ///< Pending continuation task

  /// async_task final awaiter type. It runs pending continuations and suspends
  struct final_awaiter {
    async_task_promise_base* me;

    bool await_ready() const { return false; }

    /// resumes awaiting continuation(s) of task that just finished
    void await_suspend(coro_handle<> cb)
    {
      coro_handle<> old_then = std::exchange(me->continuation, nullptr);
      if (not old_then.empty()) {
        old_then.resume();
      }
    }

    void await_resume() {}

    /// Points to itself as an awaiter
    final_awaiter& get_awaiter() { return *this; }
  };

  /// Initial suspension awaiter
  suspend_never initial_suspend() { return {}; }

  /// Final suspension awaiter
  final_awaiter final_suspend() { return {this}; }

  /// Append continuation to task
  bool set_continuation(coro_handle<> promise_)
  {
    continuation = promise_;
    return true;
  }

  /// Check if return has been stored
  bool ready() const { return has_value; }
};

} // namespace detail

/// Non-lazy coroutine type that outputs a result T when finished
/// @tparam T Result of the task
template <typename T>
class async_task
{
public:
  using result_type = T;

  /// async_task promise type. It can be awaitable by one single other task
  class promise_type : public detail::promise_data<T, detail::async_task_promise_base>
  {
  public:
    /// Construct respective async_task from promise object and run it eagerly
    async_task<T> get_return_object()
    {
      coro_handle<promise_type> cb = coro_handle<promise_type>::from_promise(this);
      // Eargerly start async task
      cb.resume();
      return async_task<T>{cb};
    }
  };

  async_task() = default;
  explicit async_task(coro_handle<promise_type> cb) : handle(cb) {}
  async_task(const async_task&) = delete;
  async_task& operator=(const async_task&) = delete;
  async_task(async_task&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
  async_task& operator=(async_task&& other) noexcept
  {
    handle.destroy();
    handle       = other.handle;
    other.handle = nullptr;
    return *this;
  }
  ~async_task()
  {
    if (not empty()) {
      handle.destroy();
    }
  }

  // awaiter interface
  async_task<T>& get_awaiter() { return *this; }
  bool           await_ready() const noexcept { return handle.empty() or handle.promise().ready(); }
  bool           await_suspend(coro_handle<> cb)
  {
    srsran_sanity_check(not empty(), "Awaiting an empty async_task");
    return handle.promise().set_continuation(cb);
  }
  template <typename R = result_type>
  detail::enable_if_nonvoid<R> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty async_task");
    return handle.promise().get();
  }
  template <typename R = result_type>
  detail::enable_if_void<R> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty async_task");
  }

  /// Checks if async task has an handle
  bool empty() const { return handle.empty(); }

  /// Returns true if async_task is complete
  bool ready() const { return empty() or handle.promise().ready(); }

  /// Called to get result of task once it is complete
  template <bool enable = not std::is_same<result_type, void>::value>
  typename std::enable_if<enable, const std::decay_t<result_type>&>::type get() &
  {
    srsran_assert(not empty() and ready(), "Called async_task::get() for task that is not ready");
    return handle.promise().get();
  }
  template <bool enable = not std::is_same<result_type, void>::value>
  typename std::enable_if<enable, std::decay_t<result_type> >::type get() &&
  {
    srsran_assert(not empty() and ready(), "Called async_task::get() for task that is not ready");
    return std::move(handle.promise()).get();
  }

private:
  coro_handle<promise_type> handle;
};

/// Base class of non-coroutine resumable tasks
template <typename R>
using async_procedure = base_resumable_procedure<async_task<R> >;

} // namespace srsgnb

#endif // SRSGNB_ASYNC_TASK_H
