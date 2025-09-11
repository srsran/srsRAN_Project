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

#include "si_scheduler.h"
#include "../support/dci_builder.h"
#include "srsran/ran/pdcch/dci_packing.h"

using namespace srsran;

si_scheduler::si_scheduler(const cell_configuration&                       cfg_,
                           pdcch_resource_allocator&                       pdcch_sch_,
                           const sched_cell_configuration_request_message& msg) :
  cell_cfg(cfg_),
  scs_common(cell_cfg.scs_common),
  paging_helper(cfg_, msg),
  default_paging_cycle(static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle)),
  si_change_mod_period(default_paging_cycle * static_cast<unsigned>(cell_cfg.dl_cfg_common.bcch_cfg.mod_period_coeff)),
  pdcch_sch(pdcch_sch_),
  logger(srslog::fetch_basic_logger("SCHED")),
  sib1_sched(cell_cfg, pdcch_sch, msg.sib1_payload_size),
  si_msg_sched(cell_cfg, pdcch_sch, msg.si_scheduling),
  pending_req(si_scheduling_update_request{INVALID_DU_CELL_INDEX, last_version, units::bytes{0U}, {}})
{
}

void si_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  if (SRSRAN_UNLIKELY(not last_sl_tx.valid())) {
    // First call to run_slot.
    slot_count = 0;

    // Fill resource grid up to max_dl_slot_alloc_delay - 1, which corresponds to the grid DL size.
    for (unsigned i = 0, e = res_alloc.max_dl_slot_alloc_delay; i != e; ++i) {
      sib1_sched.run_slot(res_alloc[i]);
      si_msg_sched.run_slot(res_alloc[i]);
    }
  } else {
    // Note: Detect slot skips.
    unsigned nof_slots_elapsed = res_alloc[0].slot - last_sl_tx;
    slot_count += nof_slots_elapsed;
  }
  last_sl_tx = res_alloc[0].slot;

  // If no on-going request is being handled, try to fetch a new request.
  handle_pending_request(res_alloc);

  auto& slot_res_alloc = res_alloc[res_alloc.max_dl_slot_alloc_delay];

  // Run SIB1 scheduling
  sib1_sched.run_slot(slot_res_alloc);

  // Run SI-message scheduling.
  si_msg_sched.run_slot(slot_res_alloc);
}

void si_scheduler::stop()
{
  last_sl_tx = {};

  // Reset on-going requests.
  const auto& next = pending_req.read();
  last_version     = next.version;
  on_going_req.reset();

  sib1_sched.stop();
  si_msg_sched.stop();
}

void si_scheduler::handle_pending_request(cell_resource_allocator& res_alloc)
{
  // The SI is scheduled ahead of time.
  slot_point     slot_sched       = res_alloc.slot_tx() + res_alloc.max_dl_slot_alloc_delay;
  unsigned       slot_count_sched = slot_count + res_alloc.max_dl_slot_alloc_delay;
  const unsigned slots_per_frame  = get_nof_slots_per_subframe(scs_common) * NOF_SUBFRAMES_PER_FRAME;

  if (not on_going_req.has_value()) {
    // Check if there is any pending SI change request to handle.
    const auto& next = pending_req.read();
    if (next.version != last_version) {
      // New pushed request. Save request to be handled in the following slots.
      on_going_req = next;
      last_version = on_going_req->version;

      // Determine the start of SI change modification window.
      const unsigned nof_sfns_until_mod_window = si_change_mod_period - (slot_sched.sfn() % si_change_mod_period);
      si_change_start_count = slot_count_sched + nof_sfns_until_mod_window * slots_per_frame - slot_sched.slot_index();
      if (nof_sfns_until_mod_window < default_paging_cycle) {
        // The next modification window is too close to the current slot to leave enough time to broadcast short
        // messages to all UEs (assuming that we need at least one full default paging cycle to notify all UEs). Thus,
        // we delay the SI change by one full SI change period.
        si_change_start_count += si_change_mod_period * slots_per_frame;
      }
    }
  }

  if (not on_going_req.has_value()) {
    // No pending request to handle.
    return;
  }

  if (slot_count_sched < si_change_start_count - default_paging_cycle * slots_per_frame) {
    // The SI change indication (short message) signalling window has not started yet.
    return;
  }

  if (slot_count_sched < si_change_start_count) {
    // We are inside the SI change indication signalling window.
    try_schedule_short_message(res_alloc[slot_sched]);
    return;
  }

  // We are inside the SI change window.
  logger.debug("SI change with version {} starting after slot {}", on_going_req->version, slot_sched);

  // Apply the SIB1 and SI message changes.
  sib1_sched.handle_sib1_update_indication(on_going_req->version, on_going_req->sib1_len);
  si_msg_sched.handle_si_message_update_indication(on_going_req->version, on_going_req->si_sched_cfg);

  // Delete the on-going request.
  on_going_req = std::nullopt;
}

