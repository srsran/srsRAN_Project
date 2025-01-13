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

#include "scheduler_time_pf.h"
#include "../support/csi_report_helpers.h"

using namespace srsran;

// [Implementation-defined] Limit for the coefficient of the proportional fair metric to avoid issues with double
// imprecision.
constexpr unsigned MAX_PF_COEFF = 10;

scheduler_time_pf::scheduler_time_pf(const scheduler_ue_expert_config& expert_cfg_) :
  fairness_coeff(std::get<time_pf_scheduler_expert_config>(expert_cfg_.strategy_cfg).pf_sched_fairness_coeff)
{
}

dl_alloc_result scheduler_time_pf::schedule_dl_retxs(ue_pdsch_allocator&          pdsch_alloc,
                                                     const ue_resource_grid_view& res_grid,
                                                     dl_ran_slice_candidate&      slice_candidate,
                                                     dl_harq_pending_retx_list    harq_list)
{
  const ran_slice_id_t slice_id = slice_candidate.id();
  auto&                ue_db    = slice_candidate.get_slice_ues();

  for (auto it = harq_list.begin(); it != harq_list.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != slice_id or not ue_db.contains(h.ue_index())) {
      continue;
    }
    const slice_ue& u = ue_db[h.ue_index()];

    // Prioritize PCell over SCells.
    for (unsigned i = 0; i != u.nof_cells(); ++i) {
      const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
      srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                    "Policy scheduler called for UE={} in fallback",
                    fmt::underlying(ue_cc.ue_index));

      // [Implementation-defined] Skip UE if PDCCH is already allocated for this UE in this slot.
      if (res_grid.has_ue_dl_pdcch(ue_cc.cell_index, u.crnti()) or
          not ue_cc.is_pdcch_enabled(res_grid.get_pdcch_slot(ue_cc.cell_index))) {
        continue;
      }

      ue_pdsch_grant        grant{&u, ue_cc.cell_index, h.id()};
      const dl_alloc_result result = pdsch_alloc.allocate_dl_grant(grant);
      // Continue iteration until skip slot indication is received.
      // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
      // HARQ pending to newest. Hence, other allocation status are redundant.
      if (result.status == alloc_status::skip_slot) {
        return result;
      }
    }
  }

  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

void scheduler_time_pf::dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 dl_ran_slice_candidate&      slice_candidate,
                                 dl_harq_pending_retx_list    harq_pending_retx_list)
{
  const slice_ue_repository& ues = slice_candidate.get_slice_ues();

  // Remove deleted users from history.
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ues.contains(it->ue_index)) {
      ue_history_db.erase(it->ue_index);
    }
    ++it;
  }
  // Add new users to history db, and update DL priority queue.
  for (const auto& u : ues) {
    if (not ue_history_db.contains(u.ue_index())) {
      // TODO: Consider other cells when carrier aggregation is supported.
      ue_history_db.emplace(u.ue_index(), ue_ctxt{u.ue_index(), u.get_pcell().cell_index, this});
    }
  }

  // Schedule HARQ retxs.
  auto retx_result = schedule_dl_retxs(pdsch_alloc, res_grid, slice_candidate, harq_pending_retx_list);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  // Update DL priority queue.
  dl_queue.clear();
  slot_point pdsch_slot        = slice_candidate.get_slot_tx();
  unsigned   nof_slots_elapsed = last_pdsch_slot.valid() ? pdsch_slot - last_pdsch_slot : 1;
  last_pdsch_slot              = pdsch_slot;
  for (const auto& u : ues) {
    ue_ctxt& ctxt = ue_history_db[u.ue_index()];
    ctxt.compute_dl_prio(
        u, slice_candidate.id(), res_grid.get_pdcch_slot(u.get_pcell().cell_index), pdsch_slot, nof_slots_elapsed);
    dl_queue.push(&ctxt);
  }

  dl_alloc_result alloc_result = {alloc_status::invalid_params};
  unsigned        rem_rbs      = slice_candidate.remaining_rbs();
  while (not dl_queue.empty() and rem_rbs > 0 and alloc_result.status != alloc_status::skip_slot) {
    ue_ctxt& ue  = *dl_queue.top();
    alloc_result = try_dl_alloc(ue, ues, pdsch_alloc, rem_rbs);
    dl_queue.pop();
    rem_rbs = slice_candidate.remaining_rbs();
  }
}

