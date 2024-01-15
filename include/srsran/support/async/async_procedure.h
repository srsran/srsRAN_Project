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
#include "coroutine.h"

namespace srsran {

/// Base class for resumable tasks that are not coroutines
template <typename FutureType>
struct base_resumable_procedure {
  using future_type  = FutureType;
  using promise_type = typename FutureType::promise_type;
  using result_type  = typename FutureType::result_type;

  /// Initiation of the resumable task
  virtual void start() = 0;

  /// Resume method
  void operator()(detail::base_coro_frame<promise_type>& ctx)
  {
    if (frame_ptr == nullptr) {
      srsran_sanity_check(ctx.state_index == detail::tag_init, "Invalid coro state");
      ctx.state_index = 10;
      frame_ptr       = &ctx;
      async_await(suspend_always{}, &base_resumable_procedure<FutureType>::start);
      return;
    }
    resume_method();
  }

protected:
  /// Called by resumable task to await Awaitable object to finish
  template <typename Awaitable, typename Derived, typename... Args>
  void async_await(Awaitable&& a, void (Derived::*action)(Args...))
  {
    auto resume_func = [this, action]() {
      if (this->frame_ptr->state_index > 0) {
        await_resume_helper<Awaitable>(action);
      } else {
        srsran_sanity_check(frame_ptr->state_index != detail::coro_state_tag_t::tag_cancelled,
                            "Calling resume on cancelled task");
        srsran_sanity_check(frame_ptr->state_index != detail::coro_state_tag_t::tag_final_suspend,
                            "Calling on task on suspended point");
        // cancelled
        frame_ptr->template on_await_cancel<Awaitable>();
        frame_ptr->state_index = detail::coro_state_tag_t::tag_cancelled;
        frame_ptr->on_return();
        // Note: Do not touch any member at this point
      }
    };

    // Note: we have to store the resume point before, in case the coroutine changes execution context and calls resume
    // before on_await_call is complete
    resume_method = resume_func;
    if (frame_ptr->on_await_call(std::forward<Awaitable>(a))) {
      resume_func();
    }
  }

  /// Called on resumable task completion
  template <typename R = result_type>
  detail::enable_if_void<R> async_return()
  {
    frame_ptr->promise().return_value();
    final_await();
  }

  /// Called on resumable task completion with non-void return
  template <typename U, typename R = result_type>
  detail::enable_if_nonvoid<R, void> async_return(U&& r)
  {
    static_assert(std::is_convertible<U, result_type>::value, "Invalid value returned in async_return(R)");
    frame_ptr->promise().return_value(std::forward<U>(r));
    final_await();
  }

  ~base_resumable_procedure() = default;

private:
  void final_await()
  {
    // destroy procedure object including its members (without destroying the rest of the frame)
    auto* local_ptr        = frame_ptr;
    frame_ptr->state_index = detail::coro_state_tag_t::tag_final_suspend;
    frame_ptr->on_return();
    // Note: Do not touch any procedure variable from now on.

    // suspend on final awaiter
    if (local_ptr->on_await_call(local_ptr->promise().final_suspend())) {
      // destroy coroutine frame
      local_ptr->destroy();
    }
  }

  /// Called when Awaitable returns a non-void value
  template <typename Awaitable, typename Derived, typename Arg>
  detail::enable_if_nonvoid<detail::awaitable_result_t<Awaitable>, void>
  await_resume_helper(void (Derived::*action)(Arg))
  {
    (static_cast<Derived*>(this)->*action)(frame_ptr->template on_await_resume<Awaitable>());
  }

  /// Called when Awaitable returns void
  template <typename Awaitable, typename Derived>
  detail::enable_if_void<detail::awaitable_result_t<Awaitable>> await_resume_helper(void (Derived::*action)())
  {
    frame_ptr->template on_await_resume<Awaitable>();
    (static_cast<Derived*>(this)->*action)();
  }

  detail::base_coro_frame<promise_type>* frame_ptr = nullptr; ///< pointer to coroutine frame
  std::function<void()>                  resume_method;       ///< next method to call on resume
};

/// Base class of non-coroutine resumable tasks
template <typename R>
using async_procedure = base_resumable_procedure<async_task<R>>;

} // namespace srsran
