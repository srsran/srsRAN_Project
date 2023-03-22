/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_scheduler.h"
#include "ue_scheduling/ue_scheduler_impl.h"

using namespace srsran;

cell_scheduler::cell_scheduler(const scheduler_expert_config&                  sched_cfg,
                               const sched_cell_configuration_request_message& msg,
                               ue_scheduler&                                   ue_sched_,
                               scheduler_event_logger&                         ev_logger) :
  cell_cfg(msg),
  ue_sched(ue_sched_),
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
  // Register new cell in the UE scheduler.
  ue_sched.add_cell(ue_scheduler_cell_params{msg.cell_index, &pdcch_sch, &pucch_alloc, &uci_alloc, &res_grid});
}

void cell_scheduler::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  bool has_msg3_crcs = std::any_of(
      crc_ind.crcs.begin(), crc_ind.crcs.end(), [](const auto& pdu) { return pdu.ue_index == INVALID_DU_UE_INDEX; });

  if (has_msg3_crcs) {
    ul_crc_indication msg3_crcs{}, ue_crcs{};
    msg3_crcs.sl_rx      = crc_ind.sl_rx;
    msg3_crcs.cell_index = crc_ind.cell_index;
    ue_crcs.sl_rx        = crc_ind.sl_rx;
    ue_crcs.cell_index   = crc_ind.cell_index;
    for (const ul_crc_pdu_indication& crc_pdu : crc_ind.crcs) {
      if (crc_pdu.ue_index == INVALID_DU_UE_INDEX) {
        msg3_crcs.crcs.push_back(crc_pdu);
      } else {
        ue_crcs.crcs.push_back(crc_pdu);
      }
    }
    // Forward CRC to Msg3 HARQs that has no ueId yet associated.
    ra_sch.handle_crc_indication(msg3_crcs);
    // Forward remaining CRCs to UE scheduler.
    ue_sched.get_feedback_handler().handle_crc_indication(ue_crcs);
  } else {
    ue_sched.get_feedback_handler().handle_crc_indication(crc_ind);
  }
}

void cell_scheduler::run_slot(slot_point sl_tx)
{
  // > Start with clearing old allocations from the grid.
  res_grid.slot_indication(sl_tx);
  pdcch_sch.slot_indication(sl_tx);
  pucch_alloc.slot_indication(sl_tx);

  // > SSB scheduling.
  ssb_sch.run_slot(res_grid, sl_tx);

  // > Schedule CSI-RS.
  csi_sch.run_slot(res_grid[0]);

  // > Schedule SI signalling.
  sib1_sch.schedule_sib1(res_grid[0], sl_tx);

  // > Schedule PUCCH guardbands.
  pucch_guard_sch.run_slot(res_grid);

  // > Schedule PRACH PDUs.
  prach_sch.run_slot(res_grid);

  // > Schedule RARs and Msg3.
  ra_sch.run_slot(res_grid);

  // > Schedule Paging.
  pg_sch.schedule_paging(res_grid);

  // > Schedule UE DL and UL data.
  ue_sched.run_slot(sl_tx, cell_cfg.cell_index);
}
