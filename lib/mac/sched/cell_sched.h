
#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "resource_grid.h"
#include "sched_strategy/bwp_data_sched.h"

namespace srsgnb {

class bwp_sched
{
public:
  std::unique_ptr<bwp_data_sched> data_sched;
};

class cell_sched
{
public:
  cell_resource_grid     res_grid;
  std::vector<bwp_sched> bwps;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
