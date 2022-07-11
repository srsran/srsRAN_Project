/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsran_assert.h"
#include <cassert>
#include <cmath>

namespace srsgnb {

/// Defines the complex imaginary unit.
static constexpr cf_t COMPLEX_I(0, 1);

/// Defines two times Pi.
static constexpr float TWOPI = 2.0F * static_cast<float>(M_PI);

/// \brief Performs an integer division rounding up.
///
/// \param[in] num Provides the numerator.
/// \param[out] den Provides the denominator.
/// \return The result of the operation.
inline constexpr unsigned divide_ceil(unsigned num, unsigned den)
{
  srsran_sanity_check(den != 0, "Denominator cannot be zero.");
  return (num + (den - 1)) / den;
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

/// \brief Finds the biggest prime number lesser than \c n.
/// \remark Only works for prime numbers not larger than 3299.
unsigned prime_lower_than(unsigned n);

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

} // namespace srsgnb
