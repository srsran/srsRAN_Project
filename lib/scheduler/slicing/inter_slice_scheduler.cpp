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

#include "inter_slice_scheduler.h"
#include "../policy/scheduler_policy_factory.h"
#include "../support/pusch/pusch_td_resource_indices.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

inter_slice_scheduler::inter_slice_scheduler(const cell_configuration& cell_cfg_, ue_repository& ues_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED")), ues(ues_)
{
  // Create a number of slices equal to the number of configured RRM Policy members + 1 (default SRB slice) + 1 (default
  // DRB slice).
  unsigned capacity = cell_cfg.rrm_policy_members.size() + 2;
  dl_prio_queue.reserve(capacity);
  // We use a larger capacity for the UL priority queue to account for the maximum number of PUSCH TD resources.
  ul_prio_queue.reserve(capacity * pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS);
  slices.reserve(capacity);

  // NOTE: We assume nof. CRBs in a cell for both DL and UL are same.
  const unsigned cell_max_rbs = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();

  // Create RAN slice instances.
  // Default SRB slice.
  // NOTE: We set \c min_prb for default SRB slice to maximum nof. PRBs of a UE carrier to give maximum priority to this
  // slice.
  slices.emplace_back(
      SRB_RAN_SLICE_ID,
      cell_cfg,
      slice_rrm_policy_config{.rbs = {cell_max_rbs, cell_max_rbs}, .priority = slice_rrm_policy_config::max_priority},
      create_scheduler_strategy(cell_cfg.expert_cfg.ue, cell_cfg.cell_index));
  // Default DRB slice.
  slices.emplace_back(DEFAULT_DRB_RAN_SLICE_ID,
                      cell_cfg,
                      slice_rrm_policy_config{.rbs = {0, cell_max_rbs}},
                      create_scheduler_strategy(cell_cfg.expert_cfg.ue, cell_cfg.cell_index));
  // NOTE: RAN slice IDs 0 and 1 are reserved for default SRB and default DRB slice respectively.
  ran_slice_id_t id_count{2};
  // Configured RRM policy members.
  for (const slice_rrm_policy_config& rrm : cell_cfg.rrm_policy_members) {
    auto rrm_adjusted = rrm;
    // Adjust maximum PRBs per slice based on the number of PRBs in a cell.
    rrm_adjusted.rbs      = {std::min(rrm_adjusted.rbs.dedicated(), cell_max_rbs),
                             std::min(rrm_adjusted.rbs.min(), cell_max_rbs),
                             std::min(rrm_adjusted.rbs.max(), cell_max_rbs)};
    rrm_adjusted.priority = std::min(rrm.priority, slice_rrm_policy_config::max_priority);
    // Set policy scheduler based on slice configuration.
    scheduler_ue_expert_config slice_scheduler_ue_expert_cfg{cell_cfg.expert_cfg.ue};
    slice_scheduler_ue_expert_cfg.strategy_cfg = rrm.policy_sched_cfg;
    // Create custom RAN slice.
    slices.emplace_back(id_count,
                        cell_cfg,
                        rrm_adjusted,
                        create_scheduler_strategy(slice_scheduler_ue_expert_cfg, cell_cfg.cell_index));
    ++id_count;
  }

  // Generate the slot ring context.
  auto pusch_list = get_fairly_distributed_pusch_td_resource_indices(cell_cfg);
  slot_ring.resize(pusch_list.size());
  for (unsigned i = 0, sz = pusch_list.size(); i != sz; ++i) {
    slot_ring[i].valid_pusch_td_list.assign(pusch_list[i].begin(), pusch_list[i].end());
    slot_ring[i].count_dl_ded_rbs = 0;
    slot_ring[i].count_ul_ded_rbs = 0;
  }
}

void inter_slice_scheduler::slot_indication(slot_point slot_tx, const cell_resource_allocator& res_grid)
{
  // Update the context of each slice.
  if (not current_slot.valid()) {
    // First call.
    current_slot = slot_tx - 1;
  }
  while (current_slot != slot_tx) {
    ++current_slot;
    for (auto& slice : slices) {
      slice.inst.slot_indication(current_slot);
    }

    // Reset remaining dedicated RBs for the last slot.
    auto& slot_ctx            = slot_ring[(current_slot - 1).count() % slot_ring.size()];
    slot_ctx.count_dl_ded_rbs = 0;
    slot_ctx.count_ul_ded_rbs = 0;
  }

  // Clear the priority queues.
  dl_prio_queue.clear();
  ul_prio_queue.clear();

  if (not cell_cfg.is_dl_enabled(slot_tx)) {
    // If PDCCH is inactive in this slot, we don't generate any slice candidates.
    return;
  }

  // Recompute the priority queues.
  const bool pdsch_enabled =
      cell_cfg.expert_cfg.ue.enable_csi_rs_pdsch_multiplexing or res_grid[0].result.dl.csi_rs.empty();
  for (const auto& slice : slices) {
    if (not pdsch_enabled or slice.inst.pdsch_rb_count >= slice.inst.cfg.rbs.max()) {
      // PDSCH is disabled or slice already reached max RBs. We can skip this slice.
      continue;
    }

    // When minRB > 0, minRB != maxRB, sliceRBs < min_RB, we create two candidates. One with limit set to minRB
    // with high priority, and another one with limit set to maxRB with normal priority.
    interval<unsigned> rb_lims;
    if (slice.inst.pdsch_rb_count < slice.inst.cfg.rbs.min() and slice.inst.cfg.rbs.min() > 0 and
        slice.inst.cfg.rbs.shared() > 0) {
      rb_lims         = {slice.inst.pdsch_rb_count, slice.inst.cfg.rbs.min()};
      const auto prio = slice.get_prio(true, slot_tx, slot_tx, rb_lims.stop());
      dl_prio_queue.push(slice_candidate_context{slice.inst.id, prio, rb_lims, slot_tx});
      rb_lims = {slice.inst.cfg.rbs.min(), slice.inst.cfg.rbs.max()};
    } else {
      rb_lims = {slice.inst.pdsch_rb_count, slice.inst.cfg.rbs.max()};
    }
    const auto prio = slice.get_prio(true, slot_tx, slot_tx, rb_lims.stop());
    dl_prio_queue.push(slice_candidate_context{slice.inst.id, prio, rb_lims, slot_tx});
  }
  dl_prio_queue.sort();

  // TODO: Revisit when PUSCH time domain resource list is also defined in UE dedicated configuration.
  span<const pusch_time_domain_resource_allocation> pusch_time_domain_list =
      cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.value().pusch_td_alloc_list;
  for (const auto& slice : slices) {
    for (const unsigned pusch_td_res_idx : slot_ring[slot_tx.count() % slot_ring.size()].valid_pusch_td_list) {
      unsigned pusch_delay = pusch_time_domain_list[pusch_td_res_idx].k2 + cell_cfg.ntn_cs_koffset;
      const cell_slot_resource_allocator& pusch_alloc = res_grid[pusch_delay];
      slot_point                          pusch_slot  = slot_tx + pusch_delay;

      unsigned pusch_rb_count = slice.inst.nof_pusch_rbs_allocated(pusch_slot);
      if (pusch_rb_count >= slice.inst.cfg.rbs.max()) {
        // Slice reached max RBs.
        continue;
      }

      const crb_bitmap pusch_used_crbs =
          pusch_alloc.ul_res_grid.used_crbs(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs,
                                            cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs,
                                            pusch_time_domain_list[pusch_td_res_idx].symbols);
      if (pusch_used_crbs.all()) {
        // No more RBs left to allocated so skip adding slice candidate.
        continue;
      }

      // When minRB > 0, minRB != maxRB, sliceRBs < min_RB, we create two candidates. One with limit set to minRB
      // with high priority, and another one with limit set to maxRB with normal priority.
      interval<unsigned> rb_lims;
      if (slice.inst.cfg.rbs.min() > 0 and slice.inst.cfg.rbs.shared() > 0 and
          pusch_rb_count < slice.inst.cfg.rbs.min()) {
        rb_lims         = {pusch_rb_count, slice.inst.cfg.rbs.min()};
        const auto prio = slice.get_prio(false, slot_tx, pusch_slot, rb_lims.stop());
        ul_prio_queue.push(slice_candidate_context{slice.inst.id, prio, rb_lims, pusch_slot});
        rb_lims = {slice.inst.cfg.rbs.min(), slice.inst.cfg.rbs.max()};
      } else {
        rb_lims = {pusch_rb_count, slice.inst.cfg.rbs.max()};
      }
      const auto prio = slice.get_prio(false, slot_tx, pusch_slot, rb_lims.stop());
      ul_prio_queue.push(slice_candidate_context{slice.inst.id, prio, rb_lims, pusch_slot});
    }
  }
  ul_prio_queue.sort();
}

void inter_slice_scheduler::add_ue(du_ue_index_t ue_idx)
{
  ue* u = fetch_ue_to_update(ue_idx);
  if (u == nullptr) {
    return;
  }

  // Add UE and new bearers.
  add_impl(*u);
}

void inter_slice_scheduler::reconf_ue(du_ue_index_t ue_idx)
{
  // When the UE is reconfigured, it enters fallback mode and only leaves it when config_applied is called.
  // For this reason, we remove the UE from its slices.
  rem_ue(ue_idx);
}

void inter_slice_scheduler::rem_ue(du_ue_index_t ue_idx)
{
  // Remove all logical channels of UE.
  // Note: We take the conservative approach of traversing all slices, because the current UE config might not match
  // the UE repositories inside each slice instance (e.g. in case of fallback or during reconfig).
  for (auto& slice : slices) {
    if (slice.inst.get_ues().contains(ue_idx)) {
      get_policy(slice.inst.id).rem_ue(ue_idx);
    }
    slice.inst.get_ues().rem_ue(ue_idx);
  }
}

void inter_slice_scheduler::config_applied(du_ue_index_t ue_idx)
{
  ue* u = fetch_ue_to_update(ue_idx);
  if (u == nullptr) {
    logger.error("Config applied to a UE that is inactive or in fallback");
    return;
  }

  // Add UE to slices.
  add_impl(*u);
}

void inter_slice_scheduler::add_impl(ue& u)
{
  const ue_configuration& ue_cfg = *u.ue_cfg_dedicated();
  for (logical_channel_config_ptr lc_cfg : *ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(*lc_cfg);
    if (lc_cfg->lcid != LCID_SRB0 and not sl_inst.get_ues().contains(u.ue_index)) {
      get_policy(sl_inst.id).add_ue(u.ue_index);
    }
    sl_inst.get_ues().add_logical_channel(u, lc_cfg->lcid, lc_cfg->lc_group);
  }
}

ue* inter_slice_scheduler::fetch_ue_to_update(du_ue_index_t ue_idx)
{
  if (not ues.contains(ue_idx)) {
    // UE should be added to the repository at this stage.
    logger.warning("ue={}: Not configuring UE to slice scheduler. Cause: No UE context found", fmt::underlying(ue_idx));
    return nullptr;
  }

  auto&                   u      = ues[ue_idx];
  const ue_configuration& ue_cfg = *u.ue_cfg_dedicated();

  // If UE does not have complete configuration, we won't be added to any slice.
  if (not ue_cfg.is_ue_cfg_complete()) {
    return nullptr;
  }

  // If UE is in fallback mode, we do not add it to the slice scheduler.
  const ue_cell* ue_cc = u.find_cell(cell_cfg.cell_index);
  if (ue_cc == nullptr) {
    logger.warning("ue={}: Not adding UE to slice scheduler. Cause: No UE context found in cell {}",
                   fmt::underlying(ue_cfg.ue_index),
                   fmt::underlying(cell_cfg.cell_index));
    return nullptr;
  }
  if (ue_cc->is_in_fallback_mode()) {
    // Do not include yet the UE in the slice scheduler.
    return nullptr;
  }

  return &u;
}

ran_slice_instance& inter_slice_scheduler::get_slice(const logical_channel_config& lc_cfg)
{
  // Return default SRB slice if LCID belongs to a SRB.
  if (lc_cfg.lcid < LCID_MIN_DRB) {
    return slices[SRB_RAN_SLICE_ID.value()].inst;
  }
  auto it = std::find_if(slices.begin(), slices.end(), [&lc_cfg](const ran_slice_sched_context& slice) {
    return slice.inst.cfg.rrc_member == lc_cfg.rrm_policy;
  });
  if (it == slices.end() or lc_cfg.rrm_policy == rrm_policy_member{}) {
    // Slice with the provided RRM policy member was not found. Return default DRB slice.
    return slices[DEFAULT_DRB_RAN_SLICE_ID.value()].inst;
  }
  return it->inst;
}

template <bool IsDownlink>
std::optional<std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>>
inter_slice_scheduler::get_next_candidate()
{
  using candidate_type = std::conditional_t<IsDownlink, dl_ran_slice_candidate, ul_ran_slice_candidate>;

  slice_prio_queue& prio_queue = IsDownlink ? dl_prio_queue : ul_prio_queue;
  while (not prio_queue.empty()) {
    ran_slice_sched_context& chosen_slice = slices[prio_queue.top().id.value()];
    interval<unsigned>       rb_lims      = prio_queue.top().rb_lims;
    slot_point               pxsch_slot   = prio_queue.top().slot_tx;
    prio_queue.pop();

    // Check if two consecutive candidates belong to the same slice. If so, we can combine them into a single candidate.
    if (not prio_queue.empty() and prio_queue.top().id == chosen_slice.inst.id and
        prio_queue.top().slot_tx == pxsch_slot) {
      rb_lims = {std::min(rb_lims.start(), prio_queue.top().rb_lims.start()),
                 std::max(rb_lims.stop(), prio_queue.top().rb_lims.stop())};
      prio_queue.pop();
    }

    // Compute how many RBs can be allocated for this slice scheduling opportunity.
    auto&     ring_pos  = slot_ring[pxsch_slot.count() % slot_ring.size()];
    unsigned& count_rbs = IsDownlink ? ring_pos.count_dl_ded_rbs : ring_pos.count_ul_ded_rbs;
    // Consider the limit imposed by dedicated resources for already scheduled slices.
    const unsigned cell_max_rbs      = IsDownlink ? cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length()
                                                  : cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
    const unsigned rem_rbs           = cell_max_rbs - count_rbs;
    const unsigned max_rbs_candidate = std::min(rb_lims.stop(), rem_rbs);
    if (rb_lims.start() < chosen_slice.inst.cfg.rbs.min()) {
      // If we are still scheduling dedicated+prioritised resources, we update the sum of dedicated RBs allocated for
      // this slot with the dedicated resources of this slice candidate.
      count_rbs += chosen_slice.inst.cfg.rbs.dedicated();
    }
    if (max_rbs_candidate == 0) {
      // We can skip this candidate, as there is no more RB space for it.
      continue;
    }
    unsigned rb_count =
        IsDownlink ? chosen_slice.inst.pdsch_rb_count : chosen_slice.inst.nof_pusch_rbs_allocated(pxsch_slot);
    if (not rb_lims.contains(rb_count)) {
      // The number of RBs already allocated to this slice is outside the range of the candidate valid RB limits.
      // Note: This can happen when the scheduler could not fill up to minRB limit of the first candidate when
      // minRB > 0. At this point, we should skip the candidate {minRB, maxRB} as well.
      continue;
    }

    // Return the candidate.
    return candidate_type{chosen_slice.inst, pxsch_slot, max_rbs_candidate};
  }
  return std::nullopt;
}

std::optional<dl_ran_slice_candidate> inter_slice_scheduler::get_next_dl_candidate()
{
  return get_next_candidate<true>();
}

std::optional<ul_ran_slice_candidate> inter_slice_scheduler::get_next_ul_candidate()
{
  return get_next_candidate<false>();
}

void inter_slice_scheduler::handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req)
{
  for (const auto& rrm : req.rrm_policies) {
    bool found = false;
    for (auto& slice : slices) {
      if (slice.inst.cfg.rrc_member == rrm.rrc_member) {
        found              = true;
        slice.inst.cfg.rbs = rrm.rbs;
      }
    }

    if (not found) {
      logger.warning(
          "No slice RRM policy found for {} in cell {}.", rrm.rrc_member, fmt::underlying(cell_cfg.cell_index));
    }
  }
}

