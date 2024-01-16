/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/support/compiler.h"
#include <cstdio>

namespace srsran {

namespace detail {

/// \brief helper function to format and print assertion messages. Before printing, the srslog is flushed.
/// \param filename file name where assertion failed.
/// \param line line in which assertion was placed.
/// \param funcname function name where assertion failed.
/// \param condstr assertion condition that failed.
/// \param msg additional assertion message.
[[gnu::noinline, noreturn]] inline void print_and_abort(const char*        filename,
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
  fmt::print(stderr, "{}", fmtbuf.data());
  std::abort();
}

} // namespace detail

} // namespace srsran

// NOLINTBEGIN

/// Helper macro to log assertion message and terminate program.
#define SRSRAN_ASSERT_FAILURE__(condmessage, fmtstr, ...)                                                              \
  srsran::detail::print_and_abort(                                                                                     \
      __FILE__, __LINE__, __PRETTY_FUNCTION__, condmessage, fmt::format(FMT_STRING(fmtstr), ##__VA_ARGS__))

/// \brief Helper macro that asserts condition is true. If false, it logs the remaining macro args, flushes the log,
/// prints the backtrace (if it was activated) and closes the application.
#define SRSRAN_ALWAYS_ASSERT__(condition, fmtstr, ...)                                                                 \
  (void)((condition) || (SRSRAN_ASSERT_FAILURE__((#condition), fmtstr, ##__VA_ARGS__), 0))

/// Same as "SRSRAN_ALWAYS_ASSERT__" but it is only active when "enable_check" flag is defined
#define SRSRAN_ALWAYS_ASSERT_IFDEF__(enable_check, condition, fmtstr, ...)                                             \
  (void)((not SRSRAN_IS_DEFINED(enable_check)) || (SRSRAN_ALWAYS_ASSERT__(condition, fmtstr, ##__VA_ARGS__), 0))

/// \brief Terminates program with an assertion failure. No condition message is provided.
#define srsran_assertion_failure(fmtstr, ...)                                                                          \
  (void)((not SRSRAN_IS_DEFINED(ASSERTS_ENABLED)) || (SRSRAN_ASSERT_FAILURE__(nullptr, fmtstr, ##__VA_ARGS__), 0))

/// Specialization of "SRSRAN_ALWAYS_ASSERT_IFDEF__" for the ASSERTS_ENABLED flag.
#define srsran_assert(condition, fmtstr, ...)                                                                          \
  SRSRAN_ALWAYS_ASSERT_IFDEF__(ASSERTS_ENABLED, condition, fmtstr, ##__VA_ARGS__)

/// Specialization of "SRSRAN_ALWAYS_ASSERT_IFDEF__" for the PARANOID_ASSERTS_ENABLED flag.
#define srsran_sanity_check(condition, fmtstr, ...)                                                                    \
  SRSRAN_ALWAYS_ASSERT_IFDEF__(PARANOID_ASSERTS_ENABLED, condition, fmtstr, ##__VA_ARGS__)

#define srsran_assume(condition) static_cast<void>((condition) ? void(0) : SRSRAN_UNREACHABLE)

// NOLINTEND
