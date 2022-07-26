/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

/// \brief Command to terminate application with an error message, ensuring first that the log is flushed.
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool srsgnb_terminate(const char* fmt, Args&&... args) noexcept
{
  srslog::flush();
  fmt::print(stderr, fmt, std::forward<Args>(args)...);
  std::abort();
}

/// \brief Reports a fatal error and handles the application shutdown. This function is intended to be used for
/// error conditions that aren't neither caught by the compiler nor possible to handle by the application at runtime.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_fatal_error(const char* reason_fmt, Args&&... args) noexcept
{
  srsgnb_terminate("FATAL: {}", fmt::format(reason_fmt, std::forward<Args>(args)...));
}

/// \brief Verifies if condition is true. If not, report a fatal error and close application.
#define REPORT_FATAL_ERROR_IF_NOT(condition, fmtstr, ...)                                                              \
  (void)((condition) || (report_fatal_error(fmtstr, ##__VA_ARGS__), 0))

} // namespace srsgnb