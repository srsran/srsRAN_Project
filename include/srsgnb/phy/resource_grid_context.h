#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H

#include "resource_grid.h"
#include "srsgnb/ran/slot_context.h"

namespace srsgnb {

/// Describes the transmission and reception context
struct resource_grid_context {
  slot_context_t slot;
  unsigned       sector;
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
