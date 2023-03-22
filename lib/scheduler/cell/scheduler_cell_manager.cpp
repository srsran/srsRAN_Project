/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_cell_manager.h"

using namespace srsran;

scheduler_cell::scheduler_cell(const scheduler_expert_config&                  sched_cfg,
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

scheduler_cell_manager::scheduler_cell_manager(const scheduler_expert_config& sched_cfg_,
                                               scheduler_event_logger&        ev_logger_) :
  sched_cfg(sched_cfg_), ev_logger(ev_logger_)
{
}

void scheduler_cell_manager::add_cell(du_cell_index_t cell_index, const sched_cell_configuration_request_message& msg)
{
  srsran_assert(cell_index < MAX_NOF_DU_CELLS, "Cell index={} is not valid", cell_index);
  srsran_assert(not cell_exists(cell_index), "Cell={} already exists", cell_index);
  cells[cell_index] = std::make_unique<scheduler_cell>(sched_cfg, msg, ev_logger);
  nof_cells_++;
}
