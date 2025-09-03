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
