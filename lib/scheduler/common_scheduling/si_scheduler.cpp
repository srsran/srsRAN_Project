/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "si_scheduler.h"

using namespace srsran;

si_scheduler::si_scheduler(const cell_configuration&                  cfg_,
                           pdcch_resource_allocator&                  pdcch_sch,
                           units::bytes                               sib1_payload_size,
                           const std::optional<si_scheduling_config>& si_sched_cfg) :
  cell_cfg(cfg_),
  scs_common(cell_cfg.scs_common),
  default_paging_cycle(static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle)),
  si_change_mod_period(default_paging_cycle * static_cast<unsigned>(cell_cfg.dl_cfg_common.bcch_cfg.mod_period_coeff)),
  logger(srslog::fetch_basic_logger("SCHED")),
  sib1_sched(cell_cfg, pdcch_sch, sib1_payload_size),
  si_msg_sched(cell_cfg, pdcch_sch, si_sched_cfg),
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

void si_scheduler::handle_pending_request(cell_resource_allocator& res_alloc)
{
  // The SI is scheduled ahead of time.
  slot_point     slot_sched       = res_alloc.slot_tx() + res_alloc.max_dl_slot_alloc_delay;
  unsigned       slot_count_sched = slot_count + res_alloc.max_dl_slot_alloc_delay;
  const unsigned slots_per_frame  = get_nof_slots_per_subframe(scs_common) * NOF_SUBFRAMES_PER_FRAME;

  if (not on_going_req.has_value()) {
    // Check if there is any pending SI change request to handle.
    auto& next = pending_req.read();
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
    // TODO
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
