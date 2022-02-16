#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H

#include "srsgnb/adt/slot_context.h"
#include "srsgnb/support/resource_grid.h"

namespace srsgnb {

/// Describes the transmission and reception context
struct resource_grid_context {
  slot_context_t slot;
  unsigned       sector;
  unsigned       antenna;
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_CONTEXT_H
