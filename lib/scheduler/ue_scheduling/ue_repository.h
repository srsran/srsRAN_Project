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

  /// \brief Mark start of new slot and update UEs states.
  void slot_indication(slot_point sl_tx);

  /// \brief Contains ue index.
  bool contains(du_ue_index_t ue_index) const { return ues.contains(ue_index); }

  ue&       operator[](du_ue_index_t ue_index) { return ues[ue_index]; }
  const ue& operator[](du_ue_index_t ue_index) const { return ues[ue_index]; }

  /// \brief Add new UE in the UE repository.
  void add_ue(std::unique_ptr<ue> u);

  /// \brief Initiate removal of existing UE from the repository.
  void schedule_ue_rem(du_ue_index_t ue_index);

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
