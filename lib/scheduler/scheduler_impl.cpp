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

#include "scheduler_impl.h"
#include "ue_scheduling/ue_scheduler_impl.h"
#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "srsran/scheduler/config/scheduler_ue_config_validator.h"

using namespace srsran;

scheduler_impl::scheduler_impl(const scheduler_config& sched_cfg_) :
  expert_params(sched_cfg_.expert_params),
  config_notifier(sched_cfg_.config_notifier),
  logger(srslog::fetch_basic_logger("SCHED")),
  metrics(expert_params.metrics_report_period, sched_cfg_.metrics_notifier)
{
}

bool scheduler_impl::handle_cell_configuration_request(const sched_cell_configuration_request_message& msg)
{
  srsran_assert(msg.cell_index < MAX_NOF_DU_CELLS, "cell index={} is not valid", msg.cell_index);
  srsran_assert(not cells.contains(msg.cell_index), "cell={} already exists", msg.cell_index);
  srsran_assert(
      not config_validators::validate_sched_cell_configuration_request_message(msg, expert_params).is_error(),
      "Invalid cell configuration request message. Cause: {}",
      config_validators::validate_sched_cell_configuration_request_message(msg, expert_params).error().c_str());

  // Check if it is a new DU Cell Group.
  if (not groups.contains(msg.cell_group_index)) {
    // If it is a new group, create a new instance.
    groups.emplace(msg.cell_group_index,
                   std::make_unique<ue_scheduler_impl>(expert_params.ue, config_notifier, metrics, sched_ev_logger));
  }

  // Create a new cell scheduler instance.
  cells.emplace(
      msg.cell_index,
      std::make_unique<cell_scheduler>(expert_params, msg, *groups[msg.cell_group_index], sched_ev_logger, metrics));

  logger.info("Cell with cell_index={} was configured.", msg.cell_index);

  return true;
}

void scheduler_impl::handle_ue_creation_request(const sched_ue_creation_request_message& ue_request)
{
  // Add Mapping UE index -> DU Cell Group index.
  du_cell_index_t pcell_index = ue_request.cfg.cells[0].serv_cell_cfg.cell_index;

  error_type<std::string> result =
      config_validators::validate_sched_ue_creation_request_message(ue_request, cells[pcell_index]->cell_cfg);
  if (result.is_error()) {
    logger.warning(
        "ue={} rnti={:#x}: Discarding invalid UE creation request. Cause: {}", ue_request.crnti, result.error());
    config_notifier.on_ue_config_complete(ue_request.ue_index, false);
  }

  ue_to_cell_group_index.emplace(ue_request.ue_index, cells[pcell_index]->cell_cfg.cell_group_index);

  // Dispatch UE creation to UE scheduler associated to the PCell.
  cells[pcell_index]->ue_sched.get_ue_configurator().handle_ue_creation_request(ue_request);
}

void scheduler_impl::handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request)
{
  srsran_assert(ue_to_cell_group_index.contains(ue_request.ue_index), "UE={} not yet created", ue_request.ue_index);
  du_cell_group_index_t group_index = ue_to_cell_group_index[ue_request.ue_index];

  groups[group_index]->get_ue_configurator().handle_ue_reconfiguration_request(ue_request);
}

void scheduler_impl::handle_ue_removal_request(du_ue_index_t ue_index)
{
  srsran_assert(ue_to_cell_group_index.contains(ue_index), "UE={} not yet created", ue_index);
  du_cell_group_index_t group_index = ue_to_cell_group_index[ue_index];

  groups[group_index]->get_ue_configurator().handle_ue_removal_request(ue_index);
}

void scheduler_impl::handle_rach_indication(const rach_indication_message& msg)
{
  srsran_assert(cells.contains(msg.cell_index), "cell={} does not exist", msg.cell_index);
  cells[msg.cell_index]->handle_rach_indication(msg);
}

void scheduler_impl::handle_ul_bsr_indication(const ul_bsr_indication_message& bsr)
{
  if (not ue_to_cell_group_index.contains(bsr.ue_index)) {
    logger.warning("ue={}: Discarding UL BSR. Cause: UE not recognized", bsr.ue_index);
    return;
  }
  du_cell_group_index_t group_index = ue_to_cell_group_index[bsr.ue_index];

  groups[group_index]->get_feedback_handler().handle_ul_bsr_indication(bsr);
}

void scheduler_impl::handle_ul_phr_indication(const ul_phr_indication_message& phr_ind)
{
  srsran_assert(cells.contains(phr_ind.cell_index), "cell={} does not exist", phr_ind.cell_index);

  cells[phr_ind.cell_index]->ue_sched.get_feedback_handler().handle_ul_phr_indication(phr_ind);
}

void scheduler_impl::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs)
{
  if (not ue_to_cell_group_index.contains(bs.ue_index)) {
    logger.warning("ue={}: Discarding DL buffer status update. Cause: UE not recognized", bs.ue_index);
    return;
  }
  du_cell_group_index_t group_index = ue_to_cell_group_index[bs.ue_index];

  groups[group_index]->get_dl_buffer_state_indication_handler().handle_dl_buffer_state_indication(bs);
}

void scheduler_impl::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  srsran_assert(cells.contains(crc_ind.cell_index), "cell={} does not exist", crc_ind.cell_index);

  cells[crc_ind.cell_index]->handle_crc_indication(crc_ind);
}

void scheduler_impl::handle_uci_indication(const uci_indication& uci)
{
  srsran_assert(cells.contains(uci.cell_index), "cell={} does not exist", uci.cell_index);

  cells[uci.cell_index]->ue_sched.get_feedback_handler().handle_uci_indication(uci);
}

void scheduler_impl::handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce)
{
  if (not ue_to_cell_group_index.contains(mac_ce.ue_index)) {
    logger.warning("ue={}: Discarding MAC CE update. Cause: UE not recognized", mac_ce.ue_index);
    return;
  }
  du_cell_group_index_t group_index = ue_to_cell_group_index[mac_ce.ue_index];

  groups[group_index]->get_feedback_handler().handle_dl_mac_ce_indication(mac_ce);
}

const sched_result& scheduler_impl::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  srsran_assert(cells.contains(cell_index), "cell={} does not exist", cell_index);
  cell_scheduler& cell = *cells[cell_index];

  if (cell_index == to_du_cell_index(0)) {
    // Set scheduler logger context only once per slot.
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
  }

  // > Run scheduler for the given slot and cell.
  cell.run_slot(sl_tx);

  // Return result for the slot.
  return cell.last_result();
}

void scheduler_impl::handle_paging_information(const sched_paging_information& pi)
{
  for (const auto cell_id : pi.paging_cells) {
    cells[cell_id]->handle_paging_information(pi);
  }
}
