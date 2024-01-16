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

class prach_buffer;
struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;

namespace ofh {

/// \brief Open Fronthaul uplink request handler.
///
/// Handles PRACH and uplink data requests to capture uplink data. The uplink received data will be notified through the
/// \ref ofh_uplane_rx_symbol_notifier notifier.
class uplink_request_handler
{
public:
  /// Default destructor.
  virtual ~uplink_request_handler() = default;

  /// \brief Requests the Open Fronthaul to capture a PRACH window.
  ///
  /// The Open Fronthaul must capture the window identified by \c context.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  PRACH buffer used to write the PRACH window.
  virtual void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Requests the Open Fronthaul to provide an uplink slot.
  ///
  /// The Open Fronthaul must process the slot described by \c context.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] buffer  Resource grid to store the processed slot.
  virtual void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace ofh
} // namespace srsran
