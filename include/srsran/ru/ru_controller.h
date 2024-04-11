/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// Prints RU specific metrics once.
  virtual void print_metrics() = 0;
};

} // namespace srsran
