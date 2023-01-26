/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_scheduler_impl.h"
#include "../policy/scheduler_policy_factory.h"

using namespace srsgnb;

ue_scheduler_impl::ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_,
                                     sched_configuration_notifier&     mac_notif,
                                     scheduler_metrics_handler&        metric_handler,
                                     scheduler_event_logger&           sched_ev_logger) :
  expert_cfg(expert_cfg_),
  sched_strategy(create_scheduler_strategy(scheduler_strategy_params{"time_rr", &srslog::fetch_basic_logger("SCHED")})),
  ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED")),
  event_mng(expert_cfg, ue_db, mac_notif, metric_handler, sched_ev_logger)
{
}

void ue_scheduler_impl::add_cell(const ue_scheduler_cell_params& params)
{
  ue_res_grid_view.add_cell(*params.cell_res_alloc);
  cells[params.cell_index] = std::make_unique<cell>(expert_cfg, params, ue_db);
  event_mng.add_cell(params.cell_res_alloc->cfg, cells[params.cell_index]->srb0_sched);
  ue_alloc.add_cell(params.cell_index, *params.pdcch_sched, *params.uci_alloc, *params.cell_res_alloc);
}

void ue_scheduler_impl::run_sched_strategy(slot_point slot_tx)
{
  if (not ue_res_grid_view.get_cell_cfg_common(to_du_cell_index(0)).is_dl_enabled(slot_tx)) {
    // This slot is inactive for PDCCH in this cell. We therefore, can skip the scheduling strategy.
    // Note: we are currently assuming that all cells have the same TDD pattern and that the scheduling strategy
    // only allocates PDCCHs for the current slot_tx.
    return;
  }

  // Perform round-robin prioritization of UL and DL scheduling. This avoids that we give unfair preference to DL over
  // UL scheduling or vice-versa, when allocating PDCCHs.
  if (slot_tx.to_uint() % 2 == 0) {
    // Start with DL re-Tx and then new Tx.
    sched_strategy->dl_sched(ue_alloc, ue_res_grid_view, ue_db, true);
    sched_strategy->dl_sched(ue_alloc, ue_res_grid_view, ue_db, false);
    // UL re-Tx and then new Tx
    sched_strategy->ul_sched(ue_alloc, ue_res_grid_view, ue_db, true);
    sched_strategy->ul_sched(ue_alloc, ue_res_grid_view, ue_db, false);
  } else {
    // Start with UL re-Tx and then new Tx
    sched_strategy->ul_sched(ue_alloc, ue_res_grid_view, ue_db, true);
    sched_strategy->ul_sched(ue_alloc, ue_res_grid_view, ue_db, false);
    // Start with DL re-Tx, then SRB0 and then new Tx
    sched_strategy->dl_sched(ue_alloc, ue_res_grid_view, ue_db, true);
    sched_strategy->dl_sched(ue_alloc, ue_res_grid_view, ue_db, false);
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Run cell-specific SRB0 scheduler.
  cells[cell_index]->srb0_sched.run_slot(*cells[cell_index]->cell_res_alloc);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, ue_alloc.nof_cells(), [this, slot_tx]() { run_sched_strategy(slot_tx); });

  // Schedule UCI as the last step.
  cells[cell_index]->uci_sched.run_slot(*cells[cell_index]->cell_res_alloc, slot_tx);
}
