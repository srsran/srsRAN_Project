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

/// \brief Radio Unit - carrier frequency offset control interface.
///
/// Allows modify the carrier frequency offset for downlink and uplink of the Radio Unit.
class ru_cfo_controller
{
public:
  virtual ~ru_cfo_controller() = default;

  /// \brief Sets the downlink carrier frequency offset for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] cfo_hz    Transmission CFO in Hz.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_tx_cfo(unsigned sector_id, float cfo_offset) = 0;

  /// \brief Sets the uplink carrier frequency offset for the specified sector.
  /// \param[in] sector_id Sector identifier.
  /// \param[in] cfo_hz    Transmission CFO in Hz.
  /// \return \c true if the operation is successful, \c false otherwise.
  virtual bool set_rx_cfo(unsigned sector_id, float cfo_offset) = 0;
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

  /// Returns the carrier frequency offset of this Radio Unit or nullptr if the Radio Unit does not support it.
  virtual ru_cfo_controller* get_cfo_controller() = 0;

  /// Prints RU specific metrics once.
  virtual void print_metrics() = 0;
};

} // namespace srsran
