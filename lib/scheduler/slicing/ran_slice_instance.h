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

#include "../config/cell_configuration.h"
#include "../ue_scheduling/ue_repository.h"
#include "ran_slice_id.h"
#include "slice_ue_repository.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"

namespace srsran {

/// This class stores all the internal information relative to a RAN slice instantiation.
class ran_slice_instance
{
public:
  ran_slice_instance(ran_slice_id_t id_, const cell_configuration& cell_cfg_, const slice_rrm_policy_config& cfg_);

  void slot_indication(const ue_repository& cell_ues);

  bool active() const { return not bearers.empty(); }

  /// Save PDSCH grant.
  void store_pdsch_grant(unsigned crbs) { pdsch_rb_count += crbs; }

  /// Save PUSCH grant.
  void store_pusch_grant(unsigned crbs) { pusch_rb_count += crbs; }

  /// Determine if at least one bearer of the given UE is currently managed by this slice.
  bool contains(du_ue_index_t ue_idx) const { return bearers.contains(ue_idx); }

  /// Determine if a (UE, LCID) tuple are managed by this slice.
  bool contains(du_ue_index_t ue_idx, lcid_t lcid) const { return contains(ue_idx) and bearers[ue_idx].test(lcid); }

  /// Add a new UE to list of UEs (if not exists) and a new (UE, LCID) to the list of bearers managed by this slice.
  void add_logical_channel(const ue* u, lcid_t lcid);

  /// Remove a UE and all associated LCIDs or only a (UE, LCID) from the list of bearers managed by this slice.
  /// \remark UE is removed if all LCIDs of a UE are removed.
  void rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid = MAX_NOF_RB_LCIDS);

  /// Returns UEs belonging to this slice.
  const slice_ue_repository& get_ues();

  ran_slice_id_t            id;
  const cell_configuration* cell_cfg;
  slice_rrm_policy_config   cfg;

  slotted_id_table<du_ue_index_t, bounded_bitset<MAX_NOF_RB_LCIDS>, MAX_NOF_DU_UES> bearers;

  /// Counter of how many RBs have been scheduled for PDSCH in the current slot for this slice.
  unsigned pdsch_rb_count = 0;
  /// Counter of how many RBs have been scheduled for PUSCH in the current slot for this slice.
  unsigned pusch_rb_count = 0;

private:
  slice_ue_repository                          slice_ues;
  static_vector<du_ue_index_t, MAX_NOF_DU_UES> slice_ues_to_rem;
};

} // namespace srsran
