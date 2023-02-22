/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/srslog/srslog.h"

namespace srsran {

/// \brief Command to terminate application with an error message, ensuring first that the log is flushed.
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool srsran_terminate(const char* fmt, Args&&... args) noexcept
{
  srslog::flush();
  fmt::print(stderr, fmt, std::forward<Args>(args)...);
  std::abort();
}

/// \brief Reports an error and closes the application gracefully. This function is intended to be used for error
/// conditions that may be triggered by the user or through invalid configurations.
template <typename... Args>
[[gnu::noinline, noreturn]] inline void report_error(const char* reason_fmt, Args&&... args) noexcept
{
  srslog::flush();
  fmt::print(stderr, "srsGNB ERROR: {}", fmt::format(reason_fmt, std::forward<Args>(args)...));
  std::exit(1);
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

} // namespace srsran
