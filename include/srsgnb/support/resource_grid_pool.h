#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H

#include "srsgnb/support/resource_grid.h"
#include "srsgnb/support/resource_grid_context.h"

namespace srsgnb {

/// Describes the transmission and reception context
struct resource_grid_context {
  slot_context_t slot;
  unsigned       sector;
  unsigned       antenna;
  enum { DOWNLINK, UPLINK } direction;
};

/// Describes a resource grid pool interface
class resource_grid_pool
{
public:
  /// Get a resource grid for the given context
  /// \param [in] context provides the given context
  resource_grid get_resource_grid(const srsgnb::resource_grid_context& context);
};

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
