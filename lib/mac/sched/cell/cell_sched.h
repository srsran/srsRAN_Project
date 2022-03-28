
#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "../sched_strategy/data_scheduler.h"
#include "cell_configuration.h"
#include "ra_sched.h"
#include "resource_grid.h"

namespace srsgnb {

class cell_sched
{
public:
  cell_sched(const cell_configuration_request_message& msg) : cell_cfg(msg), res_grid_pool(cell_cfg), ra_sch(cell_cfg)
  {}

  void slot_indication(slot_point sl_tx) { res_grid_pool.slot_indication(sl_tx); }

  dl_sched_result* get_dl_sched(slot_point sl_tx) { return &res_grid_pool[sl_tx].dl_grants; }
  ul_sched_result* get_ul_sched(slot_point sl_tx) { return &res_grid_pool[sl_tx].ul_grants; }

  const cell_configuration cell_cfg;
  cell_resource_grid_pool  res_grid_pool;

  ra_sched ra_sch;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
