/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SUPPORT_MATH_UTILS_H_
#define SRSGNB_SUPPORT_MATH_UTILS_H_

#include "srsgnb/adt/complex.h"
#include <cassert>
#include <cmath>
#include "srsran_assert.h"

namespace srsgnb {

/// Defines the complex imaginary unit.
static constexpr cf_t COMPLEX_I(0, 1);

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

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_MATH_UTILS_H_
