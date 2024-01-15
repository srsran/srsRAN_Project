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

#include "srsran_assert.h"

namespace srsran {

namespace detail {

/// Helper function to format TESTASSERT_EQ
template <typename T, typename U>
[[gnu::noinline]] inline std::string
assert_eq_format_helper(T expected_val, U actual_val, bool eq_cmp, const std::string& msg) noexcept
{
  fmt::memory_buffer fmtbuf;
  if (eq_cmp) {
    fmt::format_to(fmtbuf, "Actual value '{}' differs from expected '{}'", actual_val, expected_val);
  } else {
    fmt::format_to(fmtbuf, "Value '{}' should not be equal to '{}'", actual_val, expected_val);
  }
  if (not msg.empty()) {
    fmt::format_to(fmtbuf, ". {}", msg);
  }
  return fmt::to_string(fmtbuf);
}

} // namespace detail

#define TESTASSERT_EQ(EXPECTED, ACTUAL, ...)                                                                           \
  SRSRAN_ALWAYS_ASSERT__(((EXPECTED) == (ACTUAL)),                                                                     \
                         "{}",                                                                                         \
                         srsran::detail::assert_eq_format_helper(EXPECTED, ACTUAL, true, fmt::format("" __VA_ARGS__)))

#define TESTASSERT_NEQ(EXPECTED, ACTUAL, ...)                                                                          \
  SRSRAN_ALWAYS_ASSERT__(                                                                                              \
      ((EXPECTED) != (ACTUAL)),                                                                                        \
      "{}",                                                                                                            \
      srsran::detail::assert_eq_format_helper(EXPECTED, ACTUAL, false, fmt::format("" __VA_ARGS__)))

#define TESTASSERT(cond, ...)                                                                                          \
  SRSRAN_ALWAYS_ASSERT__(                                                                                              \
      cond, "{}", srsran::detail::assert_eq_format_helper(true, false, true, fmt::format("" __VA_ARGS__)))

} // namespace srsran
