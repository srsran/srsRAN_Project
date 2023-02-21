/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_dl_processor.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

using namespace srsran;

mac_dl_processor::mac_dl_processor(mac_common_config_t& cfg_, mac_scheduler& sched_, du_rnti_table& rnti_table_) :
  cfg(cfg_), logger(cfg.logger), ue_mng(rnti_table_), sched_obj(sched_)
{
  (void)logger;
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
                                           sched_obj,
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

async_task<bool> mac_dl_processor::add_ue(const mac_ue_create_request_message& request)
{
  // > Allocate UE DL HARQ buffers.
  // Note: This is a large allocation, and therefore, should be done outside of the cell thread to avoid causing lates.
  std::vector<std::vector<uint8_t>> harq_buffers;
  harq_buffers.resize(MAX_NOF_HARQS);
  for (auto& h : harq_buffers) {
    h.resize(MAX_DL_PDU_LENGTH);
  }

  return launch_async([this, request, harq_buffers = std::move(harq_buffers)](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // > Change to respective DL executor
    CORO_AWAIT(execute_on(cfg.cell_exec_mapper.executor(request.cell_index)));

    // > Insert UE and DL bearers
    ue_mng.add_ue(request, std::move(harq_buffers));

    // > Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}

async_task<void> mac_dl_processor::remove_ue(const mac_ue_delete_request_message& request)
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

async_task<bool> mac_dl_processor::reconfigure_ue(const mac_ue_reconfiguration_request_message& request)
{
  return launch_async([this, request](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);

    // 1. Change to respective DL executor
    CORO_AWAIT(execute_on(cfg.cell_exec_mapper.executor(request.pcell_index)));

    // 2. Remove UE DL bearers
    ue_mng.remove_bearers(request.ue_index, request.bearers_to_rem);

    // 3. AddMod UE DL bearers
    ue_mng.addmod_bearers(request.ue_index, request.bearers_to_addmod);

    // 3. Change back to CTRL executor before returning
    CORO_AWAIT(execute_on(cfg.ctrl_exec));

    CORO_RETURN(true);
  });
}
