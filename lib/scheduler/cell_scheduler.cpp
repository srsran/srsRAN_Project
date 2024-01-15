/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "cell_scheduler.h"
#include "logging/scheduler_metrics_handler.h"
#include "ue_scheduling/ue_scheduler_impl.h"

using namespace srsran;

cell_scheduler::cell_scheduler(const scheduler_expert_config&                  sched_cfg,
                               const sched_cell_configuration_request_message& msg,
                               const cell_configuration&                       cell_cfg_,
                               ue_scheduler&                                   ue_sched_,
                               scheduler_event_logger&                         ev_logger,
                               scheduler_metrics_handler&                      metrics_handler) :
  cell_cfg(cell_cfg_),
  ue_sched(ue_sched_),
  res_grid(cell_cfg),
  event_logger(ev_logger),
  metrics(metrics_handler),
  result_logger(sched_cfg.log_broadcast_messages, cell_cfg.cell_index),
  logger(srslog::fetch_basic_logger("SCHED")),
  ssb_sch(cell_cfg),
  pdcch_sch(cell_cfg),
  csi_sch(cell_cfg),
  ra_sch(sched_cfg.ra, cell_cfg, pdcch_sch, ev_logger),
  prach_sch(cell_cfg),
  pucch_alloc(cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot),
  uci_alloc(pucch_alloc),
  sib1_sch(sched_cfg.si, cell_cfg, pdcch_sch, msg),
  si_msg_sch(sched_cfg.si, cell_cfg, pdcch_sch, msg),
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
  // If there are skipped slots, handle them. Otherwise, the cell grid and cached results are not correctly cleared.
  if (SRSRAN_LIKELY(res_grid.slot_tx().valid())) {
    while (SRSRAN_UNLIKELY(res_grid.slot_tx() + 1 != sl_tx)) {
      slot_point skipped_slot = res_grid.slot_tx() + 1;
      logger.info("Status: Detected skipped slot={}.", skipped_slot);
      res_grid.slot_indication(skipped_slot);
      pdcch_sch.slot_indication(skipped_slot);
      pucch_alloc.slot_indication(skipped_slot);
      uci_alloc.slot_indication(skipped_slot);
    }
  }

  // > Start with clearing old allocations from the grid.
  res_grid.slot_indication(sl_tx);
  pdcch_sch.slot_indication(sl_tx);
  pucch_alloc.slot_indication(sl_tx);
  uci_alloc.slot_indication(sl_tx);

  // > Mark slot start for logging purposes.
  result_logger.on_slot_start();

  // > SSB scheduling.
  ssb_sch.run_slot(res_grid, sl_tx);

  // > Schedule CSI-RS.
  csi_sch.run_slot(res_grid[0]);

  // > Schedule SIB1 and SI-message signalling.
  sib1_sch.schedule_sib1(res_grid[0], sl_tx);
  si_msg_sch.run_slot(res_grid[0]);

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

  // > Log processed events.
  event_logger.log();

  // > Log the scheduler results.
  result_logger.on_scheduler_result(last_result());

  // > Push the scheduler results to the metrics handler.
  metrics.push_result(sl_tx, last_result());
}
