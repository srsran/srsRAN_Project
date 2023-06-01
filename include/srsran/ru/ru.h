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

namespace srsran {

class ru_controller;
class ru_downlink_plane_handler;
class ru_uplink_plane_handler;

/// \brief Radio Unit interface.
///
/// The Radio Unit interface provides downlink and uplink functionality through the uplink and downlink planes, allowing
/// data transmission and reception using a radio. It also notifies timing events using the \ref ru_timing_notifier.
/// Only a single Radio Unit interface should be used when operating with multiple sectors.
class radio_unit
{
public:
  /// Default destructor.
  virtual ~radio_unit() = default;

  /// Returns the controller interface of this Radio Unit.
  virtual ru_controller& get_controller() = 0;

  /// Returns the downlink plane handler interface of this Radio Unit.
  virtual ru_downlink_plane_handler& get_downlink_plane_handler() = 0;

  /// Returns the uplink plane interface handler of this Radio Unit.
  virtual ru_uplink_plane_handler& get_uplink_plane_handler() = 0;
};

} // namespace srsran
