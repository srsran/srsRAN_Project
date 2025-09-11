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

#include "srsran/support/rtsan.h"
#include "fmt/base.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>

namespace srsran {

/// Callback type for the error handler.
using srsran_error_handler = void (*)();
/// Error handler instance.
inline std::atomic<srsran_error_handler> error_report_handler = nullptr;

/// Sets a new error handler to be used whenever a serious (non-recoverable) error is encountered.
inline void set_error_handler(srsran_error_handler handler)
{
  error_report_handler = handler;
}

/// \brief Command to terminate application with an error message.
///
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool srsran_terminate(const char* reason_fmt, Args&&... args) noexcept
{
  SRSRAN_RTSAN_SCOPED_DISABLER(d);

  if (auto handler = error_report_handler.exchange(nullptr)) {
    handler();
  }
  ::fflush(stdout);
  fmt::print(stderr, "srsRAN FATAL ERROR: ");
  fmt::println(stderr, reason_fmt, std::forward<Args>(args)...);

  std::abort();
}

/// \brief Reports an error and closes the application gracefully.
///
/// This function is intended to be used for error conditions that may be triggered by the user or through invalid
/// configurations.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_error(const char* reason_fmt, Args&&... args) noexcept
{
  SRSRAN_RTSAN_SCOPED_DISABLER(d);

  if (auto handler = error_report_handler.exchange(nullptr)) {
    handler();
  }
  ::fflush(stdout);
  fmt::print(stderr, "srsRAN ERROR: ");
  fmt::println(stderr, reason_fmt, std::forward<Args>(args)...);

  std::quick_exit(1);
}

/// \brief Reports a fatal error and handles the application shutdown.
///
/// This function is intended to be used for error conditions that are neither caught by the compiler nor possible to
/// be handled by the application at runtime.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_fatal_error(const char* reason_fmt, Args&&... args) noexcept
{
  srsran_terminate(reason_fmt, std::forward<Args>(args)...);
}

/// \brief Verifies if condition is true. If not, report a fatal error and closes the application.
#define report_fatal_error_if_not(condition, fmtstr, ...)                                                              \
  (void)((condition) || (report_fatal_error(fmtstr, ##__VA_ARGS__), 0))

/// \brief Verifies if condition is true. If not, report an error and close the application.
#define report_error_if_not(condition, fmtstr, ...) (void)((condition) || (report_error(fmtstr, ##__VA_ARGS__), 0))

} // namespace srsran
