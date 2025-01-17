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
  fairness_coeff(std::get<time_pf_scheduler_expert_config>(expert_cfg_.strategy_cfg).pf_sched_fairness_coeff),
  weight_func(std::get<time_pf_scheduler_expert_config>(expert_cfg_.strategy_cfg).qos_weight_func)
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
  ctxt.save_ul_alloc(alloc_result.alloc_bytes);
  return alloc_result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

// [Implementation-defined] Helper value to set a maximum metric weight that is low enough to avoid overflows during
// the final QoS weight computation.
constexpr double max_metric_weight = 1.0e12;

double compute_pf_metric(double estim_rate, double avg_rate, double fairness_coeff)
{
  double pf_weight = 0.0;
  if (estim_rate > 0) {
    if (avg_rate != 0) {
      if (fairness_coeff >= MAX_PF_COEFF) {
        // For very high coefficients, the pow(.) will be very high, leading to pf_weight of 0 due to lack of precision.
        // In such scenarios, we change the way to compute the PF weight. Instead, we completely disregard the estimated
        // rate, as its impact is minimal.
        pf_weight = 1 / avg_rate;
      } else {
        pf_weight = estim_rate / pow(avg_rate, fairness_coeff);
      }
    } else {
      // In case the avg rate is zero, the division would be inf. Instead, we give the highest priority to the UE.
      pf_weight = max_metric_weight;
    }
  }
  return pf_weight;
}

double combine_qos_metrics(double                                           pf_weight,
                           double                                           gbr_weight,
                           double                                           prio_weight,
                           double                                           delay_weight,
                           time_pf_scheduler_expert_config::weight_function weight_func)
{
  if (weight_func == time_pf_scheduler_expert_config::weight_function::gbr_prioritized and gbr_weight > 1.0) {
    // GBR target has not been met and we prioritize GBR over PF.
    pf_weight = std::max(1.0, pf_weight);
  }

  // The return is a combination of QoS priority, GBR and PF weight functions.
  return gbr_weight * pf_weight * prio_weight * delay_weight;
}

/// \brief Computes DL rate weight used in computation of DL priority value for a UE in a slot.
double compute_dl_qos_weights(const slice_ue&                                  u,
                              double                                           estim_dl_rate,
                              double                                           avg_dl_rate,
                              time_pf_scheduler_expert_config::weight_function weight_func,
                              double                                           fairness_coeff,
                              slot_point                                       slot_tx)
{
  if (avg_dl_rate == 0) {
    // Highest priority to UEs that have not yet received any allocation.
    return std::numeric_limits<double>::max();
  }

  uint8_t min_prio_level = qos_prio_level_t::max();
  double  gbr_weight     = 0;
  double  delay_weight   = 0;
  for (const logical_channel_config& lc : u.logical_channels()) {
    if (not u.contains(lc.lcid) or not lc.qos.has_value() or u.pending_dl_newtx_bytes(lc.lcid) == 0) {
      // LC is not part of the slice, No QoS config was provided for this LC or there is no pending data for this LC
      continue;
    }

    // Track the LC with the lowest priority.
    min_prio_level = std::min(lc.qos->qos.priority.value(), min_prio_level);

    slot_point hol_toa = u.dl_hol_toa(lc.lcid);
    if (hol_toa.valid() and slot_tx >= hol_toa) {
      const unsigned hol_delay_ms = (slot_tx - hol_toa) / slot_tx.nof_slots_per_subframe();
      const unsigned pdb          = lc.qos->qos.packet_delay_budget_ms;
      delay_weight += hol_delay_ms / static_cast<double>(pdb);
    }

    if (not lc.qos->gbr_qos_info.has_value()) {
      // LC is a non-GBR flow.
      continue;
    }

    // GBR flow.
    double dl_avg_rate = u.dl_avg_bit_rate(lc.lcid);
    if (dl_avg_rate != 0) {
      gbr_weight += std::min(lc.qos->gbr_qos_info->gbr_dl / dl_avg_rate, max_metric_weight);
    } else {
      gbr_weight += max_metric_weight;
    }
  }

  // If no QoS flows are configured, the weight is set to 1.0.
  gbr_weight   = gbr_weight == 0 ? 1.0 : gbr_weight;
  delay_weight = delay_weight == 0 ? 1.0 : delay_weight;

  double pf_weight = compute_pf_metric(estim_dl_rate, avg_dl_rate, fairness_coeff);
  double prio_weight =
      (qos_prio_level_t::max() + 1 - min_prio_level) / static_cast<double>(qos_prio_level_t::max() + 1);

  // The return is a combination of QoS priority, GBR and PF weight functions.
  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, delay_weight, weight_func);
}

