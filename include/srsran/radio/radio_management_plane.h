/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/radio/radio_base.h"

namespace srsran {

/// Defines a radio baseband unit management plane interface.
class radio_management_plane : public radio_base
{
public:
  /// \brief Set the transmission gain of a given port in runtime.
  /// \param[in] port_id Indicates the port identifier.
  /// \param[in] gain_dB Indicates the new gain.
  /// \return True if the operation is successful. Otherwise, false.
  /// \note The port identifier is indexed from stream ascending order and then port.
  virtual bool set_tx_gain(unsigned port_id, double gain_dB) = 0;

  /// \brief Set the reception gain of a given port in runtime.
  /// \param[in] port_id Indicates the port identifier.
  /// \param[in] gain_dB Indicates the new gain.
  /// \return True if the operation is successful. Otherwise, false.
  /// \note The port identifier is indexed from stream ascending order and then port.
  virtual bool set_rx_gain(unsigned port_id, double gain_dB) = 0;

  /// \brief Set the transmission center frequency of all ports related to a stream.
  /// \param[in] stream_id      Radio stream identifier.
  /// \param[in] center_freq_Hz New center frequency in Hertz.
  /// \return True if the operation is successful. Otherwise, false.
  virtual bool set_tx_freq(unsigned stream_id, double center_freq_Hz) = 0;

  /// \brief Set the reception center frequency of all ports related to a stream.
  /// \param[in] stream_id      Radio stream identifier.
  /// \param[in] center_freq_Hz New center frequency in Hertz.
  /// \return True if the operation is successful. Otherwise, false.
  virtual bool set_rx_freq(unsigned stream_id, double center_freq_Hz) = 0;
};

} // namespace srsran
