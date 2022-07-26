/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../common_scheduling/ra_scheduler.h"
#include "../common_scheduling/sib_scheduler.h"
#include "../pdcch_scheduling/pdcch_scheduler_impl.h"
#include "cell_configuration.h"
#include "resource_grid.h"

namespace srsgnb {

/// \brief This class holds all the resources that are specific to a cell.
/// This includes the SIB and RA scheduler objects, PDCCH scheduler object, the cell resource grid, etc.
class scheduler_cell
{
public:
  explicit scheduler_cell(const sched_cell_configuration_request_message& msg) :
    cell_cfg(msg),
    res_grid(cell_cfg),
    pdcch_sch(cell_cfg),
    ra_sch(cell_cfg, pdcch_sch),
    sib1_sch(cell_cfg, pdcch_sch, msg)
  {
  }

  void slot_indication(slot_point sl_tx)
  {
    res_grid.slot_indication(sl_tx);
    pdcch_sch.slot_indication(sl_tx);
  }

  const cell_configuration cell_cfg;
  cell_resource_allocator  res_grid;

  pdcch_scheduler_impl pdcch_sch;
  ra_scheduler         ra_sch;
  sib1_scheduler       sib1_sch;
};

class scheduler_cell_manager
{
public:
  /// Verifies if cell with provided index exists in the scheduler.
  bool cell_exists(du_cell_index_t cell_index) const
  {
    return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
  }

  /// \brief Creates new cell in the scheduler.
  ///
  /// \param cell_index index of cell to created.
  /// \param msg configuration parameters of the new cell.
  void add_cell(du_cell_index_t cell_index, const sched_cell_configuration_request_message& msg)
  {
    srsgnb_assert(cell_index < MAX_NOF_DU_CELLS, "Cell index={} is not valid", cell_index);
    srsgnb_assert(not cell_exists(cell_index), "Cell={} already exists", cell_index);
    cells[cell_index] = std::make_unique<scheduler_cell>(msg);
    nof_cells_++;
  }

  scheduler_cell& operator[](du_cell_index_t cell_index)
  {
    srsgnb_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  const scheduler_cell& operator[](du_cell_index_t cell_index) const
  {
    srsgnb_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  size_t nof_cells() const { return nof_cells_; }

private:
  size_t                                                        nof_cells_ = 0;
  std::array<std::unique_ptr<scheduler_cell>, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsgnb
