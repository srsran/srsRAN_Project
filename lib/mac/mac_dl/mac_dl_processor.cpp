/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "mac_dl_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/async/execute_on.h"

using namespace srsran;

mac_dl_processor::mac_dl_processor(const mac_dl_config&             mac_cfg,
                                   mac_scheduler_cell_info_handler& sched_,
                                   du_rnti_table&                   rnti_table_) :
  cfg(mac_cfg), ue_mng(rnti_table_), sched(sched_)
{
}

bool mac_dl_processor::has_cell(du_cell_index_t cell_index) const
{
  return cell_index < MAX_NOF_DU_CELLS and cells[cell_index] != nullptr;
}

void mac_dl_processor::add_cell(const mac_cell_creation_request& cell_cfg_req)
{
  srsran_assert(not has_cell(cell_cfg_req.cell_index), "Overwriting existing cell is invalid.");

  // Create MAC cell and add it to list.
  cells[cell_cfg_req.cell_index] =
      std::make_unique<mac_cell_processor>(cell_cfg_req,
                                           sched,
                                           ue_mng,
                                           cfg.phy_notifier.get_cell(cell_cfg_req.cell_index),
                                           cfg.cell_exec_mapper.executor(cell_cfg_req.cell_index),
                                           cfg.cell_exec_mapper.slot_ind_executor(cell_cfg_req.cell_index),
                                           cfg.ctrl_exec,
                                           cfg.pcap);
}

void mac_dl_processor::remove_cell(du_cell_index_t cell_index)
{
  srsran_assert(has_cell(cell_index), "Accessing non-existing cell.");

  // Remove cell from cell manager.
  cells[cell_index].reset();
}

async_task<bool> mac_dl_processor::add_ue(const mac_ue_create_request& request)
{
  // > Allocate UE DL HARQ buffers.
  // Note: This is a large allocation, and therefore, should be done outside of the cell thread to avoid causing lates.
  mac_dl_ue_context ue_inst(request);

  return launch_async([this, request, ue_inst = std::move(ue_inst)](coro_context<async_task<bool>>& ctx) mutable {
    CORO_BEGIN(ctx);

    // > Change to respective DL executor
    CORO_AWAIT(execute_on(cfg.cell_exec_mapper.executor(request.cell_index)));

    // > Insert UE and DL bearers
    ue_mng.add_ue(std::move(ue_inst));

    // > Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}

async_task<void> mac_dl_processor::remove_ue(const mac_ue_delete_request& request)
{
  return launch_async([this, request](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(cfg.cell_exec_mapper.executor(request.cell_index)));

    // 2. Remove UE associated DL channels
    ue_mng.remove_ue(request.ue_index);

    // 3. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN();
  });
}

async_task<bool> mac_dl_processor::addmod_bearers(du_ue_index_t                                  ue_index,
                                                  du_cell_index_t                                pcell_index,
                                                  const std::vector<mac_logical_channel_config>& logical_channels)
{
  return dispatch_and_resume_on(
      cfg.cell_exec_mapper.executor(pcell_index), cfg.ctrl_exec, [this, ue_index, logical_channels]() {
        return ue_mng.addmod_bearers(ue_index, logical_channels);
      });
}

async_task<bool>
mac_dl_processor::remove_bearers(du_ue_index_t ue_index, du_cell_index_t pcell_index, span<const lcid_t> lcids_to_rem)
{
  std::vector<lcid_t> lcids(lcids_to_rem.begin(), lcids_to_rem.end());
  return dispatch_and_resume_on(
      cfg.cell_exec_mapper.executor(pcell_index), cfg.ctrl_exec, [this, ue_index, lcids = std::move(lcids)]() {
        return ue_mng.remove_bearers(ue_index, lcids);
      });
}