/// \brief Computes UL weights used in computation of UL priority value for a UE in a slot.
double compute_ul_qos_weights(const slice_ue&                                  u,
                              double                                           estim_ul_rate,
                              double                                           avg_ul_rate,
                              time_pf_scheduler_expert_config::weight_function weight_func,
                              double                                           fairness_coeff)
{
  if (u.has_pending_sr() or avg_ul_rate == 0) {
    // Highest priority to SRs and UEs that have not yet received any allocation.
    return std::numeric_limits<double>::max();
  }

  uint8_t min_prio_level = qos_prio_level_t::max();
  double  gbr_weight     = 0;
  for (const logical_channel_config& lc : u.logical_channels()) {
    if (not u.contains(lc.lcid) or not lc.qos.has_value() or u.pending_ul_unacked_bytes(lc.lc_group) == 0) {
      // LC is not part of the slice or no QoS config was provided for this LC or there are no pending bytes for this
      // group.
      continue;
    }

    // Track the LC with the lowest priority.
    min_prio_level = std::min(lc.qos->qos.priority.value(), min_prio_level);

    if (not lc.qos->gbr_qos_info.has_value()) {
      // LC is a non-GBR flow.
      continue;
    }

    // GBR flow.
    lcg_id_t lcg_id  = u.get_lcg_id(lc.lcid);
    double   ul_rate = u.ul_avg_bit_rate(lcg_id);
    if (ul_rate != 0) {
      gbr_weight += std::min(lc.qos->gbr_qos_info->gbr_ul / ul_rate, max_metric_weight);
    } else {
      gbr_weight = max_metric_weight;
    }
  }

  // If no GBR flows are configured, the gbr rate is set to 1.0.
  gbr_weight = gbr_weight == 0 ? 1.0 : gbr_weight;

  double pf_weight = compute_pf_metric(estim_ul_rate, avg_ul_rate, fairness_coeff);
  double prio_weight =
      (qos_prio_level_t::max() + 1 - min_prio_level) / static_cast<double>(qos_prio_level_t::max() + 1);

  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, 1.0, weight_func);
}

} // namespace

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
  dl_prio                             = compute_dl_qos_weights(
      u, estimated_rate, current_total_avg_rate, parent->weight_func, parent->fairness_coeff, pdcch_slot);
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

  // Compute LC weight function.
  ul_prio = compute_ul_qos_weights(u, estimated_rate, current_avg_rate, parent->weight_func, parent->fairness_coeff);
}

void scheduler_time_pf::ue_ctxt::compute_dl_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed)
{
  // In case more than one slot elapsed.
  for (unsigned s = 0; s != nof_slots_elapsed - 1; ++s) {
    if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      total_dl_avg_rate_ -= total_dl_avg_rate_ / (dl_nof_samples + 1);
    } else {
      total_dl_avg_rate_ -= parent->exp_avg_alpha * total_dl_avg_rate_;
    }
    dl_nof_samples++;
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
  // In case more than one slot elapsed.
  for (unsigned s = 0; s != nof_slots_elapsed - 1; ++s) {
    if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
      // Fast start before transitioning to exponential average.
      total_ul_avg_rate_ -= total_ul_avg_rate_ / (ul_nof_samples + 1);
    } else {
      total_ul_avg_rate_ -= parent->exp_avg_alpha * total_ul_avg_rate_;
    }
    ul_nof_samples++;
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
  dl_sum_alloc_bytes += total_alloc_bytes;
}

void scheduler_time_pf::ue_ctxt::save_ul_alloc(unsigned alloc_bytes)
{
  if (alloc_bytes == 0) {
    return;
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
  // All other cases compare priorities.
  return lhs->ul_prio < rhs->ul_prio;
}
