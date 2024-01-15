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

namespace srsran {

/// Launches an async task that finishes immediately with the result set by the caller to this function.
template <typename Resp>
async_task<std::decay_t<Resp>> launch_no_op_task(Resp&& r)
{
  return launch_async([r = std::forward<Resp>(r)](coro_context<async_task<Resp>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN(r);
  });
}

/// Launches an async task that finishes immediately with the result set by the caller to this function.
inline async_task<void> launch_no_op_task()
{
  return launch_async([](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);
    CORO_RETURN();
  });
}

} // namespace srsran
