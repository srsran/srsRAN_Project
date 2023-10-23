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

#include "ue.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"

using namespace srsran;

ue::ue(const scheduler_ue_expert_config&        expert_cfg_,
       const cell_configuration&                cell_cfg_common_,
       const sched_ue_creation_request_message& req,
       harq_timeout_handler&                    harq_timeout_notifier_) :
  ue_index(req.ue_index),
  crnti(req.crnti),
  expert_cfg(expert_cfg_),
  cell_cfg_common(cell_cfg_common_),
  harq_timeout_notif(harq_timeout_notifier_, ue_index),
  logger(srslog::fetch_basic_logger("SCHED")),
  ta_mgr(expert_cfg, cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs, &dl_lc_ch_mgr)
{
  srsran_assert(req.cfg.cells.has_value(), "Creation of a UE requires at least Pcell configuration.");

  // Apply configuration.
  handle_reconfiguration_request(req.cfg);

  for (auto& cell : ue_du_cells) {
    if (cell != nullptr) {
      cell->set_fallback_state(req.starts_in_fallback);
    }
  }
}

void ue::slot_indication(slot_point sl_tx)
{
  for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
    if (ue_du_cells[i] != nullptr) {
      // Clear old HARQs.
      ue_du_cells[i]->harqs.slot_indication(sl_tx);
    }
  }

  ta_mgr.slot_indication(sl_tx);
}

void ue::deactivate()
{
  // Disable DL DRBs.
  for (unsigned lcid = LCID_MIN_DRB; lcid <= LCID_MAX_DRB; lcid++) {
    dl_lc_ch_mgr.set_status((lcid_t)lcid, false);
  }

  // Disable UL SRBs and DRBs.
  ul_lc_ch_mgr.deactivate();

  // Stop UL HARQ retransmissions.
  // Note: We do no stop DL retransmissions because we are still relying on DL to send a potential RRC Release.
  for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
    if (ue_du_cells[i] != nullptr) {
      for (unsigned hid = 0; hid != ue_du_cells[i]->harqs.nof_ul_harqs(); ++hid) {
        ue_du_cells[i]->harqs.ul_harq(hid).cancel_harq();
      }
    }
  }
}

void ue::handle_reconfiguration_request(const sched_ue_config_request& cfg)
{
  // Configure Logical Channels.
  if (cfg.lc_config_list.has_value()) {
    log_channels_configs = cfg.lc_config_list.value();
    dl_lc_ch_mgr.configure(log_channels_configs);
    ul_lc_ch_mgr.configure(log_channels_configs);
  }

  // Scheduling Request config.
  if (cfg.sched_request_config_list.has_value()) {
    sched_request_configs = cfg.sched_request_config_list.value();
  }

  // Cell configuration.
  if (cfg.cells.has_value()) {
    // Handle removed cells.
    for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
      if (ue_du_cells[i] != nullptr) {
        if (std::none_of(cfg.cells->begin(), cfg.cells->end(), [i](const cell_config_dedicated& c) {
              return c.serv_cell_cfg.cell_index == to_du_cell_index(i);
            })) {
          // TODO: Handle SCell deletions.
        }
      }
    }
    // Handle new cell creations or reconfigurations.
    for (unsigned ue_cell_index = 0; ue_cell_index != cfg.cells->size(); ++ue_cell_index) {
      ue_cells.resize(cfg.cells->size(), nullptr);
      auto& ue_cell_inst = ue_du_cells[(*cfg.cells)[ue_cell_index].serv_cell_cfg.cell_index];
      if (ue_cell_inst == nullptr) {
        ue_cell_inst = std::make_unique<ue_cell>(
            ue_index, crnti, cell_cfg_common, (*cfg.cells)[ue_cell_index].serv_cell_cfg, harq_timeout_notif);
        if (ue_cell_index >= ue_cells.size()) {
          ue_cells.resize(ue_cell_index + 1);
        }
      } else {
        // Reconfiguration of the cell.
        ue_cell_inst->handle_reconfiguration_request((*cfg.cells)[ue_cell_index].serv_cell_cfg);
      }
      ue_cells[ue_cell_index] = ue_cell_inst.get();
    }
  }

  // Handle new resource allocation config.
  if (cfg.res_alloc_cfg.has_value()) {
    for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
      if (ue_du_cells[i] != nullptr) {
        ue_du_cells[i]->handle_resource_allocation_reconfiguration_request(cfg.res_alloc_cfg.value());
      }
    }
  }
}

unsigned ue::pending_dl_newtx_bytes() const
{
  return dl_lc_ch_mgr.pending_bytes();
}

unsigned ue::pending_dl_srb0_newtx_bytes() const
{
  unsigned pending_bytes = dl_lc_ch_mgr.pending_bytes(LCID_SRB0);

  if (pending_bytes > 0) {
    // In case SRB0 has data, only allocate SRB0 and CEs.
    return pending_bytes + dl_lc_ch_mgr.pending_ue_con_res_id_ce_bytes();
  }
  return pending_bytes;
}

unsigned ue::pending_ul_newtx_bytes() const
{
  constexpr static unsigned SR_GRANT_BYTES = 512;

  // Sum the last BSRs.
  unsigned pending_bytes = ul_lc_ch_mgr.pending_bytes();

  // Subtract the bytes already allocated in UL HARQs.
  for (const ue_cell* ue_cc : ue_cells) {
    if (pending_bytes == 0) {
      break;
    }
    unsigned harq_bytes = 0;
    for (unsigned i = 0; i != ue_cc->harqs.nof_ul_harqs(); ++i) {
      const ul_harq_process& h_ul = ue_cc->harqs.ul_harq(i);
      if (not h_ul.empty()) {
        harq_bytes += h_ul.last_tx_params().tbs_bytes;
      }
    }
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes > 0 ? pending_bytes : (ul_lc_ch_mgr.has_pending_sr() ? SR_GRANT_BYTES : 0);
}

bool ue::has_pending_sr() const
{
  return ul_lc_ch_mgr.has_pending_sr();
}

unsigned ue::build_dl_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_mac_ces(tb_info, dl_lc_ch_mgr, tb_size_bytes);
  total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes);
  return total_subpdu_bytes;
}

unsigned ue::build_dl_srb0_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_ue_con_res_id_mac_ce(tb_info, dl_lc_ch_mgr, tb_size_bytes);
  total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes);
  return total_subpdu_bytes;
}