ul_alloc_result scheduler_time_pf::schedule_ul_retxs(ue_pusch_allocator&          pusch_alloc,
                                                     const ue_resource_grid_view& res_grid,
                                                     ul_ran_slice_candidate&      slice_candidate,
                                                     ul_harq_pending_retx_list    harq_list)
{
  auto& ue_db = slice_candidate.get_slice_ues();

  for (auto it = harq_list.begin(); it != harq_list.end();) {
    // Note: During retx alloc, the pending HARQ list will mutate. So, we prefetch the next node.
    auto prev_it = it++;
    auto h       = *prev_it;
    if (h.get_grant_params().slice_id != slice_candidate.id() or not ue_db.contains(h.ue_index())) {
      continue;
    }
    const slice_ue& u = ue_db[h.ue_index()];
    // Prioritize PCell over SCells.
    for (unsigned i = 0; i != u.nof_cells(); ++i) {
      const ue_cell& ue_cc = u.get_cell(to_ue_cell_index(i));
      srsran_assert(ue_cc.is_active() and not ue_cc.is_in_fallback_mode(),
                    "Policy scheduler called for UE={} in fallback",
                    fmt::underlying(ue_cc.ue_index));

      if (not ue_cc.is_pdcch_enabled(res_grid.get_pdcch_slot(ue_cc.cell_index)) or
          not ue_cc.is_ul_enabled(slice_candidate.get_slot_tx())) {
        // Either the PDCCH slot or PUSCH slots are not available.
        continue;
      }

      ue_pusch_grant        grant{&u, ue_cc.cell_index, h.id()};
      const ul_alloc_result result = pusch_alloc.allocate_ul_grant(grant);
      // Continue iteration until skip slot indication is received.
      // NOTE: Allocation status other than skip_slot can be ignored because allocation of reTxs is done from oldest
      // HARQ pending to newest. Hence, other allocation status are redundant.
      if (result.status == alloc_status::skip_slot) {
        return result;
      }
    }
  }
  // Return successful outcome in all other cases.
  // Other cases:
  //  - No pending HARQs to allocate.
  //  - At the end of pending HARQs iteration.
  return {alloc_status::success};
}

void scheduler_time_pf::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 ul_ran_slice_candidate&      slice_candidate,
                                 ul_harq_pending_retx_list    harq_pending_retx_list)
{
  const slice_ue_repository& ues = slice_candidate.get_slice_ues();
  // Remove deleted users from history.
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ues.contains(it->ue_index)) {
      ue_history_db.erase(it->ue_index);
    }
    ++it;
  }
  // Add new users to history db, and update UL priority queue.
  for (const auto& u : ues) {
    if (not ue_history_db.contains(u.ue_index())) {
      // TODO: Consider other cells when carrier aggregation is supported.
      ue_history_db.emplace(u.ue_index(), ue_ctxt{u.ue_index(), u.get_pcell().cell_index, this});
    }
  }

  // Schedule HARQ retxs.
  auto retx_result = schedule_ul_retxs(pusch_alloc, res_grid, slice_candidate, harq_pending_retx_list);
  if (retx_result.status == alloc_status::skip_slot) {
    return;
  }

  // Clear the existing contents of the queue.
  ul_queue.clear();
  slot_point pusch_slot        = slice_candidate.get_slot_tx();
  unsigned   nof_slots_elapsed = last_pusch_slot.valid() ? pusch_slot - last_pusch_slot : 1;
  last_pusch_slot              = pusch_slot;
  for (const auto& u : ues) {
    ue_ctxt& ctxt = ue_history_db[u.ue_index()];
    ctxt.compute_ul_prio(u,
                         slice_candidate.id(),
                         res_grid.get_pdcch_slot(u.get_pcell().cell_index),
                         slice_candidate.get_slot_tx(),
                         nof_slots_elapsed);
    ul_queue.push(&ctxt);
  }

  ul_alloc_result alloc_result = {alloc_status::invalid_params};
  unsigned        rem_rbs      = slice_candidate.remaining_rbs();
  while (not ul_queue.empty() and rem_rbs > 0 and alloc_result.status != alloc_status::skip_slot) {
    ue_ctxt& ue  = *ul_queue.top();
    alloc_result = try_ul_alloc(ue, ues, pusch_alloc, rem_rbs);
    ul_queue.pop();
    rem_rbs = slice_candidate.remaining_rbs();
  }
}

