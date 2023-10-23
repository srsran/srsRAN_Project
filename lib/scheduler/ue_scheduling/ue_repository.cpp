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

#include "ue_repository.h"

using namespace srsran;

ue_repository::ue_repository(sched_configuration_notifier& mac_notif_) :
  mac_notif(mac_notif_), logger(srslog::fetch_basic_logger("SCHED"))
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
}

/// \brief This function checks whether it is safe to remove a UE. Currently we verify that:
/// - The UE has no pending DL SRB data. This ensures that messages like RRC Release are sent before the UE removal.
/// - The UE has no DL or UL HARQ awaiting an ACK.
static bool is_ue_ready_for_removal(ue& u)
{
  // Ensure that there no currently active HARQs.
  unsigned nof_ue_cells = u.nof_cells();
  for (unsigned cell_idx = 0; cell_idx != nof_ue_cells; ++cell_idx) {
    ue_cell& c = u.get_cell((ue_cell_index_t)cell_idx);
    for (unsigned i = 0; i != c.harqs.nof_dl_harqs(); ++i) {
      if (not c.harqs.dl_harq(i).empty()) {
        return false;
      }
    }
    for (unsigned i = 0; i != c.harqs.nof_ul_harqs(); ++i) {
      if (not c.harqs.ul_harq(i).empty()) {
        return false;
      }
    }
  }

  return true;
}

// Helper function to search in lookup.
static auto search_rnti(const std::vector<std::pair<rnti_t, du_ue_index_t>>& rnti_to_ue_index, rnti_t rnti)
{
  auto it =
      std::lower_bound(rnti_to_ue_index.begin(), rnti_to_ue_index.end(), rnti, [](const auto& lhs, rnti_t rnti_v) {
        return lhs.first < rnti_v;
      });
  return it != rnti_to_ue_index.end() and it->first == rnti ? it : rnti_to_ue_index.end();
}

void ue_repository::slot_indication(slot_point sl_tx)
{
  for (du_ue_index_t& ue_index : ues_to_rem) {
    if (ue_index == INVALID_DU_UE_INDEX) {
      // Already removed.
      continue;
    }
    if (not ues.contains(ue_index)) {
      logger.warning("Unexpected removal of ue={} already took place", ue_index);
      ue_index = INVALID_DU_UE_INDEX;
      // Notify MAC of the successful UE removal.
      mac_notif.on_ue_delete_response(ue_index);
      continue;
    }
    ue& u = *ues[ue_index];

    // Check if UEs can be safely removed.
    if (is_ue_ready_for_removal(u)) {
      logger.debug("ue={} has been successfully removed.", ue_index);

      // Notify MAC of the successful UE removal.
      mac_notif.on_ue_delete_response(ue_index);

      // Remove UE from lookup.
      auto it = search_rnti(rnti_to_ue_index_lookup, u.crnti);
      if (it != rnti_to_ue_index_lookup.end()) {
        rnti_to_ue_index_lookup.erase(it);
      } else {
        logger.warning("UE with c-rnti={:#x} not found in RNTI -> UE index lookup.", u.crnti);
      }

      // Remove UE from the repository.
      ues.erase(ue_index);

      // Mark UE as ready for removal.
      ue_index = INVALID_DU_UE_INDEX;
    }
  }

  // In case the elements at the front of the ring has been marked for removal, pop them from the queue.
  while (not ues_to_rem.empty() and ues_to_rem[0] == INVALID_DU_UE_INDEX) {
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
  rnti_to_ue_index_lookup.emplace_back(rnti, ue_index);
  std::sort(rnti_to_ue_index_lookup.begin(), rnti_to_ue_index_lookup.end());
}

void ue_repository::schedule_ue_rem(du_ue_index_t ue_index)
{
  if (contains(ue_index)) {
    // Start deactivation of UE bearers.
    ues[ue_index]->deactivate();

    // Register UE for later removal.
    ues_to_rem.push(ue_index);
  }
}

ue* ue_repository::find_by_rnti(rnti_t rnti)
{
  auto it = search_rnti(rnti_to_ue_index_lookup, rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}

const ue* ue_repository::find_by_rnti(rnti_t rnti) const
{
  auto it = search_rnti(rnti_to_ue_index_lookup, rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}
