/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_RX_SYMBOL_HANDLER_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_RX_SYMBOL_HANDLER_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {
/// Describes context of the new received symbol
struct upper_phy_rx_symbol_context {
  slot_point slot;
  unsigned   sector;
  unsigned   symbol;
};

/// Upper physical layer's handler to process receive symbol
class upper_phy_rx_symbol_handler
{
public:
  /// Default destructor
  virtual ~upper_phy_rx_symbol_handler() = default;

  /// \brief Handles the reception of OFDM symbol for given sector.
  /// \param [in] context Provides the notification context.
  /// \param [in] grid Provides the resource grids for each port of the given sector.
  virtual void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) = 0;

  /// Handles the arrival of PRACH packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_rx_prach_symbol(const upper_phy_rx_symbol_context& context) = 0;

  /// Handles the arrival of SRS packets for given CC
  ///
  /// \param [in] context Provides the notification context
  virtual void handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context) = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_UPPER_PHY_RX_SYMBOL_HANDLER_H