dl_alloc_result scheduler_time_pf::try_dl_alloc(ue_ctxt&                   ctxt,
                                                const slice_ue_repository& ues,
                                                ue_pdsch_allocator&        pdsch_alloc,
                                                unsigned                   max_rbs)
{
  dl_alloc_result alloc_result = {alloc_status::invalid_params};
  ue_pdsch_grant  grant{&ues[ctxt.ue_index], ctxt.cell_index};

  grant.h_id                  = INVALID_HARQ_ID;
  grant.recommended_nof_bytes = ues[ctxt.ue_index].pending_dl_newtx_bytes();
  grant.max_nof_rbs           = max_rbs;
  alloc_result                = pdsch_alloc.allocate_dl_grant(grant);
  if (alloc_result.status == alloc_status::success) {
    ctxt.dl_prio = forbid_prio;
  }
  ctxt.save_dl_alloc(alloc_result.alloc_bytes, alloc_result.tb_info);
  return alloc_result;
}

ul_alloc_result scheduler_time_pf::try_ul_alloc(ue_ctxt&                   ctxt,
                                                const slice_ue_repository& ues,
                                                ue_pusch_allocator&        pusch_alloc,
                                                unsigned                   max_rbs)
{
  ul_alloc_result alloc_result = {alloc_status::invalid_params};
  ue_pusch_grant  grant{&ues[ctxt.ue_index], ctxt.cell_index};

  grant.h_id                  = INVALID_HARQ_ID;
  grant.recommended_nof_bytes = ues[ctxt.ue_index].pending_ul_newtx_bytes();
  grant.max_nof_rbs           = max_rbs;
  alloc_result                = pusch_alloc.allocate_ul_grant(grant);
  if (alloc_result.status == alloc_status::success) {
    ctxt.ul_prio = forbid_prio;
  }
  ctxt.save_ul_alloc(ues[ctxt.ue_index], alloc_result.alloc_bytes);
  return alloc_result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// \brief Returns rate expressed in bytes per slot for a given rate in bit per second.
static double to_bytes_per_slot(uint64_t bitrate_bps, subcarrier_spacing bwp_scs)
{
  // Deduce the number of slots per subframe.
  const unsigned nof_slots_per_subframe   = get_nof_slots_per_subframe(bwp_scs);
  const unsigned nof_subframes_per_second = 1000;
  // Deduce the number of slots per second.
  const unsigned nof_slots_per_second = nof_slots_per_subframe * nof_subframes_per_second;

  return (static_cast<double>(bitrate_bps) / static_cast<double>(nof_slots_per_second * 8U));
}

/// \brief Computes DL rate weight used in computation of DL priority value for a UE in a slot.
static double compute_dl_rate_weight(const slice_ue& u, span<double> dl_avg_rate_per_lc, subcarrier_spacing bwp_scs)
{
  // [Implementation-defined] Rate weight to assign when average rate in all GBR bearers is zero or if the UE has only
  // non-GBR bearers.
  const double initial_rate_weight = 1;

  double rate_weight = 0;
  for (const logical_channel_config& lc : u.logical_channels()) {
    // LC not part of the slice or non-GBR flow.
    if (not u.contains(lc.lcid) or not lc.qos.has_value() or not lc.qos->gbr_qos_info.has_value()) {
      continue;
    }

    // GBR flow.
    if (dl_avg_rate_per_lc[lc.lcid] != 0) {
      rate_weight += (to_bytes_per_slot(lc.qos->gbr_qos_info->gbr_dl, bwp_scs) / dl_avg_rate_per_lc[lc.lcid]);
    }
  }

  return rate_weight == 0 ? initial_rate_weight : rate_weight;
}

/// \brief Computes UL rate weight used in computation of UL priority value for a UE in a slot.
static double compute_ul_rate_weight(const slice_ue& u, span<double> ul_avg_rate_per_lcg, subcarrier_spacing bwp_scs)
{
  // [Implementation-defined] Rate weight to assign if the UE has only non-GBR bearers or average UL rate of UE is 0.
  const double initial_rate_weight = 1;

  double rate_weight = 0;
  // Compute sum of GBR rates of all LCs belonging to this slice.
  for (const logical_channel_config& lc : u.logical_channels()) {
    // LC is not part of the slice or a non-GBR flow.
    if (not u.contains(lc.lcid) or not lc.qos.has_value() or not lc.qos->gbr_qos_info.has_value()) {
      continue;
    }

    // GBR flow.
    lcg_id_t lcg_id = u.get_lcg_id(lc.lcid);
    if (ul_avg_rate_per_lcg[lcg_id] != 0) {
      rate_weight += (to_bytes_per_slot(lc.qos->gbr_qos_info->gbr_ul, bwp_scs) / ul_avg_rate_per_lcg[lcg_id]);
    }
  }

  return rate_weight == 0 ? initial_rate_weight : rate_weight;
}

void scheduler_time_pf::ue_ctxt::compute_dl_prio(const slice_ue& u,
                                                 ran_slice_id_t  slice_id,
                                                 slot_point      pdcch_slot,
                                                 slot_point      pdsch_slot,
                                                 unsigned        nof_slots_elapsed)
{
  dl_prio = forbid_prio;

  // Process previous slot allocated bytes and compute average.
  compute_dl_avg_rate(u, nof_slots_elapsed);

  const ue_cell* ue_cc = u.find_cell(cell_index);
  if (ue_cc == nullptr) {
    return;
  }
  srsran_assert(ue_cc->is_active() and not ue_cc->is_in_fallback_mode(),
                "Policy scheduler called for UE={} in fallback",
                fmt::underlying(ue_cc->ue_index));
  if (not ue_cc->is_pdcch_enabled(pdcch_slot) or not ue_cc->is_pdsch_enabled(pdsch_slot)) {
    // Cannot allocate PDCCH/PDSCH for this UE in this slot.
    return;
  }
  if (not ue_cc->harqs.has_empty_dl_harqs() or not u.has_pending_dl_newtx_bytes()) {
    // No available HARQs or no pending data.
    return;
  }

  // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
  const auto* ss_info =
      ue_cc->cfg().find_search_space(ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
  if (ss_info == nullptr) {
    return;
  }
  span<const pdsch_time_domain_resource_allocation> pdsch_td_res_list = ss_info->pdsch_time_domain_list;
  // [Implementation-defined] We pick the first element since PDSCH time domain resource list is sorted in descending
  // order of nof. PDSCH symbols. And, we want to calculate estimate of instantaneous achievable rate with maximum
  // nof. PDSCH symbols.
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_td_res_list.front();

  pdsch_config_params pdsch_cfg;
  switch (ss_info->get_dl_dci_format()) {
    case dci_dl_format::f1_0:
      pdsch_cfg = get_pdsch_config_f1_0_c_rnti(ue_cc->cfg().cell_cfg_common, &ue_cc->cfg(), pdsch_td_cfg);
      break;
    case dci_dl_format::f1_1:
      pdsch_cfg =
          get_pdsch_config_f1_1_c_rnti(ue_cc->cfg(), pdsch_td_cfg, ue_cc->channel_state_manager().get_nof_dl_layers());
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI DL format");
  }

  std::optional<sch_mcs_index> mcs = ue_cc->link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table);
  if (not mcs.has_value()) {
    // CQI is either 0 or above 15, which means no DL.
    return;
  }

  // Calculate DL PF priority.
  // NOTE: Estimated instantaneous DL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const double estimated_rate = ue_cc->get_estimated_dl_rate(pdsch_cfg, mcs.value(), ss_info->dl_crb_lims.length());
  const double current_total_avg_rate = total_dl_avg_rate();
  double       pf_weight              = 0;
  if (current_total_avg_rate != 0) {
    if (parent->fairness_coeff >= MAX_PF_COEFF) {
      // For very high coefficients, the pow(.) will be very high, leading to pf_weight of 0 due to lack of precision.
      // In such scenarios, we change the way to compute the PF weight. Instead, we completely disregard the estimated
      // rate, as its impact is minimal.
      pf_weight = 1 / current_total_avg_rate;
    } else {
      pf_weight = estimated_rate / pow(current_total_avg_rate, parent->fairness_coeff);
    }
  } else {
    // Give the highest priority to new UE.
    pf_weight = estimated_rate == 0 ? 0 : std::numeric_limits<double>::max();
  }
  const double rate_weight = compute_dl_rate_weight(
      u, dl_avg_rate_per_lc, ue_cc->cfg().cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.scs);
  dl_prio = rate_weight * pf_weight;
}

void scheduler_time_pf::ue_ctxt::compute_ul_prio(const slice_ue& u,
                                                 ran_slice_id_t  slice_id,
                                                 slot_point      pdcch_slot,
                                                 slot_point      pusch_slot,
                                                 unsigned        nof_slots_elapsed)
{
  ul_prio = forbid_prio;

  // Process bytes allocated in previous slot and compute average.
  compute_ul_avg_rate(u, nof_slots_elapsed);

  const ue_cell* ue_cc = u.find_cell(cell_index);
  if (ue_cc == nullptr) {
    return;
  }
  srsran_assert(ue_cc->is_active() and not ue_cc->is_in_fallback_mode(),
                "Policy scheduler called for UE={} in fallback",
                fmt::underlying(ue_cc->ue_index));
  if (not ue_cc->is_pdcch_enabled(pdcch_slot) or not ue_cc->is_ul_enabled(pusch_slot)) {
    // Cannot allocate PDCCH/PUSCH for this UE in the provided slots.
    return;
  }
  if (not ue_cc->harqs.has_empty_ul_harqs()) {
    // No HARQs for newTxs.
    return;
  }

  if (u.pending_ul_newtx_bytes() == 0) {
    // No new bytes to allocate.
    return;
  }

  // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
  const auto* ss_info =
      ue_cc->cfg().find_search_space(ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
  if (ss_info == nullptr) {
    return;
  }

  span<const pusch_time_domain_resource_allocation> pusch_td_res_list = ss_info->pusch_time_domain_list;
  // [Implementation-defined] We pick the first element since PUSCH time domain resource list is sorted in descending
  // order of nof. PUSCH symbols. And, we want to calculate estimate of instantaneous achievable rate with maximum
  // nof. PUSCH symbols.
  const pusch_time_domain_resource_allocation& pusch_td_cfg = pusch_td_res_list.front();
  // [Implementation-defined] We assume nof. HARQ ACK bits is zero at PUSCH slot as a simplification in calculating
  // estimated instantaneous achievable rate.
  constexpr unsigned nof_harq_ack_bits = 0;
  const bool is_csi_report_slot = csi_helper::is_csi_reporting_slot(u.get_pcell().cfg().cfg_dedicated(), pusch_slot);

  pusch_config_params pusch_cfg;
  switch (ss_info->get_ul_dci_format()) {
    case dci_ul_format::f0_0:
      pusch_cfg = get_pusch_config_f0_0_c_rnti(ue_cc->cfg().cell_cfg_common,
                                               &ue_cc->cfg(),
                                               ue_cc->cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp,
                                               pusch_td_cfg,
                                               nof_harq_ack_bits,
                                               is_csi_report_slot);
      break;
    case dci_ul_format::f0_1:
      pusch_cfg = get_pusch_config_f0_1_c_rnti(ue_cc->cfg(),
                                               pusch_td_cfg,
                                               ue_cc->channel_state_manager().get_nof_ul_layers(),
                                               nof_harq_ack_bits,
                                               is_csi_report_slot);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  sch_mcs_index mcs = ue_cc->link_adaptation_controller().calculate_ul_mcs(pusch_cfg.mcs_table);

  // Calculate UL PF priority.
  // NOTE: Estimated instantaneous UL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const double estimated_rate   = ue_cc->get_estimated_ul_rate(pusch_cfg, mcs.value(), ss_info->ul_crb_lims.length());
  const double current_avg_rate = total_ul_avg_rate();
  double       pf_weight        = 0;
  if (current_avg_rate != 0) {
    if (parent->fairness_coeff >= MAX_PF_COEFF) {
      // For very high coefficients, the pow(.) will be very high, leading to pf_weight of 0 due to lack of precision.
      // In such scenarios, we change the way to compute the PF weight. Instead, we completely disregard the estimated
      // rate, as its impact is minimal.
      pf_weight = 1 / current_avg_rate;
    } else {
      pf_weight = estimated_rate / pow(current_avg_rate, parent->fairness_coeff);
    }
  } else {
    pf_weight = estimated_rate == 0 ? 0 : std::numeric_limits<double>::max();
  }
  const double rate_weight = compute_ul_rate_weight(
      u, ul_avg_rate_per_lcg, ue_cc->cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs);
  ul_prio         = rate_weight * pf_weight;
  sr_ind_received = u.has_pending_sr();
}

void scheduler_time_pf::ue_ctxt::compute_dl_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed)
{
  // Redimension LCID arrays to the UE configured bearers.
  dl_alloc_bytes_per_lc.resize(u.get_bearers().size(), 0);
  dl_avg_rate_per_lc.resize(dl_alloc_bytes_per_lc.size(), 0);

  // In case more than one slot elapsed.
  for (unsigned s = 0; s != nof_slots_elapsed - 1; ++s) {
    for (unsigned i = 0; i != dl_alloc_bytes_per_lc.size(); ++i) {
      if (not u.contains(uint_to_lcid(i))) {
        // Skip LCIDs that are not configured.
        dl_alloc_bytes_per_lc[i] = 0;
        dl_avg_rate_per_lc[i]    = 0;
        continue;
      }
      if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
        // Fast start before transitioning to exponential average.
        dl_avg_rate_per_lc[i] -= dl_avg_rate_per_lc[i] / (dl_nof_samples + 1);
      } else {
        dl_avg_rate_per_lc[i] -= parent->exp_avg_alpha * dl_avg_rate_per_lc[i];
      }
    }
    if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      total_dl_avg_rate_ -= total_dl_avg_rate_ / (dl_nof_samples + 1);
    } else {
      total_dl_avg_rate_ -= parent->exp_avg_alpha * total_dl_avg_rate_;
    }
    dl_nof_samples++;
  }

  // Compute DL average rate on a per-logical channel basis.
  for (unsigned i = 0; i != dl_alloc_bytes_per_lc.size(); ++i) {
    if (not u.contains(uint_to_lcid(i))) {
      // Skip LCIDs that are not configured.
      dl_alloc_bytes_per_lc[i] = 0;
      dl_avg_rate_per_lc[i]    = 0;
      continue;
    }

    unsigned sched_bytes = dl_alloc_bytes_per_lc[i];
    if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      dl_avg_rate_per_lc[i] += (sched_bytes - dl_avg_rate_per_lc[i]) / (dl_nof_samples + 1);
    } else {
      dl_avg_rate_per_lc[i] =
          (1 - parent->exp_avg_alpha) * dl_avg_rate_per_lc[i] + (parent->exp_avg_alpha) * sched_bytes;
    }

    // Flush allocated bytes for the current slot.
    dl_alloc_bytes_per_lc[i] = 0;
  }

  // Compute DL average rate of the UE.
  unsigned sched_bytes = dl_sum_alloc_bytes;
  if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
    // Fast start before transitioning to exponential average.
    total_dl_avg_rate_ = total_dl_avg_rate_ + (sched_bytes - total_dl_avg_rate_) / (dl_nof_samples + 1);
  } else {
    total_dl_avg_rate_ = (1 - parent->exp_avg_alpha) * total_dl_avg_rate_ + (parent->exp_avg_alpha) * sched_bytes;
  }

  // Flush allocated bytes for the current slot.
  dl_sum_alloc_bytes = 0;

  // Increment number of samples.
  dl_nof_samples++;
}

