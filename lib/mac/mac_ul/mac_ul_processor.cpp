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

#include "mac_ul_processor.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

mac_ul_processor::mac_ul_processor(const mac_ul_config& cfg_) :
  cfg(cfg_),
  logger(srslog::fetch_basic_logger("MAC")),
  ue_manager(cfg.rnti_table),
  pdu_handler(cfg.ul_ccch_notifier, cfg.ue_exec_mapper, cfg.sched, ue_manager, cfg.rnti_table, cfg.pcap)
{
}

async_task<bool> mac_ul_processor::add_ue(const mac_ue_create_request& request)
{
  // Update UE executor to match new PCell.
  cfg.ue_exec_mapper.rebind_executors(request.ue_index, request.cell_index);
  task_executor& ul_exec = cfg.ue_exec_mapper.ctrl_executor(request.ue_index);

  // Dispatch UE creation task to new UL executor.
  return execute_and_continue_on_blocking(
      ul_exec, cfg.ctrl_exec, [this, request]() { return ue_manager.add_ue(request); });
}

async_task<bool> mac_ul_processor::addmod_bearers(du_ue_index_t                                  ue_index,
                                                  const std::vector<mac_logical_channel_config>& ul_logical_channels)
{
  return execute_and_continue_on_blocking(
      cfg.ue_exec_mapper.ctrl_executor(ue_index), cfg.ctrl_exec, [this, ue_index, ul_logical_channels]() {
        return ue_manager.addmod_bearers(ue_index, ul_logical_channels);
      });
}

async_task<bool> mac_ul_processor::remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem)
{
  std::vector<lcid_t> lcids(lcids_to_rem.begin(), lcids_to_rem.end());
  return execute_and_continue_on_blocking(
      cfg.ue_exec_mapper.ctrl_executor(ue_index), cfg.ctrl_exec, [this, ue_index, lcids = std::move(lcids)]() {
        return ue_manager.remove_bearers(ue_index, lcids);
      });
}

async_task<void> mac_ul_processor::remove_ue(const mac_ue_delete_request& msg)
{
  return execute_and_continue_on_blocking(cfg.ue_exec_mapper.ctrl_executor(msg.ue_index),
                                          cfg.ctrl_exec,
                                          [this, ue_index = msg.ue_index]() { ue_manager.remove_ue(ue_index); });
}

bool mac_ul_processor::flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ccch_pdu)
{
  if (not cfg.ue_exec_mapper.ctrl_executor(ue_index).execute([this, ue_index, pdu = std::move(ccch_pdu)]() mutable {
        pdu_handler.push_ul_ccch_msg(ue_index, std::move(pdu));
      })) {
    logger.warning("ue={}: Unable to forward UL-CCCH message to upper layers. Cause: task queue is full.", ue_index);
    // Note: The UE is not yet created in the CU, so there in no inactivity timer.
    return false;
  }
  return true;
}

void mac_ul_processor::handle_ue_config_applied(du_ue_index_t ue_index)
{
  if (not cfg.ue_exec_mapper.ctrl_executor(ue_index).execute(
          [this, ue_index]() { ue_manager.handle_ue_config_applied(ue_index); })) {
    logger.warning("ue={}: Unable to forward UE config applied to upper layers. Cause: task queue is full.", ue_index);
  }
}

void mac_ul_processor::handle_rx_data_indication(mac_rx_data_indication msg)
{
  for (mac_rx_pdu& pdu : msg.pdus) {
    if (pdu.pdu.empty()) {
      logger.error("cell={} slot_rx={} rnti={}: Received empty MAC RX PDU from lower layers",
                   msg.cell_index,
                   msg.sl_rx,
                   pdu.rnti);
      continue;
    }

    // > Convert C-RNTI to DU-specific UE index.
    // Note: for Msg3, the UE context is not yet created, and ue_index will be an invalid index. This situation is
    // handled inside the pdu_handler.
    du_ue_index_t ue_index = cfg.rnti_table[pdu.rnti];

    // > Fork each PDU handling to different executors based on the PDU RNTI.
    if (not cfg.ue_exec_mapper.mac_ul_pdu_executor(ue_index).execute(
            [this, slot_rx = msg.sl_rx, cell_idx = msg.cell_index, pdu = std::move(pdu)]() mutable {
              // > Decode Rx PDU and handle respective subPDUs.
              pdu_handler.handle_rx_pdu(slot_rx, cell_idx, std::move(pdu));
            })) {
      logger.warning("cell={} slot_rx={}: Discarding Rx PDU. Cause: Rx task queue is full.", msg.cell_index, msg.sl_rx);
    }
  }
}
