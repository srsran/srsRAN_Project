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
  event_mng(ue_db, mac_notif),
  ue_alloc(ue_db, srslog::fetch_basic_logger("MAC")),
  sched_strategy(create_scheduler_strategy(scheduler_strategy_params{"time_rr", &srslog::fetch_basic_logger("MAC")}))
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
    // Start with DL
    sched_strategy->dl_sched(ue_alloc, ue_db);
    sched_strategy->ul_sched(ue_alloc, ue_db);
  } else {
    // Start with UL
    sched_strategy->ul_sched(ue_alloc, ue_db);
    sched_strategy->dl_sched(ue_alloc, ue_db);
  }
}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, ue_alloc.nof_cells(), [this, slot_tx]() { run_sched_strategy(slot_tx); });
}
