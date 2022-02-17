
#ifndef SRSGNB_RESOURCE_GRID_POOL_IMPL_H
#define SRSGNB_RESOURCE_GRID_POOL_IMPL_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/resource_grid_pool.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <vector>

namespace srsgnb {

class resource_grid_impl : public resource_grid
{
private:
  unsigned                  nof_symb;
  unsigned                  nof_subc;
  srsvec::aligned_vec<cf_t> buffer;

public:
  resource_grid_impl() = default;
  void alloc(unsigned nof_symb, unsigned nof_subc);
  void put(unsigned int l, unsigned int k, cf_t value) override;
  cf_t get(unsigned int l, unsigned int k) const override;
};

class resource_grid_pool_impl : public resource_grid_pool
{
private:
  unsigned nof_slots;
  unsigned nof_sectors;
  unsigned nof_antennas;

  std::vector<resource_grid_impl> grids;

public:
  resource_grid_pool_impl(const resource_grid_pool_config& config);
  resource_grid& get_resource_grid(const resource_grid_context& context) override;
};
} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_POOL_IMPL_H