void scheduler_time_pf::ue_ctxt::compute_ul_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed)
{
  // Redimension LCID arrays to the UE configured bearers.
  ul_alloc_bytes_per_lcg.resize(u.get_lcgs().size(), 0);
  ul_avg_rate_per_lcg.resize(ul_alloc_bytes_per_lcg.size(), 0);

  // In case more than one slot elapsed.
  for (unsigned s = 0; s != nof_slots_elapsed - 1; ++s) {
    for (unsigned i = 0; i != ul_alloc_bytes_per_lcg.size(); ++i) {
      if (not u.contains(uint_to_lcid(i))) {
        // Skip LCIDs that are not configured.
        ul_alloc_bytes_per_lcg[i] = 0;
        ul_avg_rate_per_lcg[i]    = 0;
        continue;
      }
      if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
        // Fast start before transitioning to exponential average.
        ul_avg_rate_per_lcg[i] -= ul_avg_rate_per_lcg[i] / (ul_nof_samples + 1);
      } else {
        ul_avg_rate_per_lcg[i] -= parent->exp_avg_alpha * ul_avg_rate_per_lcg[i];
      }
    }
    if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      total_ul_avg_rate_ -= total_ul_avg_rate_ / (ul_nof_samples + 1);
    } else {
      total_ul_avg_rate_ -= parent->exp_avg_alpha * total_ul_avg_rate_;
    }
    ul_nof_samples++;
  }

  // Compute UL average rate on a per-logical channel group basis.
  for (unsigned i = 0; i != ul_alloc_bytes_per_lcg.size(); ++i) {
    if (not u.contains(uint_to_lcg_id(i))) {
      // Skip LCIDs that are not configured.
      ul_alloc_bytes_per_lcg[i] = 0;
      ul_avg_rate_per_lcg[i]    = 0;
    }

    unsigned sched_bytes = ul_alloc_bytes_per_lcg[i];

    if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      ul_avg_rate_per_lcg[i] += (sched_bytes - ul_avg_rate_per_lcg[i]) / (ul_nof_samples + 1);
    } else {
      ul_avg_rate_per_lcg[i] =
          (1 - parent->exp_avg_alpha) * ul_avg_rate_per_lcg[i] + (parent->exp_avg_alpha) * sched_bytes;
    }

    // Flush allocated bytes for the current slot.
    ul_alloc_bytes_per_lcg[i] = 0;
  }

  // Compute UL average rate of the UE.
  if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
    // Fast start before transitioning to exponential average.
    total_ul_avg_rate_ += (ul_sum_alloc_bytes - total_ul_avg_rate_) / (ul_nof_samples + 1);
  } else {
    total_ul_avg_rate_ =
        (1 - parent->exp_avg_alpha) * total_ul_avg_rate_ + (parent->exp_avg_alpha) * ul_sum_alloc_bytes;
  }

  // Flush allocated bytes for the current slot.
  ul_sum_alloc_bytes = 0;

  // Increment number of samples.
  ul_nof_samples++;
}

