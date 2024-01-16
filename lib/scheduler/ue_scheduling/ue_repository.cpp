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

#include "ue_repository.h"

using namespace srsran;

ue_repository::ue_repository() : logger(srslog::fetch_basic_logger("SCHED"))
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
  for (ue_config_delete_event& p : ues_to_rem) {
    if (not p.valid()) {
      // Already removed.
      continue;
    }
    const du_ue_index_t ue_idx = p.ue_index();
    if (not ues.contains(ue_idx)) {
      logger.error("ue={}: Unexpected UE removal from UE repository", ue_idx);
      p.reset();
      continue;
    }
    ue&    u     = *ues[ue_idx];
    rnti_t crnti = u.crnti;

    // Check if UEs can be safely removed.
    if (not is_ue_ready_for_removal(u)) {
      continue;
    }

    // Remove UE from lookup.
    auto it = search_rnti(rnti_to_ue_index_lookup, crnti);
    if (it != rnti_to_ue_index_lookup.end()) {
      rnti_to_ue_index_lookup.erase(it);
    } else {
      logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.", ue_idx, crnti);
    }

    // Remove UE from the repository.
    ues.erase(ue_idx);

    // Marks UE config removal as complete.
    p.reset();

    logger.debug("ue={} rnti={}: UE has been successfully removed.", ue_idx, crnti);
  }

  // In case the elements at the front of the ring has been marked for removal, pop them from the queue.
  while (not ues_to_rem.empty() and not ues_to_rem[0].valid()) {
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

void ue_repository::schedule_ue_rem(ue_config_delete_event ev)
{
  if (contains(ev.ue_index())) {
    // Start deactivation of UE bearers.
    ues[ev.ue_index()]->deactivate();

    // Register UE for later removal.
    ues_to_rem.push(std::move(ev));
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
