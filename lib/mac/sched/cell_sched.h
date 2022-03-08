
#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "cell/cell_configuration.h"
#include "resource_grid.h"
#include "sched_strategy/data_scheduler.h"

namespace srsgnb {

class bwp_sched
{
public:
};

class cell_sched
{
public:
  cell_sched(const cell_configuration_request_message& msg) : cell_cfg(msg) {}

  dl_sched_result* get_dl_sched(slot_point sl_tx) { return &res_grid[sl_tx].dl_grants; }
  ul_sched_result* get_ul_sched(slot_point sl_tx) { return &res_grid[sl_tx].ul_grants; }

  const cell_configuration cell_cfg;
  cell_resource_grid     res_grid;
  std::vector<bwp_sched> bwps;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
