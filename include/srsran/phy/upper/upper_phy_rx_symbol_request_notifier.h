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

/// \file
/// \brief Interface of the upper-PHY notifier in charge of requesting symbol captures.

#pragma once

namespace srsran {

class prach_buffer;
struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;

/// \brief Interface of the upper-PHY notifier in charge of requesting symbol captures.
class upper_phy_rx_symbol_request_notifier
{
public:
  virtual ~upper_phy_rx_symbol_request_notifier() = default;

  /// \brief Notifies the event of a PRACH capture request.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  Buffer to be used by the request handler to write the PRACH window.
  virtual void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Notifies the event of an uplink slot request.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid    Grid in which to store the captured data.
  virtual void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsran
