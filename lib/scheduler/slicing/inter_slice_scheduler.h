/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
class inter_slice_scheduler
{
  using priority_type                      = uint32_t;
  using slot_count_type                    = uint32_t;
  static constexpr priority_type skip_prio = 0;

public:
  inter_slice_scheduler(const cell_configuration& cell_cfg_, ue_repository& ues_);

  /// Reset the state of the slices.
  void slot_indication(slot_point slot_tx, const cell_resource_allocator& res_grid);

  /// Update the state of the slice with the provided UE configs.
  void add_ue(du_ue_index_t ue_idx);
  void reconf_ue(du_ue_index_t ue_idx);
  void rem_ue(du_ue_index_t ue_idx);
  void config_applied(du_ue_index_t ue_idx);

  /// Get next RAN slice for PDSCH scheduling.
  std::optional<dl_ran_slice_candidate> get_next_dl_candidate();

  /// Get next RAN slice for PUSCH scheduling.
  std::optional<ul_ran_slice_candidate> get_next_ul_candidate();

  size_t                         nof_slices() const { return slices.size(); }
  const slice_rrm_policy_config& slice_config(ran_slice_id_t id) const { return slices[id.value()].inst.cfg; }
  scheduler_policy&              get_policy(ran_slice_id_t id) { return *slices[id.value()].policy; }

  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req);

private:
  /// Class responsible for tracking the scheduling context of each RAN slice instance.
  struct ran_slice_sched_context {
    ran_slice_instance                inst;
    std::unique_ptr<scheduler_policy> policy;

    ran_slice_sched_context(ran_slice_id_t                    id,
                            const cell_configuration&         cell_cfg,
                            const slice_rrm_policy_config&    cfg,
                            std::unique_ptr<scheduler_policy> policy_) :
      inst(id, cell_cfg, cfg), policy(std::move(policy_))
    {
    }

    /// Determines the slice candidate priority.
    priority_type get_prio(bool is_dl, slot_point pdcch_slot, slot_point pxsch_slot, bool slice_resched) const;
  };

  struct slice_candidate_context {
    ran_slice_id_t     id;
    priority_type      prio;
    interval<unsigned> rb_lims;
    /// Slot at which PUSCH/PDSCH needs to be scheduled for this slice candidate.
    slot_point slot_tx;

    slice_candidate_context(ran_slice_id_t id_, priority_type prio_, interval<unsigned> rb_lims_, slot_point slot_tx_) :
      id(id_), prio(prio_), rb_lims(rb_lims_), slot_tx(slot_tx_)
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

    void reserve(size_t capacity) { c.reserve(capacity); }

    // Adapter of the priority_queue push method to avoid adding candidates with skip priority level.
    void push(const slice_candidate_context& elem)
    {
      if (elem.prio == skip_prio) {
        return;
      }
      std::priority_queue<slice_candidate_context>::push(elem);
    }
  };

  ran_slice_instance& get_slice(const logical_channel_config& lc_cfg);

  // Fetch UE if it is in a state to be added/reconfigured.
  ue* fetch_ue_to_update(du_ue_index_t ue_idx);

  void add_impl(ue& u);

  template <bool IsDownlink>
  std::optional<std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>> get_next_candidate();

  const cell_configuration& cell_cfg;
  srslog::basic_logger&     logger;

  // Represents current slot in the scheduler. This is updated on each slot indication.
  slot_point current_slot;

  ue_repository& ues;

  /// Vector circularly indexed by slot with the list of applicable PUSCH time domain resources per slot.
  /// NOTE: The list would be empty for UL slots.
  std::vector<static_vector<unsigned, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> valid_pusch_td_list_per_slot;

  std::vector<ran_slice_sched_context> slices;

  // Queue of slice candidates sorted by priority.
  slice_prio_queue dl_prio_queue;
  slice_prio_queue ul_prio_queue;
};

} // namespace srsran
