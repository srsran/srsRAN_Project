/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_ASSERT_H
#define SRSRAN_ASSERT_H

#include "srsgnb/srslog/srslog.h"
#include <cstdio>
#include <stdarg.h>

namespace srsgnb {

/// Provides a hint to the compiler that a condition is likely false.
#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

/// Verifies if compile-time symbol is defined.
#define SRSRAN_IS_DEFINED(x) SRSRAN_IS_DEFINED2(x)
#define SRSRAN_IS_DEFINED2(x) (#x[0] == 0 || (#x[0] >= '1' && #x[0] <= '9'))

namespace detail {

/// \brief helper to format and print assertion messages. Before printing, it first ensures srslog is flushed.
template <typename... Args>
[[gnu::noinline]] inline void assert_print_helper(const char* filename,
                                                  int         line,
                                                  const char* funcname,
                                                  const char* condstr = nullptr,
                                                  const char* fmtstr  = nullptr,
                                                  Args&&... args) noexcept
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "{}:{}: {}: ", filename, line, funcname);
  if (condstr == nullptr) {
    fmt::format_to(fmtbuf, "Assertion failed");
  } else {
    fmt::format_to(fmtbuf, "Assertion `{}' failed", condstr);
  }
  if (fmtstr != nullptr) {
    fmt::format_to(fmtbuf, " - {}", fmt::format(fmtstr, std::forward<Args>(args)...));
  }
  fmt::format_to(fmtbuf, ".\n");
  fmtbuf.push_back('\0'); // make it a c-string

  srslog::flush();
  fprintf(stderr, "%s", fmtbuf.data());
}

} // namespace detail

/// \brief Command to terminate application with an error message, ensuring first that the log is flushed.
/// Attribute noinline is used to signal to the compiler that this path should rarely occur and therefore doesn't need
/// to get optimized.
template <typename... Args>
[[gnu::noinline, noreturn]] inline bool srsran_terminate(const char* fmt, Args&&... args) noexcept
{
  srslog::flush();
  fprintf(stderr, "%s", fmt::format(fmt, std::forward<Args>(args)...).c_str());
  std::abort();
}

#define srsran_assertion_failure(fmt, ...)                                                                             \
  (void)((detail::assert_print_helper(__FILE__, __LINE__, __PRETTY_FUNCTION__, nullptr, fmt, ##__VA_ARGS__), 0) ||     \
         (std::abort(), 0))

/// \brief Macro that asserts condition is true. If false, it logs the remaining macro args, flushes the log, prints
/// the backtrace (if it was activated) and closes the application.
#define srsran_always_assert(condition, ...)                                                                           \
  (void)((condition) ||                                                                                                \
         (detail::assert_print_helper(__FILE__, __LINE__, __PRETTY_FUNCTION__, (#condition), ##__VA_ARGS__), 0) ||     \
         (std::abort(), 0))

/// Same as "srsran_always_assert" but it is only active when "enable_check" flag is defined
#define srsran_assert_ifdef(enable_check, condition, fmt, ...)                                                         \
  (void)((not SRSRAN_IS_DEFINED(enable_check)) || (srsran_always_assert(condition, fmt, ##__VA_ARGS__), 0))

/// Specialization of "srsran_assert_ifdef" for the ASSERTS_ENABLED flag
#define srsran_assert(condition, fmt, ...) srsran_assert_ifdef(ASSERTS_ENABLED, condition, fmt, ##__VA_ARGS__)

/// Specialization of "srsran_assert_ifdef" for the SANITY_CHECKS_ENABLED flag
#ifndef NDEBUG
#define SANITY_CHECKS_ENABLED
#endif
#define srsran_sanity_check(condition, fmt, ...)                                                                       \
  srsran_assert_ifdef(SANITY_CHECKS_ENABLED, condition, fmt, ##__VA_ARGS__)

} // namespace srsgnb

#endif // SRSRAN_ASSERT_H
