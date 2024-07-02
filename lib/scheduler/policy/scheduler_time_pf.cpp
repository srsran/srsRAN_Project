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

#include "scheduler_time_pf.h"
#include "../support/csi_report_helpers.h"

using namespace srsran;

scheduler_time_pf::scheduler_time_pf(const scheduler_ue_expert_config& expert_cfg_) :
  fairness_coeff(std::get<time_pf_scheduler_expert_config>(expert_cfg_.strategy_cfg).pf_sched_fairness_coeff)
{
}

void scheduler_time_pf::dl_sched(ue_pdsch_allocator&          pdsch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues)
{
  // Clear the existing contents of the queue.
  dl_queue.clear();

  // Remove deleted users from history.
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ues.contains(it->ue_index)) {
      ue_history_db.erase(it->ue_index);
    }
    ++it;
  }

  // Add new users to history db, and update DL priority queue.
  for (const auto& u : ues) {
    if (not ue_history_db.contains(u->ue_index)) {
      // TODO: Consider other cells when carrier aggregation is supported.
      ue_history_db.emplace(u->ue_index, ue_ctxt{u->ue_index, u->get_pcell().cell_index, this});
    }
    ue_ctxt& ctxt = ue_history_db[u->ue_index];
    ctxt.compute_dl_prio(*u);
    dl_queue.push(&ctxt);
  }

  alloc_result alloc_result = {alloc_status::invalid_params};
  while (not dl_queue.empty()) {
    ue_ctxt& ue = *dl_queue.top();
    if (alloc_result.status != alloc_status::skip_slot) {
      alloc_result = try_dl_alloc(ue, ues, pdsch_alloc);
    }
    ue.save_dl_alloc(alloc_result.alloc_bytes);
    // Re-add the UE to the queue if scheduling of re-transmission fails so that scheduling of retransmission are
    // attempted again before scheduling new transmissions.
    if (ue.dl_retx_h != nullptr and alloc_result.status == alloc_status::invalid_params) {
      dl_queue.push(&ue);
    }
    dl_queue.pop();
  }
}

void scheduler_time_pf::ul_sched(ue_pusch_allocator&          pusch_alloc,
                                 const ue_resource_grid_view& res_grid,
                                 const ue_repository&         ues)
{
  // Clear the existing contents of the queue.
  ul_queue.clear();

  // Remove deleted users from history.
  for (auto it = ue_history_db.begin(); it != ue_history_db.end();) {
    if (not ues.contains(it->ue_index)) {
      ue_history_db.erase(it->ue_index);
    }
    ++it;
  }

  // Add new users to history db, and update UL priority queue.
  for (const auto& u : ues) {
    if (not ue_history_db.contains(u->ue_index)) {
      // TODO: Consider other cells when carrier aggregation is supported.
      ue_history_db.emplace(u->ue_index, ue_ctxt{u->ue_index, u->get_pcell().cell_index, this});
    }
    ue_ctxt& ctxt = ue_history_db[u->ue_index];
    ctxt.compute_ul_prio(*u, res_grid);
    ul_queue.push(&ctxt);
  }

  alloc_result alloc_result = {alloc_status::invalid_params};
  while (not ul_queue.empty()) {
    ue_ctxt& ue = *ul_queue.top();
    if (alloc_result.status != alloc_status::skip_slot) {
      alloc_result = try_ul_alloc(ue, ues, pusch_alloc);
    }
    ue.save_ul_alloc(alloc_result.alloc_bytes);
    // Re-add the UE to the queue if scheduling of re-transmission fails so that scheduling of retransmission are
    // attempted again before scheduling new transmissions.
    if (ue.ul_retx_h != nullptr and alloc_result.status == alloc_status::invalid_params) {
      ul_queue.push(&ue);
    }
    ul_queue.pop();
  }
}

alloc_result scheduler_time_pf::try_dl_alloc(ue_ctxt& ctxt, const ue_repository& ues, ue_pdsch_allocator& pdsch_alloc)
{
  alloc_result   alloc_result = {alloc_status::invalid_params};
  ue_pdsch_grant grant{&ues[ctxt.ue_index], ctxt.cell_index};
  // Prioritize reTx over newTx.
  if (ctxt.dl_retx_h != nullptr) {
    grant.h_id   = ctxt.dl_retx_h->id;
    alloc_result = pdsch_alloc.allocate_dl_grant(grant);
    if (alloc_result.status == alloc_status::success) {
      ctxt.dl_retx_h = nullptr;
    }
    // Return result here irrespective of the outcome so that reTxs of UEs are scheduled before scheduling newTxs of
    // UEs.
    return alloc_result;
  }

  if (ctxt.dl_newtx_h != nullptr) {
    grant.h_id                  = ctxt.dl_newtx_h->id;
    grant.recommended_nof_bytes = ues[ctxt.ue_index].pending_dl_newtx_bytes();
    alloc_result                = pdsch_alloc.allocate_dl_grant(grant);
    if (alloc_result.status == alloc_status::success) {
      ctxt.dl_newtx_h = nullptr;
    }
    return alloc_result;
  }

  return {alloc_status::skip_ue};
}

alloc_result scheduler_time_pf::try_ul_alloc(ue_ctxt& ctxt, const ue_repository& ues, ue_pusch_allocator& pusch_alloc)
{
  alloc_result   alloc_result = {alloc_status::invalid_params};
  ue_pusch_grant grant{&ues[ctxt.ue_index], ctxt.cell_index};
  // Prioritize reTx over newTx.
  if (ctxt.ul_retx_h != nullptr) {
    grant.h_id   = ctxt.ul_retx_h->id;
    alloc_result = pusch_alloc.allocate_ul_grant(grant);
    if (alloc_result.status == alloc_status::success) {
      ctxt.ul_retx_h = nullptr;
    }
    // Return result here irrespective of the outcome so that reTxs of UEs are scheduled before scheduling newTxs of
    // UEs.
    return alloc_result;
  }

  if (ctxt.ul_newtx_h != nullptr) {
    grant.h_id                  = ctxt.ul_newtx_h->id;
    grant.recommended_nof_bytes = ues[ctxt.ue_index].pending_ul_newtx_bytes();
    alloc_result                = pusch_alloc.allocate_ul_grant(grant);
    if (alloc_result.status == alloc_status::success) {
      ctxt.ul_newtx_h = nullptr;
    }
    return alloc_result;
  }

  return {alloc_status::skip_ue};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void scheduler_time_pf::ue_ctxt::compute_dl_prio(const ue& u)
{
  dl_retx_h            = nullptr;
  dl_newtx_h           = nullptr;
  dl_prio              = 0;
  const ue_cell* ue_cc = u.find_cell(cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active() or ue_cc->is_in_fallback_mode()) {
    return;
  }

  // Calculate DL priority.
  dl_retx_h  = ue_cc->harqs.find_pending_dl_retx();
  dl_newtx_h = ue_cc->harqs.find_empty_dl_harq();
  if (dl_retx_h != nullptr or (dl_newtx_h != nullptr and u.has_pending_dl_newtx_bytes())) {
    // NOTE: It does not matter whether it's a reTx or newTx since DL priority is computed based on estimated
    // instantaneous achievable rate to the average throughput of the user.
    // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
    const auto* ss_info = ue_cc->cfg().find_search_space(
        ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
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
        pdsch_cfg = get_pdsch_config_f1_1_c_rnti(
            ue_cc->cfg(), pdsch_td_cfg, ue_cc->channel_state_manager().get_nof_dl_layers());
        break;
      default:
        report_fatal_error("Unsupported PDCCH DCI DL format");
    }

    std::optional<sch_mcs_index> mcs = ue_cc->link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table);
    if (not mcs.has_value()) {
      // CQI is either 0, or > 15.
      dl_retx_h  = nullptr;
      dl_newtx_h = nullptr;
      return;
    }

    // Calculate DL PF priority.
    // NOTE: Estimated instantaneous DL rate is calculated assuming entire BWP CRBs are allocated to UE.
    const double estimated_rate   = ue_cc->get_estimated_dl_rate(pdsch_cfg, mcs.value(), ss_info->dl_crb_lims.length());
    const double current_avg_rate = dl_avg_rate();
    if (current_avg_rate != 0) {
      dl_prio = estimated_rate / pow(current_avg_rate, parent->fairness_coeff);
    } else {
      dl_prio = estimated_rate == 0 ? 0 : std::numeric_limits<double>::max();
    }
    return;
  }
  dl_newtx_h = nullptr;
}

