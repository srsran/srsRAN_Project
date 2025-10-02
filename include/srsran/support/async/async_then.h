/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/async/coroutine.h"

namespace srsran {

template <typename Task>
[[nodiscard]] Task async_then(Task&& first, Task&& second)
{
  return launch_async(
      [first = std::forward<Task>(first), second = std::forward<Task>(second)](coro_context<Task>& ctx) mutable {
        CORO_BEGIN(ctx);
        CORO_AWAIT(first);
        CORO_AWAIT(second);
        CORO_RETURN();
      });
}

} // namespace srsran
