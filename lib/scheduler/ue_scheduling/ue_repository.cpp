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

#include "ue_repository.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue_repository::ue_repository() : logger(srslog::fetch_basic_logger("SCHED")), ues_to_destroy(MAX_NOF_DU_UES)
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
}

ue_repository::~ue_repository()
{
  for (auto& u : ues_to_rem) {
    u.second.release();
  }
}

/// \brief This function checks whether it is safe to remove a UE. Currently we verify that the UE has no DL or UL
/// HARQ awaiting an ACK.
static bool is_ue_ready_for_removal(ue& u)
{
  // Ensure that there no currently active HARQs.
  unsigned nof_ue_cells = u.nof_cells();
  for (unsigned cell_idx = 0; cell_idx != nof_ue_cells; ++cell_idx) {
    const ue_cell& c = u.get_cell((ue_cell_index_t)cell_idx);
    for (unsigned i = 0, e = c.harqs.nof_dl_harqs(); i != e; ++i) {
      if (c.harqs.dl_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
    for (unsigned i = 0, e = c.harqs.nof_ul_harqs(); i != e; ++i) {
      if (c.harqs.ul_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
  }

  return true;
}

void ue_repository::slot_indication(slot_point sl_tx)
{
  last_sl_tx = sl_tx;

  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (not rem_ev.valid()) {
      // Already removed.
      continue;
    }
    if (p.first > sl_tx) {
      // UE is not yet ready to be removed as there may be still pending allocations for it in the resource grid.
      continue;
    }

    const du_ue_index_t ue_idx = rem_ev.ue_index();
    if (not ues.contains(ue_idx)) {
      logger.error("ue={}: Unexpected UE removal from UE repository", fmt::underlying(ue_idx));
      rem_ev.reset();
      continue;
    }
    ue&    u     = *ues[ue_idx];
    rnti_t crnti = u.crnti;

    // Check if UEs can be safely removed.
    if (not is_ue_ready_for_removal(u)) {
      continue;
    }

    // Remove UE from the repository.
    rem_ue(u);

    // Marks UE config removal as complete.
    rem_ev.reset();

    logger.debug("ue={} rnti={}: UE has been successfully removed.", fmt::underlying(ue_idx), crnti);
  }

  // In case the elements at the front of the ring has been marked for removal, pop them from the queue.
  while (not ues_to_rem.empty() and not ues_to_rem[0].second.valid()) {
    ues_to_rem.pop();
  }

  // Update state of existing UEs.
  for (auto& u : ues) {
    u->slot_indication(sl_tx);
  }
}

void ue_repository::add_ue(std::unique_ptr<ue> u)
{
  // Add UE in repository.
  du_ue_index_t ue_index = u->ue_index;
  rnti_t        rnti     = u->crnti;
  ues.insert(ue_index, std::move(u));

  // Update RNTI -> UE index lookup.
  rnti_to_ue_index_lookup.insert(std::make_pair(rnti, ue_index));
}

void ue_repository::schedule_ue_rem(ue_config_delete_event ev)
{
  if (contains(ev.ue_index())) {
    // Start deactivation of UE bearers.
    auto& u = ues[ev.ue_index()];
    u->deactivate();

    // Register UE for later removal.
    // We define a time window when the UE removal is not allowed, as there are pending CSI/SR PDUs in the resource grid
    // ready to be sent to the PHY. Removing the UE earlier would mean that its PUCCH resources would become available
    // to a newly created UE and there could be a PUCCH collision.
    slot_point rem_slot =
        last_sl_tx + get_max_slot_ul_alloc_delay(u->get_pcell().cfg().cell_cfg_common.ntn_cs_koffset) + 1;
    ues_to_rem.push(std::make_pair(rem_slot, std::move(ev)));
  }
}

ue* ue_repository::find_by_rnti(rnti_t rnti)
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}

const ue* ue_repository::find_by_rnti(rnti_t rnti) const
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}

void ue_repository::destroy_pending_ues()
{
  std::unique_ptr<ue> removed_ue;
  while (ues_to_destroy.try_pop(removed_ue)) {
  }
}

void ue_repository::rem_ue(const ue& u)
{
  const rnti_t        crnti  = u.crnti;
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE from lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.",
                 fmt::underlying(ue_idx),
                 crnti);
  }

  // Take the UE from the repository and schedule its destruction outside the critical section.
  auto ue_ptr = ues.take(ue_idx);
  ue_ptr->release_resources();
  if (not ues_to_destroy.try_push(std::move(ue_ptr))) {
    logger.warning("Failed to offload UE destruction. Performance may be affected");
  }
}

void ue_repository::handle_cell_deactivation(du_cell_index_t cell_index)
{
  for (std::unique_ptr<ue>& u : ues) {
    ue_cell* ue_cc = u->find_cell(cell_index);
    if (ue_cc == nullptr) {
      // UE does not have this cell, so we can skip it.
      continue;
    }

    // Note: We now remove the UE from the repository, indepedently of whether it is a PCell or SCell. It would be very
    // hard to handle a UE that has a config for a cell that is not active.
    rem_ue(*u);
  }

  // We may have removed UEs that were scheduled for removal in an earlier slot. We need to clean up the ues_to_rem.
  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (rem_ev.valid()) {
      const du_ue_index_t ue_idx = rem_ev.ue_index();
      if (not ues.contains(ue_idx)) {
        // UE removed in the previous loop, so we need to clear this event.
        rem_ev.reset();
      }
    }
  }
}
