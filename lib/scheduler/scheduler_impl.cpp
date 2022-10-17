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
  feedback_handler(ue_sched->get_feedback_handler()),
  dl_bs_handler(ue_sched->get_dl_buffer_state_indication_handler())
{
}

bool scheduler_impl::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  cells.add_cell(msg.cell_index, msg);

  ue_sched->add_cell(ue_scheduler_cell_params{msg.cell_index,
                                              &cells[msg.cell_index].pdcch_sch,
                                              &cells[msg.cell_index].pucch_sch,
                                              &cells[msg.cell_index].res_grid,
                                              msg.max_msg4_mcs_index});

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

void scheduler_impl::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  bool has_msg3_crcs = std::any_of(
      crc_ind.crcs.begin(), crc_ind.crcs.end(), [](const auto& pdu) { return pdu.ue_index == INVALID_DU_UE_INDEX; });

  if (has_msg3_crcs) {
    ul_crc_indication msg3_crcs{}, ue_crcs{};
    for (const ul_crc_pdu_indication& crc_pdu : crc_ind.crcs) {
      if (crc_pdu.ue_index == INVALID_DU_UE_INDEX) {
        msg3_crcs.crcs.push_back(crc_pdu);
      } else {
        ue_crcs.crcs.push_back(crc_pdu);
      }
    }
    // Forward CRC to Msg3 HARQs that has no ueId yet associated.
    cells[crc_ind.cell_index].ra_sch.handle_crc_indication(msg3_crcs);
    // Forward remaining CRCs to UE scheduler.
    feedback_handler.handle_crc_indication(ue_crcs);
  } else {
    feedback_handler.handle_crc_indication(crc_ind);
  }
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
