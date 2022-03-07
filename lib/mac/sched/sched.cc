
#include "sched.h"

using namespace srsgnb;

const dl_sched_result* sched::get_dl_sched(slot_point sl_tx, du_cell_index_t cc)
{
  // TODO
  return &cells[cc].res_grid[sl_tx].dl_grants;
}

const ul_sched_result* sched::get_ul_sched(slot_point sl_tx, du_cell_index_t cc)
{
  // TODO
  return &cells[cc].res_grid[sl_tx].ul_grants;
}
