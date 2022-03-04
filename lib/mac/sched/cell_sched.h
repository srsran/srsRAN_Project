
#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "resource_grid.h"

namespace srsgnb {

class cell_sched
{
public:
  dl_sched_result* get_dl_sched(slot_point sl_tx) { return &res_grid[sl_tx].dl_grants; }
  ul_sched_result* get_ul_sched(slot_point sl_tx) { return &res_grid[sl_tx].ul_grants; }

private:
  cell_resource_grid res_grid;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
