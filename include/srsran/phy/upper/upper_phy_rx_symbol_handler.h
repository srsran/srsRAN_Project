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
/// \brief Interface of the upper-PHY handler in charge of uplink OFDM symbols.

#pragma once

#include "srsran/ran/slot_point.h"

namespace srsran {

class prach_buffer;
struct prach_buffer_context;
class resource_grid_reader;

/// Describes the context of a newly received symbol.
struct upper_phy_rx_symbol_context {
  /// Identifier of the sector the symbol is received from.
  unsigned sector;
  /// Describes the current slot.
  slot_point slot;
  /// Symbol index within the slot.
  unsigned symbol;
};

/// \brief Interface of the upper-PHY handler in charge of processing uplink OFDM symbols.
class upper_phy_rx_symbol_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_rx_symbol_handler() = default;

  /// \brief Handles the reception of an OFDM symbol.
  /// \param[in] context Notification context: specifies sector, slot and symbol.
  /// \param[in] grid    Resource grid for the current slot (encompasses all receive antenna ports).
  virtual void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) = 0;

  /// \brief Handles the arrival of PRACH sequences.
  /// \param[in] context PRACH context: specifies sector, slot and window.
  /// \param[in] buffer  Read-only buffer containing the PRACH sequence.
  virtual void handle_rx_prach_window(const prach_buffer_context& context, const prach_buffer& buffer) = 0;

  /// \brief Handles the arrival of SRS packets at a given symbol.
  /// \param[in] context Notification context: specifies sector, slot and symbol.
  virtual void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) = 0;
};

} // namespace srsran
