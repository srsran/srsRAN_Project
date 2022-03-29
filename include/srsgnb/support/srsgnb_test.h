
#ifndef SRSGNB_TEST_H
#define SRSGNB_TEST_H

#include "srsran_assert.h"

namespace srsgnb {

namespace detail {

/// Helper function to format TESTASSERT_EQ
template <typename T, typename U, typename... Args>
[[gnu::noinline]] inline std::string assert_eq_format_helper(const T&    expected_val,
                                                             const U&    actual_val,
                                                             bool        eq_cmp,
                                                             const char* fmtstr = nullptr,
                                                             Args&&... args) noexcept
{
  fmt::memory_buffer fmtbuf;
  if (eq_cmp) {
    fmt::format_to(fmtbuf, "Actual value '{}' differs from expected '{}'", actual_val, expected_val);
  } else {
    fmt::format_to(fmtbuf, "Value '{}' should not be equal to '{}'", actual_val, expected_val);
  }
  if (fmtstr != nullptr) {
    fmt::format_to(fmtbuf, ". Cause: ");
    fmt::format_to(fmtbuf, fmtstr, std::forward<Args>(args)...);
  }
  return fmt::to_string(fmtbuf);
}

} // namespace detail

#define TESTASSERT_EQ(EXPECTED, ACTUAL, ...)                                                                           \
  srsran_always_assert((EXPECTED == ACTUAL),                                                                           \
                       srsgnb::detail::assert_eq_format_helper(EXPECTED, ACTUAL, true, ##__VA_ARGS__).c_str())

#define TESTASSERT_NEQ(EXPECTED, ACTUAL, ...)                                                                          \
  srsran_always_assert((EXPECTED == ACTUAL),                                                                           \
                       srsgnb::detail::assert_eq_format_helper(EXPECTED, ACTUAL, false, ##__VA_ARGS__).c_str())

#define TESTASSERT(cond, ...) srsran_always_assert(cond, ##__VA_ARGS__)

} // namespace srsgnb

#endif // SRSGNB_TEST_H
