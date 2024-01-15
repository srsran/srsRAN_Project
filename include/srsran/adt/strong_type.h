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

#include <type_traits>

namespace srsran {

namespace detail {

template <typename Property, typename T>
using strong_property_t = typename Property::template strong_property<T>;

/// Base class that defines the equality operators for two different types.
template <typename T, typename U>
class strong_equality_with_type
{
public:
  friend constexpr bool operator==(const T& lhs, const U& rhs) { return lhs.value() == rhs; }
  friend constexpr bool operator==(const U& lhs, const T& rhs) { return lhs == rhs.value(); }
  friend constexpr bool operator!=(const T& lhs, const U& rhs) { return lhs.value() != rhs; }
  friend constexpr bool operator!=(const U& lhs, const T& rhs) { return lhs != rhs.value(); }
};

/// Base class that defines the comparison operators for two different types.
template <typename T, typename U>
class strong_comparison_with_type
{
public:
  friend constexpr bool operator<(const T& lhs, const U& rhs) { return lhs.value() < rhs; }
  friend constexpr bool operator<(const U& lhs, const T& rhs) { return lhs < rhs.value(); }
  friend constexpr bool operator>(const T& lhs, const U& rhs) { return lhs.value() > rhs; }
  friend constexpr bool operator>(const U& lhs, const T& rhs) { return lhs > rhs.value(); }
  friend constexpr bool operator<=(const T& lhs, const U& rhs) { return lhs.value() <= rhs; }
  friend constexpr bool operator<=(const U& lhs, const T& rhs) { return lhs <= rhs.value(); }
  friend constexpr bool operator>=(const T& lhs, const U& rhs) { return lhs.value() >= rhs; }
  friend constexpr bool operator>=(const U& lhs, const T& rhs) { return lhs >= rhs.value(); }
};

/// Base class that defines the conversion operator for the given type.
template <typename T, typename U>
class strong_convertion_to_type
{
public:
  explicit operator U() const { return static_cast<U>(static_cast<const T&>(*this).value()); }
};

/// Base class that defines the multiplication operators for the given type.
template <typename T, typename U>
class strong_multiplication_with_type
{
public:
  friend constexpr T& operator*=(T& lhs, U rhs)
  {
    lhs.value() *= rhs;
    return lhs;
  }

  friend constexpr T operator*(T lhs, U rhs)
  {
    lhs *= rhs;
    return lhs;
  }

  friend constexpr T operator*(U lhs, T rhs)
  {
    rhs *= lhs;
    return rhs;
  }
};

} // namespace detail

/// \brief Template class for strong typing arithmetic types.
///
/// This template class wraps an arithmetic type and allows enabling the common operators through the Properties
/// template argument pack.
template <typename T, typename Tag, typename... Properties>
class strong_type : public detail::strong_property_t<Properties, strong_type<T, Tag, Properties...>>...
{
  static_assert(std::is_arithmetic<T>::value, "T should be an arithmetic type");
  T val;

public:
  using value_type = T;
  using tag_type   = Tag;

  /// Special member function definitions.
  constexpr strong_type() = default;
  explicit constexpr strong_type(T val_) : val(val_) {}
  constexpr strong_type(const strong_type&)            = default;
  constexpr strong_type& operator=(const strong_type&) = default;

  /// Accessor for the underlying value of the strong type.
  constexpr T&       value() { return val; }
  constexpr const T& value() const { return val; }
};

struct strong_equality {
  template <typename T>
  class strong_property;
};

/// Equality operator definitions for strong types.
template <typename T, typename Tag, typename... Properties>
class strong_equality::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr bool operator==(const base_type& lhs, const base_type& rhs) { return lhs.value() == rhs.value(); }
  friend constexpr bool operator!=(const base_type& lhs, const base_type& rhs) { return lhs.value() != rhs.value(); }
};

