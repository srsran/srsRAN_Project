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

#include "srsran/ran/phy_time_unit.h"
#include <optional>

namespace srsran {

/// \brief Radio Unit - operation control interface.
///
/// Allows starting and stopping the Radio Unit.
///
/// \remark The methods \c start() and \c stop() must be called in order and only once; other uses will result in
/// undefined behavior.
class ru_operation_controller
{
public:
  virtual ~ru_operation_controller() = default;

  /// \brief Starts the Radio Unit operation.
  /// \note Caller will be blocked until the controller is fully started.
  virtual void start() = 0;

  /// \brief Stops the Radio Unit operation.
  /// \note Caller will be blocked until the controller is fully stopped.
  virtual void stop() = 0;
};

/// \brief Radio Unit - gain control interface.
///
/// Allows to modify the gain of the transmission and reception ports of the Radio Unit.
class ru_gain_controller
{
public:
  virtual ~ru_gain_controller() = default;

  /// \brief Sets the transmission gain for the specified port.
  /// \param[in] port_id Port identifier.
  /// \param[in] gain_dB Transmission gain in dB.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_tx_gain(unsigned port_id, double gain_dB) = 0;

  /// \brief Sets the receive gain for the specified port.
  /// \param[in] port_id Port identifier.
  /// \param[in] gain_dB Receive gain in dB.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_rx_gain(unsigned port_id, double gain_dB) = 0;
};

/// \brief Structure used to set CFO via RU CFO controller.
struct cfo_compensation_request {
  using time_point = std::chrono::system_clock::time_point;
  /// Carrier Frequency Offset in Hz.
  float cfo_hz = 0.0;
  /// Carrier Frequency Offset drift in Hz/s relative to the start time. Set it to 0 for no drift.
  float cfo_drift_hz_s = 0.0;
  /// Optional timestamp at which the CFO command is applied. Set to \c std::nullopt for applying it immediately.
  std::optional<time_point> start_timestamp;
};

/// \brief Radio Unit - carrier frequency offset control interface.
///
/// Provides an interface for modifying the carrier frequency offset for downlink and uplink in runtime.
class ru_cfo_controller
{
public:
  /// Default destructor.
  virtual ~ru_cfo_controller() = default;

  /// \brief Sets the downlink carrier frequency offset for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] cfo_request CFO config to be set.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_tx_cfo(unsigned sector_id, const cfo_compensation_request& cfo_request) = 0;

  /// \brief Sets the uplink carrier frequency offset for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] cfo_request CFO config to be set.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_rx_cfo(unsigned sector_id, const cfo_compensation_request& cfo_request) = 0;
};

/// \brief Radio Unit - carrier center frequency control interface.
///
/// Provides an interface for modifying the carrier center frequency for downlink and uplink in runtime.
class ru_center_frequency_controller
{
public:
  /// Default destructor.
  virtual ~ru_center_frequency_controller() = default;

  /// \brief Sets the downlink carrier center frequency for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] center_freq_Hz Center frequency in Hertz.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_tx_center_frequency(unsigned sector_id, double center_freq_Hz) = 0;

  /// \brief Sets the uplink carrier center frequency for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] center_freq_Hz Center frequency in Hertz.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_rx_center_frequency(unsigned sector_id, double center_freq_Hz) = 0;
};

/// \brief Radio Unit - transmit time offset control interface.
///
/// Allows setting in runtime a transmit time offset. The transmit signal continuity is not guaranteed for the slot in
/// which the transmission time offset is applied.
class ru_tx_time_offset_controller
{
public:
  /// Default destructor.
  virtual ~ru_tx_time_offset_controller() = default;

  /// \brief Sets the transmit time offset for a specified sector.
  /// \param[in] sector_id      Sector identifier.
  /// \param[in] tx_time_offset Time offset value.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_tx_time_offset(unsigned sector_id, phy_time_unit tx_time_offset) = 0;
};

/// \brief Radio Unit - control interface.
///
/// Allows starting and stopping the Radio Unit.
///
/// \remark The methods \c start() and \c stop() must be called in order and only once; other uses will result in
/// undefined behavior.
class ru_controller
{
public:
  /// Default destructor.
  virtual ~ru_controller() = default;

  /// Returns the operation controller of this Radio Unit.
  virtual ru_operation_controller& get_operation_controller() = 0;

  /// Returns the gain controller of this Radio Unit or nullptr if the Radio Unit does not support it.
  virtual ru_gain_controller* get_gain_controller() = 0;

  /// Returns the carrier frequency offset controller of this Radio Unit or nullptr if the Radio Unit does not support
  /// it.
  virtual ru_cfo_controller* get_cfo_controller() = 0;

  /// Returns the carrier center frequency controller of this Radio Unit or \c nullptr if the Radio Unit does not
  /// support.
  virtual ru_center_frequency_controller* get_center_frequency_controller() = 0;

  /// Returns the transmit time offset controller of this Radio Unit or nullptr if the Radio unit does not support it.
  virtual ru_tx_time_offset_controller* get_tx_time_offset_controller() = 0;
};

} // namespace srsran
