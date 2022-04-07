
#ifndef SRSGNB_BASE_TASK_H
#define SRSGNB_BASE_TASK_H

#include "../coroutine.h"
#include "promise_data.h"
#include "unique_coroutine.h"

namespace srsgnb {

namespace detail {

/// Base class of async/lazy task promises
template <typename InitialSuspend>
struct task_promise_base {
  bool          has_value = false; ///< If result of task has been stored
  coro_handle<> continuation;      ///< Pending continuation task

  /// base_task final awaiter type. It runs pending continuations and suspends
  struct final_awaiter {
    task_promise_base* me;

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
  InitialSuspend initial_suspend() { return {}; }

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

template <typename Derived, typename Result>
class task_crtp
{
public:
  using result_type = Result;

  /// Checks if async task has an handle.
  bool empty() const { return derived().handle.empty(); }

  /// Returns true if base_task is complete
  bool ready() const { return empty() or derived().handle.promise().ready(); }

  /// Called to get result of task once it is complete
  template <typename Res = std::decay_t<result_type>, std::enable_if_t<not std::is_same<Res, void>::value, bool> = true>
  const Res& get() &
  {
    srsran_assert(not empty() and derived().handle.promise().ready(), "Called task::get() for task that is not ready");
    return derived().handle.promise().get();
  }
  template <typename Res = std::decay_t<result_type>, std::enable_if_t<not std::is_same<Res, void>::value, bool> = true>
  Res get() &&
  {
    srsran_assert(not empty() and derived().handle.promise().ready(), "Called task::get() for task that is not ready");
    return std::move(derived().handle.promise()).get();
  }

  /// Awaiter interface.
  bool await_ready() const noexcept { return derived().handle.empty() or derived().handle.promise().ready(); }
  template <typename Res = result_type>
  detail::enable_if_nonvoid<Res> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty base_task");
    return derived().handle.promise().get();
  }
  template <typename Res = result_type>
  detail::enable_if_void<Res> await_resume()
  {
    srsran_sanity_check(not empty(), "Resuming an empty base_task");
  }

private:
  Derived&       derived() { return static_cast<Derived&>(*this); }
  const Derived& derived() const { return static_cast<const Derived&>(*this); }
};

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_BASE_TASK_H
