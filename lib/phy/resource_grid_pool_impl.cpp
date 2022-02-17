#include "resource_grid_pool_impl.h"

using namespace srsgnb;

void resource_grid_impl::alloc(unsigned nof_symb_, unsigned nof_subc_)
{
  nof_symb = nof_symb_;
  nof_subc = nof_subc_;
  buffer.resize(nof_symb * nof_subc);
}

void resource_grid_impl::put(unsigned l, unsigned k, cf_t value)
{
  assert(l < nof_symb);
  assert(k < nof_subc);
  buffer[l * nof_subc + k] = value;
}

cf_t resource_grid_impl::get(unsigned l, unsigned k) const
{
  assert(l < nof_symb);
  assert(k < nof_subc);
  return buffer[l * nof_subc + k];
}

resource_grid_pool_impl::resource_grid_pool_impl(const resource_grid_pool_config& config) :
  nof_slots(config.nof_slots), nof_sectors(config.nof_sectors), nof_antennas(config.nof_antennas)
{
  grids.resize(nof_slots * nof_sectors * nof_antennas);
  for (unsigned i = 0; i != nof_slots * nof_sectors * nof_antennas; ++i) {
    grids[i].alloc(config.nof_symbols, config.nof_subc);
  }
}

resource_grid& resource_grid_pool_impl::get_resource_grid(const resource_grid_context& context)
{
  assert(context.antenna < nof_antennas);
  assert(context.sector < nof_sectors);

  // Select slot from the absolute system slot
  unsigned slot = context.slot.get_system_slot() % nof_slots;

  return grids[(slot * nof_sectors + context.sector) * nof_antennas + context.antenna];
}

std::unique_ptr<resource_grid_pool> srsgnb::create_resource_grid_pool(const resource_grid_pool_config& config)
{
  return std::make_unique<resource_grid_pool_impl>(config);
}