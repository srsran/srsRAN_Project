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

#include "../../scheduler/cell/resource_grid.h"
#include "../config/cell_configuration.h"
#include "../ue_scheduling/ue_repository.h"
#include "ran_slice_id.h"
#include "slice_ue_repository.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"

namespace srsran {

/// This class stores all the internal information relative to a RAN slice instantiation.
class ran_slice_instance
{
  // Const for the max difference in number of slots between the current slot and the last allocation slot.
  // This limit ensures lack of ambiguity in the slot point comparison
  static constexpr unsigned MAX_SLOTS_SINCE_LAST_PXSCH = 256;

public:
  ran_slice_instance(ran_slice_id_t id_, const cell_configuration& cell_cfg_, const slice_rrm_policy_config& cfg_);

  void slot_indication(slot_point slot_tx);

  /// \brief Handle skipped slot indication.
  void skipped_slot_indication(slot_point prev_slot, slot_point current_slot);

  bool active() const { return not slice_ues.empty(); }

  /// Save PDSCH grant.
  void store_pdsch_grant(unsigned crbs, slot_point pdsch_slot)
  {
    pdsch_rb_count += crbs;
    last_pdsch_alloc_slot = pdsch_slot;
  }

  /// Save PUSCH grant.
  void store_pusch_grant(unsigned crbs, slot_point pusch_slot)
  {
    pusch_rb_count_per_slot[pusch_slot.to_uint() % pusch_rb_count_per_slot.size()] += crbs;
    last_pusch_alloc_slot = pusch_slot;
  }

  /// Determine if at least one bearer of the given UE is currently managed by this slice.
  bool contains(du_ue_index_t ue_idx) const
  {
    return slice_ues.contains(ue_idx) and slice_ues[ue_idx].has_bearers_in_slice();
  }

  /// Determine if a (UE, LCID) tuple are managed by this slice.
  bool contains(du_ue_index_t ue_idx, lcid_t lcid) const
  {
    return contains(ue_idx) and slice_ues[ue_idx].contains(lcid);
  }

  /// Add a new UE to list of UEs (if not exists) and a new (UE, LCID) to the list of bearers managed by this slice.
  void add_logical_channel(const ue& u, lcid_t lcid, lcg_id_t lcg_id);

  /// Remove a (UE, LCID) from the list of bearers managed by this slice.
  /// \remark UE is removed if all LCIDs of a UE are removed.
  void rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid);

  /// Remove UE all associated LCIDs.
  void rem_ue(du_ue_index_t ue_idx);

  /// Returns UEs belonging to this slice.
  const slice_ue_repository& get_ues();

  unsigned nof_pusch_rbs_allocated(slot_point pusch_slot) const
  {
    return pusch_rb_count_per_slot[pusch_slot.to_uint() % pusch_rb_count_per_slot.size()];
  }

  /// Number of slots elapsed between the provided PDSCH slot and the last time this slice was scheduled.
  unsigned nof_slots_since_last_pdsch(slot_point pdsch_slot) const
  {
    return last_pdsch_alloc_slot.valid()
               ? (pdsch_slot >= last_pdsch_alloc_slot ? pdsch_slot - last_pdsch_alloc_slot : 0)
               : MAX_SLOTS_SINCE_LAST_PXSCH;
  }
  unsigned nof_slots_since_last_pusch(slot_point pusch_slot) const
  {
    return last_pusch_alloc_slot.valid()
               ? (pusch_slot >= last_pusch_alloc_slot ? pusch_slot - last_pusch_alloc_slot : 0)
               : MAX_SLOTS_SINCE_LAST_PXSCH;
  }

  ran_slice_id_t            id;
  const cell_configuration* cell_cfg;
  slice_rrm_policy_config   cfg;

  /// Counter of how many RBs have been scheduled for PDSCH in the current slot for this slice.
  unsigned pdsch_rb_count = 0;
  /// Ring of counters of how many RBs have been scheduled for PUSCH in a particular slot for this slice.
  std::vector<unsigned> pusch_rb_count_per_slot;

  /// Nof. of previous slots in which RB count needs to be cleared in \c pusch_rb_count_per_slot upon receiving slot
  /// indication.
  /// \remark This is needed since slot_indication() is called only over DL slots.
  unsigned nof_slots_to_clear =
      cell_cfg->tdd_cfg_common.has_value() ? nof_slots_per_tdd_period(*cell_cfg->tdd_cfg_common) : 1;

private:
  // Last slot that the PDSCH was allocated.
  slot_point last_pdsch_alloc_slot;

  // Last slot that the PUSCH was allocated.
  slot_point last_pusch_alloc_slot;

  slice_ue_repository slice_ues;
};

} // namespace srsran
