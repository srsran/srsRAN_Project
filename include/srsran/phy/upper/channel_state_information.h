/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/constants.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/phy_time_unit.h"
#include <optional>

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

  /// \brief Sets the total Error Vector Magnitude (EVM).
  ///
  /// The EVM value is ignored if it is NaN.
  void set_total_evm(float evm_)
  {
    // Ignore measurement if it is NaN.
    if (std::isnan(evm_)) {
      return;
    }

    total_evm.emplace(evm_);
  }

  /// \brief Gets the total EVM.
  ///
  /// \return The measured EVM if present, otherwise \c std::nullopt.
  std::optional<float> get_total_evm() const
  {
    // If the total EVM has value, use that one.
    if (total_evm.has_value()) {
      return total_evm;
    }

    // Otherwise try to combine all EVM.
    unsigned count = 0;
    float    sum   = 0.0f;
    std::for_each(symbol_evm.begin(), symbol_evm.end(), [&count, &sum](std::optional<float> elem) {
      // Skip accumulating if it does not contain a value or it is infinity or nan.
      if (!elem.has_value() || std::isinf(elem.value()) || std::isnan(elem.value())) {
        return;
      }
      sum += elem.value();
      ++count;
    });

    if (count == 0) {
      return std::nullopt;
    }

    return sum / static_cast<float>(count);
  }

  /// \brief Sets an OFDM symbol Error Vector Magnitude (EVM).
  ///
  /// The EVM value is ignored if it is NaN.
  void set_symbol_evm(unsigned i_symbol, float evm_)
  {
    // Ignore measurement if it is NaN.
    if (std::isnan(evm_)) {
      return;
    }

    srsran_assert(i_symbol < symbol_evm.size(), "The OFDM symbol index (i.e., {}) exceeds slot duration.");
    symbol_evm[i_symbol].emplace(evm_);
  }

  /// \brief Gets the EVM per symbol.
  span<const std::optional<float>> get_symbol_evm() const
  {
    return span<const std::optional<float>>(symbol_evm.begin(), symbol_evm.end());
  }

  /// Sets the time alignment measurement in PHY time units.
  void set_time_alignment(const phy_time_unit& time_alignment_) { time_alignment.emplace(time_alignment_); }

  /// Resets the time alignment.
  void reset_time_alignment() { time_alignment.reset(); }

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

  /// \brief Sets an RSRP value that is representative of the received reference signal power across all ports.
  ///
  /// The input RSRP value is ignored if it is NaN.
  void set_rsrp_dB(float rsrp_dB_)
  {
    if (std::isnan(rsrp_dB_)) {
      return;
    }
    rsrp_dB = rsrp_dB_;
  }

  /// \brief Sets the Reference Signal Received Power (RSRP) for each antenna port.
  ///
  /// Takes a list of port RSRP values in linear units and stores the measurements in normalized dB units. A global RSRP
  /// metric is also computed as the average of all valid RSRP values. Input RSRP values are ignored if they are NaN.
  ///
  /// \param[in] rsrp_per_port_lin RSRP values in linear units for each antenna port.
  void set_rsrp_lin(span<const float> rsrp_per_port_lin)
  {
    unsigned nof_ports = rsrp_per_port_lin.size();
    srsran_assert(nof_ports <= MAX_PORTS,
                  "The number of ports (i.e., {}) exceeds the maximum allowed (i.e., {})",
                  nof_ports,
                  MAX_PORTS);

    port_rsrp_dB.resize(nof_ports);

    float    rsrp_total_lin        = 0.0f;
    unsigned nof_valid_rsrp_values = 0;
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      float rsrp_lin = rsrp_per_port_lin[i_port];
      // Set the RSRP to NaN if the measurement is not valid and don't include it in the total RSRP.
      if (std::isnan(rsrp_lin)) {
        port_rsrp_dB[i_port] = std::numeric_limits<float>::quiet_NaN();
        continue;
      }

      port_rsrp_dB[i_port] = convert_power_to_dB(rsrp_lin);

      // Accumulate all valid RSRP values.
      rsrp_total_lin += rsrp_lin;
      ++nof_valid_rsrp_values;
    }

    // Compute a global RSRP metric as the average of all valid RSRP values.
    if (rsrp_total_lin > 0.0F) {
      rsrp_dB = convert_power_to_dB(rsrp_total_lin / static_cast<float>(nof_valid_rsrp_values));
    } else {
      rsrp_dB.reset();
    }
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

  /// \brief Gets the Reference Signal Received Power (RSRP) in normalized dB units, for each receive port.
  /// \return The measured RSRP for each port, and \c NaN if not available.
  span<const float> get_port_rsrp_dB() const { return span<const float>(port_rsrp_dB.begin(), port_rsrp_dB.end()); }

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
  /// Error Vector Magnitude (EVM), obtained after QAM demodulation.
  std::optional<float> total_evm;
  /// Error Vector Magnitude (EVM) per OFDM symbol, obtained after QAM demodulation.
  std::array<std::optional<float>, MAX_NSYMB_PER_SLOT> symbol_evm;
  /// SINR obtained from EVM.
  std::optional<float> sinr_evm_dB;
  /// Average SINR in decibels, computed by the channel estimator.
  std::optional<float> sinr_ch_estimator_dB;
  /// Average EPRE in decibels.
  std::optional<float> epre_dB;
  /// RSRP in decibels, averaged across all antenna ports.
  std::optional<float> rsrp_dB;
  /// RSRP per antenna port in decibels.
  static_vector<float, MAX_PORTS> port_rsrp_dB;
  /// CFO measurement in hertz.
  std::optional<float> cfo_Hz;
};

} // namespace srsran
