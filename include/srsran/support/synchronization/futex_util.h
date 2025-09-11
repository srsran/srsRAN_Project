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

#include <atomic>
#include <cstdint>

namespace srsran {
namespace futex_util {

/// \brief Suspends running thread if state == expected. Otherwise, it returns immediately.
/// \return 0 on success (state != expected or spurious wake-up), -1 on error (errno is set accordingly).
/// \remark Spurious wakeups or EINTR can happen. So callers need to re-check the condition in a loop.
long wait(std::atomic<uint32_t>& state, uint32_t expected);

/// \brief Wakes all threads waiting on the state variable.
/// \return number of awoken threads or -1 on error (errno is set accordingly).
long wake_all(std::atomic<uint32_t>& state);

} // namespace futex_util
} // namespace srsran
