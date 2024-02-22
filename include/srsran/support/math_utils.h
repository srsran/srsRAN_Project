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

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "srsran_assert.h"
#include "srsran/adt/complex.h"

namespace srsran {

/// Defines two times Pi.
static constexpr float TWOPI = 2.0F * static_cast<float>(M_PI);

/// Floating point near zero value.
static constexpr float near_zero = 1e-9;

/// \brief Performs an integer division rounding up.
///
/// \param[in]  num Numerator.
/// \param[out] den Denominator.
/// \return The result of the operation.
inline constexpr unsigned divide_ceil(unsigned num, unsigned den)
{
  srsran_sanity_check(den != 0, "Denominator cannot be zero.");
  return (num + (den - 1)) / den;
}

/// \brief Performs an integer division rounding to the nearest integer.
///
/// \param[in]  num Numerator.
/// \param[out] den Denominator.
/// \return The result of the operation.
inline constexpr unsigned divide_round(unsigned num, unsigned den)
{
  srsran_sanity_check(den != 0, "Denominator cannot be zero.");
  return static_cast<unsigned>(std::round(static_cast<float>(num) / static_cast<float>(den)));
}

/// \brief Calculates the integer power of 2.
///
/// \param[in] power Indicates the power of 2 to calculate.
/// \return The result of the operation.
inline constexpr unsigned pow2(unsigned power)
{
  return 1U << power;
}

/// \brief Calculates the squared modulus of a complex value.
/// \param[in] x Complex value.
/// \return The squared absolute of the given value, i.e. \f$\abs{x}^2=x\cdot\conj{x}=\Re(x)^2+\Im(x)^2\f$.
inline constexpr float abs_sq(cf_t x)
{
  // Equivalent to but computationally simpler than std::pow(std::abs(x),2).
  return x.real() * x.real() + x.imag() * x.imag();
}

/// Determines whether a floating point value is near zero.
inline bool is_near_zero(float value)
{
  return std::abs(value) < near_zero;
}

/// Determines whether a complex floating point value is near zero.
inline bool is_near_zero(cf_t value)
{
  return abs_sq(value) < near_zero;
}

/// \brief Calculates \f$\left \lceil log_2(n) \right \rceil\f$.
///
/// \tparam Integer Any unsigned integer type.
/// \param[in] value Parameter \f$n\f$.
/// \return The result of the calculation if \c value is not zero. Otherwise 0.
template <typename Integer>
inline constexpr Integer log2_ceil(Integer value)
{
  static_assert(std::is_unsigned<Integer>::value, "log2_ceil only works for unsigned integers");

  // Avoid unbounded results.
  if (value == 0) {
    return 0;
  }
  return static_cast<Integer>(std::ceil(std::log2(static_cast<float>(value))));
}

/// \brief Converts a value in decibels to linear amplitude ratio
/// \param [in] value is in decibels
/// \return the resultant amplitude ratio
inline float convert_dB_to_amplitude(float value)
{
  return std::pow(10.0F, value / 20.0F);
}

/// \brief Converts a value in decibels to linear power ratio
/// \param [in] value is in decibels
/// \return the resultant power ratio
inline float convert_dB_to_power(float value)
{
  return std::pow(10.0F, value / 10.0F);
}

/// \brief Converts a linear amplitude ratio to decibels
/// \param [in] value is the linear amplitude
/// \return the resultant decibels
inline float convert_amplitude_to_dB(float value)
{
  return 20.0F * std::log10(value);
}

/// \brief Converts a linear power ratio to decibels
/// \param [in] value is the linear power
/// \return the resultant decibels
inline float convert_power_to_dB(float value)
{
  return 10.0F * std::log10(value);
}

/// \brief Finds the smallest prime number greater than \c n.
/// \remark Only works for prime numbers not larger than 3299.
unsigned prime_greater_than(unsigned n);

/// \brief Finds the biggest prime number less than \c n.
/// \remark Only works for prime numbers not larger than 3299.
unsigned prime_lower_than(unsigned n);

/// Reverses the bits of a given byte.
template <typename Integer>
inline Integer reverse_byte(Integer byte)
{
  static_assert(std::is_convertible<Integer, uint8_t>::value,
                "The input type must be convertible to an unsigned integer of eight bits");
  static const std::array<Integer, 256> reverse_lut = {
      0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000, 0b00010000,
      0b10010000, 0b01010000, 0b11010000, 0b00110000, 0b10110000, 0b01110000, 0b11110000, 0b00001000, 0b10001000,
      0b01001000, 0b11001000, 0b00101000, 0b10101000, 0b01101000, 0b11101000, 0b00011000, 0b10011000, 0b01011000,
      0b11011000, 0b00111000, 0b10111000, 0b01111000, 0b11111000, 0b00000100, 0b10000100, 0b01000100, 0b11000100,
      0b00100100, 0b10100100, 0b01100100, 0b11100100, 0b00010100, 0b10010100, 0b01010100, 0b11010100, 0b00110100,
      0b10110100, 0b01110100, 0b11110100, 0b00001100, 0b10001100, 0b01001100, 0b11001100, 0b00101100, 0b10101100,
      0b01101100, 0b11101100, 0b00011100, 0b10011100, 0b01011100, 0b11011100, 0b00111100, 0b10111100, 0b01111100,
      0b11111100, 0b00000010, 0b10000010, 0b01000010, 0b11000010, 0b00100010, 0b10100010, 0b01100010, 0b11100010,
      0b00010010, 0b10010010, 0b01010010, 0b11010010, 0b00110010, 0b10110010, 0b01110010, 0b11110010, 0b00001010,
      0b10001010, 0b01001010, 0b11001010, 0b00101010, 0b10101010, 0b01101010, 0b11101010, 0b00011010, 0b10011010,
      0b01011010, 0b11011010, 0b00111010, 0b10111010, 0b01111010, 0b11111010, 0b00000110, 0b10000110, 0b01000110,
      0b11000110, 0b00100110, 0b10100110, 0b01100110, 0b11100110, 0b00010110, 0b10010110, 0b01010110, 0b11010110,
      0b00110110, 0b10110110, 0b01110110, 0b11110110, 0b00001110, 0b10001110, 0b01001110, 0b11001110, 0b00101110,
      0b10101110, 0b01101110, 0b11101110, 0b00011110, 0b10011110, 0b01011110, 0b11011110, 0b00111110, 0b10111110,
      0b01111110, 0b11111110, 0b00000001, 0b10000001, 0b01000001, 0b11000001, 0b00100001, 0b10100001, 0b01100001,
      0b11100001, 0b00010001, 0b10010001, 0b01010001, 0b11010001, 0b00110001, 0b10110001, 0b01110001, 0b11110001,
      0b00001001, 0b10001001, 0b01001001, 0b11001001, 0b00101001, 0b10101001, 0b01101001, 0b11101001, 0b00011001,
      0b10011001, 0b01011001, 0b11011001, 0b00111001, 0b10111001, 0b01111001, 0b11111001, 0b00000101, 0b10000101,
      0b01000101, 0b11000101, 0b00100101, 0b10100101, 0b01100101, 0b11100101, 0b00010101, 0b10010101, 0b01010101,
      0b11010101, 0b00110101, 0b10110101, 0b01110101, 0b11110101, 0b00001101, 0b10001101, 0b01001101, 0b11001101,
      0b00101101, 0b10101101, 0b01101101, 0b11101101, 0b00011101, 0b10011101, 0b01011101, 0b11011101, 0b00111101,
      0b10111101, 0b01111101, 0b11111101, 0b00000011, 0b10000011, 0b01000011, 0b11000011, 0b00100011, 0b10100011,
      0b01100011, 0b11100011, 0b00010011, 0b10010011, 0b01010011, 0b11010011, 0b00110011, 0b10110011, 0b01110011,
      0b11110011, 0b00001011, 0b10001011, 0b01001011, 0b11001011, 0b00101011, 0b10101011, 0b01101011, 0b11101011,
      0b00011011, 0b10011011, 0b01011011, 0b11011011, 0b00111011, 0b10111011, 0b01111011, 0b11111011, 0b00000111,
      0b10000111, 0b01000111, 0b11000111, 0b00100111, 0b10100111, 0b01100111, 0b11100111, 0b00010111, 0b10010111,
      0b01010111, 0b11010111, 0b00110111, 0b10110111, 0b01110111, 0b11110111, 0b00001111, 0b10001111, 0b01001111,
      0b11001111, 0b00101111, 0b10101111, 0b01101111, 0b11101111, 0b00011111, 0b10011111, 0b01011111, 0b11011111,
      0b00111111, 0b10111111, 0b01111111, 0b11111111,
  };
  return reverse_lut[byte];
}

/// \brief Ensures a value is between the given bounds, according to a specified order relation.
///
/// \tparam T            Class of the value to clamp. A strict order relation (less than) should be definable between
///                      elements of class \c T.
/// \tparam CompareLess  Class of the comparison object (see below).
/// \param value         The value to clamp.
/// \param lower_bound   Minimum value after clamping.
/// \param upper_bound   Maximum value after clamping.
/// \param comp          Comparison function object implementing an order relation between elements of \c T. The
///                      signature should be equivalent to `bool comp(const T& a, const T& b)` and should return \c true
///                      if <tt>a < b</tt> (according to any order relation in \c T), \c false otherwise.
/// \return A reference to \c lower_bound if <tt>value < lower_bound</tt>; a reference to \c upper_bound if
///         <tt>upper_bound < value</tt>; a reference to \c value itself otherwise.
template <class T, class CompareLess>
constexpr const T& clamp(const T& value, const T& lower_bound, const T& upper_bound, CompareLess comp)
{
  return comp(value, lower_bound) ? lower_bound : (comp(upper_bound, value) ? upper_bound : value);
}

/// \brief Ensures a value is between the given bounds, according to the default order relation.
///
/// See the documentation for the extended form for more information.
template <class T>
constexpr const T& clamp(const T& value, const T& lower_bound, const T& upper_bound)
{
  return clamp(value, lower_bound, upper_bound, std::less<T>());
}

} // namespace srsran
