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

#include "srsran/ran/subcarrier_spacing.h"
#include "srsran/support/math_utils.h"

namespace srsran {

/// \brief Physical layer time unit.
///
/// This class abstracts the PHY reference time unit \f$T_c\f$ described in TS38.211 Section 4.1.
class phy_time_unit
{
private:
  /// Internal value type. It uses 64-bit word to avoid intermediate multiplications to overflow.
  using value_type = int64_t;
  /// Maximum subcarrier spacing in hertz, constant \f$\Delta f_{max}\f$.
  static constexpr value_type MAX_SCS_HZ = 480 * 1000;
  /// Constant \f$N_f\f$.
  static constexpr value_type N_F = 4096;
  /// Reference time unit in seconds, that is \f$T_c = 1/(\Delta f_{max} \times N_f)\f$.
  static constexpr double T_C = 1.0 / static_cast<double>(MAX_SCS_HZ * N_F);
  /// Reference subcarrier spacing, constant \f$\Delta f_{ref}\f$
  static constexpr value_type SCS_REF_HZ = 15 * 1000;
  /// DFT size for the reference subcarrier spacing, constant \f$N_{f,ref}\f$
  static constexpr value_type N_F_REF = 2048;
  /// Constant \f$\kappa=T_s/T_c=64\f$.
  static constexpr value_type KAPPA = 64;

  /// Actual value as a multiple of \f$T_c\f$.
  value_type value = 0;

  /// Private constructor from a value in units of \f$T_c\f$.
  explicit constexpr phy_time_unit(value_type value_) : value(value_)
  {
    // Do nothing.
  }

public:
  /// Creates a default physical layer time of zero units.
  phy_time_unit() = default;

  /// \brief Gets the stored time unit in multiple of \f$T_c\f$.
  const value_type to_Tc() const { return value; }

  /// \brief Gets the time in seconds.
  /// \tparam U Return type. Must be a floating point type (default: double).
  template <class U = double>
  constexpr U to_seconds() const
  {
    static_assert(std::is_convertible<double, U>::value && std::is_floating_point<U>::value, "Invalid type.");
    return static_cast<U>(value) * static_cast<U>(T_C);
  }

  /// \brief Determines if the time value is sample accurate given a sampling rate in hertz.
  ///
  /// The time value is sample accurate if the time is equivalent to an integer number of samples for the given sampling
  /// rate.
  ///
  /// \tparam U Any data type that can be converted to \c value_type.
  /// \param[in] sampling_rate_Hz Sample rate in hertz.
  /// \return True if the time is equivalent to an integer number of samples, false otherwise.
  template <class U>
  constexpr bool is_sample_accurate(U sampling_rate_Hz_) const
  {
    static_assert(std::is_convertible<U, value_type>::value, "Invalid type.");
    value_type sampling_rate_Hz = static_cast<value_type>(sampling_rate_Hz_);
    return ((std::abs(value) * sampling_rate_Hz) % (SCS_REF_HZ * N_F_REF * KAPPA)) == 0;
  }

  /// \brief Gets the time expressed as a number of samples for the given sampling rate.
  ///
  /// An assertion is triggered if the result would be a non-integer number of samples.
  ///
  /// \tparam U Any data type that can be converted to \c value_type.
  /// \param[in] scs      Subcarrier spacing in hertz.
  /// \param[in] dft_size DFT size corresponding to the subcarrier spacing.
  /// \return The time value in samples.
  template <typename U>
  constexpr value_type to_samples(U sampling_rate_Hz_) const
  {
    static_assert(std::is_convertible<U, value_type>::value, "Invalid type.");
    value_type sampling_rate_Hz = static_cast<value_type>(sampling_rate_Hz_);
    srsran_assert(is_sample_accurate(sampling_rate_Hz),
                  "Incompatible sampling rate {}.{:02} MHz with time {}.",
                  sampling_rate_Hz / 1000000,
                  (sampling_rate_Hz % 1000000) / 10000,
                  value);
    return (value * sampling_rate_Hz) / (SCS_REF_HZ * N_F_REF * KAPPA);
  }

  /// \brief Gets the time expressed in units of \f$T_{\textup{A}}\f$.
  ///
  /// Conversion is performed as per TS38.213 Section 4.2, rounding to the nearest integer.
  ///
  /// \param[in] scs Subcarrier spacing.
  /// \remark Used to compute the Timing Advance Command to be sent in RAR (not in MAC CE).
  constexpr unsigned to_Ta(subcarrier_spacing scs) const
  {
    return divide_round(value * pow2(to_numerology_value(scs)), 16U * KAPPA);
  }

