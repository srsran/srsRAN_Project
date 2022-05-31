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
#include "../cell/resource_grid.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ue_scheduler_impl::ue_scheduler_impl(sched_configuration_notifier& mac_notif) : event_mng(ue_db, mac_notif) {}

void ue_scheduler_impl::add_cell(const ue_scheduler_cell_params& params)
{
  cells.emplace(params.cell_index, params);
  event_mng.add_cell_config(params.cell_res_alloc->cfg);
}

void ue_scheduler_impl::run_sched_strategy() {}

void ue_scheduler_impl::run_slot(slot_point slot_tx, du_cell_index_t cell_index)
{
  // Process any pending events that are directed at UEs.
  event_mng.run(slot_tx, cell_index);

  // Synchronize all carriers. Last thread to reach this synchronization point, runs UE scheduling strategy.
  sync_point.wait(slot_tx, cells.size(), [this]() { run_sched_strategy(); });
}
