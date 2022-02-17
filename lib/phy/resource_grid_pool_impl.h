
#ifndef SRSGNB_RESOURCE_GRID_POOL_IMPL_H
#define SRSGNB_RESOURCE_GRID_POOL_IMPL_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/resource_grid_pool.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <vector>

namespace srsgnb {

class resource_grid_pool_impl : public resource_grid_pool
{
private:
  unsigned nof_slots;
  unsigned nof_sectors;
  unsigned nof_antennas;

  std::vector<std::unique_ptr<resource_grid> > grids;

public:
  resource_grid_pool_impl(resource_grid_pool_config& config);
  resource_grid& get_resource_grid(const resource_grid_context& context) override;
};
} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_POOL_IMPL_H
