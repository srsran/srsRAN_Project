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

class ru_downlink_plane_handler;
class ru_uplink_plane_handler;

/// \brief Radio Unit interface.
///
/// The radio unit interface provides downlink and uplink functionality through the uplink and downlink planes, allowing
/// data transmission and reception using a radio. It also notifies timing events using the \c ru_timing_notifier.
class radio_unit
{
public:
  virtual ~radio_unit() = default;

  /// Returns the downlink plane handler interface of this RU.
  virtual ru_downlink_plane_handler& get_downlink_plane_handler() = 0;

  /// Returns the uplink plane interface handler of this RU.
  virtual ru_uplink_plane_handler& get_uplink_plane_handler() = 0;
};

} // namespace srsran