/// Equality operator definitions for strong types and the list of types in the template argument pack.
template <typename... Ts>
struct strong_equality_with {
  static_assert(std::is_arithmetic<Ts...>::value, "All Ts should be arithmetic types");

  template <typename T>
  class strong_property : public detail::strong_equality_with_type<T, Ts>...
  {
  };
};

/// Multiplication operator definitions for strong types and the list of types in the template argument pack.
template <typename... Ts>
struct strong_multiplication_with {
  static_assert(std::is_arithmetic<Ts...>::value, "All Ts should be arithmetic types");

  template <typename T>
  class strong_property : public detail::strong_multiplication_with_type<T, Ts>...
  {
  };
};

struct strong_comparison {
  template <typename T>
  class strong_property;
};

/// Comparison operator definitions for strong types.
template <typename T, typename Tag, typename... Properties>
class strong_comparison::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr bool operator<(const base_type& lhs, const base_type& rhs) { return lhs.value() < rhs.value(); }
  friend constexpr bool operator>(const base_type& lhs, const base_type& rhs) { return lhs.value() > rhs.value(); }
  friend constexpr bool operator<=(const base_type& lhs, const base_type& rhs) { return lhs.value() <= rhs.value(); }
  friend constexpr bool operator>=(const base_type& lhs, const base_type& rhs) { return lhs.value() >= rhs.value(); }
};

/// Comparison operator definitions for strong types and the list of types in the template argument pack.
template <typename... Ts>
struct strong_comparison_with {
  static_assert(std::is_arithmetic<Ts...>::value, "All Ts should be arithmetic types");

  template <typename T>
  class strong_property : public detail::strong_comparison_with_type<T, Ts>...
  {
  };
};

struct strong_increment_decrement {
  template <typename T>
  class strong_property;
};

/// Increment and decrement operator definitions for strong types.
template <typename T, typename Tag, typename... Properties>
class strong_increment_decrement::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr base_type& operator++(base_type& v)
  {
    ++v.value();
    return v;
  }

  friend constexpr base_type operator++(base_type& v, int)
  {
    auto tmp = v;
    ++v;
    return tmp;
  }

  friend constexpr base_type& operator--(base_type& v)
  {
    --v.value();
    return v;
  }

  friend constexpr base_type operator--(base_type& v, int)
  {
    auto tmp = v;
    --v;
    return tmp;
  }
};

struct strong_arithmetic {
  template <typename T>
  class strong_property;
};

/// Arithmetic operator definitions for strong types.
template <typename T, typename Tag, typename... Properties>
class strong_arithmetic::strong_property<strong_type<T, Tag, Properties...>>
  : public strong_comparison::strong_property<strong_type<T, Tag, Properties...>>,
    public strong_equality::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr base_type operator-(const base_type& v) { return base_type(-v.value()); }

  friend constexpr base_type& operator+=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() += rhs.value();
    return lhs;
  }

  friend constexpr base_type& operator-=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() -= rhs.value();
    return lhs;
  }

  friend constexpr base_type& operator*=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() *= rhs.value();
    return lhs;
  }

  friend constexpr base_type& operator/=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() /= rhs.value();
    return lhs;
  }

  friend constexpr base_type operator+(base_type lhs, const base_type& rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend constexpr base_type operator-(base_type lhs, const base_type& rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend constexpr base_type operator*(base_type lhs, const base_type& rhs)
  {
    lhs *= rhs;
    return lhs;
  }

  friend constexpr base_type operator/(base_type lhs, const base_type& rhs)
  {
    lhs /= rhs;
    return lhs;
  }
};

struct strong_arithmetic_with_underlying_type {
  template <typename T>
  class strong_property;
};

/// Increment and decrement operator definitions for strong types and its underlying type.
template <typename T, typename Tag, typename... Properties>
class strong_arithmetic_with_underlying_type::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr base_type& operator+=(base_type& lhs, T rhs)
  {
    lhs.value() += rhs;
    return lhs;
  }

  friend constexpr base_type& operator-=(base_type& lhs, T rhs)
  {
    lhs.value() -= rhs;
    return lhs;
  }

  friend constexpr base_type& operator*=(base_type& lhs, T rhs)
  {
    lhs.value() *= rhs;
    return lhs;
  }

  friend constexpr base_type& operator/=(base_type& lhs, T rhs)
  {
    lhs.value() /= rhs;
    return lhs;
  }

  friend constexpr base_type operator+(base_type lhs, T rhs)
  {
    lhs += rhs;
    return lhs;
  }

  friend constexpr base_type operator+(T lhs, base_type rhs)
  {
    rhs += lhs;
    return rhs;
  }

  friend constexpr base_type operator-(base_type lhs, T rhs)
  {
    lhs -= rhs;
    return lhs;
  }

  friend constexpr base_type operator-(T lhs, base_type rhs)
  {
    lhs -= rhs.value();
    return base_type(lhs);
  }

  friend constexpr base_type operator*(base_type lhs, T rhs)
  {
    lhs *= rhs;
    return lhs;
  }

  friend constexpr base_type operator*(T lhs, base_type rhs)
  {
    rhs += lhs;
    return rhs;
  }

  friend constexpr base_type operator/(base_type lhs, T rhs)
  {
    lhs /= rhs;
    return lhs;
  }

  friend constexpr base_type operator/(T lhs, base_type rhs)
  {
    lhs /= rhs.value();
    return base_type(lhs);
  }
};

