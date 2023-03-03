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

ue_repository::ue_repository(sched_configuration_notifier& mac_notif_) :
  mac_notif(mac_notif_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

/// \brief This function checks whether it is safe to remove a UE. Currently we verify that:
/// - The UE has no pending DL SRB data. This ensures that messages like RRC Release are sent before the UE removal.
/// - The UE has no DL or UL HARQ awaiting an ACK.
static bool is_ue_ready_for_removal(ue& u)
{
  // Ensure DL SRB data is flushed.
  for (lcid_t lcid : {LCID_SRB0, LCID_SRB1, LCID_SRB2}) {
    if (u.has_pending_dl_newtx_bytes(lcid)) {
      return false;
    }
  }

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

void ue_repository::slot_indication(slot_point sl_tx)
{
  for (du_ue_index_t& ue_index : ues_to_rem) {
    if (ue_index == INVALID_DU_UE_INDEX) {
      // Already removed.
      continue;
    }
    auto it = ues.find(ue_index);
    if (it == ues.end()) {
      logger.warning("Unexpected removal of ue={} already took place", ue_index);
      ue_index = INVALID_DU_UE_INDEX;
      // Notify MAC of the successful UE removal.
      mac_notif.on_ue_delete_response(ue_index);
      continue;
    }

    // Check if UEs can be safely removed.
    if (is_ue_ready_for_removal(*it)) {
      logger.debug("ue={} has been successfully removed.", ue_index);

      // Notify MAC of the successful UE removal.
      mac_notif.on_ue_delete_response(ue_index);

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
