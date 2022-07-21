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

#include "srsgnb/srslog/bundled/fmt/format.h"
#include "srsgnb/support/srsran_assert.h"
#include <type_traits>

namespace srsgnb {

/// This class represents an integer whose value is within the set of possible values: {MIN_VALUE, ..., MAX_VALUE}.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
class bounded_integer
{
  static_assert(std::is_integral<Integer>::value, "Template argument must be an integer");
  static_assert(MIN_VALUE <= MAX_VALUE, "Provided bounds for bounded_integer are not valid");

public:
  using value_type = Integer;

  constexpr bounded_integer() : val(MAX_VALUE + 1) {}

  constexpr bounded_integer(const bounded_integer&) noexcept = default;

  constexpr bounded_integer(bounded_integer&&) noexcept = default;

  template <typename U>
  constexpr bounded_integer(U v) : val(v)
  {
    static_assert(std::is_convertible<U, value_type>::value, "Invalid value type.");
    assert_bounds(v);
  }

  constexpr bounded_integer& operator=(const bounded_integer&) noexcept = default;

  constexpr bounded_integer& operator=(bounded_integer&&) noexcept = default;

  template <typename U>
  bounded_integer& operator=(U v)
  {
    static_assert(std::is_convertible<U, value_type>::value, "Invalid value type.");
    assert_bounds(v);
    val = v;
    return *this;
  }

  constexpr static Integer min() { return MIN_VALUE; }
  constexpr static Integer max() { return MAX_VALUE; }

  /// Checks whether the value is within the defined boundaries.
  bool is_valid() const { return val >= MIN_VALUE and val <= MAX_VALUE; }

  /// Cast operator to primitive integer type.
  explicit operator Integer() const { return val; }
  template <typename U = Integer, std::enable_if_t<std::is_unsigned<U>::value, int> = 0>
  constexpr Integer to_uint() const
  {
    return val;
  }
  template <typename U = Integer, std::enable_if_t<std::is_signed<U>::value, int> = 0>
  constexpr Integer to_int() const
  {
    return val;
  }

  constexpr bool operator==(bounded_integer other) const { return val == other.val; }
  constexpr bool operator!=(bounded_integer other) const { return val != other.val; }
  constexpr bool operator<(bounded_integer other) const { return val < other.val; }
  constexpr bool operator<=(bounded_integer other) const { return val <= other.val; }
  constexpr bool operator>(bounded_integer other) const { return val > other.val; }
  constexpr bool operator>=(bounded_integer other) const { return val >= other.val; }

protected:
  constexpr void assert_bounds(Integer v) const
  {
    srsran_assert(
        v >= MIN_VALUE and v <= MAX_VALUE, "Passed value={} outside bounds {{{},...,{}}}", v, MIN_VALUE, MAX_VALUE);
  }

  Integer val;
};

} // namespace srsgnb

namespace fmt {

/// Formatter for bounded_integer<...> types.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
struct formatter<srsgnb::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>> : public formatter<Integer> {
  template <typename FormatContext>
  auto format(const srsgnb::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (s.is_valid()) {
      return fmt::format_to(ctx.out(), "{}", static_cast<Integer>(s));
    }
    return fmt::format_to(ctx.out(), "INVALID");
  }
};

} // namespace fmt