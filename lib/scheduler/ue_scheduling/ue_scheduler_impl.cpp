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

ue_scheduler_impl::ue_scheduler_impl(sched_configuration_notifier& mac_notif) :
  srb0_sched(srslog::fetch_basic_logger("MAC")),
  sched_strategy(create_scheduler_strategy(scheduler_strategy_params{"time_rr", &srslog::fetch_basic_logger("MAC")})),
  ue_alloc(ue_db, srslog::fetch_basic_logger("MAC")),
  event_mng(ue_db, mac_notif, srb0_sched)
{
}

void ue_scheduler_impl::add_cell(const ue_scheduler_cell_params& params)
{
  event_mng.add_cell_config(params.cell_res_alloc->cfg);
  ue_alloc.add_cell(params.cell_index, *params.pdcch_sched, *params.pucch_sched, *params.cell_res_alloc);
}

void ue_scheduler_impl::run_sched_strategy(slot_point slot_tx)
{
  // Perform round-robin prioritization of UL and DL scheduling. This avoids that we give unfair preference to DL over
  // UL scheduling or vice-versa, when allocating PDCCHs.
  if (slot_tx.to_uint() % 2 == 0) {
    // Start with DL re-Tx, then SRB0 and then new Tx
    sched_strategy->dl_sched(ue_alloc, ue_db, true);
    // Schedule SRB0 first before scheduling other bearers
    srb0_sched.run_slot(ue_alloc, ue_db);
    sched_strategy->dl_sched(ue_alloc, ue_db, false);
    // UL re-Tx and then new Tx
    sched_strategy->ul_sched(ue_alloc, ue_db, true);
    sched_strategy->ul_sched(ue_alloc, ue_db, false);
  } else {
    // Start with UL re-Tx and then new Tx
    sched_strategy->ul_sched(ue_alloc, ue_db, true);
    sched_strategy->ul_sched(ue_alloc, ue_db, false);
    // Start with DL re-Tx, then SRB0 and then new Tx
    sched_strategy->dl_sched(ue_alloc, ue_db, true);
    // Schedule SRB0 first before scheduling other bearers
    srb0_sched.run_slot(ue_alloc, ue_db);
    sched_strategy->dl_sched(ue_alloc, ue_db, false);
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, ue_alloc.nof_cells(), [this, slot_tx]() { run_sched_strategy(slot_tx); });
}
