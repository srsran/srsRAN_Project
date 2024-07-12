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

#include "ue.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue::ue(const ue_creation_command& cmd) :
  ue_index(cmd.cfg.ue_index),
  crnti(cmd.cfg.crnti),
  expert_cfg(cmd.cfg.expert_cfg()),
  cell_cfg_common(cmd.cfg.pcell_cfg().cell_cfg_common),
  ue_ded_cfg(&cmd.cfg),
  harq_timeout_notif(cmd.harq_timeout_notifier, ue_index),
  logger(srslog::fetch_basic_logger("SCHED")),
  ta_mgr(expert_cfg, cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs, &dl_lc_ch_mgr)
{
  // Apply configuration.
  handle_reconfiguration_request(ue_reconf_command{cmd.cfg});

  for (auto& cell : ue_du_cells) {
    if (cell != nullptr) {
      cell->set_fallback_state(cmd.starts_in_fallback);
    }
  }
}

void ue::slot_indication(slot_point sl_tx)
{
  for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
    if (ue_du_cells[i] != nullptr) {
      // Clear old HARQs.
      ue_du_cells[i]->harqs.slot_indication(sl_tx);

      // [Implementation-defined]
      // Clear last PxSCH allocated slot if gap to current \c sl_tx is too large. This is done in order to circumvent
      // the ambiguity caused by the slot_point wrap around while scheduling next PxSCHs. e.g. last PxSCH allocated
      // slot=289.0 and next PxSCH to be allocated slot=(289.0 - SCHEDULER_MAX_K0/SCHEDULER_MAX_K2) after wrap around.
      if (ue_du_cells[i]->last_pdsch_allocated_slot.valid() and
          std::abs(sl_tx - ue_du_cells[i]->last_pdsch_allocated_slot) > SCHEDULER_MAX_K0) {
        ue_du_cells[i]->last_pdsch_allocated_slot.clear();
      }
      if (ue_du_cells[i]->last_pusch_allocated_slot.valid() and
          std::abs(sl_tx - ue_du_cells[i]->last_pusch_allocated_slot) > SCHEDULER_MAX_K2) {
        ue_du_cells[i]->last_pusch_allocated_slot.clear();
      }
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
      ue_du_cells[i]->deactivate();
    }
  }
}

void ue::handle_reconfiguration_request(const ue_reconf_command& cmd)
{
  srsran_assert(cmd.cfg.nof_cells() > 0, "Creation of a UE requires at least PCell configuration.");
  ue_ded_cfg = &cmd.cfg;

  // Configure Logical Channels.
  dl_lc_ch_mgr.configure(ue_ded_cfg->logical_channels());
  ul_lc_ch_mgr.configure(ue_ded_cfg->logical_channels());

  // Cell configuration.
  // Handle removed cells.
  for (unsigned i = 0; i != ue_du_cells.size(); ++i) {
    if (ue_du_cells[i] != nullptr) {
      if (not ue_ded_cfg->contains(to_du_cell_index(i))) {
        // TODO: Handle SCell deletions.
      }
    }
  }
  // Handle new cell creations or reconfigurations.
  for (unsigned ue_cell_index = 0; ue_cell_index != ue_ded_cfg->nof_cells(); ++ue_cell_index) {
    du_cell_index_t cell_index   = ue_ded_cfg->ue_cell_cfg(to_ue_cell_index(ue_cell_index)).cell_cfg_common.cell_index;
    auto&           ue_cell_inst = ue_du_cells[cell_index];
    if (ue_cell_inst == nullptr) {
      ue_cell_inst =
          std::make_unique<ue_cell>(ue_index, crnti, ue_ded_cfg->ue_cell_cfg(cell_index), harq_timeout_notif);
      if (ue_cell_index >= ue_cells.size()) {
        ue_cells.resize(ue_cell_index + 1);
      }
    } else {
      // Reconfiguration of the cell.
      ue_cell_inst->handle_reconfiguration_request(ue_ded_cfg->ue_cell_cfg(cell_index));
    }
  }

  // Recompute mapping of UE cell indexing to DU cell indexing.
  ue_cells.resize(ue_ded_cfg->nof_cells(), nullptr);
  for (unsigned ue_cell_index = 0; ue_cell_index != ue_ded_cfg->nof_cells(); ++ue_cell_index) {
    auto& ue_cell_inst =
        ue_du_cells[ue_ded_cfg->ue_cell_cfg(to_ue_cell_index(ue_cell_index)).cell_cfg_common.cell_index];
    ue_cells[ue_cell_index] = ue_cell_inst.get();
  }
}

unsigned ue::pending_dl_newtx_bytes(lcid_t lcid) const
{
  return lcid != INVALID_LCID ? dl_lc_ch_mgr.pending_bytes(lcid) : dl_lc_ch_mgr.pending_bytes();
}

unsigned ue::pending_dl_srb_newtx_bytes() const
{
  return dl_lc_ch_mgr.pending_bytes(LCID_SRB1) + dl_lc_ch_mgr.pending_bytes(LCID_SRB2);
}

unsigned ue::pending_ul_srb_newtx_bytes() const
{
  // LCG ID 0 is used by default for SRBs as per TS 38.331, clause 9.2.1.
  // NOTE: Ensure SRB LCG ID matches the one sent to UE.
  const lcg_id_t srb_lcg_id = uint_to_lcg_id(0);
  return ul_lc_ch_mgr.pending_bytes(srb_lcg_id);
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
    harq_bytes += ue_cc->harqs.ntn_get_tbs_pending_crcs();
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes > 0 ? pending_bytes : (ul_lc_ch_mgr.has_pending_sr() ? SR_GRANT_BYTES : 0);
}

bool ue::has_pending_sr() const
{
  return ul_lc_ch_mgr.has_pending_sr();
}

unsigned ue::build_dl_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes, lcid_t lcid)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_mac_ces(tb_info, dl_lc_ch_mgr, tb_size_bytes);
  total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes, lcid);
  return total_subpdu_bytes;
}

unsigned ue::build_dl_fallback_transport_block_info(dl_msg_tb_info& tb_info, unsigned tb_size_bytes)
{
  unsigned total_subpdu_bytes = 0;
  total_subpdu_bytes += allocate_ue_con_res_id_mac_ce(tb_info, dl_lc_ch_mgr, tb_size_bytes);
  // Since SRB0 PDU cannot be segmented, skip SRB0 if remaining TB size is not enough to fit entire PDU.
  if (dl_lc_ch_mgr.has_pending_bytes(LCID_SRB0) and
      ((tb_size_bytes - total_subpdu_bytes) >= dl_lc_ch_mgr.pending_bytes(LCID_SRB0))) {
    total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes, LCID_SRB0);
    return total_subpdu_bytes;
  }
  total_subpdu_bytes += allocate_mac_sdus(tb_info, dl_lc_ch_mgr, tb_size_bytes - total_subpdu_bytes, LCID_SRB1);
  return total_subpdu_bytes;
}
