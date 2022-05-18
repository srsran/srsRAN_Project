/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CELL_SCHED_H
#define SRSGNB_CELL_SCHED_H

#include "../sched_strategy/data_scheduler.h"
#include "cell_configuration.h"
#include "ra_scheduler.h"
#include "resource_grid.h"

namespace srsgnb {

class cell_sched
{
public:
  cell_sched(const sched_cell_configuration_request_message& msg) : cell_cfg(msg), res_grid(cell_cfg), ra_sch(cell_cfg)
  {}

  void slot_indication(slot_point sl_tx) { res_grid.slot_indication(sl_tx); }

  const cell_configuration cell_cfg;
  cell_resource_allocator  res_grid;

  ra_scheduler ra_sch;
};

class cell_sched_manager
{
public:
  bool cell_exists(du_cell_index_t cell_index) const
  {
    return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
  }

  void add_cell(du_cell_index_t cell_index, const sched_cell_configuration_request_message& msg)
  {
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Cell index={} is not valid", cell_index);
    srsran_assert(not cell_exists(cell_index), "Cell={} already exists", cell_index);
    cells[cell_index] = std::make_unique<cell_sched>(msg);
    nof_cells_++;
  }

  cell_sched& operator[](du_cell_index_t cell_index)
  {
    srsran_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  const cell_sched& operator[](du_cell_index_t cell_index) const
  {
    srsran_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  size_t nof_cells() const { return nof_cells_; }

private:
  size_t                                                    nof_cells_ = 0;
  std::array<std::unique_ptr<cell_sched>, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsgnb

#endif // SRSGNB_CELL_SCHED_H
