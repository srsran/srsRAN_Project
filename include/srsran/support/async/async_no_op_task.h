/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/async/async_task.h"

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
