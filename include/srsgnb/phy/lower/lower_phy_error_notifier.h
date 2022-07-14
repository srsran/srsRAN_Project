/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Lower physical layer's interface to notify errors.
class lower_phy_error_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_error_notifier() = default;

  /// Contains the context of the event of a late resource grid.
  struct late_resource_grid_context {
    /// Sector identifier.
    unsigned sector;
    /// Slot context.
    slot_point slot;
    /// Symbol index within the slot.
    unsigned symbol;
  };

  /// \brief Notifies the event of a missing downlink resource grid for a given context.
  ///
  /// An event of a late resource grid event happens when the lower physical layer is not provided in time with a
  /// resource grid to transmit in a given slot.
  ///
  /// \param[in] context Provides the resource grid context.
  virtual void on_late_resource_grid(const late_resource_grid_context& context) = 0;
};

} // namespace srsgnb
