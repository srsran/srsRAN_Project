/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_dl_processor.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;

mac_dl_processor::mac_dl_processor(const mac_dl_config&             mac_cfg,
                                   mac_scheduler_cell_info_handler& sched_,
                                   du_rnti_table&                   rnti_table_) :
  cfg(mac_cfg), rnti_table(rnti_table_), sched(sched_)
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
                                           rnti_table,
                                           cfg.phy_notifier.get_cell(cell_cfg_req.cell_index),
                                           cfg.cell_exec_mapper.executor(cell_cfg_req.cell_index),
                                           cfg.cell_exec_mapper.slot_ind_executor(cell_cfg_req.cell_index),
                                           cfg.ctrl_exec,
                                           cfg.pcap,
                                           cfg.timers);
}

void mac_dl_processor::remove_cell(du_cell_index_t cell_index)
{
  srsran_assert(has_cell(cell_index), "Accessing non-existing cell.");

  // Remove cell from cell manager.
  cells[cell_index].reset();
}

async_task<bool> mac_dl_processor::add_ue(const mac_ue_create_request& request)
{
  if (not has_cell(request.cell_index)) {
    return launch_no_op_task<bool>(false);
  }
  return cells[request.cell_index]->add_ue(request);
}

async_task<void> mac_dl_processor::remove_ue(const mac_ue_delete_request& request)
{
  if (not has_cell(request.cell_index)) {
    return launch_no_op_task();
  }
  return cells[request.cell_index]->remove_ue(request);
}

async_task<bool> mac_dl_processor::addmod_bearers(du_ue_index_t                                  ue_index,
                                                  du_cell_index_t                                pcell_index,
                                                  const std::vector<mac_logical_channel_config>& logical_channels)
{
  if (not has_cell(pcell_index)) {
    return launch_no_op_task<bool>(false);
  }
  return cells[pcell_index]->addmod_bearers(ue_index, logical_channels);
}

async_task<bool>
mac_dl_processor::remove_bearers(du_ue_index_t ue_index, du_cell_index_t pcell_index, span<const lcid_t> lcids_to_rem)
{
  if (not has_cell(pcell_index)) {
    return launch_no_op_task<bool>(false);
  }
  return cells[pcell_index]->remove_bearers(ue_index, lcids_to_rem);
}
