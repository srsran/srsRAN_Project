/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_repository.h"

using namespace srsran;

void ue_repository::slot_indication(slot_point sl_tx)
{
  for (du_ue_index_t& ue_index : ues_to_rem) {
    auto it = ues.find(ue_index);

    // Check if UEs can be safely removed.
    if (it != ues.end()) {
      unsigned nof_ue_cells = it->nof_cells();
      for (unsigned cell_idx = 0; cell_idx != nof_ue_cells; ++cell_idx) {
        ue_cell& c = it->get_cell((ue_cell_index_t)cell_idx);
        if (c.harqs.find_pending_dl_retx() != nullptr or c.harqs.find_pending_ul_retx() != nullptr) {
          // There are still pending HARQs. Postpone removal of this UE.
          continue;
        }
      }
    }

    // Notify MAC of the successful UE removal.
    mac_notif.on_ue_delete_response(ue_index);

    // Mark UE as ready for removal.
    ue_index = INVALID_DU_UE_INDEX;

    // In case the element at the front of the ring has been marked for removal, pop it from the queue.
    if (ues_to_rem[0] == INVALID_DU_UE_INDEX) {
      ues_to_rem.pop();
    }
  }

  // Update state of existing UEs.
  for (auto& u : ues) {
    u.slot_indication(sl_tx);
  }
}

void ue_repository::add_ue(std::unique_ptr<ue> u)
{
  du_ue_index_t ue_index = u->ue_index;
  ues.insert(ue_index, std::move(u));
}

void ue_repository::schedule_ue_rem(du_ue_index_t ue_index)
{
  if (contains(ue_index)) {
    // Start deactivation of UE bearers.
    ues[ue_index].deactivate();

    // Register UE for later removal.
    ues_to_rem.push(ue_index);
  }
}
