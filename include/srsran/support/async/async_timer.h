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

namespace srsran {

/// \brief Returns an awaitable object that is only ready when the passed duration_msec has elapsed.
///
/// \param timer unique_timer object that is used to set the duration and timeout callback.
/// \param duration_msec duration in msec until the timer gets triggered.
/// \return awaitable object which returns true on resume if timer was notify_stop, and false if it expired.
template <typename UniqueTimer>
auto async_wait_for(UniqueTimer&& timer, std::chrono::milliseconds duration_msec)
{
  class async_timer
  {
  public:
    async_timer(UniqueTimer timer_, std::chrono::milliseconds duration_) :
      timer(std::forward<UniqueTimer>(timer_)), duration(duration_)
    {
      timer.stop();
    }

    bool await_ready() const { return duration.count() == 0; }

    void await_suspend(coro_handle<> suspending_awaitable)
    {
      timer.set(duration, [ch = suspending_awaitable](timer_id_t tid) mutable { ch.resume(); });
      timer.run();
    }

    bool await_resume() { return not timer.has_expired(); }

    async_timer& get_awaiter() { return *this; }

  private:
    UniqueTimer               timer;
    std::chrono::milliseconds duration;
  };

  return async_timer{std::forward<UniqueTimer>(timer), duration_msec};
}

} // namespace srsran
