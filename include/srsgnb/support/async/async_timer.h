/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "coroutine.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

/// \brief Returns an awaitable object that is only ready when the passed duration_msec has elapsed.
///
/// \param timer unique_timer object that is used to set the duration and timeout callback.
/// \param duration_msec duration in msec until the timer gets triggered.
/// \return awaitable object which returns true on resume if timer was stopped, and false if it expired.
template <typename UniqueTimer>
auto async_wait_for(UniqueTimer&& timer, unsigned duration_msec)
{
  class async_timer
  {
  public:
    async_timer(UniqueTimer timer_, unsigned duration_) : timer(std::forward<UniqueTimer>(timer_)), duration(duration_)
    {
      timer.stop();
    }

    bool await_ready() const { return duration == 0; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      timer.set(duration, [ch = suspending_awaitable](unsigned tid) mutable { ch.resume(); });
      timer.run();
    }

    bool await_resume() { return not timer.has_expired(); }

    async_timer& get_awaiter() { return *this; }

  private:
    UniqueTimer timer;
    unsigned    duration;
  };

  return async_timer{std::forward<UniqueTimer>(timer), duration_msec};
}

} // namespace srsgnb
