/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/phy/upper/phy_tap/phy_tap.h"

namespace srsran {

/// Implements a PHY tap interface for testing purposes.
class phy_tap_spy : public phy_tap
{
public:
  // See the phy_tap interface for documentation.
  void handle_ul_symbol(resource_grid_writer&                                     grid_writer,
                        const resource_grid_reader&                               grid_reader,
                        slot_point                                                slot,
                        unsigned                                                  symbol,
                        span<const uplink_pdu_slot_repository::pusch_pdu>         pusch_pdus,
                        span<const uplink_pdu_slot_repository::pucch_pdu>         pucch_pdus,
                        span<const pucch_processor::format1_common_configuration> pucch_f1_pdus,
                        span<const uplink_pdu_slot_repository::srs_pdu>           srs_pdus) override
  {
    ++handle_ul_symbol_count;
  }

  // See the phy_tap interface for documentation.
  void handle_quiet_grid(const resource_grid_reader& grid_reader, slot_point slot) override
  {
    ++handle_quiet_grid_count;
  }

  /// Gets the number of times handle_ul_symbol() is invoked.
  unsigned get_handle_ul_symbol_count() const { return handle_ul_symbol_count; }

  /// Gets the number of times handle_quiet_grid() is invoked.
  unsigned get_handle_quiet_grid_count() const { return handle_quiet_grid_count; }

private:
  /// Counts the number of times \ref handle_ul_symbol() is invoked.
  unsigned handle_ul_symbol_count = 0;
  /// Counts the number of times \ref handle_quiet_grid() is invoked.
  unsigned handle_quiet_grid_count = 0;
};

} // namespace srsran
