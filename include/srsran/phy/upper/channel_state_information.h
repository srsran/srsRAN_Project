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

#include "srsran/adt/optional.h"
#include "srsran/ran/phy_time_unit.h"

namespace srsran {

/// \brief Channel State Information measurements.
///
/// All measurements are optional and they are initialized with \c std::nullopt.
///
/// The measurements shall not store NaN in their values.
class channel_state_information
{
public:
  /// Types of SINR that can be computed by the upper PHY.
  enum class sinr_type {
    /// SINR computed by the channel estimator, using reference signals.
    channel_estimator,
    /// SINR computed at the output of the channel equalizer, using the noise variances of the equalized RE.
    post_equalization,
    /// SINR obtained from the EVM of the demodulated symbols.
    evm
  };

  /// Default constructor - Reported SINR is obtained from the channel estimator.
  channel_state_information() : sinr_report_type(sinr_type::channel_estimator)
  {
    // Do nothing.
  }

  /// Constructor that allows selection of the reported SINR type.
  explicit channel_state_information(sinr_type sinr_report_type_) : sinr_report_type(sinr_report_type_)
  {
    // Do nothing.
  }

  /// Copy constructor.
  channel_state_information(const channel_state_information& other) = default;

  /// Gets a SINR type from a string representation.
  static sinr_type sinr_type_from_string(const std::string& sinr_type_str)
  {
    if (sinr_type_str == "channel_estimator") {
      return sinr_type::channel_estimator;
    }
    if (sinr_type_str == "post_equalization") {
      return sinr_type::post_equalization;
    }
    if (sinr_type_str == "evm") {
      return sinr_type::evm;
    }
    srsran_assertion_failure("Invalid SINR type");
    return sinr_type::channel_estimator;
  }

  /// \brief Gets the SINR value that is used to select the modulation and coding scheme.
  ///
  /// \return The selected SINR in decibels if it is available, otherwise \c std::nullopt.
  std::optional<float> get_sinr_dB() const
  {
    switch (sinr_report_type) {
      case sinr_type::channel_estimator:
        return sinr_ch_estimator_dB;
      case sinr_type::post_equalization:
        return sinr_post_eq_dB;
      case sinr_type::evm:
      default:
        return sinr_evm_dB;
    }
  }

  /// \brief Sets a SINR estimation in dB units.
  ///
  /// The SINR value is ignored if it is NaN.
  ///
  /// \param[in] type SINR measurement type to set.
  /// \param[in] sinr_dB The SINR value in dB.
  void set_sinr_dB(sinr_type type, float sinr_dB)
  {
    // Ignore measurement if it is NaN.
    if (std::isnan(sinr_dB)) {
      return;
    }

    switch (type) {
      case sinr_type::channel_estimator:
        sinr_ch_estimator_dB.emplace(sinr_dB);
        break;
      case sinr_type::post_equalization:
        sinr_post_eq_dB.emplace(sinr_dB);
        break;
      case sinr_type::evm:
        sinr_evm_dB.emplace(sinr_dB);
        break;
    }
  }

  /// \brief Sets the Error Vector Magnitude (EVM).
  ///
  /// The EVM value is ignored if it is NaN.
  void set_evm(float evm_)
  {
    // Ignore measurement if it is NaN.
    if (std::isnan(evm_)) {
      return;
    }

    evm.emplace(evm_);
  }

  /// \brief Gets the EVM.
  ///
  /// \return The measured EVM if present, otherwise \c std::nullopt.
  std::optional<float> get_evm() const { return evm; }

  /// \brief Sets the time alignment measurement in PHY time units.
  void set_time_alignment(const phy_time_unit& time_alignment_) { time_alignment.emplace(time_alignment_); }

  /// \brief Gets the time alignment.
  ///
  /// \return The time aligment measurement if present, otherise \c std::nullopt.
  std::optional<phy_time_unit> get_time_alignment() const { return time_alignment; }

  /// \brief Sets the Energy Per Resource Element (EPRE) in normalized dB units.
  ///
  /// The EPRE value is ignored if it is NaN.
  void set_epre(float epre_dB_)
  {
    if (std::isnan(epre_dB_)) {
      return;
    }
    epre_dB.emplace(epre_dB_);
  }

  /// \brief Gets the Energy Per Resource Element (EPRE) in normalized dB units.
  /// \return The measured EPRE if present, otherwise \c std::nullopt.
  std::optional<float> get_epre_dB() const { return epre_dB; }

  /// \brief Sets the Reference Signal Received Power (RSRP) in normalized dB units.
  ///
  /// The RSRP value is ignored if it is NaN.
  void set_rsrp(float rsrp_dB_)
  {
    if (std::isnan(rsrp_dB_)) {
      return;
    }

    rsrp_dB.emplace(rsrp_dB_);
  }

  /// \brief Gets the Reference Signal Received Power (RSRP) in normalized dB units.
  /// \return The measured RSRP if present, otherwise \c std::nullopt.
  std::optional<float> get_rsrp_dB() const { return rsrp_dB; }

  /// \brief Sets the measured Carrier Frequency Offset (CFO) in hertz.
  ///
  /// The CFO value is ignored if NaN.
  void set_cfo(float cfo_Hz_)
  {
    if (std::isnan(cfo_Hz_)) {
      return;
    }

    cfo_Hz.emplace(cfo_Hz_);
  }

  /// \brief Gets the measured Carrier Frequency Offset (CFO) in hertz.
  /// \return The measured CFO if present, \c std::nullopt otherwise.
  std::optional<float> get_cfo_Hz() const { return cfo_Hz; }

private:
  friend struct fmt::formatter<channel_state_information>;
  /// \brief SINR type that can be accessed by \ref get_sinr_dB.
  ///
  /// It is used to select, from all the available SINR values, the one that is used for choosing the modulation and
  /// coding scheme.
  sinr_type sinr_report_type;
  /// Time alignment measurement.
  std::optional<phy_time_unit> time_alignment;
  /// Average post-equalization SINR.
  std::optional<float> sinr_post_eq_dB;
  /// Error Vector Magnitude, obtained after QAM demodulation.
  std::optional<float> evm;
  /// SINR obtained from EVM.
  std::optional<float> sinr_evm_dB;
  /// Average SINR in decibels, computed by the channel estimator.
  std::optional<float> sinr_ch_estimator_dB;
  /// Average EPRE in decibels.
  std::optional<float> epre_dB;
  /// Average RSRP in decibels.
  std::optional<float> rsrp_dB;
  /// CFO measurement in hertz.
  std::optional<float> cfo_Hz;
};

} // namespace srsran
