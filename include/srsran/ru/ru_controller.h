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

namespace srsran {

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

  /// \brief Starts the Radio Unit operation.
  /// \note Caller will be blocked until the controller is fully started.
  virtual void start() = 0;

  /// \brief Stops the Radio Unit operation.
  /// \note Caller will be blocked until the controller is fully stopped.
  virtual void stop() = 0;

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

} // namespace srsran
