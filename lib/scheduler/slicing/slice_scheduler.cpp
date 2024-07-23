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

#include "slice_scheduler.h"
#include "../policy/scheduler_policy_factory.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

slice_scheduler::slice_scheduler(const cell_configuration& cell_cfg_, const ue_repository& ues_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED")), ues(ues_)
{
  // Create a number of slices equal to the number of configured RRM Policy members + 1 (default slice).
  slices.reserve(cell_cfg.rrm_policy_members.size() + 1);

  // Create RAN slice instances.
  ran_slice_id_t id_count{0};
  // Default slice.
  slices.emplace_back(id_count, cell_cfg, slice_rrm_policy_config{});
  slices.back().policy = create_scheduler_strategy(cell_cfg.expert_cfg.ue);
  ++id_count;
  // Configured RRM policy members.
  for (const slice_rrm_policy_config& rrm : cell_cfg.rrm_policy_members) {
    slices.emplace_back(id_count, cell_cfg, rrm);
    slices.back().policy = create_scheduler_strategy(cell_cfg.expert_cfg.ue);
    ++id_count;
  }
}

void slice_scheduler::slot_indication()
{
  slot_count++;

  // Update the context of each slice.
  for (auto& slice : slices) {
    slice.inst.slot_indication(ues);
  }

  // Recompute the priority queues.
  dl_prio_queue.clear();
  ul_prio_queue.clear();
  for (const auto& slice : slices) {
    unsigned max_rbs = slice.inst.cfg.min_prb > 0 ? slice.inst.cfg.min_prb : slice.inst.cfg.max_prb;
    dl_prio_queue.push(slice_candidate_context{slice.inst.id, slice.get_prio(true, slot_count, false), {0, max_rbs}});
    ul_prio_queue.push(slice_candidate_context{slice.inst.id, slice.get_prio(false, slot_count, false), {0, max_rbs}});
  }
}

void slice_scheduler::add_ue(const ue_configuration& ue_cfg)
{
  for (const logical_channel_config& lc_cfg : ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    if (ues.contains(ue_cfg.ue_index)) {
      sl_inst.add_logical_channel(&ues[ue_cfg.ue_index], lc_cfg.lcid);
    }
  }
}

void slice_scheduler::reconf_ue(const ue_configuration& next_ue_cfg, const ue_configuration& prev_ue_cfg)
{
  // Remove old bearers.
  for (const logical_channel_config& lc_cfg : prev_ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    sl_inst.rem_logical_channel(prev_ue_cfg.ue_index, lc_cfg.lcid);
  }

  // Add new bearers.
  for (const logical_channel_config& lc_cfg : next_ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    if (ues.contains(next_ue_cfg.ue_index)) {
      sl_inst.add_logical_channel(&ues[next_ue_cfg.ue_index], lc_cfg.lcid);
    }
  }
}

void slice_scheduler::rem_ue(du_ue_index_t ue_idx)
{
  for (auto& slice : slices) {
    slice.inst.rem_logical_channel(ue_idx);
  }
}

ran_slice_instance& slice_scheduler::get_slice(const rrm_policy_member& rrm)
{
  auto it = std::find_if(slices.begin(), slices.end(), [&rrm](const ran_slice_sched_context& slice) {
    return slice.inst.cfg.rrc_member == rrm;
  });
  if (it == slices.end()) {
    // Slice with the provided RRM policy member was not found. Return default slice.
    return slices.front().inst;
  }
  return it->inst;
}

template <bool IsDownlink>
std::optional<std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>>
slice_scheduler::get_next_candidate()
{
  slice_prio_queue& prio_queue = IsDownlink ? dl_prio_queue : ul_prio_queue;
  while (not prio_queue.empty()) {
    ran_slice_sched_context& chosen_slice = slices[prio_queue.top().id.value()];
    interval<unsigned>       rb_lims      = prio_queue.top().rb_lims;
    prio_queue.pop();

    unsigned rb_count = IsDownlink ? chosen_slice.inst.pdsch_rb_count : chosen_slice.inst.pusch_rb_count;
    if (not rb_lims.contains(rb_count)) {
      // The slice has been scheduled in this slot with a number of RBs that is not within the limits for this
      // candidate. This could happen, for instance, if the scheduler could not schedule all RBs of a candidate
      // bounded between {0, minRB}. In this case, the second candidate for the same slice with bounds {minRB, maxRB}
      // is skipped.
      continue;
    }

    const slice_rrm_policy_config& cfg = chosen_slice.inst.cfg;
    if (cfg.min_prb != cfg.max_prb and rb_lims.stop() == cfg.min_prb) {
      // For the special case when minRB ratio>0, the first candidate for this slice was bounded between {0, minRB}.
      // We re-add the slice as a candidate, this time, with RB bounds {minRB, maxRB}.
      priority_type prio = chosen_slice.get_prio(true, slot_count, true);
      prio_queue.push(slice_candidate_context{chosen_slice.inst.id, prio, {cfg.min_prb, cfg.max_prb}});
    }

    // Save current slot count.
    unsigned& count_to_set = IsDownlink ? chosen_slice.last_dl_slot : chosen_slice.last_ul_slot;
    count_to_set           = slot_count;

    // Return the candidate.
    return std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>{chosen_slice.inst,
                                                                                          rb_lims.stop()};
  }
  return std::nullopt;
}

std::optional<dl_ran_slice_candidate> slice_scheduler::get_next_dl_candidate()
{
  return get_next_candidate<true>();
}

std::optional<ul_ran_slice_candidate> slice_scheduler::get_next_ul_candidate()
{
  return get_next_candidate<false>();
}

slice_scheduler::priority_type slice_scheduler::ran_slice_sched_context::get_prio(bool            is_dl,
                                                                                  slot_count_type current_slot_count,
                                                                                  bool            slice_resched) const
{
  // Note: The positive integer representing the priority of a slice consists of a concatenation of three priority
  // values:
  // 1. slice traffic priority (16 most significant bits). It differentiates slices based on whether they have
  // minimum required traffic agreements (e.g. minRB ratio).
  // 2. delay priority (8 bits), which attributes the highest priority to slices that have not been scheduled for a
  // long time.
  // 3. round-robin based on slot indication count (8 least significant bits).

  // Priority when slice has already reached its minimum RB ratio agreement.
  constexpr static priority_type default_prio = 0x1U;
  // Priority when slice still needs to reach its minimum RB ratio agreement.
  constexpr static priority_type high_prio     = 0x2U;
  constexpr static priority_type delay_bitsize = 8U;
  constexpr static priority_type rr_bitsize    = 8U;

  unsigned rb_count = is_dl ? inst.pdsch_rb_count : inst.pusch_rb_count;
  if (not inst.active() or rb_count >= inst.cfg.max_prb) {
    // If the slice is not in a state to be scheduled in this slot, return skip priority level.
    return skip_prio;
  }

  // In case minRB > 0 and this is the first time the slice is proposed as a candidate, we give it a higher priority.
  priority_type slice_prio = inst.cfg.min_prb > 0 and not slice_resched ? high_prio : default_prio;

  // Increase priorities of slices that have not been scheduled for a long time.
  unsigned      last_count = is_dl ? last_dl_slot : last_ul_slot;
  priority_type delay_prio = (current_slot_count - last_count) & ((1U << delay_bitsize) - 1U);

  // Round-robin across slices with the same slice and delay priorities.
  priority_type rr_prio = (inst.id.value() % current_slot_count) & ((1U << rr_bitsize) - 1U);

  return (slice_prio << (delay_bitsize + rr_bitsize)) + (delay_prio << rr_bitsize) + rr_prio;
}