  /// Overload addition operator.
  constexpr phy_time_unit operator+(phy_time_unit other) const
  {
    phy_time_unit ret(*this);
    ret += other;
    return ret;
  }

  /// Overload addition assignment operator.
  constexpr phy_time_unit operator+=(phy_time_unit other)
  {
    value += other.value;
    return *this;
  }

  /// Overload subtraction operator.
  constexpr phy_time_unit operator-(phy_time_unit other) const
  {
    phy_time_unit ret(*this);
    ret -= other;
    return ret;
  }

  /// Overload subtraction operator.
  constexpr phy_time_unit operator-=(phy_time_unit other)
  {
    value -= other.value;
    return *this;
  }

  /// Overload multiplication assignment operator.
  constexpr phy_time_unit operator*(unsigned multiplier) const
  {
    phy_time_unit ret(*this);
    ret *= multiplier;
    return ret;
  }

  /// Overload multiplication assignment operator.
  constexpr phy_time_unit operator*=(unsigned multiplier)
  {
    value *= multiplier;
    return *this;
  }

  /// Overload division operator.
  constexpr phy_time_unit operator/(unsigned divisor) const
  {
    phy_time_unit ret(*this);
    ret /= divisor;
    return ret;
  }

  /// Overload division assignment operator.
  constexpr phy_time_unit operator/=(unsigned divisor)
  {
    value /= divisor;
    return *this;
  }

  /// Overload equal to operator.
  constexpr bool operator==(phy_time_unit other) const { return value == other.value; }

  /// Overload not equal to operator.
  constexpr bool operator!=(phy_time_unit other) const { return value != other.value; }

  /// Overload greater than operator.
  constexpr bool operator>(phy_time_unit other) const { return value > other.value; }

  /// Overload lower than operator.
  constexpr bool operator<(phy_time_unit other) const { return value < other.value; }

  /// Overload greater than or equal to operator.
  constexpr bool operator>=(phy_time_unit other) const { return value >= other.value; }

  /// Overload lower than or equal to operator.
  constexpr bool operator<=(phy_time_unit other) const { return value <= other.value; }

  /// Creates a physical layer time from units of \f$\kappa\f$.
  static constexpr inline phy_time_unit from_units_of_kappa(unsigned units_of_kappa)
  {
    return phy_time_unit(static_cast<value_type>(units_of_kappa) * KAPPA);
  }

  /// Creates a physical layer time from units of \f$T_c\f$.
  static constexpr inline phy_time_unit from_units_of_Tc(unsigned units_of_Tc)
  {
    return phy_time_unit(static_cast<value_type>(units_of_Tc));
  }

  /// Creates a physical layer time from a timing advance command \f$T_A\f$, as per TS38.213 Section 4.2.
  static constexpr inline phy_time_unit from_timing_advance(unsigned units_of_Ta, subcarrier_spacing scs)
  {
    return from_units_of_Tc(units_of_Ta * 16U * KAPPA / pow2(to_numerology_value(scs)));
  }

  /// Creates a physical layer time from seconds.
  static constexpr inline phy_time_unit from_seconds(double seconds)
  {
    // Convert to units of Tc.
    double tc_units_dbl = seconds / T_C;
    // Multiply by ten and convert to value_type.
    value_type tc_units = static_cast<value_type>(tc_units_dbl * 10.0);
    // Round to the nearest integer avoiding using std::round.
    return phy_time_unit(tc_units / 10 + (tc_units % 10) / 5);
  }
};

/// \brief Gets the sampling rate from a subcarrier spacing and a DFT size combination.
/// \tparam U       Return data type. Must be convertible to unsigned (default: double).
/// \param[in] scs  Subcarrier spacing.
/// \param dft_size DFT size.
/// \return The sampling rate in hertz from the given SCS and DFT size.
template <typename U = double>
inline constexpr U to_sampling_rate_Hz(subcarrier_spacing scs, unsigned dft_size)
{
  static_assert(std::is_convertible<unsigned, U>::value, "Invalid type.");
  return static_cast<U>(scs_to_khz(scs) * 1000 * dft_size);
}

} // namespace srsran
