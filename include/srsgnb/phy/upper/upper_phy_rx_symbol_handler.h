/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interface of the upper-PHY handler in charge of uplink OFDM symbols.

#pragma once

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {
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

  /// \brief Handles the arrival of PRACH sequences at a given symbol.
  /// \param[in] context PRACH context: specifies sector, slot and window.
  /// \param[in] buffer  Read-only buffer containing the PRACH sequence.
  virtual void handle_rx_prach_symbol(const prach_buffer_context& context, const prach_buffer* buffer) = 0;

  /// \brief Handles the arrival of SRS packets at a given symbol.
  ///
  /// \param[in] context Notification context: specifies sector, slot and symbol.
  virtual void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) = 0;
};
} // namespace srsgnb
