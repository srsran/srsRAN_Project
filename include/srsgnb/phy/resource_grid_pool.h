#ifndef SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
#define SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H

#include "resource_grid.h"
#include "resource_grid_context.h"
#include <memory>

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
  unsigned nof_symbols = 14;      /// Number of symbols per resource grid
  unsigned nof_subc    = 52 * 12; /// Number of subcarriers per resource grid

  unsigned nof_sectors  = 1;  /// Number of sectors
  unsigned nof_antennas = 1;  /// Number of antennas per sectors
  unsigned nof_slots    = 40; /// Number of slots to buffer
};

/// Creates a default resource grid pool
///
/// \param config Provides the configuration of the resource grid pool
/// \return A unique pointer with the default resource grid pool
std::unique_ptr<resource_grid_pool> create_resource_grid_pool(const resource_grid_pool_config& config);

} // namespace srsgnb

#endif // SRSGNB_SUPPORT_RESOURCE_GRID_POOL_H