void scheduler_time_pf::ue_ctxt::compute_ul_prio(const ue& u, const ue_resource_grid_view& res_grid)
{
  ul_retx_h            = nullptr;
  ul_newtx_h           = nullptr;
  ul_prio              = 0;
  sr_ind_received      = false;
  const ue_cell* ue_cc = u.find_cell(cell_index);
  if (ue_cc == nullptr or not ue_cc->is_active() or ue_cc->is_in_fallback_mode()) {
    return;
  }

  // Calculate UL priority.
  ul_retx_h       = ue_cc->harqs.find_pending_ul_retx();
  ul_newtx_h      = ue_cc->harqs.find_empty_ul_harq();
  sr_ind_received = u.has_pending_sr();
  if (ul_retx_h != nullptr or (ul_newtx_h != nullptr and u.pending_ul_newtx_bytes() > 0)) {
    // NOTE: It does not matter whether it's a reTx or newTx since UL priority is computed based on estimated
    // instantaneous achievable rate to the average throughput of the user.
    // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
    const auto* ss_info = ue_cc->cfg().find_search_space(
        ue_cc->cfg().cfg_dedicated().init_dl_bwp.pdcch_cfg->search_spaces.back().get_id());
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
    constexpr unsigned nof_harq_ack_bits  = 0;
    const bool         is_csi_report_slot = csi_helper::is_csi_reporting_slot(
        u.get_pcell().cfg().cfg_dedicated(), res_grid.get_pusch_slot(cell_index, pusch_td_cfg.k2));

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
    const double current_avg_rate = ul_avg_rate();
    if (current_avg_rate != 0) {
      ul_prio = estimated_rate / pow(current_avg_rate, parent->fairness_coeff);
    } else {
      ul_prio = estimated_rate == 0 ? 0 : std::numeric_limits<double>::max();
    }
    return;
  }
  ul_newtx_h = nullptr;
}

void scheduler_time_pf::ue_ctxt::save_dl_alloc(uint32_t alloc_bytes)
{
  if (dl_nof_samples < 1 / parent->exp_avg_alpha) {
    // Fast start before transitioning to exponential average.
    dl_avg_rate_ = dl_avg_rate_ + (alloc_bytes - dl_avg_rate_) / (dl_nof_samples + 1);
  } else {
    dl_avg_rate_ = (1 - parent->exp_avg_alpha) * dl_avg_rate_ + (parent->exp_avg_alpha) * alloc_bytes;
  }
  dl_nof_samples++;
}

void scheduler_time_pf::ue_ctxt::save_ul_alloc(uint32_t alloc_bytes)
{
  if (ul_nof_samples < 1 / parent->exp_avg_alpha) {
    // Fast start before transitioning to exponential average.
    ul_avg_rate_ = ul_avg_rate_ + (alloc_bytes - ul_avg_rate_) / (ul_nof_samples + 1);
  } else {
    ul_avg_rate_ = (1 - parent->exp_avg_alpha) * ul_avg_rate_ + (parent->exp_avg_alpha) * alloc_bytes;
  }
  ul_nof_samples++;
}

bool scheduler_time_pf::ue_dl_prio_compare::operator()(const scheduler_time_pf::ue_ctxt* lhs,
                                                       const scheduler_time_pf::ue_ctxt* rhs) const
{
  const bool is_lhs_retx = lhs->dl_retx_h != nullptr;
  const bool is_rhs_retx = rhs->dl_retx_h != nullptr;
  return (not is_lhs_retx and is_rhs_retx) or (is_lhs_retx == is_rhs_retx and lhs->dl_prio < rhs->dl_prio);
}

bool scheduler_time_pf::ue_ul_prio_compare::operator()(const scheduler_time_pf::ue_ctxt* lhs,
                                                       const scheduler_time_pf::ue_ctxt* rhs) const
{
  const bool is_lhs_retx = lhs->ul_retx_h != nullptr;
  const bool is_rhs_retx = rhs->ul_retx_h != nullptr;
  if (not is_lhs_retx and is_rhs_retx) {
    return true;
  }
  if (is_lhs_retx == is_rhs_retx) {
    if (not is_lhs_retx) {
      // NewTx and SR indication received for one of the UEs.
      if (not lhs->sr_ind_received and rhs->sr_ind_received) {
        return true;
      }
    }
    // All other cases compare priorities.
    return lhs->ul_prio < rhs->ul_prio;
  }
  return false;
}
