#include "resource_grid_pool_impl.h"

using namespace srsgnb;

resource_grid_pool_impl::resource_grid_pool_impl(resource_grid_pool_config& config) :
  nof_slots(config.nof_slots), nof_sectors(config.nof_sectors)
{
  assert(config.nof_slots * config.nof_sectors == config.grids.size());

  // Reserve and move ownership of the resource grids to the pool
  grids.reserve(config.grids.size());
  for (auto& g : config.grids) {
    grids.emplace_back(std::move(g));
  }
}

resource_grid& resource_grid_pool_impl::get_resource_grid(const resource_grid_context& context)
{
  assert(context.sector < nof_sectors);

  // Select slot from the absolute system slot
  unsigned slot = context.slot.get_system_slot() % nof_slots;

  // Select resource grid and return the reference
  return *grids[slot * nof_sectors + context.sector];
}

std::unique_ptr<resource_grid_pool> srsgnb::create_resource_grid_pool(resource_grid_pool_config& config)
{
  return std::make_unique<resource_grid_pool_impl>(config);
}