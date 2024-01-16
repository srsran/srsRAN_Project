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

#include "coroutine.h"
#include "detail/base_task.h"

namespace srsran {

/// Eager coroutine type that outputs a result of type R, when completed.
/// \tparam R Result of the task
template <typename R>
class eager_async_task : public detail::common_task_crtp<eager_async_task<R>, R>
{
public:
  using result_type = R;

  struct promise_type : public detail::promise_data<result_type, detail::task_promise_base> {
    /// base_task final awaiter type. It runs pending continuations and suspends
    struct final_awaiter {
      promise_type* me;

      bool await_ready() const { return false; }

      /// Tail-resumes continuation, if it was previously stored via an AWAIT call.
      /// \param h suspending coroutine.
      coro_handle<> await_suspend(coro_handle<promise_type> h)
      {
        return not me->continuation.empty() ? me->continuation : noop_coroutine();
      }

      void await_resume() {}

      /// Points to itself as an awaiter
      final_awaiter& get_awaiter() { return *this; }
    };

    /// Initial suspension awaiter. Eager tasks never suspend at initial suspension point.
    suspend_never initial_suspend() { return {}; }

    /// Final suspension awaiter
    final_awaiter final_suspend() { return {this}; }

    eager_async_task<R> get_return_object()
    {
      auto corohandle = coro_handle<promise_type>::from_promise(this);
      corohandle.resume();
      return eager_async_task<R>{corohandle};
    }
  };

  eager_async_task() = default;
  explicit eager_async_task(coro_handle<promise_type> cb) : handle(cb) {}

  /// awaiter interface.
  eager_async_task<R>& get_awaiter() { return *this; }

  /// \brief Register suspending coroutine as continuation of the current async_task.
  /// Given that "this" task type is eager, it can be at any suspension point when await_suspend is called.
  /// \param h suspending coroutine that is calling await_suspend.
  void await_suspend(coro_handle<> h) noexcept
  {
    srsran_sanity_check(not this->empty(), "Awaiting an empty async_task");
    srsran_sanity_check(handle.promise().continuation.empty(), "Async task can only be awaited once.");
    handle.promise().continuation = h;
  }

private:
  friend class detail::common_task_crtp<eager_async_task<R>, R>;

  unique_coroutine<promise_type> handle;
};

} // namespace srsran
