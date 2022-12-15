/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_guardbands_sched.h"
#include "../cell/resource_grid.h"

using namespace srsgnb;

pucch_guardbands_sched::pucch_guardbands_sched(const cell_configuration& cell_cfg_) : cell_cfg{cell_cfg_} {};

void pucch_guardbands_sched::allocate_pucch_guardbands(cell_slot_resource_allocator& slot_alloc)
{
  for (const auto& pucch_res : cell_cfg.pucch_guardbands) {
    crb_interval crbs = prb_to_crb(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, pucch_res.prbs);
    grant_info   grant{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs, pucch_res.symbols, crbs};
    slot_alloc.dl_res_grid.fill(grant);
  }
}

void pucch_guardbands_sched::run_slot(cell_resource_allocator& res_alloc)
{
  if (first_run_slot) {
    for (unsigned sl = 0; sl < res_alloc.RING_ALLOCATOR_SIZE; ++sl) {
      allocate_pucch_guardbands(res_alloc[sl]);
    }
    first_run_slot = false;
  } else {
    allocate_pucch_guardbands(res_alloc[res_alloc.RING_ALLOCATOR_SIZE - 1]);
  }
}
