/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/adt/stable_id_map.h"

namespace srsran {

/// Container that stores all scheduler UEs.
class ue_repository
{
  using ue_list = stable_id_map<du_ue_index_t, ue, MAX_NOF_DU_UES>;

public:
  using value_type     = ue_list::value_type;
  using iterator       = ue_list::iterator;
  using const_iterator = ue_list::const_iterator;

  explicit ue_repository(sched_configuration_notifier& mac_notif_) : mac_notif(mac_notif_) {}

  void slot_indication(slot_point sl_tx)
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

  /// \brief Contains ue index.
  bool contains(du_ue_index_t ue_index) const { return ues.contains(ue_index); }

  ue&       operator[](du_ue_index_t ue_index) { return ues[ue_index]; }
  const ue& operator[](du_ue_index_t ue_index) const { return ues[ue_index]; }

  /// \brief Add new UE in the UE repository.
  void add_ue(std::unique_ptr<ue> u)
  {
    du_ue_index_t ue_index = u->ue_index;
    ues.insert(ue_index, std::move(u));
  }

  /// \brief Initiate removal of existing UE from the repository.
  void schedule_ue_rem(du_ue_index_t ue_index)
  {
    if (contains(ue_index)) {
      // Start deactivation of UE bearers.
      ues[ue_index].deactivate();

      // Register UE for later removal.
      ues_to_rem.push(ue_index);
    }
  }

  iterator       find(du_ue_index_t ue_index) { return ues.find(ue_index); }
  const_iterator find(du_ue_index_t ue_index) const { return ues.find(ue_index); }

  size_t size() const { return ues.size(); }

  bool empty() const { return ues.empty(); }

  iterator       begin() { return ues.begin(); }
  iterator       end() { return ues.end(); }
  const_iterator begin() const { return ues.begin(); }
  const_iterator end() const { return ues.end(); }

  const_iterator lower_bound(du_ue_index_t ue_index) const { return ues.lower_bound(ue_index); }

private:
  sched_configuration_notifier& mac_notif;

  /// Repository of UEs.
  ue_list ues;

  /// Queue of UEs marked for later removal.
  ring_buffer<du_ue_index_t> ues_to_rem{MAX_NOF_DU_UES};
};

} // namespace srsran
