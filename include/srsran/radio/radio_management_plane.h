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
};

} // namespace srsran
