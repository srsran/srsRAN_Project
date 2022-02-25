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

#include <cmath>

namespace srsgnb {

inline unsigned divide_ceil(unsigned num, unsigned den)
{
  assert(den != 0);
  return (num + (den - 1)) / den;
}

/// \brief Converts a value in decibels to linear amplitude
/// \param [in] value is in decibels
/// \return the resultant amplitude
inline float convert_dB_to_amplitude(float value)
{
  return std::pow(10.0F, value / 20.0F);
}

/// \brief Converts a value in decibels to linear power
/// \param [in] value is in decibels
/// \return the resultant power
inline float convert_dB_to_power(float value)
{
  return std::pow(10.0F, value / 10.0F);
}

/// \brief Converts a linear amplitude to decibels
/// \param [in] value is the linear amplitude
/// \return the resultant decibels
inline float convert_amplitude_to_dB(float value)
{
  return 20.0F * std::log10(value);
}

/// \brief Converts a linear power to decibels
/// \param [in] value is the linear power
/// \return the resultant decibels
inline float convert_power_to_dB(float value)
{
  return 10.0F * std::log10(value);
}

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_MATH_UTILS_H_
