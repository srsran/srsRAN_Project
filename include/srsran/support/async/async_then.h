/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/support/async/coroutine.h"

namespace srsran {

template <typename Task>
Task async_then(Task&& first, Task&& second)
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
