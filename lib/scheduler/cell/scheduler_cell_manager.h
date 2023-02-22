/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../common_scheduling/csi_rs_scheduler.h"
#include "../common_scheduling/paging_scheduler.h"
#include "../common_scheduling/prach_scheduler.h"
#include "../common_scheduling/ra_scheduler.h"
#include "../common_scheduling/sib_scheduler.h"
#include "../common_scheduling/ssb_scheduler.h"
#include "../pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "../pucch_scheduling/pucch_allocator_impl.h"
#include "../pucch_scheduling/pucch_guardbands_scheduler.h"
#include "../uci_scheduling/uci_allocator_impl.h"
#include "cell_configuration.h"
#include "resource_grid.h"

namespace srsran {

/// \brief This class holds all the resources that are specific to a cell.
/// This includes the SIB and RA scheduler objects, PDCCH scheduler object, the cell resource grid, etc.
class scheduler_cell
{
public:
  explicit scheduler_cell(const scheduler_expert_config&                  sched_cfg,
                          const sched_cell_configuration_request_message& msg,
                          scheduler_event_logger&                         ev_logger) :
    cell_cfg(msg),
    res_grid(cell_cfg),
    ssb_sch(cell_cfg),
    pdcch_sch(cell_cfg),
    csi_sch(cell_cfg),
    ra_sch(sched_cfg.ra, cell_cfg, pdcch_sch, ev_logger),
    prach_sch(cell_cfg),
    pucch_alloc(cell_cfg),
    uci_alloc(pucch_alloc),
    sib1_sch(sched_cfg.si, cell_cfg, pdcch_sch, msg),
    pucch_guard_sch(cell_cfg),
    pg_sch(sched_cfg, cell_cfg, pdcch_sch, msg)
  {
  }

  void slot_indication(slot_point sl_tx)
  {
    res_grid.slot_indication(sl_tx);
    pdcch_sch.slot_indication(sl_tx);
    pucch_alloc.slot_indication(sl_tx);
  }

  const cell_configuration cell_cfg;
  cell_resource_allocator  res_grid;

  ssb_scheduler                 ssb_sch;
  pdcch_resource_allocator_impl pdcch_sch;
  csi_rs_scheduler              csi_sch;
  ra_scheduler                  ra_sch;
  prach_scheduler               prach_sch;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  sib1_scheduler                sib1_sch;
  pucch_guardbands_scheduler    pucch_guard_sch;
  paging_scheduler              pg_sch;
};

class scheduler_cell_manager
{
public:
  explicit scheduler_cell_manager(const scheduler_expert_config& sched_cfg_, scheduler_event_logger& ev_logger_) :
    sched_cfg(sched_cfg_), ev_logger(ev_logger_)
  {
  }

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
    srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Cell index={} is not valid", cell_index);
    srsran_assert(not cell_exists(cell_index), "Cell={} already exists", cell_index);
    cells[cell_index] = std::make_unique<scheduler_cell>(sched_cfg, msg, ev_logger);
    nof_cells_++;
  }

  scheduler_cell& operator[](du_cell_index_t cell_index)
  {
    srsran_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  const scheduler_cell& operator[](du_cell_index_t cell_index) const
  {
    srsran_assert(cell_exists(cell_index), "Cell={} does not exist", cell_index);
    return *cells[cell_index];
  }

  size_t nof_cells() const { return nof_cells_; }

private:
  const scheduler_expert_config& sched_cfg;
  scheduler_event_logger&        ev_logger;

  size_t                                                        nof_cells_ = 0;
  std::array<std::unique_ptr<scheduler_cell>, MAX_NOF_DU_CELLS> cells;
};

} // namespace srsran
