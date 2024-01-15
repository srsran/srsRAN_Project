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

/// Channel State Information parameters.
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

  /// Default constructor. Reported SINR is obtained from the channel estimator.
  channel_state_information() : sinr_report_type(sinr_type::channel_estimator), epre_dB(NAN), rsrp_dB(NAN)
  {
    // Do nothing.
  }

  /// Constructor that allows selection of the reported SINR type.
  explicit channel_state_information(sinr_type sinr_report_type_) :
    sinr_report_type(sinr_report_type_), epre_dB(NAN), rsrp_dB(NAN)
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

  /// Gets the SINR value that is used to select the modulation and coding scheme.
  float get_sinr_dB() const
  {
    switch (sinr_report_type) {
      case sinr_type::channel_estimator:
        srsran_assert(sinr_ch_estimator_dB.has_value(),
                      "Selected CSI SINR type, i.e., channel estimator, is not available.");
        return sinr_ch_estimator_dB.value();
      case sinr_type::post_equalization:
        srsran_assert(sinr_post_eq_dB.has_value(),
                      "Selected CSI SINR type, i.e., post equalization, is not available.");
        return sinr_post_eq_dB.value();
      case sinr_type::evm:
      default:
        srsran_assert(sinr_evm_dB.has_value(), "Selected CSI SINR type, i.e., from EVM, is not available.");
        return sinr_evm_dB.value();
    }
  }

  /// \brief Sets SINR values.
  ///
  /// \param[in] type SINR type to set.
  /// \param[in] sinr_dB The SINR value in dB.
  void set_sinr_dB(sinr_type type, float sinr_dB)
  {
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

  /// Sets the Error Vector Magnitude (EVM).
  void set_evm(float evm_) { evm.emplace(evm_); }

  /// Sets the time alignment.
  void set_time_alignment(const phy_time_unit& time_alignment_) { time_alignment = time_alignment_; }
  /// Gets the time alignment.
  phy_time_unit get_time_alignment() const { return time_alignment; }

  /// Sets the Energy Per Resource Element (EPRE) in normalized dB units.
  void set_epre(float epre_dB_) { epre_dB = epre_dB_; }
  /// Gets the Energy Per Resource Element (EPRE) in normalized dB units.
  float get_epre_dB() const { return epre_dB; }

  /// Sets the Reference Signal Received Power (RSRP) in normalized dB units.
  void set_rsrp(float rsrp_dB_) { rsrp_dB = rsrp_dB_; }
  /// Gets the Reference Signal Received Power (RSRP) in normalized dB units.
  float get_rsrp_dB() const { return rsrp_dB; }

private:
  friend struct fmt::formatter<channel_state_information>;
  /// \brief SINR type that can be accessed by \ref get_sinr_dB.
  ///
  /// It is used to select, from all the available SINR values, the one that is used for choosing the modulation and
  /// coding scheme.
  sinr_type sinr_report_type;
  /// Time alignment measurement.
  phy_time_unit time_alignment;
  /// Average post-equalization SINR.
  optional<float> sinr_post_eq_dB;
  /// Error Vector Magnitude, obtained after QAM demodulation.
  optional<float> evm;
  /// SINR obtained from EVM.
  optional<float> sinr_evm_dB;
  /// Average SINR in decibels, computed by the channel estimator.
  optional<float> sinr_ch_estimator_dB;
  /// Average EPRE in decibels.
  float epre_dB;
  /// Average RSRP in decibels.
  float rsrp_dB;
};

} // namespace srsran