inter_slice_scheduler::priority_type inter_slice_scheduler::ran_slice_sched_context::get_prio(bool       is_dl,
                                                                                              slot_point pdcch_slot,
                                                                                              slot_point pxsch_slot,
                                                                                              unsigned   rb_lims) const
{
  // Note: The positive integer representing the priority of a slice consists of a concatenation of several priority
  // values:
  // 1. slice slot distance (7 most significant bits). It prioritizes slices whose PXSCH slot is closer to the PDCCH
  // slot. This is to avoid scheduling PDSCHs/PUSCHs out-of-order for the same UE.
  // 2. slice min/max RB priority (1 bit). It prioritizes slices that have not yet achieved their minRB ratio.
  // 3. slice traffic priority (8 bits). It differentiates slices based on their configured priority value.
  // 4. delay priority (8 bits), which attributes the highest priority to slices that have not been scheduled for a
  // long time.
  // 5. round-robin based on slot indication count (7 least significant bits).
  // 6. one bit set to 1 to differentiate from "skip" priority.

  static constexpr priority_type slice_dist_bitsize       = 7U;
  static constexpr priority_type slice_dist_bitmask       = (1U << slice_dist_bitsize) - 1U;
  static constexpr priority_type slice_minrb_prio_bitsize = 1U;
  static constexpr priority_type slice_prio_bitsize       = 8U;
  static constexpr priority_type slice_prio_bitmask       = (1U << slice_prio_bitsize) - 1U;
  static constexpr priority_type delay_prio_bitsize       = 8U;
  static constexpr priority_type delay_prio_bitmask       = (1U << delay_prio_bitsize) - 1U;
  static constexpr priority_type rr_bitsize               = 7U;
  static constexpr priority_type rr_bitmask               = (1U << rr_bitsize) - 1U;

  if (not inst.active()) {
    // If the slice is not in a state to be scheduled in this slot, return skip priority level.
    return skip_prio;
  }

  // 1. (most significant bits) Prioritize closer slots over slots further away into the future.
  // Note: This is relevant for UL-heavy cases, to avoid scheduling PUSCHs out-of-order for the same UE.
  const priority_type slot_dist =
      slice_dist_bitmask - std::min(static_cast<unsigned>(pxsch_slot - pdcch_slot), slice_dist_bitmask);
  priority_type prio = slot_dist;

  // 2. In case minRB > 0 and minimum RB ratio agreement is not yet reached, we give it a higher priority.
  const priority_type slice_minrb_prio = rb_lims <= inst.cfg.rbs.min() and inst.cfg.rbs.min() > 0 ? 1U : 0U;
  prio                                 = (prio << slice_minrb_prio_bitsize) + slice_minrb_prio;

  // 3. Prioritize slices with higher priority.
  prio = (prio << slice_prio_bitsize) + std::min(inst.cfg.priority, slice_prio_bitmask);

  // 4. Increase priorities of slices that have not been scheduled for a long time.
  const priority_type delay_prio =
      std::min(is_dl ? inst.nof_slots_since_last_pdsch(pxsch_slot) : inst.nof_slots_since_last_pusch(pxsch_slot),
               delay_prio_bitmask);
  prio = (prio << delay_prio_bitsize) + delay_prio;

  // 5. Round-robin across slices with the same slice and delay priorities.
  float               rbs_per_slot = is_dl ? inst.average_pdsch_rbs_per_slot() : inst.average_pusch_rbs_per_slot();
  const priority_type rr_prio      = rr_bitmask - std::min((unsigned)std::round(rbs_per_slot), rr_bitmask);
  prio                             = (prio << rr_bitsize) + rr_prio;

  // Add one bit to differentiate from skip priority.
  return (prio << 1U) + 1U;
}
