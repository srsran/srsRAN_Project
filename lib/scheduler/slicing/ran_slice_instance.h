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
#include "../policy/scheduler_policy.h"
#include "ran_slice_id.h"
#include "srsran/scheduler/config/slice_rrm_policy_config.h"

namespace srsran {

/// This class stores all the internal information relative to a RAN slice instantiation.
class ran_slice_instance
{
public:
  constexpr static int skip_slice_prio = std::numeric_limits<int>::min();

  ran_slice_instance(ran_slice_id_t id_, const cell_configuration& cell_cfg_, const slice_rrm_policy_config& cfg_);

  void slot_indication();

  bool active() const { return not bearers.empty(); }

  int get_dl_prio()
  {
    if (not active() or pdsch_stopped or cfg.max_prb <= pdsch_rb_count) {
      return skip_slice_prio;
    }
    return cfg.min_prb > pdsch_rb_count ? cfg.min_prb - pdsch_rb_count : 0;
  }

  int get_ul_prio()
  {
    if (not active() or pusch_stopped or cfg.max_prb <= pusch_rb_count) {
      return skip_slice_prio;
    }
    return cfg.min_prb > pusch_rb_count ? cfg.min_prb - pusch_rb_count : 0;
  }

  /// Save PDSCH grant.
  void store_pdsch_grant(unsigned crbs) { pdsch_rb_count += crbs; }

  /// Save PUSCH grant.
  void store_pusch_grant(unsigned crbs) { pusch_rb_count += crbs; }

  /// Mark the allocation of PDSCH for this slice and the current slot as complete.
  void pdsch_completed() { pdsch_stopped = true; }

  /// Mark the allocation of PUSCH for this slice and the current slot as complete.
  void pusch_completed() { pusch_stopped = true; }

  /// Determine if a UE is a candidate for this slice.
  bool is_candidate(du_ue_index_t ue_idx) const { return bearers.contains(ue_idx); }

  /// Determine if a (UE, LCID) is a candidate for this slice.
  bool is_candidate(du_ue_index_t ue_idx, lcid_t lcid) const
  {
    return is_candidate(ue_idx) and bearers[ue_idx].test(lcid);
  }

  void set_logical_channel(du_ue_index_t ue_idx, lcid_t lcid);
  void rem_logical_channel(du_ue_index_t ue_idx, lcid_t lcid);
  void rem_ue(du_ue_index_t ue_idx);

  ran_slice_id_t            id;
  const cell_configuration* cell_cfg;
  slice_rrm_policy_config   cfg;

  std::unique_ptr<scheduler_policy> policy;

  slotted_id_table<du_ue_index_t, bounded_bitset<MAX_NOF_RB_LCIDS>, MAX_NOF_DU_UES> bearers;

  /// Counter of how many RBs have been scheduled for PDSCH in the current slot for this slice.
  unsigned pdsch_rb_count = 0;
  /// Counter of how many RBs have been scheduled for PUSCH in the current slot for this slice.
  unsigned pusch_rb_count = 0;

private:
  bool pdsch_stopped = false;
  bool pusch_stopped = false;
};

} // namespace srsran