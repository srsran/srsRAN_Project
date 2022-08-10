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
#include "srsgnb/support/srsgnb_assert.h"
#include <cstdint>

namespace srsgnb {

/// Sampling rate.
class sampling_rate
{
public:
  /// Value storage type.
  using value_type = uint64_t;

  /// Default constructor.
  sampling_rate() = default;

  /// \brief Determines if the sampling rate is valid for a certain subcarrier spacing.
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] scs Subcarrier spacing in Hz.
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
  /// \param[in] scs Subcarrier spacing enumarator.
  /// \return True if the sampling rate value is not zero and is divisible by the subcarrier spacing, false
  /// otherwise.
  constexpr bool is_valid(subcarrier_spacing scs) const { return is_valid(scs_to_khz(scs) * 1000); }

  /// \brief Get the DFT size for a given subcarrier spacing.
  ///
  /// An assertion is triggered if the combination of sampling rate and subcarrier spacing is invalid (see is_valid()).
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] scs_Hz The given subcarrier spacing in Hz.
  /// \return The DFT size according to the value and the subcarrier spacing.
  template <typename T>
  constexpr unsigned get_dft_size(T scs_Hz) const
  {
    srsgnb_assert(is_valid(scs_Hz), "Subcarrier spacing must be valid.");
    return value / static_cast<value_type>(scs_Hz);
  }

  /// \brief Get the DFT size for a given subcarrier spacing.
  ///
  /// An assertion is triggered if the combination of sampling rate and subcarrier spacing is invalid (see is_valid()).
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] scs_Hz The given subcarrier spacing in Hz.
  /// \return The DFT size according to the value and the subcarrier spacing.
  constexpr unsigned get_dft_size(subcarrier_spacing scs) const { return get_dft_size(scs_to_khz(scs) * 1000); }

  /// \brief Gets the value in Hz.
  /// \tparam T Any type that can be converted from \ref value_type.
  /// \return The value in Hz.
  template <typename T = unsigned>
  constexpr T to_Hz() const
  {
    static_assert(std::is_convertible<value_type, T>::value, "Invalid data type.");
    return static_cast<T>(value);
  }

  /// Gets the value in MHz.
  constexpr double to_MHz() const { return static_cast<double>(value) / 1e6; }

  /// \brief Create from a value in Hz.
  ///
  /// An assertion is triggered if:
  /// - the sampling rate is zero, or
  /// - it is not multiple of 15kHz.
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] value_Hz Sampling rate in Hz.
  template <typename T>
  static sampling_rate from_Hz(T value_Hz)
  {
    static_assert(std::is_convertible<T, value_type>::value, "Invalid data type.");
    return sampling_rate(static_cast<value_type>(value_Hz));
  }

  /// \brief Create from a value in MHz.
  ///
  /// An assertion is triggered if:
  /// - the sampling rate is zero, or
  /// - it is not multiple of 15kHz.
  ///
  /// \tparam T Any type convertible to \ref value_type.
  /// \param[in] value_Hz Sampling rate in Hz.
  template <typename T>
  static sampling_rate from_MHz(T value_MHz)
  {
    return from_Hz(value_MHz * 1e6);
  }

private:
  /// Private constructor.
  constexpr explicit sampling_rate(value_type value_) : value(value_)
  {
    srsgnb_assert(is_valid(15000), "Sampling rate cannot be zero.");
  }

  /// Actual value in Hz.
  value_type value = 0;
};

} // namespace srsgnb

namespace fmt {

/// FMT formatter of sampling_rate type.
template <>
struct formatter<srsgnb::sampling_rate> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(srsgnb::sampling_rate srate, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{:.2f}MHz", srate.to_MHz());
  }
};
} // namespace fmt