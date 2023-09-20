/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/srslog/srslog.h"
#include <signal.h>
#include <sys/prctl.h>

namespace srsran {

/// \brief Command to terminate application with an error message, ensuring first that the log is flushed.
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool srsran_terminate(const char* fmt, Args&&... args) noexcept
{
  srslog::flush();
  ::fflush(stdout);
  fmt::print(stderr, fmt, std::forward<Args>(args)...);
  std::abort();
}

/// \brief Reports an error and closes the application gracefully. This function is intended to be used for error
/// conditions that may be triggered by the user or through invalid configurations.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_error(const char* reason_fmt, Args&&... args) noexcept
{
  srslog::flush();
  ::fflush(stdout);
  fmt::print(stderr, "srsGNB ERROR: {}\n", fmt::format(reason_fmt, std::forward<Args>(args)...));

  // Disable coredump.
  int ret = prctl(PR_SET_DUMPABLE, 0, 0, 0, 0);
  if (ret != 0) {
    fmt::print(stderr, "Could not disable coredump: {}\n", strerror(errno));
  }

  // Disable backtrace for SIGABRT.
  signal(SIGABRT, SIG_DFL);
  std::abort();
}

/// \brief Reports a fatal error and handles the application shutdown. This function is intended to be used for
/// error conditions that aren't neither caught by the compiler nor possible to handle by the application at runtime.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_fatal_error(const char* reason_fmt, Args&&... args) noexcept
{
  srsran_terminate("srsGNB FATAL ERROR: {}\n", fmt::format(reason_fmt, std::forward<Args>(args)...));
}

/// \brief Verifies if condition is true. If not, report a fatal error and close application.
#define report_fatal_error_if_not(condition, fmtstr, ...)                                                              \
  (void)((condition) || (report_fatal_error(fmtstr, ##__VA_ARGS__), 0))

/// \brief Verifies if condition is true. If not, report an error and close application.
#define report_error_if_not(condition, fmtstr, ...) (void)((condition) || (report_error(fmtstr, ##__VA_ARGS__), 0))

} // namespace srsran
