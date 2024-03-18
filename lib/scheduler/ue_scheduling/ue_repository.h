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

#pragma once

#include "../config/sched_config_manager.h"
#include "ue.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/adt/unique_function.h"

namespace srsran {

/// Container that stores all scheduler UEs.
class ue_repository
{
  using ue_list = slotted_id_table<du_ue_index_t, std::unique_ptr<ue>, MAX_NOF_DU_UES>;

public:
  using value_type     = ue_list::value_type;
  using iterator       = ue_list::iterator;
  using const_iterator = ue_list::const_iterator;

  explicit ue_repository();

  /// \brief Mark start of new slot and update UEs states.
  void slot_indication(slot_point sl_tx);

  /// \brief Contains ue index.
  bool contains(du_ue_index_t ue_index) const { return ues.contains(ue_index); }

  ue&       operator[](du_ue_index_t ue_index) { return *ues[ue_index]; }
  const ue& operator[](du_ue_index_t ue_index) const { return *ues[ue_index]; }

  /// \brief Search UE context based on TC-RNTI/C-RNTI.
  ue*       find_by_rnti(rnti_t rnti);
  const ue* find_by_rnti(rnti_t rnti) const;

  /// \brief Add new UE in the UE repository.
  void add_ue(std::unique_ptr<ue> u);

  /// \brief Initiate removal of existing UE from the repository.
  void schedule_ue_rem(ue_config_delete_event ev);

  ue*       find(du_ue_index_t ue_index) { return ues.contains(ue_index) ? ues[ue_index].get() : nullptr; }
  const ue* find(du_ue_index_t ue_index) const { return ues.contains(ue_index) ? ues[ue_index].get() : nullptr; }

  size_t size() const { return ues.size(); }

  bool empty() const { return ues.empty(); }

  iterator       begin() { return ues.begin(); }
  iterator       end() { return ues.end(); }
  const_iterator begin() const { return ues.begin(); }
  const_iterator end() const { return ues.end(); }

  const_iterator lower_bound(du_ue_index_t ue_index) const { return ues.lower_bound(ue_index); }

private:
  srslog::basic_logger& logger;

  // Repository of UEs.
  ue_list ues;

  // Mapping of RNTIs to UE indexes.
  std::vector<std::pair<rnti_t, du_ue_index_t>> rnti_to_ue_index_lookup;

  // Queue of UEs marked for later removal. For each UE, we store the slot after which its removal can be safely
  // carried out, and the original UE removal command.
  ring_buffer<std::pair<slot_point, ue_config_delete_event>> ues_to_rem{MAX_NOF_DU_UES};

  // Last slot indication.
  slot_point last_sl_tx;
};

} // namespace srsran
