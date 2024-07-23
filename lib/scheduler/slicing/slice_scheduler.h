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

#include "../policy/scheduler_policy.h"
#include "ran_slice_candidate.h"
#include "ran_slice_instance.h"
#include <queue>

namespace srsran {

/// Inter-slice Scheduler.
class slice_scheduler
{
  using priority_type                      = uint32_t;
  using slot_count_type                    = uint32_t;
  constexpr static priority_type skip_prio = 0;

public:
  slice_scheduler(const cell_configuration& cell_cfg_, const ue_repository& ues_);

  /// Reset the state of the slices.
  void slot_indication();

  /// Update the state of the slice with the provided UE configs.
  void add_ue(const ue_configuration& ue_cfg);
  void reconf_ue(const ue_configuration& next_ue_cfg, const ue_configuration& prev_ue_cfg);
  void rem_ue(du_ue_index_t ue_idx);

  /// Get next RAN slice for PDSCH scheduling.
  std::optional<dl_ran_slice_candidate> get_next_dl_candidate();

  /// Get next RAN slice for PUSCH scheduling.
  std::optional<ul_ran_slice_candidate> get_next_ul_candidate();

  size_t                         nof_slices() const { return slices.size(); }
  const slice_rrm_policy_config& slice_config(ran_slice_id_t id) const { return slices[id.value()].inst.cfg; }
  scheduler_policy&              get_policy(ran_slice_id_t id) { return *slices[id.value()].policy; }

private:
  /// Class responsible for tracking the scheduling context of each RAN slice instance.
  struct ran_slice_sched_context {
    ran_slice_instance                inst;
    std::unique_ptr<scheduler_policy> policy;
    // Counter tracking the last time this slice was scheduled as a candidate.
    slot_count_type last_dl_slot = 0;
    slot_count_type last_ul_slot = 0;

    ran_slice_sched_context(ran_slice_id_t id, const cell_configuration& cell_cfg, const slice_rrm_policy_config& cfg) :
      inst(id, cell_cfg, cfg)
    {
    }

    /// Determines the slice candidate priority.
    priority_type get_prio(bool is_dl, slot_count_type current_slot_count, bool slice_resched) const;
  };

  struct slice_candidate_context {
    ran_slice_id_t     id;
    priority_type      prio;
    interval<unsigned> rb_lims;

    slice_candidate_context(ran_slice_id_t id_, priority_type prio_, interval<unsigned> rb_lims_) :
      id(id_), prio(prio_), rb_lims(rb_lims_)
    {
    }

    /// Compares priorities between two slice contexts.
    bool operator<(const slice_candidate_context& rhs) const { return prio < rhs.prio; }
    bool operator>(const slice_candidate_context& rhs) const { return prio > rhs.prio; }
  };

  // Note: the std::priority_queue makes its underlying container protected, so it seems that they are ok with
  // inheritance.
  class slice_prio_queue : public std::priority_queue<slice_candidate_context>
  {
  public:
    // Note: faster than while(!empty()) pop() because it avoids the O(NlogN). Faster than = {}, because it preserves
    // memory.
    void clear()
    {
      // Access to underlying vector.
      this->c.clear();
    }

    // Adapter of the priority_queue push method to avoid adding candidates with skip priority level.
    void push(const slice_candidate_context& elem)
    {
      if (elem.prio == skip_prio) {
        return;
      }
      std::priority_queue<slice_candidate_context>::push(elem);
    }
  };

  ran_slice_instance& get_slice(const rrm_policy_member& rrm);

  template <bool IsDownlink>
  std::optional<std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>> get_next_candidate();

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  const ue_repository& ues;

  std::vector<ran_slice_sched_context> slices;

  // Queue of slice candidates sorted by priority.
  slice_prio_queue dl_prio_queue;
  slice_prio_queue ul_prio_queue;

  // Count that gets incremented with every new slot. Useful for time round-robin of slices with the same priority.
  // Note: This unsigned value will wrap-around.
  slot_count_type slot_count = 0;
};

} // namespace srsran
