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
#include "srsran/support/srsran_assert.h"
#include <cstdint>

namespace srsran {

/// \brief Sampling rate.
///
/// This class abstracts the concept of sampling rate from its units of measure. Specifically, the sampling rate is
/// stored, privately, in a convenient representation and it can be accessed by getter and setter methods that
/// automatically convert the sampling rate to and from, respectively, the desired representation/unit of measure.
class sampling_rate
{
public:
  /// Value storage type.
  using value_type = uint64_t;

  /// Default constructor.
  sampling_rate() = default;

  /// \brief Determines if the sampling rate is valid for a certain subcarrier spacing.
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] scs_Hz Subcarrier spacing in hertz.
  /// \return True if the sampling rate value is not zero and is divisible by the subcarrier spacing, false
  /// otherwise.
  template <typename T>
  constexpr bool is_valid(T scs_Hz) const
  {
    static_assert(std::is_convertible<T, value_type>::value, "Invalid data type.");
    if (value == 0 || scs_Hz == 0) {
      return false;
    }
    return value % static_cast<value_type>(scs_Hz) == 0;
  }

  /// \brief Determines if the sampling rate is valid for a certain subcarrier spacing.
  /// \param[in] scs Subcarrier spacing enumerator.
  /// \return True if the sampling rate value is not zero and is divisible by the subcarrier spacing, false
  /// otherwise.
  constexpr bool is_valid(subcarrier_spacing scs) const { return is_valid(scs_to_khz(scs) * 1000); }

  /// \brief Gets the DFT size corresponding to the sampling rate, for a given subcarrier spacing.
  ///
  /// An assertion is triggered if the combination of sampling rate and subcarrier spacing is invalid (see is_valid()).
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] scs_Hz The given subcarrier spacing in hertz.
  /// \return The sampling rate represented as the size of a DFT with the given subcarrier spacing.
  template <typename T>
  constexpr unsigned get_dft_size(T scs_Hz) const
  {
    srsran_assert(is_valid(scs_Hz), "Subcarrier spacing must be valid.");
    return value / static_cast<value_type>(scs_Hz);
  }

  /// \brief Gets the DFT size corresponding to the sampling rate, for a given subcarrier spacing.
  ///
  /// An assertion is triggered if the combination of sampling rate and subcarrier spacing is invalid (see is_valid()).
  ///
  /// \param[in] scs Subcarrier spacing enumerator.
  /// \return The sampling rate represented as the size of a DFT with the given subcarrier spacing.
  constexpr unsigned get_dft_size(subcarrier_spacing scs) const { return get_dft_size(scs_to_khz(scs) * 1000); }

  /// \brief Gets the sampling rate in hertz.
  /// \tparam T Any type that can be converted from \ref value_type.
  /// \return The value in hertz.
  template <typename T = unsigned>
  constexpr T to_Hz() const
  {
    static_assert(std::is_convertible<value_type, T>::value, "Invalid data type.");
    return static_cast<T>(value);
  }

  /// \brief Gets the sampling rate in kHz.
  /// \tparam T Any type that can be converted from \ref value_type.
  /// \return The value in hertz.
  template <typename T = unsigned>
  constexpr T to_kHz() const
  {
    static_assert(std::is_convertible<value_type, T>::value, "Invalid data type.");
    return static_cast<T>(value / 1000);
  }

  /// Gets the sampling rate in MHz.
  constexpr double to_MHz() const { return static_cast<double>(value) / 1e6; }

  /// \brief Creates a \c sampling_rate object from its value in hertz.
  ///
  /// An assertion is triggered if:
  /// - the sampling rate is zero, or
  /// - it is not a multiple of 15kHz.
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] value_Hz Sampling rate in hertz.
  template <typename T>
  static sampling_rate from_Hz(T value_Hz)
  {
    static_assert(std::is_convertible<T, value_type>::value, "Invalid data type.");
    return sampling_rate(static_cast<value_type>(value_Hz));
  }

  /// \brief Creates a \c sampling_rate object from its value in kHz.
  ///
  /// An assertion is triggered if:
  /// - the sampling rate is zero, or
  /// - it is not multiple of 15kHz.
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] value_kHz Sampling rate in kHz.
  template <typename T>
  static sampling_rate from_kHz(T value_kHz)
  {
    return from_Hz(value_kHz * 1e3);
  }

  /// \brief Creates a \c sampling_rate object from its value in MHz.
  ///
  /// An assertion is triggered if:
  /// - the sampling rate is zero, or
  /// - it is not multiple of 15kHz.
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] value_MHz Sampling rate in Mhz.
  template <typename T>
  static sampling_rate from_MHz(T value_MHz)
  {
    return from_Hz(value_MHz * 1e6);
  }

  /// Overload equal to operator.
  constexpr bool operator==(sampling_rate other) const { return value == other.value; }

  /// Overload different than operator.
  constexpr bool operator!=(sampling_rate other) const { return value != other.value; }

private:
  /// Private constructor.
  constexpr explicit sampling_rate(value_type value_) : value(value_)
  {
    srsran_assert(is_valid(15000), "The sampling rate must be multiple of 15kHz.");
  }

  /// Actual value in hertz.
  value_type value = 0;
};

} // namespace srsran

namespace fmt {

/// FMT formatter of sampling_rate type.
template <>
struct formatter<srsran::sampling_rate> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsran::sampling_rate srate, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{:.2f}MHz", srate.to_MHz());
  }
};
} // namespace fmt
