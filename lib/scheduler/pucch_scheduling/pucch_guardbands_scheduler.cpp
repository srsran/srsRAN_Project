/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pucch_guardbands_scheduler.h"
#include "../cell/resource_grid.h"

using namespace srsran;

pucch_guardbands_scheduler::pucch_guardbands_scheduler(const cell_configuration& cell_cfg_) :
  cell_cfg{cell_cfg_}, logger(srslog::fetch_basic_logger("SCHED")){};

void pucch_guardbands_scheduler::allocate_pucch_guardbands(cell_slot_resource_allocator& slot_alloc)
{
  // Reserve all the resources on the grid for this slot.
  for (const auto& pucch_res : cell_cfg.pucch_guardbands) {
    crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, pucch_res.prbs);
    grant_info   grant{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, pucch_res.symbols, crbs};
    slot_alloc.ul_res_grid.fill(grant);
  }
}

void pucch_guardbands_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  // If called for the first time, pre-reserves the PUCCH resource over the entire grid, until the last (farthest in the
  // future) usable slot.
  if (first_run_slot) {
    for (unsigned sl = 0; sl < res_alloc.max_ul_slot_alloc_delay + 1; ++sl) {
      // Do not schedule PUCCH guardbands on DL slots.
      slot_point grid_sl_point{res_alloc[sl].slot};
      if (not cell_cfg.is_ul_enabled(grid_sl_point)) {
        continue;
      }
      allocate_pucch_guardbands(res_alloc[sl]);
    }
    first_run_slot = false;
  }
  // After the first time, pre-reserves the PUCCH resource on the grid only for the last slot.
  else {
    // Do not schedule PUCCH guardbands on DL slots.
    slot_point grid_sl_point{res_alloc[res_alloc.max_ul_slot_alloc_delay].slot};
    if (not cell_cfg.is_ul_enabled(grid_sl_point)) {
      return;
    }
    allocate_pucch_guardbands(res_alloc[res_alloc.max_ul_slot_alloc_delay]);
  }
}
