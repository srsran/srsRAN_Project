/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/gateways/baseband/baseband_gateway.h"
#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include "srsran/radio/radio_configuration.h"
#include "srsran/radio/radio_management_plane.h"
#include "srsran/radio/radio_notification_handler.h"

namespace srsran {

/// Describes a radio baseband unit session interface.
class radio_session : radio_base
{
public:
  /// \brief Gets the management plane.
  /// \return The reference to the management plane for this radio session.
  virtual radio_management_plane& get_management_plane() = 0;

  /// \brief Gets the data plane.
  /// \param[in] stream_id Stream identifier.
  /// \return The reference to the data plane for this radio session.
  /// \remark An assertion is triggered if the stream identifier is invalid.
  virtual baseband_gateway& get_baseband_gateway(unsigned stream_id) = 0;

  /// \brief Gets the current time.
  virtual baseband_gateway_timestamp read_current_time() = 0;

  /// \brief Starts the radio operation.
  ///
  /// It requests all radio streams to start simultaneously.
  ///
  /// \param[in] init_time Initial time in which streaming shall start.
  /// \remark Radio streams run until halted by a call of the stop() method.
  /// \remark Calling start() more than once results in undefined behavior, even after calling stop().
  virtual void start(baseband_gateway_timestamp init_time) = 0;

  /// \brief Stops the radio session operation.
  ///
  /// Any call to radio operation after calling stop() will return instantly without interacting with the physical
  /// radio.
  ///
  /// \remark Calling stop() without having called start() results in undefined behavior.
  /// \remark Calling start() after stop() results in undefined behavior.
  virtual void stop() = 0;
};

} // namespace srsran
