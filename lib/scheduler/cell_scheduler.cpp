/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
                               cell_metrics_handler&                           metrics_handler) :
  cell_cfg(cell_cfg_),
  ue_sched(ue_sched_),
  res_grid(cell_cfg),
  event_logger(cell_cfg.cell_index, cell_cfg.pci),
  metrics(metrics_handler),
  result_logger(sched_cfg.log_broadcast_messages, cell_cfg.pci),
  logger(srslog::fetch_basic_logger("SCHED")),
  ssb_sch(cell_cfg),
  pdcch_sch(cell_cfg),
  si_sch(cell_cfg, pdcch_sch, msg),
  csi_sch(cell_cfg),
  ra_sch(sched_cfg.ra, cell_cfg, pdcch_sch, event_logger, metrics),
  prach_sch(cell_cfg),
  pucch_alloc(cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot),
  uci_alloc(pucch_alloc),
  pucch_guard_sch(cell_cfg),
  pg_sch(sched_cfg, cell_cfg, pdcch_sch, msg)
{
  // Register new cell in the UE scheduler.
  ue_sched.add_cell(ue_scheduler_cell_params{
      msg.cell_index, &pdcch_sch, &pucch_alloc, &uci_alloc, &res_grid, &metrics, &event_logger});
}

void cell_scheduler::handle_si_update_request(const si_scheduling_update_request& msg)
{
  si_sch.handle_si_update_request(msg);
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
  // Mark the start of the slot.
  auto slot_start_tp = std::chrono::high_resolution_clock::now();

  // If there are skipped slots, handle them. Otherwise, the cell grid and cached results are not correctly cleared.
  if (SRSRAN_LIKELY(res_grid.slot_tx().valid())) {
    while (SRSRAN_UNLIKELY(res_grid.slot_tx() + 1 != sl_tx)) {
      slot_point skipped_slot = res_grid.slot_tx() + 1;
      logger.info("Status: Detected skipped slot={}.", skipped_slot);
      reset_resource_grid(skipped_slot);
    }
  }

  // > Start with clearing old allocations from the grid.
  reset_resource_grid(sl_tx);

  // > Handle commands to start/stop cell if any.
  handle_pending_cell_activity_commands();

  if (SRSRAN_UNLIKELY(not is_running())) {
    return;
  }
  // Cell is active. Run the cell sub-schedulers.

  // > SSB scheduling.
  ssb_sch.run_slot(res_grid, sl_tx);

  // > Schedule CSI-RS.
  csi_sch.run_slot(res_grid[0]);

  // > Schedule SIB1 and SI-message signalling.
  si_sch.run_slot(res_grid);

  // > Schedule PUCCH guardbands.
  pucch_guard_sch.run_slot(res_grid);

  // > Schedule PRACH PDUs.
  prach_sch.run_slot(res_grid);

  // > Schedule RARs and Msg3.
  ra_sch.run_slot(res_grid);

  // > Schedule Paging.
  pg_sch.run_slot(res_grid);

  // > Schedule UE DL and UL data.
  ue_sched.run_slot(sl_tx);

  // > Mark stop of the slot processing
  auto slot_stop_tp = std::chrono::high_resolution_clock::now();
  auto slot_dur     = std::chrono::duration_cast<std::chrono::microseconds>(slot_stop_tp - slot_start_tp);

  // > Log processed events.
  event_logger.log();

  // > Log the scheduler results.
  result_logger.on_scheduler_result(last_result(), slot_dur);

  // > Push the scheduler results to the metrics handler.
  metrics.push_result(sl_tx, last_result(), slot_dur);
}

void cell_scheduler::reset_resource_grid(slot_point sl_tx)
{
  // Reset cell resource grid.
  res_grid.slot_indication(sl_tx);

  // Reset PDCCH slot context.
  pdcch_sch.slot_indication(sl_tx);

  // Reset PUCCH slot context.
  pucch_alloc.slot_indication(sl_tx);

  // Reset UCI slot context.
  uci_alloc.slot_indication(sl_tx);
}

void cell_scheduler::handle_pending_cell_activity_commands()
{
  auto cmd = activ_cmd.exchange(activation_command::no_cmd, std::memory_order_relaxed);
  if (SRSRAN_LIKELY(cmd == activation_command::no_cmd)) {
    // No-op.
    return;
  }
  if ((cmd == activation_command::start_cmd and active) or (cmd == activation_command::stop_cmd and not active)) {
    // No-op.
    return;
  }

  if (cmd == activation_command::start_cmd) {
    active = true;
  } else {
    active = false;
    metrics.handle_cell_deactivation();
  }
}

void cell_scheduler::start()
{
  activ_cmd.store(activation_command::start_cmd, std::memory_order_relaxed);
}

void cell_scheduler::stop()
{
  activ_cmd.store(activation_command::stop_cmd, std::memory_order_relaxed);
}