struct strong_bitwise {
  template <typename T>
  class strong_property;
};

/// Bitwise operator definitions for strong types.
template <typename T, typename Tag, typename... Properties>
class strong_bitwise::strong_property<strong_type<T, Tag, Properties...>>
{
  using base_type = strong_type<T, Tag, Properties...>;

public:
  friend constexpr base_type& operator&=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() &= rhs.value();
    return lhs;
  }

  friend constexpr base_type& operator|=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() |= rhs.value();
    return lhs;
  }

  friend constexpr base_type& operator^=(base_type& lhs, const base_type& rhs)
  {
    lhs.value() ^= rhs.value();
    return lhs;
  }

  template <typename U>
  friend constexpr base_type& operator<<=(base_type& lhs, U shift)
  {
    static_assert(std::is_integral<U>::value, "U should be an integral type");
    lhs.value() <<= shift;
    return lhs;
  }

  template <typename U>
  friend constexpr base_type& operator>>=(base_type& lhs, U shift)
  {
    static_assert(std::is_integral<U>::value, "U should be an integral type");
    lhs.value() >>= shift;
    return lhs;
  }

  friend constexpr base_type operator~(const base_type& v) { return base_type(~v.value()); }

  friend constexpr base_type operator&(base_type lhs, base_type rhs)
  {
    lhs &= rhs;
    return lhs;
  }

  friend constexpr base_type operator|(base_type lhs, base_type rhs)
  {
    lhs |= rhs;
    return lhs;
  }

  friend constexpr base_type operator^(base_type lhs, base_type rhs)
  {
    lhs ^= rhs;
    return lhs;
  }

  template <typename U>
  friend constexpr base_type operator<<(base_type lhs, U shift)
  {
    static_assert(std::is_integral<U>::value, "U should be an integral type");
    lhs <<= shift;
    return lhs;
  }

  template <typename U>
  friend constexpr base_type operator>>(base_type lhs, U shift)
  {
    static_assert(std::is_integral<U>::value, "U should be an integral type");
    lhs >>= shift;
    return lhs;
  }
};

/// Conversion operator definitions for strong types and the list of types in the template argument pack.
template <typename... Ts>
struct strong_conversion_to {
  static_assert(std::is_arithmetic<Ts...>::value, "All Ts should be arithmetic types");

  template <typename T>
  class strong_property : public detail::strong_convertion_to_type<T, Ts>...
  {
  };
};

} // namespace srsran
