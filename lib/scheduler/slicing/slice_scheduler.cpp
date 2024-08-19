/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "slice_scheduler.h"
#include "../policy/scheduler_policy_factory.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

slice_scheduler::slice_scheduler(const cell_configuration& cell_cfg_, const ue_repository& ues_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED")), ues(ues_)
{
  // Create a number of slices equal to the number of configured RRM Policy members + 1 (default SRB slice) + 1 (default
  // DRB slice).
  slices.reserve(cell_cfg.rrm_policy_members.size() + 2);

  // Create RAN slice instances.
  // Default SRB slice.
  // NOTE: We set \c min_prb for default SRB slice to maximum nof. PRBs of a UE carrier to give maximum priority to this
  // slice.
  slices.emplace_back(default_srb_ran_slice_id, cell_cfg, slice_rrm_policy_config{.min_prb = MAX_NOF_PRBS});
  slices.back().policy = create_scheduler_strategy(cell_cfg.expert_cfg.ue);
  // Default DRB slice.
  slices.emplace_back(default_drb_ran_slice_id, cell_cfg, slice_rrm_policy_config{});
  slices.back().policy = create_scheduler_strategy(cell_cfg.expert_cfg.ue);
  // NOTE: RAN slice IDs 0 and 1 are reserved for default SRB and default DRB slice respectively.
  ran_slice_id_t id_count{2};
  // Configured RRM policy members.
  for (const slice_rrm_policy_config& rrm : cell_cfg.rrm_policy_members) {
    slices.emplace_back(id_count, cell_cfg, rrm);
    // Set policy scheduler based on slice configuration.
    scheduler_ue_expert_config slice_scheduler_ue_expert_cfg{cell_cfg.expert_cfg.ue};
    slice_scheduler_ue_expert_cfg.strategy_cfg = rrm.policy_sched_cfg;
    slices.back().policy                       = create_scheduler_strategy(slice_scheduler_ue_expert_cfg);
    ++id_count;
  }
}

void slice_scheduler::slot_indication()
{
  slot_count++;

  // Update the context of each slice.
  for (auto& slice : slices) {
    slice.inst.slot_indication();
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
  if (not ues.contains(ue_cfg.ue_index)) {
    // UE should be added to the repository at this stage.
    logger.warning("ue={}: Not adding UE to slice scheduler. Cause: No UE context found", ue_cfg.ue_index);
    return;
  }

  // Add UE and new bearers.
  // [Implementation-defined] UE does not have complete configuration. Hence, UE won't be added to any slice.
  if (not ue_cfg.is_ue_cfg_complete()) {
    return;
  }
  auto& u = ues[ue_cfg.ue_index];

  for (const logical_channel_config& lc_cfg : ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg);
    sl_inst.add_logical_channel(u, lc_cfg.lcid, lc_cfg.lc_group);
  }
}

void slice_scheduler::reconf_ue(const ue_configuration& ue_cfg)
{
  // Remove UE and previously associated bearers from all slices.
  rem_ue(ue_cfg.ue_index);

  // Re-add UE but with new bearers.
  add_ue(ue_cfg);
}

void slice_scheduler::rem_ue(du_ue_index_t ue_idx)
{
  for (auto& slice : slices) {
    // Remove all logical channels of UE.
    slice.inst.rem_ue(ue_idx);
  }
}

ran_slice_instance& slice_scheduler::get_slice(const logical_channel_config& lc_cfg)
{
  // Return default SRB slice if LCID belongs to a SRB.
  if (lc_cfg.lcid < LCID_MIN_DRB) {
    return slices[default_srb_ran_slice_id.value()].inst;
  }
  auto it = std::find_if(slices.begin(), slices.end(), [&lc_cfg](const ran_slice_sched_context& slice) {
    return slice.inst.cfg.rrc_member == lc_cfg.rrm_policy;
  });
  if (it == slices.end() or lc_cfg.rrm_policy == rrm_policy_member{}) {
    // Slice with the provided RRM policy member was not found. Return default DRB slice.
    return slices[default_drb_ran_slice_id.value()].inst;
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
