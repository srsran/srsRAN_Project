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

#include "srsran/adt/strong_type.h"
#include "fmt/format.h"
#include <climits>

namespace srsran {

namespace units {

namespace detail {
/// Tag struct used to uniquely identify the bit units type.
struct bit_tag {
  /// Text representation for the units.
  static const char* str() { return "bits"; }
};

/// Tag struct used to uniquely identify the byte units type.
struct byte_tag {
  /// Text representation for the units.
  static const char* str() { return "bytes"; }
};
} // namespace detail

class bytes;

/// \brief Abstraction of bit as a unit of digital information.
///
/// An object of this type will represent an amount of digital information expressed in bits.
class bits : public strong_type<unsigned,
                                detail::bit_tag,
                                strong_arithmetic,
                                strong_increment_decrement,
                                strong_multiplication_with<unsigned>>
{
  /// Type alias for the base class of the bits units class.
  using bits_base = strong_type<unsigned,
                                detail::bit_tag,
                                strong_arithmetic,
                                strong_increment_decrement,
                                strong_multiplication_with<unsigned>>;

public:
  using bits_base::bits_base;

  constexpr bits(const bits_base& other) : bits_base(other) {}

  /// Returns true if the amount of digital information expressed as bits is a multiple of a byte.
  constexpr bool is_byte_exact() const { return ((value() % CHAR_BIT) == 0); }

  /// Returns the amount of digital information expressed as an integer number of bytes, rounded down.
  constexpr bytes truncate_to_bytes() const;

  /// Returns the amount of digital information expressed as an integer number of bytes, rounded up.
  constexpr bytes round_up_to_bytes() const;
};

/// \brief Abstraction of byte as a unit of digital information.
///
/// An object of this class will represent an amount of digital information expressed in bytes. The class also provides
/// a method to convert such amount into a number of information bits.
class bytes : public strong_type<unsigned,
                                 detail::byte_tag,
                                 strong_arithmetic,
                                 strong_increment_decrement,
                                 strong_multiplication_with<unsigned>>
{
  /// Type alias for the base class of the byte units class.
  using bytes_base = strong_type<unsigned,
                                 detail::byte_tag,
                                 strong_arithmetic,
                                 strong_increment_decrement,
                                 strong_multiplication_with<unsigned>>;

public:
  using bytes_base::bytes_base;

  constexpr bytes(const bytes_base& other) : bytes_base(other) {}

  explicit constexpr operator bits() const { return to_bits(); }

  /// Returns the amount of digital information as a number of bits.
  constexpr bits to_bits() const { return bits(value() * CHAR_BIT); }
};

constexpr bytes bits::truncate_to_bytes() const
{
  return bytes(value() / CHAR_BIT);
}

constexpr bytes bits::round_up_to_bytes() const
{
  return bytes((value() + CHAR_BIT - 1) / CHAR_BIT);
}

namespace literals {

/// User defined literal for byte units.
constexpr bytes operator"" _bytes(unsigned long long n)
{
  return bytes(n);
}

/// User defined literal for bit units.
constexpr bits operator"" _bits(unsigned long long n)
{
  return bits(n);
}

} // namespace literals
} // namespace units
} // namespace srsran

namespace fmt {

/// Formatter for bit units.
template <>
struct formatter<srsran::units::bits> : public formatter<srsran::units::bits::value_type> {
  template <typename FormatContext>
  auto format(srsran::units::bits s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return fmt::format_to(ctx.out(), "{}{}", s.value(), srsran::units::bits::tag_type::str());
  }
};

/// Formatter for byte units.
template <>
struct formatter<srsran::units::bytes> : public formatter<srsran::units::bytes::value_type> {
  bool print_units = false;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'B') {
        print_units = true;
      }
      ++it;
    }
    return it;
  }

  template <typename FormatContext>
  auto format(srsran::units::bytes s, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return fmt::format_to(ctx.out(), "{}{}", s.value(), print_units ? srsran::units::bytes::tag_type::str() : "");
  }
};

} // namespace fmt
