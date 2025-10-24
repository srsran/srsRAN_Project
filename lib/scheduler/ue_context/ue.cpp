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

#include "ue.h"
#include "../support/dmrs_helpers.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue::ue(const ue_creation_command& cmd) :
  ue_index(cmd.cfg.ue_index),
  crnti(cmd.cfg.crnti),
  expert_cfg(cmd.cfg.expert_cfg()),
  cell_cfg_common(cmd.cfg.pcell_cfg().cell_cfg_common),
  ue_ded_cfg(&cmd.cfg),
  pcell_harq_pool(cmd.pcell_harq_pool),
  logger(srslog::fetch_basic_logger("SCHED")),
  dl_lc_ch_mgr(cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.scs,
               cmd.starts_in_fallback,
               cmd.cfg.logical_channels()),
  ul_lc_ch_mgr(cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.scs, cmd.cfg.logical_channels()),
  ta_mgr(expert_cfg,
         cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs,
         ue_ded_cfg->pcell_cfg().tag_id(),
         &dl_lc_ch_mgr),
  drx(cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs,
      cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer,
      cmd.cfg.drx_cfg(),
      ul_lc_ch_mgr,
      cmd.ul_ccch_slot_rx,
      logger)
{
  // Apply configuration.
  set_config(cmd.cfg, cmd.ul_ccch_slot_rx);

  for (auto& cell : ue_du_cells) {
    if (cell != nullptr) {
      cell->set_fallback_state(cmd.starts_in_fallback, false, false);
    }
  }
}

void ue::slot_indication(slot_point sl_tx)
{
  last_sl_tx = sl_tx;
  dl_lc_ch_mgr.slot_indication();
  ul_lc_ch_mgr.slot_indication();
  ta_mgr.slot_indication(sl_tx);
  drx.slot_indication(sl_tx);
}

void ue::deactivate()
{
  // Disable DL SRBs and DRBs.
  // Note: We assume that when this function is called any pending RRC container (e.g. containing RRC Release) has
  // already been Tx+ACKed or an upper layer timeout has triggered.
  dl_lc_ch_mgr.deactivate();

  // Disable UL SRBs and DRBs.
  ul_lc_ch_mgr.deactivate();

  // Cancel HARQ retransmissions in all UE cells.
  for (auto& cell : ue_du_cells) {
    if (cell != nullptr) {
      cell->deactivate();
    }
  }
}

void ue::release_resources()
{
  for (auto& cell : ue_du_cells) {
    if (cell != nullptr) {
      cell->harqs.reset();
    }
  }
}

void ue::handle_reconfiguration_request(const ue_reconf_command& cmd, bool reestablished_)
{
  // UE enters fallback mode when a Reconfiguration takes place.
  get_pcell().set_fallback_state(true, true, reestablished_);
  dl_lc_ch_mgr.set_fallback_state(true);

  // Update UE config.
  set_config(cmd.cfg);
}

void ue::handle_config_applied()
{
  get_pcell().set_fallback_state(false, false, false);
  dl_lc_ch_mgr.set_fallback_state(false);
}

