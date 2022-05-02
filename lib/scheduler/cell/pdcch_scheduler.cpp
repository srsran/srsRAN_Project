
#include "pdcch_scheduler.h"

using namespace srsgnb;

class pdcch_scheduler::alloc_record_list
{
public:
  struct slot_record_list {
    void clear() {}
  };
  circular_array<slot_record_list, cell_resource_grid::RESOURCE_GRID_SIZE> slots;
};

pdcch_scheduler::pdcch_scheduler(cell_resource_grid& res_grid_) :
  res_grid(res_grid_), records(std::make_unique<alloc_record_list>())
{}

pdcch_scheduler::~pdcch_scheduler() {}

void pdcch_scheduler::slot_indication(slot_point sl_tx)
{
  last_sl_ind = sl_tx;

  // Clear old records.
  records->slots[(last_sl_ind - 1).to_uint()].clear();
}

void pdcch_scheduler::alloc_dl_pdcch(slot_point sl_tx)
{
  srsran_sanity_check(sl_tx - last_sl_ind < cell_resource_grid::MAXIMUM_SLOT_DIFF, "Invalid slot value");
  cell_slot_resource_grid& slot_res = res_grid[sl_tx];

  // Create PDCCH list element.
  // TODO.

  // Allocate PDCCH.
  (void)slot_res;
  // TODO.
}
