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
#include <cstdio>

/// Provides a hint to the compiler that a condition is likely false.
#define srsran_unlikely(expr) __builtin_expect(!!(expr), 0)

/// Verifies if compile-time symbol is defined.
#define SRSRAN_IS_DEFINED(x) SRSRAN_IS_DEFINED2(x)
#define SRSRAN_IS_DEFINED2(x) (#x[0] == 0 || (#x[0] >= '1' && #x[0] <= '9'))

namespace srsgnb {

namespace detail {

/// \brief helper function to format and print assertion messages. Before printing, the srslog is flushed.
/// \param filename file name where assertion failed.
/// \param line line in which assertion was placed.
/// \param funcname function name where assertion failed.
/// \param condstr assertion condition that failed.
/// \param msg additional assertion message.
[[gnu::noinline]] inline bool assert_print_helper(const char*        filename,
                                                  int                line,
                                                  const char*        funcname,
                                                  const char*        condstr = nullptr,
                                                  const std::string& msg     = "") noexcept
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "{}:{}: {}: \n", filename, line, funcname);
  if (condstr == nullptr) {
    fmt::format_to(fmtbuf, "Assertion failed");
  } else {
    fmt::format_to(fmtbuf, "Assertion `{}' failed", condstr);
  }
  if (not msg.empty()) {
    fmt::format_to(fmtbuf, " - {}", msg);
  }
  if (msg.back() != '.') {
    fmt::format_to(fmtbuf, ".");
  }
  fmt::format_to(fmtbuf, "\n");
  fmtbuf.push_back('\0'); // make it a c-string

  srslog::flush();
  fprintf(stderr, "%s", fmtbuf.data());
  return false;
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

} // namespace srsgnb

/// Helper macro to log assertion message and terminate program.
#define srsran_assertion_failure__(condmessage, fmtstr, ...)                                                           \
  (void)(srsgnb::detail::assert_print_helper(                                                                          \
             __FILE__, __LINE__, __PRETTY_FUNCTION__, condmessage, fmt::format(FMT_STRING(fmtstr), ##__VA_ARGS__)) ||  \
         (std::abort(), 0))

/// Terminates program with an assertion failure. No condition message is provided.
#define srsran_assertion_failure(fmtstr, ...) srsran_assertion_failure__(nullptr, fmtstr, ##__VA_ARGS__)

/// \brief Macro that asserts condition is true. If false, it logs the remaining macro args, flushes the log, prints
/// the backtrace (if it was activated) and closes the application.
#define srsran_always_assert(condition, fmtstr, ...)                                                                   \
  (void)((condition) || (srsran_assertion_failure__((#condition), fmtstr, ##__VA_ARGS__), 0))

/// Same as "srsran_always_assert" but it is only active when "enable_check" flag is defined
#define srsran_assert_ifdef(enable_check, condition, fmtstr, ...)                                                      \
  (void)((not SRSRAN_IS_DEFINED(enable_check)) || (srsran_always_assert(condition, fmtstr, ##__VA_ARGS__), 0))

/// Specialization of "srsran_assert_ifdef" for the ASSERTS_ENABLED flag
#define srsran_assert(condition, fmtstr, ...) srsran_assert_ifdef(ASSERTS_ENABLED, condition, fmtstr, ##__VA_ARGS__)

/// Specialization of "srsran_assert_ifdef" for the SANITY_CHECKS_ENABLED flag
#ifndef NDEBUG
#define SANITY_CHECKS_ENABLED
#endif
#define srsran_sanity_check(condition, fmt, ...)                                                                       \
  srsran_assert_ifdef(SANITY_CHECKS_ENABLED, condition, fmt, ##__VA_ARGS__)