void si_scheduler::handle_si_update_request(const si_scheduling_update_request& req)
{
  pending_req.write_and_commit(req);
}

void si_scheduler::try_schedule_short_message(cell_slot_resource_allocator& slot_alloc)
{
  slot_point pdcch_slot = slot_alloc.slot;
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    // Skip UL slots.
    return;
  }
  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (slot_alloc.result.dl.dl_pdcchs.full()) {
    return;
  }

  // Check if this is a paging frame. Paging frames are computed according to the following formula:
  // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
  // Note: We need to notify all UEs. Since we don't know the UE ID of UEs in RRC_IDLE, we send the change notification
  // in all paging occasions.
  // DRX cycle in radio frames.
  const unsigned drx_cycle            = default_paging_cycle;
  const unsigned nof_pf_per_drx_cycle = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.nof_pf);
  const unsigned paging_frame_offset  = cell_cfg.dl_cfg_common.pcch_cfg.paging_frame_offset;
  // Number of total paging frames in a drx_cycle.
  const unsigned N = drx_cycle / nof_pf_per_drx_cycle;

  // If t_div_n doesn't evenly divide paging_offset_mod, there is no integer solution for UE_ID, so this is not a PF.
  const unsigned paging_offset_mod = (pdcch_slot.sfn() + paging_frame_offset) % drx_cycle;
  const unsigned t_div_n           = drx_cycle / N;
  if (paging_offset_mod % t_div_n != 0) {
    return;
  }

  // Number of paging occasions in a paging frame.
  const unsigned Ns = static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.ns);

  // Traverse all possible PO indices (i_s).
  // i_s = floor (UE_ID/N) mod Ns.
  // NOTE:
  //   - UE_ID [0, 1024).
  //   - Since T {32, 64, 128, 256} and nof_pf_per_drx_cycle {1, 2, 4, 8, 16}: N <= 256.
  //   => There will be *at least* 4 possible UE IDs (UE_ID mod N) with paging occasions on each PF.
  // Since Ns is at most 4, it is always guaranteed that all PO indices correspond to a valid UE_ID on each PF.
  for (unsigned i_s = 0; i_s != Ns; ++i_s) {
    // Determine if this slot is used by any PO index.
    if (paging_helper.is_paging_slot(pdcch_slot, i_s)) {
      logger.debug("Scheduling SI change notification Short Message at slot {}", slot_alloc.slot);
      allocate_short_message(slot_alloc);
      break;
    }
  }
}

void si_scheduler::allocate_short_message(cell_slot_resource_allocator& slot_alloc)
{
  const auto ss_id = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

  // > Allocate DCI_1_0 for Paging on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(slot_alloc, rnti_t::P_RNTI, ss_id, cell_cfg.expert_cfg.pg.paging_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("Could not allocate SI change notification Short Message in PDCCH");
    return;
  }

  // Fill Paging DCI.
  // Set the systemInfoModification bit, as per TS 38.331 Table 6.5-1.
  static constexpr unsigned si_modification_short_message = 0b10000000;
  build_dci_f1_0_p_rnti(pdcch->dci, cell_cfg.dl_cfg_common.init_dl_bwp, si_modification_short_message);
}
