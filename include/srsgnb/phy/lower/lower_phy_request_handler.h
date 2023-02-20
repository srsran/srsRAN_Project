/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/support/resource_grid_context.h"

namespace srsgnb {

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

} // namespace srsgnb