void scheduler_time_pf::ue_ctxt::save_dl_alloc(uint32_t total_alloc_bytes, const dl_msg_tb_info& tb_info)
{
  for (const auto& tb : tb_info.lc_chs_to_sched) {
    if (not tb.lcid.is_sdu()) {
      // Ignore CEs.
      continue;
    }
    const lcid_t lcid = tb.lcid.to_lcid();
    if (lcid >= dl_alloc_bytes_per_lc.size()) {
      // It can happen that an LCID of another slice can be added to a given grant.
      continue;
    }
    dl_alloc_bytes_per_lc[lcid] += tb.sched_bytes;
  }
  dl_sum_alloc_bytes += total_alloc_bytes;
}

void scheduler_time_pf::ue_ctxt::save_ul_alloc(const slice_ue& u, unsigned alloc_bytes)
{
  if (alloc_bytes == 0) {
    return;
  }
  // We do not know which LCGs will be allocated by the UE, but we can make an estimation based on BSRs.
  auto lcg_info = u.estimate_ul_alloc_bytes_per_lcg(alloc_bytes);
  for (auto [lcgid, lcg_bytes] : lcg_info) {
    if (lcgid < ul_alloc_bytes_per_lcg.size()) {
      ul_alloc_bytes_per_lcg[lcgid] = lcg_bytes;
    }
  }
  ul_sum_alloc_bytes += alloc_bytes;
}

bool scheduler_time_pf::ue_dl_prio_compare::operator()(const scheduler_time_pf::ue_ctxt* lhs,
                                                       const scheduler_time_pf::ue_ctxt* rhs) const
{
  return lhs->dl_prio < rhs->dl_prio;
}

bool scheduler_time_pf::ue_ul_prio_compare::operator()(const scheduler_time_pf::ue_ctxt* lhs,
                                                       const scheduler_time_pf::ue_ctxt* rhs) const
{
  // First, prioritize UEs with pending SR.
  // SR indication in one UE and not in other UE.
  if (lhs->sr_ind_received != rhs->sr_ind_received) {
    return rhs->sr_ind_received;
  }
  // SR indication for both UEs.
  if (lhs->sr_ind_received) {
    return lhs->ul_prio < rhs->ul_prio;
  }
  // All other cases compare priorities.
  return lhs->ul_prio < rhs->ul_prio;
}
