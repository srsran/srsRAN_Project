/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "scheduler_impl.h"
#include "scheduler_ssb.h"

using namespace srsgnb;

scheduler_impl::scheduler_impl(sched_configuration_notifier& notifier) :
  mac_notifier(notifier), logger(srslog::fetch_basic_logger("MAC")), pending_events(ue_db, cells, notifier)
{}

bool scheduler_impl::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  pending_events.handle_cell_configuration_request(msg);
  return true;
}

void scheduler_impl::handle_add_ue_request(const sched_ue_creation_request_message& ue_request)
{
  log_ue_proc_event(logger.info, ue_event_prefix{} | ue_request.ue_index, "Sched UE Configuration", "started.");
  mac_notifier.on_ue_config_complete(ue_request.ue_index);
  log_ue_proc_event(logger.info, ue_event_prefix{} | ue_request.ue_index, "Sched UE Configuration", "completed.");
}

void scheduler_impl::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  mac_notifier.on_ue_config_complete(ue_request.ue_index);
}

void scheduler_impl::handle_rach_indication(const rach_indication_message& msg)
{
  srsran_assert(cells.has_cell(msg.cell_index), "Invalid cell_index={}", msg.cell_index);
  cells[msg.cell_index].ra_sch.handle_rach_indication(msg);
}

const sched_result* scheduler_impl::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  srsran_sanity_check(cells.cell_exists(cell_index), "Invalid cell index");
  auto& cell = cells[cell_index];

  // 1. Reset cell resource grid state.
  cell.slot_indication(sl_tx);

  // 2. Process pending events.
  pending_events.run(sl_tx, cell_index);

  //  3. SSB scheduling.
  auto& ssb_cfg = cell.cell_cfg.ssb_cfg;
  schedule_ssb(cell.res_grid[0],
               sl_tx,
               ssb_cfg.ssb_period,
               ssb_cfg.ssb_offset_to_point_A,
               cell.cell_cfg.dl_carrier.arfcn,
               ssb_cfg.ssb_bitmap,
               cell.cell_cfg.ssb_case,
               cell.cell_cfg.paired_spectrum);

  // 3. Schedule DL signalling.
  // TODO

  // 4. Schedule RARs and Msg3.
  cell.ra_sch.run_slot(cell.res_grid);

  // 5. Schedule UE DL and UL data.
  // TODO

  // Return result for the slot.
  return &cell.res_grid[0].result;
}
