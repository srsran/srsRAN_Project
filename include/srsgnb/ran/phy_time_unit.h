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

namespace srsgnb {

/// \brief Physical layer time unit.
///
/// This class abstracts the reference time units of the physical layer described in TS38.211 Section 4.1.
class phy_time_unit
{
private:
  /// Internal value type. It uses 64-bit to avoid intermediate multiplications to overflow.
  using value_type = uint64_t;
  /// Maximum subcarrier spacing in Hz, constant \f$\Delta f_{max}\f$.
  static constexpr value_type MAX_SCS_HZ = 480UL * 1000UL;
  /// Constant \f$N_f\f$.
  static constexpr value_type N_F = 4096UL;
  /// Reference chip time in seconds, constant \f$T_c = 1/(\Delta f_{max} \times N_f)\f$.
  static constexpr double T_C = 1.0 / static_cast<double>(MAX_SCS_HZ * N_F);
  /// Reference subcarrier spacing, constant \f$\Delta f_{ref}\f$
  static constexpr value_type SCS_REF_HZ = 15UL * 1000;
  /// DFT size for the reference subcarrier spacing, constant \f$N_{f,ref}\f$
  static constexpr value_type N_F_REF = 2048UL;
  // Reference sample time, constant \f$T_s = 1/(\Delta f_{ref} \times N_{f,ref})\f$
  //  static constexpr double T_S = 1.0 / static_cast<double>(SCS_REF_HZ * N_F_REF);
  /// Constant \f$\kappa=T_s/T_c=64\f$.
  static constexpr value_type KAPPA = 64UL;

  /// Actual value in counts of \f$T_c\f$.
  value_type value = 0;

  /// Private constructor from a value in units of \f$\kappa\f$.
  explicit constexpr phy_time_unit(value_type value_) : value(value_)
  {
    // Do nothing.
  }

public:
  /// Creates a default physical layer time unit of zero.
  phy_time_unit() = default;

  /// \brief Get the time in seconds.
  /// \tparam U Output type, it must be floating point and it is set to double by default.
  template <class U = double>
  constexpr U to_seconds() const
  {
    static_assert(std::is_convertible<double, U>::value && std::is_floating_point<U>::value, "Invalid type.");
    return static_cast<U>(value) * static_cast<U>(T_C);
  }

  /// \brief Determines if the time value is sample accurate given a sampling rate in Hz.
  ///
  /// The time value is sample accurate if the time is equivalent to an integer number of samples for the given sampling
  /// rate.
  ///
  /// \tparam U Any data type that be converted to \c value_type.
  /// \param[in] sampling_rate_Hz Sample rate in Hz.
  /// \return True if the time is equivalent to an integer number of samples. Otherwise, false.
  template <class U>
  constexpr bool is_sample_accurate(U sampling_rate_Hz_) const
  {
    static_assert(std::is_convertible<U, value_type>::value, "Invalid type.");
    value_type sampling_rate_Hz = static_cast<value_type>(sampling_rate_Hz_);
    return ((value * sampling_rate_Hz) % (SCS_REF_HZ * N_F_REF * KAPPA)) == 0;
  }

  /// \brief Get the time expressed in a number of samples from the sampling rate in Hz.
  ///
  /// An assertion is triggered if the result would be non-integer number of samples.
  ///
  /// \tparam U Any data type that be converted to \c value_type.
  /// \param[in] scs      Subcarrier spacing.
  /// \param[in] dft_size DFT size corresponding to the subcarrier spacing.
  /// \return The time value in samples.
  template <typename U>
  constexpr unsigned to_samples(U sampling_rate_Hz_) const
  {
    static_assert(std::is_convertible<U, uint64_t>::value, "Invalid type.");
    value_type sampling_rate_Hz = static_cast<value_type>(sampling_rate_Hz_);
    srsgnb_assert(is_sample_accurate(sampling_rate_Hz),
                  "Incompatible sampling rate {}.{:02} MHz with time {}.",
                  sampling_rate_Hz / 1000000,
                  (sampling_rate_Hz % 1000000) / 10000,
                  value);
    return static_cast<unsigned>((value * sampling_rate_Hz) / (SCS_REF_HZ * N_F_REF * KAPPA));
  }

  /// Creates a physical layer time from multiples of \f$\kappa\f$.
  static constexpr inline phy_time_unit from_units_of_kappa(unsigned units_of_kappa)
  {
    return phy_time_unit(static_cast<value_type>(units_of_kappa) * KAPPA);
  }

  /// Creates time from seconds.
  static constexpr inline phy_time_unit from_seconds(double seconds)
  {
    return phy_time_unit(static_cast<value_type>(seconds / T_C));
  }
};

/// \brief Gets the sampling rate from a subcarrier spacing and a DFT size combination.
/// \tparam U Return data type if given, otherwise it returns double.
/// \param[in] scs  Subcarrier spacing.
/// \param dft_size DFT size.
/// \return The sampling rate in Hz from the given SCS and DFT size.
template <typename U = double>
inline constexpr U to_sampling_rate_Hz(subcarrier_spacing scs, unsigned dft_size)
{
  static_assert(std::is_convertible<unsigned, U>::value, "Invalid type.");
  return static_cast<U>(scs_to_khz(scs) * 1000 * dft_size);
}

} // namespace srsgnb
