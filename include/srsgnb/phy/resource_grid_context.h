#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H

#include "resource_grid.h"
#include "srsgnb/ran/slot_context.h"

namespace srsgnb {

/// Describes the transmission and reception context.
struct resource_grid_context {
  /// Provides the slot context within the system frame.
  slot_context_t slot;
  /// Provides the sector identifier.
  unsigned sector;
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
