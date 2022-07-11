/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_RESOURCE_GRID_CONTEXT_H
#define SRSGNB_PHY_RESOURCE_GRID_CONTEXT_H

#include "resource_grid.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the transmission and reception context.
struct resource_grid_context {
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_RESOURCE_GRID_CONTEXT_H
