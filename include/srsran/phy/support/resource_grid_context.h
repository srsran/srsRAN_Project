/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/support/resource_grid.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Describes the transmission and reception context.
struct resource_grid_context {
  /// Provides the slot context within the system frame.
  slot_point slot;
  /// Provides the sector identifier.
  unsigned sector;
};

} // namespace srsran
