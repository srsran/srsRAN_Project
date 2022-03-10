
#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "cell/cell_configuration.h"
#include "cell/resource_grid.h"
#include "sched_strategy/data_scheduler.h"

namespace srsgnb {

class bwp_sched
{
public:
};

class cell_sched
{
public:
  cell_sched(const cell_configuration_request_message& msg) : cell_cfg(msg), res_grid_pool(cell_cfg) {}

  void slot_indication(slot_point sl_tx) {
    res_grid_pool.slot_indication(sl_tx);
  }

  dl_sched_result* get_dl_sched(slot_point sl_tx) { return &res_grid_pool[0].dl_grants; }
  ul_sched_result* get_ul_sched(slot_point sl_tx) { return &res_grid_pool[0].ul_grants; }

  const cell_configuration cell_cfg;
  cell_resource_grid_pool  res_grid_pool;
  std::vector<bwp_sched>   bwps;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
