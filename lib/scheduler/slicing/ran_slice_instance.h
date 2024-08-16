/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  bool active() const { return not slice_ues.empty(); }

  /// Save PDSCH grant.
  void store_pdsch_grant(unsigned crbs) { pdsch_rb_count += crbs; }

  /// Save PUSCH grant.
  void store_pusch_grant(unsigned crbs) { pusch_rb_count += crbs; }

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

  ran_slice_id_t            id;
  const cell_configuration* cell_cfg;
  slice_rrm_policy_config   cfg;

  /// Counter of how many RBs have been scheduled for PDSCH in the current slot for this slice.
  unsigned pdsch_rb_count = 0;
  /// Counter of how many RBs have been scheduled for PUSCH in the current slot for this slice.
  unsigned pusch_rb_count = 0;

private:
  slice_ue_repository slice_ues;
};

} // namespace srsran