void ue::set_config(const ue_configuration& new_cfg, std::optional<slot_point> msg3_slot_rx)
{
  srsran_assert(new_cfg.nof_cells() > 0, "Creation of a UE requires at least PCell configuration.");
  ue_ded_cfg = &new_cfg;

  // Configure Logical Channels.
  dl_lc_ch_mgr.configure(ue_ded_cfg->logical_channels());
  ul_lc_ch_mgr.configure(ue_ded_cfg->logical_channels());

  // DRX config.
  if (ue_ded_cfg->drx_cfg().has_value()) {
    drx.reconfigure(ue_ded_cfg->drx_cfg());
  }

  // Cell configuration.
  // Handle removed cells.
  for (unsigned i = 0, e = ue_du_cells.size(); i != e; ++i) {
    if (ue_du_cells[i] != nullptr) {
      if (not ue_ded_cfg->contains(to_du_cell_index(i))) {
        // TODO: Handle SCell deletions.
      }
    }
  }
  // Handle new cell creations or reconfigurations.
  for (unsigned ue_cell_index = 0, e = ue_ded_cfg->nof_cells(); ue_cell_index != e; ++ue_cell_index) {
    du_cell_index_t cell_index   = ue_ded_cfg->ue_cell_cfg(to_ue_cell_index(ue_cell_index)).cell_cfg_common.cell_index;
    auto&           ue_cell_inst = ue_du_cells[cell_index];
    if (ue_cell_inst == nullptr) {
      ue_cell_inst = std::make_unique<ue_cell>(ue_index,
                                               crnti,
                                               to_ue_cell_index(ue_cell_index),
                                               ue_ded_cfg->ue_cell_cfg(cell_index),
                                               pcell_harq_pool,
                                               ue_shared_context{drx},
                                               msg3_slot_rx);
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
  for (unsigned ue_cell_index = 0, e = ue_ded_cfg->nof_cells(); ue_cell_index != e; ++ue_cell_index) {
    auto& ue_cell_inst =
        ue_du_cells[ue_ded_cfg->ue_cell_cfg(to_ue_cell_index(ue_cell_index)).cell_cfg_common.cell_index];
    ue_cells[ue_cell_index] = ue_cell_inst.get();
  }
}

void ue::handle_dl_buffer_state_indication(lcid_t lcid, unsigned bs, slot_point hol_toa)
{
  unsigned pending_bytes = bs;

  // Subtract bytes pending for this LCID in scheduled DL HARQ allocations (but not yet sent to the lower layers)
  // before forwarding to DL logical channel manager.
  // Note: The RLC buffer occupancy updates never account for bytes associated with future HARQ transmissions.
  // Note: The TB in the HARQ should also include RLC header segmentation overhead which is not accounted yet in the
  // reported RLC DL buffer occupancy report (reminder: we haven't built the RLC PDU yet!). If we account for this
  // overhead in the computation of pending bytes, the final value will be too low, which will lead to one extra
  // tiny grant. To avoid this, we make the pessimization that every HARQ contains one RLC header due to segmentation.
  static constexpr unsigned RLC_AM_HEADER_SIZE_ESTIM = 4;
  for (unsigned c = 0, ce = nof_cells(); c != ce; ++c) {
    auto& ue_cc = *ue_cells[c];

    if (last_sl_tx.valid() and ue_cc.harqs.last_pdsch_slot().valid() and ue_cc.harqs.last_pdsch_slot() > last_sl_tx) {
      unsigned rem_harqs = ue_cc.harqs.nof_dl_harqs() - ue_cc.harqs.nof_empty_dl_harqs();
      for (unsigned i = 0, e = ue_cc.harqs.nof_dl_harqs(); i != e and rem_harqs > 0; ++i) {
        auto h_dl = ue_cc.harqs.dl_harq(to_harq_id(i));
        if (h_dl.has_value()) {
          rem_harqs--;
          if (h_dl->pdsch_slot() > last_sl_tx and h_dl->nof_retxs() == 0 and h_dl->is_waiting_ack()) {
            for (const auto& lc : h_dl->get_grant_params().lc_sched_info) {
              if (lc.lcid.is_sdu() and lc.lcid.to_lcid() == lcid) {
                unsigned bytes_sched =
                    lc.sched_bytes.value() - std::min(lc.sched_bytes.value(), RLC_AM_HEADER_SIZE_ESTIM);
                pending_bytes -= std::min(pending_bytes, bytes_sched);
              }
            }
          }
        }
      }
    }
  }

  dl_lc_ch_mgr.handle_dl_buffer_status_indication(lcid, pending_bytes, hol_toa);
}

unsigned ue::pending_ul_newtx_bytes() const
{
  static constexpr unsigned SR_GRANT_BYTES = 512;

  // Sum the last BSRs.
  unsigned pending_bytes = ul_lc_ch_mgr.pending_bytes();

  // Subtract the bytes already allocated in UL HARQs.
  for (const ue_cell* ue_cc : ue_cells) {
    if (pending_bytes == 0) {
      break;
    }
    unsigned harq_bytes = ue_cc->harqs.total_ul_bytes_waiting_ack();
    pending_bytes -= std::min(pending_bytes, harq_bytes);
  }

  // If there are no pending bytes, check if a SR is pending.
  return pending_bytes > 0 ? pending_bytes : (ul_lc_ch_mgr.has_pending_sr() ? SR_GRANT_BYTES : 0);
}
