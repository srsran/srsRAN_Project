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
#include "common_scheduling/ssb_scheduler.h"
#include "ue_scheduling/ue_scheduler_impl.h"

using namespace srsgnb;

scheduler_impl::scheduler_impl(sched_configuration_notifier& notifier) :
  mac_notifier(notifier),
  logger(srslog::fetch_basic_logger("MAC")),
  ue_sched(std::make_unique<ue_scheduler_impl>(notifier)),
  ue_cfg_handler(ue_sched->get_ue_configurator()),
  feedback_handler(ue_sched->get_feedback_handler())
{
}

bool scheduler_impl::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  cells.add_cell(msg.cell_index, msg);

  ue_sched->add_cell(
      ue_scheduler_cell_params{msg.cell_index, &cells[msg.cell_index].pdcch_sch, &cells[msg.cell_index].res_grid});

  logger.info("SCHED: Cell with cell_index={} was configured.", msg.cell_index);

  return true;
}

void scheduler_impl::handle_add_ue_request(const sched_ue_creation_request_message& ue_request)
{
  ue_cfg_handler.handle_add_ue_request(ue_request);
}

void scheduler_impl::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  ue_cfg_handler.handle_ue_reconfiguration_request(ue_request);
}

void scheduler_impl::handle_rach_indication(const rach_indication_message& msg)
{
  cells[msg.cell_index].ra_sch.handle_rach_indication(msg);
}

void scheduler_impl::handle_crc_indication(const ul_crc_indication& crc)
{
  // TODO: Optimize.
  ul_crc_indication ue_crcs{};
  ue_crcs.cell_index = crc.cell_index;
  for (const ul_crc_pdu_indication& crc_pdu : crc.crcs) {
    if (crc_pdu.ue_index == INVALID_DU_UE_INDEX) {
      // No UE found. The CRC is directed at a Msg3 HARQ.
      cells[crc.cell_index].ra_sch.handle_crc_indication(crc_pdu);
    } else {
      ue_crcs.crcs.push_back(crc_pdu);
    }
  }
  feedback_handler.handle_crc_indication(ue_crcs);
}

const sched_result* scheduler_impl::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  auto& cell = cells[cell_index];

  // > Reset cell resource grid state.
  cell.slot_indication(sl_tx);

  // > SSB scheduling.
  schedule_ssb(cell.res_grid[0], sl_tx, cell.cell_cfg);

  // > Schedule DL signalling.
  cell.sib1_sch.schedule_sib1(cell.res_grid[0], sl_tx);

  // > Schedule PRACH PDUs.
  cell.prach_sch.run_slot(cell.res_grid[0]);

  // > Schedule RARs and Msg3.
  cell.ra_sch.run_slot(cell.res_grid);

  // > Schedule UE DL and UL data.
  ue_sched->run_slot(sl_tx, cell_index);

  // Return result for the slot.
  return &cell.res_grid[0].result;
}
