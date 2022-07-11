/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SUPPORT_SRSGNB_TEST_H
#define SRSGNB_SUPPORT_SRSGNB_TEST_H

#include "srsran_assert.h"

namespace srsgnb {

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
  srsran_always_assert(((EXPECTED) == (ACTUAL)),                                                                       \
                       "{}",                                                                                           \
                       srsgnb::detail::assert_eq_format_helper(EXPECTED, ACTUAL, true, fmt::format("" __VA_ARGS__)))

#define TESTASSERT_NEQ(EXPECTED, ACTUAL, ...)                                                                          \
  srsran_always_assert(((EXPECTED) != (ACTUAL)),                                                                       \
                       "{}",                                                                                           \
                       srsgnb::detail::assert_eq_format_helper(EXPECTED, ACTUAL, false, fmt::format("" __VA_ARGS__)))

#define TESTASSERT(cond, ...)                                                                                          \
  srsran_always_assert(                                                                                                \
      cond, "{}", srsgnb::detail::assert_eq_format_helper(true, false, true, fmt::format("" __VA_ARGS__)))

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_SRSGNB_TEST_H
