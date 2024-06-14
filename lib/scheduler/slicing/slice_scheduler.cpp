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

slice_scheduler::slice_scheduler(const cell_configuration& cell_cfg_, const slice_rrm_policy_config& cfg_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED"))
{
  // Create a number of slices equal to the number of configured RRM Policy members + 1 (default slice).
  slices.reserve(cell_cfg.rrm_policy_members.size() + 1);
  sorted_dl_prios.reserve(cell_cfg.rrm_policy_members.size() + 1);
  sorted_ul_prios.reserve(cell_cfg.rrm_policy_members.size() + 1);

  // Create RAN slice instances.
  ran_slice_id_t id_count{0};
  // Default slice.
  slices.emplace_back(id_count, cell_cfg, slice_rrm_policy_config{});
  slices.back().policy =
      create_scheduler_strategy(scheduler_strategy_params{"time_rr", &logger}, cell_cfg.expert_cfg.ue);
  sorted_dl_prios.emplace_back(id_count);
  sorted_ul_prios.emplace_back(id_count);
  // Configured RRM policy members
  for (const slice_rrm_policy_config& rrm : cell_cfg.rrm_policy_members) {
    slices.emplace_back(id_count, cell_cfg, rrm);
    slices.back().policy =
        create_scheduler_strategy(scheduler_strategy_params{"time_rr", &logger}, cell_cfg.expert_cfg.ue);
    sorted_dl_prios.emplace_back(id_count);
    sorted_ul_prios.emplace_back(id_count);
    ++id_count;
  }
}

void slice_scheduler::slot_indication()
{
  for (auto& slice : slices) {
    slice.slot_indication();
  }

  // Compute slice priorities.
  for (slice_prio_context& ctx : sorted_dl_prios) {
    ctx.prio = slices[ctx.id.value()].get_dl_prio();
  }
  for (slice_prio_context& ctx : sorted_ul_prios) {
    ctx.prio = slices[ctx.id.value()].get_ul_prio();
  }

  // Sort slices by descending priority.
  std::sort(sorted_dl_prios.begin(), sorted_dl_prios.end(), std::greater<>{});
  std::sort(sorted_ul_prios.begin(), sorted_ul_prios.end(), std::greater<>{});
}

void slice_scheduler::add_ue(du_ue_index_t ue_idx, const ue_configuration& ue_cfg)
{
  for (const logical_channel_config& lc_cfg : ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    sl_inst.set_logical_channel(ue_idx, lc_cfg.lcid);
  }
}

void slice_scheduler::reconf_ue(du_ue_index_t           ue_idx,
                                const ue_configuration& next_ue_cfg,
                                const ue_configuration& prev_ue_cfg)
{
  // Remove old bearers.
  for (const logical_channel_config& lc_cfg : prev_ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    sl_inst.rem_logical_channel(ue_idx, lc_cfg.lcid);
  }

  // Add new bearers.
  for (const logical_channel_config& lc_cfg : next_ue_cfg.logical_channels()) {
    ran_slice_instance& sl_inst = get_slice(lc_cfg.rrm_policy);
    sl_inst.set_logical_channel(ue_idx, lc_cfg.lcid);
  }
}

void slice_scheduler::rem_ue(du_ue_index_t ue_idx)
{
  for (auto& slice : slices) {
    slice.rem_ue(ue_idx);
  }
}

ran_slice_instance& slice_scheduler::get_slice(const rrm_policy_member& rrm)
{
  auto it = std::find_if(
      slices.begin(), slices.end(), [&rrm](const ran_slice_instance& slice) { return slice.cfg.rrc_member == rrm; });
  if (it == slices.end()) {
    // Slice with the provided RRM policy member was not found. Return default slice.
    return slices.front();
  }
  return *it;
}

dl_ran_slice_candidate slice_scheduler::get_next_dl_candidate()
{
  if (slices.size() == 1) {
    return create_dl_candidate();
  }

  // Check if the slice priority hasn't changed. If it did, recompute priorities.
  slice_prio_context* slice_prio_ctxt = &sorted_dl_prios.front();
  // Recompute priority
  int prio = slices[slice_prio_ctxt->id.value()].get_dl_prio();
  if (prio != slice_prio_ctxt->prio) {
    // Priority changed
    slice_prio_ctxt->prio = prio;
    // Check if sort needs to be called again
    // Note: This assumes that only the previous front slice was used in scheduling.
    if (prio < sorted_dl_prios[1].prio) {
      // Slices need to be reordered.
      std::sort(sorted_dl_prios.begin(), sorted_dl_prios.end(), std::greater<>{});
    }
  }

  return create_dl_candidate();
}

ul_ran_slice_candidate slice_scheduler::get_next_ul_candidate()
{
  if (slices.size() == 1) {
    return create_ul_candidate();
  }

  // Check if the slice priority hasn't changed. If it did, recompute priorities.
  slice_prio_context* slice_prio_ctxt = &sorted_ul_prios.front();
  // Recompute priority
  int prio = slices[slice_prio_ctxt->id.value()].get_ul_prio();
  if (prio != slice_prio_ctxt->prio) {
    // Priority changed
    slice_prio_ctxt->prio = prio;
    // Check if sort needs to be called again
    // Note: This assumes that only the previous front slice was used in scheduling.
    if (prio < sorted_ul_prios[1].prio) {
      // Slices need to be reordered.
      std::sort(sorted_ul_prios.begin(), sorted_ul_prios.end(), std::greater<>{});
    }
  }

  return create_ul_candidate();
}

dl_ran_slice_candidate slice_scheduler::create_dl_candidate()
{
  bool has_candidates = sorted_dl_prios[0].prio != ran_slice_instance::skip_slice_prio;
  return dl_ran_slice_candidate{has_candidates ? &slices[sorted_dl_prios[0].id.value()] : nullptr};
}

ul_ran_slice_candidate slice_scheduler::create_ul_candidate()
{
  bool has_candidates = sorted_ul_prios[0].prio != ran_slice_instance::skip_slice_prio;
  return ul_ran_slice_candidate{has_candidates ? &slices[sorted_ul_prios[0].id.value()] : nullptr};
}
