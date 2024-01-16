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

class slot_point;

/// Describes the context of the current timing boundary.
struct upper_phy_timing_context {
  /// Indicates the current slot.
  slot_point slot;
};

/// Upper physical layer handler to align timing boundaries.
class upper_phy_timing_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_timing_handler() = default;

  /// \brief Handles a new TTI boundary event.
  ///
  /// \param[in] context Notification context.
  virtual void handle_tti_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a half slot (7th OFDM symbol) packet arrival.
  ///
  /// \param[in] context Notification context.
  virtual void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a full slot (14th OFDM symbol) packet arrival.
  ///
  /// \param[in] context Notification context.
  virtual void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) = 0;
};

} // namespace srsran
