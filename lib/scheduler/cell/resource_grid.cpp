
#include "resource_grid.h"

using namespace srsgnb;

cell_resource_grid::cell_resource_grid(const cell_configuration& cfg_) :
  cfg(cfg_), dl_prbs(cfg.nof_dl_prbs), ul_prbs(cfg.nof_ul_prbs)
{}

void cell_resource_grid::slot_indication(slot_point sl)
{
  reset();
  slot = sl;
}

void cell_resource_grid::reset()
{
  dl_prbs.reset();
  ul_prbs.reset();
  dl_grants = {};
  ul_grants = {};
}

/////////////////////////////////////////////////////////////////////////////

cell_resource_grid_pool::cell_resource_grid_pool(const cell_configuration& cfg_) : cfg(cfg_)
{
  for (unsigned i = 0; i < RESOURCE_GRID_SIZE; ++i) {
    slots_.emplace_back(cfg);
  }
}

void cell_resource_grid_pool::slot_indication(slot_point sl_tx)
{
  srsran_sanity_check(not last_slot_ind.valid() or last_slot_ind + 1 == sl_tx, "Slot indication was skipped");

  last_slot_ind = sl_tx;

  // Clear old slots
  slots_[(last_slot_ind + MINIMUM_SLOT_DIFF).to_uint() % RESOURCE_GRID_SIZE].reset();
}
