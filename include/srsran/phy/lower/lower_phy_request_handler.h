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

class resource_grid;
class prach_buffer;
struct prach_buffer_context;
struct resource_grid_context;

/// \brief Describes the lower physical layer request handler.
///
/// Handles incoming requests and notifies their completion through the \c phy_rx_symbol_notifier interface.
class lower_phy_request_handler
{
public:
  /// Default destructor.
  virtual ~lower_phy_request_handler() = default;

  /// \brief Requests the lower PHY to capture a PRACH window.
  ///
  /// The lower PHY must capture a PHY window identified by \c context. The capture must start at slot \c context.slot
  /// and symbol \c context.start_symbol. The capture must finish once \c buffer.is_full() returns true.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  PRACH buffer used to write the PRACH window.
  virtual void request_prach_window(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Requests the lower PHY to provide an uplink slot.
  ///
  /// The lower PHY must process the slot described by \c context and notify the demodulation per symbol basis of the
  /// requested slot.
  ///
  /// The notification contains the exact context and grid.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] buffer  Resource grid to store the processed slot.
  virtual void request_uplink_slot(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsran
