/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/phy_tap/phy_tap.h"
#include "srsran/phy/upper/uplink_slot_processor.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Alternative uplink slot processor implementation.
///
/// Uplink processor that only processes PRACH and reports resource grids that are not associated with any reception
/// request.
///
/// This implementation is used when there are no uplink receive requests.
class uplink_slot_processor_alt_impl : public uplink_slot_processor
{
public:
  /// \brief Creates a dummy instance from a base processor.
  /// \param[in] base_ Base instance used for handling PRACH.
  /// \param[in] grid_ Reference to the uplink processor resource grid.
  /// \param[in] tap_  Optional pointer to the PHY tap.
  uplink_slot_processor_alt_impl(uplink_slot_processor& base_, const resource_grid_reader& grid_, phy_tap* tap_) :
    base(base_), grid(grid_), tap(tap_)
  {
  }

  /// Activates the alternative processor for the current slot context.
  void activate_slot(slot_point slot_) { slot.emplace(slot_); }

  /// Deactivates the slot.
  void deactivate_slot() { slot.reset(); }

  // See the uplink_slot_processor interface for documentation.
  void handle_rx_symbol(unsigned end_symbol_index, bool is_valid) override
  {
    if ((tap != nullptr) && (end_symbol_index == grid.get_nof_symbols() - 1) && (slot.has_value())) {
      tap->handle_quiet_grid(grid, *slot);
    }
  }

  // See the uplink_slot_processor interface for documentation.
  void process_prach(const prach_buffer& buffer, const prach_buffer_context& context) override
  {
    base.process_prach(buffer, context);
  }

  // See the uplink_slot_processor interface for documentation.
  void discard_slot() override
  {
    // Ignore.
  }

private:
  /// Base processor, used for processing PRACH.
  uplink_slot_processor& base;
  /// Reference to the resource grid associated with the uplink processor.
  const resource_grid_reader& grid;
  /// Optional physical layer tap. Set to \c nullptr if not available.
  phy_tap* tap;
  /// Activated slot context.
  std::optional<slot_point> slot;
};

} // namespace srsran
