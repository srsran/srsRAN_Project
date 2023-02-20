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

#include "srsgnb/adt/strong_type.h"
#include "srsgnb/support/srsgnb_assert.h"
#include "fmt/format.h"
#include <type_traits>

namespace srsran {

namespace detail {

/// Tag struct to uniquely identify bounded integers by template types and values.
template <typename Integer>
struct bounded_integer_tag {
};

template <typename Integer>
using bounded_integer_base = strong_type<Integer,
                                         detail::bounded_integer_tag<Integer>,
                                         strong_equality,
                                         strong_equality_with<Integer>,
                                         strong_comparison,
                                         strong_comparison_with<Integer>>;

} // namespace detail

/// Tag used to initialize a bounded_integer in invalid state.
struct bounded_integer_invalid_tag {};

/// This class represents an integer whose value is within the set of possible values: {MIN_VALUE, ..., MAX_VALUE}.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
class bounded_integer : public detail::bounded_integer_base<Integer>
{
  static_assert(std::is_integral<Integer>::value, "Template argument must be an integer");
  static_assert(MIN_VALUE <= MAX_VALUE, "Provided bounds for bounded_integer are not valid");

  using base_class = detail::bounded_integer_base<Integer>;

public:
  using base_class::base_class;

  constexpr bounded_integer() = default;
  constexpr bounded_integer(bounded_integer_invalid_tag /**/) : base_class(MAX_VALUE + 1) {}
  constexpr bounded_integer(Integer v) : base_class(v) { assert_bounds(v); }

  bounded_integer& operator=(Integer v)
  {
    assert_bounds(v);
    base_class::value() = v;
    return *this;
  }

  static constexpr Integer min() { return MIN_VALUE; }
  static constexpr Integer max() { return MAX_VALUE; }

  /// Checks whether the value is within the defined boundaries.
  constexpr bool valid() const { return base_class::value() >= MIN_VALUE && base_class::value() <= MAX_VALUE; }

  /// Cast operator to primitive integer type.
  explicit constexpr operator Integer() const { return base_class::value(); }

  /// Returns the underlying value. This method is only enabled if the type is unsigned.
  template <typename U = Integer, std::enable_if_t<std::is_unsigned<U>::value, int> = 0>
  constexpr Integer to_uint() const
  {
    return base_class::value();
  }

  /// Returns the underlying value. This method is only enabled if the type is signed.
  template <typename U = Integer, std::enable_if_t<std::is_signed<U>::value, int> = 0>
  constexpr Integer to_int() const
  {
    return base_class::value();
  }

  bounded_integer& operator++()
  {
    ++base_class::value();
    assert_bounds(base_class::value());
    return *this;
  }

  bounded_integer operator++(int)
  {
    auto tmp = base_class::value()++;
    assert_bounds(base_class::value());
    return tmp;
  }

  bounded_integer& operator--()
  {
    --base_class::value();
    assert_bounds(base_class::value());
    return *this;
  }

  bounded_integer operator--(int)
  {
    auto tmp = base_class::value()--;
    assert_bounds(base_class::value());
    return tmp;
  }
  bounded_integer& operator+=(Integer other)
  {
    base_class::value() += other;
    assert_bounds(base_class::value());
    return *this;
  }
  bounded_integer& operator+=(bounded_integer other) { return *this += other.value(); }

  bounded_integer operator+(Integer other) const
  {
    bounded_integer<Integer, MIN_VALUE, MAX_VALUE> ret{*this};
    return ret += other;
  }
  bounded_integer operator+(bounded_integer other) const { return *this + other.value(); }

  bounded_integer& operator-=(Integer other)
  {
    base_class::value() -= other;
    assert_bounds(base_class::value());
    return *this;
  }
  bounded_integer& operator-=(bounded_integer other) { return *this -= other.value(); }

  bounded_integer operator-(Integer other) const
  {
    bounded_integer<Integer, MIN_VALUE, MAX_VALUE> ret{*this};
    return ret -= other;
  }
  bounded_integer operator-(bounded_integer other) const { return *this - other.value(); }

protected:
  constexpr void assert_bounds(Integer v) const
  {
    srsgnb_assert(v >= MIN_VALUE && v <= MAX_VALUE, "Value={} out-of-bounds [{}, {}]", v, MIN_VALUE, MAX_VALUE);
  }
};

} // namespace srsran

namespace fmt {

/// Formatter for bounded_integer<...> types.
template <typename Integer, Integer MIN_VALUE, Integer MAX_VALUE>
struct formatter<srsran::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>> : public formatter<Integer> {
  template <typename FormatContext>
  auto format(const srsran::bounded_integer<Integer, MIN_VALUE, MAX_VALUE>& s, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (s.valid()) {
      return fmt::format_to(ctx.out(), "{}", static_cast<Integer>(s));
    }
    return fmt::format_to(ctx.out(), "INVALID");
  }
};

} // namespace fmt
