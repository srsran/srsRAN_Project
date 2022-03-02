#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H

#include "resource_grid.h"
#include "resource_grid_context.h"
#include <memory>
#include <vector>

namespace srsgnb {

/// Describes a resource grid pool interface
class resource_grid_pool
{
public:
  /// Default destructor
  virtual ~resource_grid_pool() = default;

  /// Get a resource grid for the given context
  /// \param [in] context provides the given context
  virtual resource_grid& get_resource_grid(const srsgnb::resource_grid_context& context) = 0;
};

/// Describes a resource grid pool configuration
struct resource_grid_pool_config {
  /// Number of sectors
  unsigned nof_sectors = 1;
  /// Number of slots to buffer per sector
  unsigned nof_slots = 40;
  /// Provides the resource grid implementations, ownership is moved to the pool
  std::vector<std::unique_ptr<resource_grid> > grids;
};

/// Creates a default resource grid pool
///
/// \param config Provides the configuration of the resource grid pool
/// \return A unique pointer with the default resource grid pool
std::unique_ptr<resource_grid_pool> create_resource_grid_pool(resource_grid_pool_config& config);

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
