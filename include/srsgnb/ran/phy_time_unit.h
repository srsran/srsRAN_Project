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

#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/support/math_utils.h"

namespace srsgnb {

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
    static_assert(std::is_convertible<U, uint64_t>::value, "Invalid type.");
    value_type sampling_rate_Hz = static_cast<value_type>(sampling_rate_Hz_);
    srsgnb_assert(is_sample_accurate(sampling_rate_Hz),
                  "Incompatible sampling rate {}.{:02} MHz with time {}.",
                  sampling_rate_Hz / 1000000,
                  (sampling_rate_Hz % 1000000) / 10000,
                  value);
    return (value * sampling_rate_Hz) / (SCS_REF_HZ * N_F_REF * KAPPA);
  }

  /// \brief Gets the time expressed in units of \f$T_{\textup{A}}\f$.
  ///
  /// Conversion is performed as per TS38.213 Section 4.2.
  ///
  /// \param[in] scs Subcarrier spacing.
  constexpr unsigned to_Ta(subcarrier_spacing scs) const
  {
    return value * pow2(to_numerology_value(scs)) / (16 * KAPPA);
  }

  /// Overload addition operator.
  constexpr phy_time_unit operator+(phy_time_unit other)
  {
    value += other.value;
    return *this;
  }

  /// Overload addition operator.
  constexpr phy_time_unit operator+=(phy_time_unit other)
  {
    value += other.value;
    return *this;
  }

  /// Overload subtraction operator.
  constexpr phy_time_unit operator-(phy_time_unit other)
  {
    value -= other.value;
    return *this;
  }

  /// Overload subtraction operator.
  constexpr phy_time_unit operator-=(phy_time_unit other)
  {
    value -= other.value;
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

  /// \brief Computes time instants in number of samples aligned with NR-PHY time units.
  ///
  /// Given a time expressed in number of samples, determine the closest inferior and superior time instants,
  /// in number of samples, that can be expressed as an integer number of \f$T_c\f$ units.
  ///
  /// \param[in] time_samples Time expressed in number of samples.
  /// \param[in] sampling_rate_Hz Sampling rate associated with the provided \c time_samples.
  /// \return The computed closest aligned time instants, in number of samples.
  /// \note If the provided time in number of samples is already aligned with an integer number of \f$T_c\f$, the
  /// return values are both set to the input \c time_samples.
  static inline std::pair<int, int> tc_aligned_nof_samples(int time_samples, unsigned sampling_rate_Hz)
  {
    // Inverse of Tc.
    value_type tc_rcp = N_F * MAX_SCS_HZ;

    // Compute the GCD of the Tc inverse and the sampling rate.
    value_type gcd = std::__gcd(tc_rcp, static_cast<value_type>(sampling_rate_Hz));

    // Use the GCD to derive the Least Common Multiple.
    value_type lcm = sampling_rate_Hz * tc_rcp / gcd;

    // Compute the periodicity at which the time instants expressed in number of samples align with a time instant
    // expressed in number of Tc. This can be used to derive which time instants in number of samples can be expressed
    // as an integer number of Tc, i.e., for a periodicity of N samples: 0, N, 2N, ...
    value_type periodicity_sps = lcm / tc_rcp;

    // Compute the time in samples aligned with Tc that is closest to time_samples and with smaller absolute value.
    int lower_aligned_t_sps = static_cast<int>((time_samples / periodicity_sps) * periodicity_sps);

    // If the provided time in number of samples is already aligned, return the same value.
    if (lower_aligned_t_sps == time_samples) {
      return std::make_pair(time_samples, time_samples);
    }

    // Compute the time in samples aligned with Tc that is closest to time_samples and with larger absolute value.
    int higher_aligned_t_sps = lower_aligned_t_sps;
    if (lower_aligned_t_sps >= 0) {
      higher_aligned_t_sps += static_cast<int>(periodicity_sps);
    } else {
      higher_aligned_t_sps -= static_cast<int>(periodicity_sps);
    }

    return std::make_pair(lower_aligned_t_sps, higher_aligned_t_sps);
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

} // namespace srsgnb
