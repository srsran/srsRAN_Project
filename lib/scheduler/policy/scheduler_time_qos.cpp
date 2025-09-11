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

#include "scheduler_time_qos.h"
#include "../slicing/slice_ue_repository.h"
#include "../support/csi_report_helpers.h"
#include "../ue_scheduling/grant_params_selector.h"
#include <algorithm>

using namespace srsran;

// [Implementation-defined] Limit for the coefficient of the proportional fair metric to avoid issues with double
// imprecision.
static constexpr unsigned MAX_PF_COEFF = 10;

// [Implementation-defined] Maximum number of slots skipped between scheduling opportunities.
static constexpr unsigned MAX_SLOT_SKIPPED = 20;

scheduler_time_qos::scheduler_time_qos(const scheduler_ue_expert_config& expert_cfg_, du_cell_index_t cell_index_) :
  params(std::get<time_qos_scheduler_expert_config>(expert_cfg_.strategy_cfg)), cell_index(cell_index_)
{
}

void scheduler_time_qos::add_ue(du_ue_index_t ue_index)
{
  srsran_assert(not ue_history_db.contains(ue_index), "UE was already added to this slice");
  ue_history_db.emplace(ue_index, ue_ctxt{ue_index, cell_index, this});
}

void scheduler_time_qos::rem_ue(du_ue_index_t ue_index)
{
  ue_history_db.erase(ue_index);
}

void scheduler_time_qos::compute_ue_dl_priorities(slot_point               pdcch_slot,
                                                  slot_point               pdsch_slot,
                                                  span<ue_newtx_candidate> ue_candidates)
{
  unsigned nof_slots_elapsed = std::min(last_pdsch_slot.valid() ? pdsch_slot - last_pdsch_slot : 1U, MAX_SLOT_SKIPPED);
  last_pdsch_slot            = pdsch_slot;

  // Compute UE candidate priorities.
  for (auto& u : ue_candidates) {
    ue_ctxt& uectxt = ue_history_db[u.ue->ue_index()];
    uectxt.compute_dl_prio(*u.ue, pdcch_slot, pdsch_slot, nof_slots_elapsed);
    u.priority = uectxt.dl_prio;
  }
}

void scheduler_time_qos::compute_ue_ul_priorities(slot_point               pdcch_slot,
                                                  slot_point               pusch_slot,
                                                  span<ue_newtx_candidate> ue_candidates)
{
  unsigned nof_slots_elapsed = std::min(last_pusch_slot.valid() ? pusch_slot - last_pusch_slot : 1U, MAX_SLOT_SKIPPED);
  last_pusch_slot            = pusch_slot;

  // Compute UE candidate priorities.
  for (auto& u : ue_candidates) {
    ue_ctxt& uectxt = ue_history_db[u.ue->ue_index()];
    uectxt.compute_ul_prio(*u.ue, pdcch_slot, pusch_slot, nof_slots_elapsed);
    u.priority = uectxt.ul_prio;
  }
}

void scheduler_time_qos::save_dl_newtx_grants(span<const dl_msg_alloc> dl_grants)
{
  // Save result of DL grants in UE history.
  for (const dl_msg_alloc& grant : dl_grants) {
    ue_history_db[grant.context.ue_index].save_dl_alloc(grant.pdsch_cfg.codewords[0].tb_size_bytes, grant.tb_list[0]);
  }
}

void scheduler_time_qos::save_ul_newtx_grants(span<const ul_sched_info> ul_grants)
{
  // Save result of UL grants in UE history.
  for (const ul_sched_info& grant : ul_grants) {
    ue_history_db[grant.context.ue_index].save_ul_alloc(grant.pusch_cfg.tb_size_bytes);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// [Implementation-defined] Helper value to set a maximum metric weight that is low enough to avoid overflows during
// the final QoS weight computation.
static constexpr double max_metric_weight = 1.0e12;

static double compute_pf_metric(double estim_rate, double avg_rate, double fairness_coeff)
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
        pf_weight = estim_rate / std::pow(avg_rate, fairness_coeff);
      }
    } else {
      // In case the avg rate is zero, the division would be inf. Instead, we give the highest priority to the UE.
      pf_weight = max_metric_weight;
    }
  }
  return pf_weight;
}

static double combine_qos_metrics(double                                  pf_weight,
                                  double                                  gbr_weight,
                                  double                                  prio_weight,
                                  double                                  delay_weight,
                                  const time_qos_scheduler_expert_config& policy_params)
{
  if (policy_params.qos_weight_func == time_qos_scheduler_expert_config::weight_function::gbr_prioritized and
      gbr_weight > 1.0) {
    // GBR target has not been met and we prioritize GBR over PF.
    pf_weight = std::max(1.0, pf_weight);
  }

  // The return is a combination of QoS priority, ARP priority, GBR and PF weight functions.
  return gbr_weight * pf_weight * prio_weight * delay_weight;
}

/// \brief Computes DL rate weight used in computation of DL priority value for a UE in a slot.
static double compute_dl_qos_weights(const slice_ue&                         u,
                                     double                                  estim_dl_rate,
                                     double                                  avg_dl_rate,
                                     slot_point                              slot_tx,
                                     const time_qos_scheduler_expert_config& policy_params)
{
  if (avg_dl_rate == 0) {
    // Highest priority to UEs that have not yet received any allocation.
    return std::numeric_limits<double>::max();
  }

  static constexpr uint16_t max_combined_prio_level = qos_prio_level_t::max() * arp_prio_level_t::max();
  uint16_t                  min_combined_prio       = max_combined_prio_level;
  double                    gbr_weight              = 0;
  double                    delay_weight            = 0;
  if (policy_params.gbr_enabled or policy_params.priority_enabled or policy_params.pdb_enabled) {
    for (logical_channel_config_ptr lc : *u.logical_channels()) {
      if (not u.contains(lc->lcid) or not lc->qos.has_value() or u.pending_dl_newtx_bytes(lc->lcid) == 0) {
        // LC is not part of the slice, No QoS config was provided for this LC or there is no pending data for this LC
        continue;
      }

      // Track the LC with the lowest combined priority (combining QoS and ARP priority levels).
      if (policy_params.priority_enabled) {
        min_combined_prio = std::min(
            static_cast<uint16_t>(lc->qos->qos.priority.value() * lc->qos->arp_priority.value()), min_combined_prio);
      }

      slot_point hol_toa = u.dl_hol_toa(lc->lcid);
      if (hol_toa.valid() and slot_tx >= hol_toa) {
        const unsigned hol_delay_ms = (slot_tx - hol_toa) / slot_tx.nof_slots_per_subframe();
        const unsigned pdb          = lc->qos->qos.packet_delay_budget_ms;
        delay_weight += hol_delay_ms / static_cast<double>(pdb);
      }

      if (not lc->qos->gbr_qos_info.has_value()) {
        // LC is a non-GBR flow.
        continue;
      }

      // GBR flow.
      double dl_avg_rate = u.dl_avg_bit_rate(lc->lcid);
      if (dl_avg_rate != 0) {
        gbr_weight += std::min(lc->qos->gbr_qos_info->gbr_dl / dl_avg_rate, max_metric_weight);
      } else {
        gbr_weight += max_metric_weight;
      }
    }
  }

  // If no QoS flows are configured, the weight is set to 1.0.
  gbr_weight   = policy_params.gbr_enabled and gbr_weight != 0 ? gbr_weight : 1.0;
  delay_weight = policy_params.pdb_enabled and delay_weight != 0 ? delay_weight : 1.0;

  double pf_weight = compute_pf_metric(estim_dl_rate, avg_dl_rate, policy_params.pf_fairness_coeff);
  // If priority is disabled, set the priority weight of all UEs to 1.0.
  double prio_weight = policy_params.priority_enabled ? (max_combined_prio_level + 1 - min_combined_prio) /
                                                            static_cast<double>(max_combined_prio_level + 1)
                                                      : 1.0;

  // The return is a combination of ARP and QoS priorities, GBR and PF weight functions.
  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, delay_weight, policy_params);
}

/// \brief Computes UL weights used in computation of UL priority value for a UE in a slot.
static double compute_ul_qos_weights(const slice_ue&                         u,
                                     double                                  estim_ul_rate,
                                     double                                  avg_ul_rate,
                                     const time_qos_scheduler_expert_config& policy_params)
{
  if (u.has_pending_sr() or avg_ul_rate == 0) {
    // Highest priority to SRs and UEs that have not yet received any allocation.
    return max_sched_priority;
  }

  static constexpr uint16_t max_combined_prio_level = qos_prio_level_t::max() * arp_prio_level_t::max();
  uint16_t                  min_combined_prio       = max_combined_prio_level;
  double                    gbr_weight              = 0;
  if (policy_params.gbr_enabled or policy_params.priority_enabled) {
    for (logical_channel_config_ptr lc : *u.logical_channels()) {
      if (not u.contains(lc->lcid) or not lc->qos.has_value() or u.pending_ul_unacked_bytes(lc->lc_group) == 0) {
        // LC is not part of the slice or no QoS config was provided for this LC or there are no pending bytes for this
        // group.
        continue;
      }

      // Track the LC with the lowest combined priority (combining QoS and ARP priority levels).
      if (policy_params.priority_enabled) {
        min_combined_prio = std::min(
            static_cast<uint16_t>(lc->qos->qos.priority.value() * lc->qos->arp_priority.value()), min_combined_prio);
      }

      if (not lc->qos->gbr_qos_info.has_value()) {
        // LC is a non-GBR flow.
        continue;
      }

      // GBR flow.
      lcg_id_t lcg_id  = u.get_lcg_id(lc->lcid);
      double   ul_rate = u.ul_avg_bit_rate(lcg_id);
      if (ul_rate != 0) {
        gbr_weight += std::min(lc->qos->gbr_qos_info->gbr_ul / ul_rate, max_metric_weight);
      } else {
        gbr_weight = max_metric_weight;
      }
    }
  }

  // If no GBR flows are configured, the gbr rate is set to 1.0.
  gbr_weight = policy_params.gbr_enabled and gbr_weight != 0 ? gbr_weight : 1.0;
  // If priority is disabled, set the priority weight of all UEs to 1.0.
  double prio_weight = policy_params.priority_enabled ? (max_combined_prio_level + 1 - min_combined_prio) /
                                                            static_cast<double>(max_combined_prio_level + 1)
                                                      : 1.0;
  double pf_weight   = compute_pf_metric(estim_ul_rate, avg_ul_rate, policy_params.pf_fairness_coeff);

  return combine_qos_metrics(pf_weight, gbr_weight, prio_weight, 1.0, policy_params);
}

scheduler_time_qos::ue_ctxt::ue_ctxt(du_ue_index_t             ue_index_,
                                     du_cell_index_t           cell_index_,
                                     const scheduler_time_qos* parent_) :
  ue_index(ue_index_),
  cell_index(cell_index_),
  parent(parent_),
  total_dl_avg_rate_(parent->exp_avg_alpha),
  total_ul_avg_rate_(parent->exp_avg_alpha)
{
}

void scheduler_time_qos::ue_ctxt::compute_dl_prio(const slice_ue& u,
                                                  slot_point      pdcch_slot,
                                                  slot_point      pdsch_slot,
                                                  unsigned        nof_slots_elapsed)
{
  dl_prio = forbid_prio;

  // Process previous slot allocated bytes and compute average.
  compute_dl_avg_rate(u, nof_slots_elapsed);

  const ue_cell& ue_cc = u.get_cc();

  // This should be ensured at this point.
  srsran_sanity_check(ue_cc.is_pdsch_enabled(pdcch_slot, pdsch_slot) and ue_cc.harqs.has_empty_dl_harqs() and
                          u.has_pending_dl_newtx_bytes(),
                      "Invalid DL UE candidate state");

  // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
  const search_space_id ue_ded_ss_id = to_search_space_id(2);
  const auto&           ss_info      = ue_cc.cfg().search_space(ue_ded_ss_id);

  // [Implementation-defined] We pick the first element since PDSCH time domain resource list is sorted in descending
  // order of nof. PDSCH symbols. And, we want to calculate estimate of instantaneous achievable rate with maximum
  // nof. PDSCH symbols.
  uint8_t                    pdsch_time_res_index = 0;
  const pdsch_config_params& pdsch_cfg =
      ss_info.get_pdsch_config(pdsch_time_res_index, ue_cc.channel_state_manager().get_nof_dl_layers());

  auto mcs = ue_cc.link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table);
  if (not mcs.has_value()) {
    // CQI is either 0 or above 15, which means no DL.
    return;
  }

  // Calculate DL PF priority.
  // NOTE: Estimated instantaneous DL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const double estimated_rate = ue_cc.get_estimated_dl_rate(pdsch_cfg, mcs.value(), ss_info.dl_crb_lims.length());
  const double current_total_avg_rate = total_dl_avg_rate();
  dl_prio = compute_dl_qos_weights(u, estimated_rate, current_total_avg_rate, pdcch_slot, parent->params);
}

void scheduler_time_qos::ue_ctxt::compute_ul_prio(const slice_ue& u,
                                                  slot_point      pdcch_slot,
                                                  slot_point      pusch_slot,
                                                  unsigned        nof_slots_elapsed)
{
  ul_prio = forbid_prio;

  // Process bytes allocated in previous slot and compute average.
  compute_ul_avg_rate(u, nof_slots_elapsed);

  const ue_cell& ue_cc = u.get_cc();
  srsran_sanity_check(not ue_cc.is_in_fallback_mode() and ue_cc.is_pusch_enabled(pdcch_slot, pusch_slot) and
                          ue_cc.harqs.has_empty_ul_harqs() and u.pending_ul_newtx_bytes() > 0,
                      "UE UL candidate in invalid state");

  // [Implementation-defined] We consider only the SearchSpace defined in UE dedicated configuration.
  const search_space_id ue_ded_ss_id = to_search_space_id(2);
  const auto&           ss_info      = ue_cc.cfg().search_space(ue_ded_ss_id);

  span<const pusch_time_domain_resource_allocation> pusch_td_res_list = ss_info.pusch_time_domain_list;
  // [Implementation-defined] We pick the first element since PUSCH time domain resource list is sorted in descending
  // order of nof. PUSCH symbols. And, we want to calculate estimate of instantaneous achievable rate with maximum
  // nof. PUSCH symbols.
  const pusch_time_domain_resource_allocation& pusch_td_cfg = pusch_td_res_list.front();
  // [Implementation-defined] We assume nof. HARQ ACK bits is zero at PUSCH slot as a simplification in calculating
  // estimated instantaneous achievable rate.
  constexpr unsigned nof_harq_ack_bits  = 0;
  const bool         is_csi_report_slot = ue_cc.cfg().csi_meas_cfg() != nullptr and
                                  csi_helper::is_csi_reporting_slot(*ue_cc.cfg().csi_meas_cfg(), pusch_slot);

  pusch_config_params pusch_cfg;
  switch (ss_info.get_ul_dci_format()) {
    case dci_ul_format::f0_0:
      pusch_cfg = get_pusch_config_f0_0_c_rnti(ue_cc.cfg().cell_cfg_common,
                                               &ue_cc.cfg(),
                                               ue_cc.cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp,
                                               pusch_td_cfg,
                                               nof_harq_ack_bits,
                                               is_csi_report_slot);
      break;
    case dci_ul_format::f0_1:
      pusch_cfg = get_pusch_config_f0_1_c_rnti(ue_cc.cfg(),
                                               pusch_td_cfg,
                                               ue_cc.channel_state_manager().get_nof_ul_layers(),
                                               nof_harq_ack_bits,
                                               is_csi_report_slot);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  sch_mcs_index mcs =
      ue_cc.link_adaptation_controller().calculate_ul_mcs(pusch_cfg.mcs_table, pusch_cfg.use_transform_precoder);

  // Calculate UL PF priority.
  // NOTE: Estimated instantaneous UL rate is calculated assuming entire BWP CRBs are allocated to UE.
  const double estimated_rate   = ue_cc.get_estimated_ul_rate(pusch_cfg, mcs.value(), ss_info.ul_crb_lims.length());
  const double current_avg_rate = total_ul_avg_rate();

  // Compute LC weight function.
  ul_prio = compute_ul_qos_weights(u, estimated_rate, current_avg_rate, parent->params);
}

void scheduler_time_qos::ue_ctxt::compute_dl_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed)
{
  // In case more than one slot elapsed.
  if (nof_slots_elapsed > 1) {
    total_dl_avg_rate_.push_zeros(nof_slots_elapsed - 1);
  }

  // Compute DL average rate of the UE.
  total_dl_avg_rate_.push(dl_sum_alloc_bytes);

  // Flush allocated bytes for the current slot.
  dl_sum_alloc_bytes = 0;
}

void scheduler_time_qos::ue_ctxt::compute_ul_avg_rate(const slice_ue& u, unsigned nof_slots_elapsed)
{
  // In case more than one slot elapsed.
  if (nof_slots_elapsed > 1) {
    total_ul_avg_rate_.push_zeros(nof_slots_elapsed - 1);
  }

  // Compute UL average rate of the UE.
  total_ul_avg_rate_.push(ul_sum_alloc_bytes);

  // Flush allocated bytes for the current slot.
  ul_sum_alloc_bytes = 0;
}

void scheduler_time_qos::ue_ctxt::save_dl_alloc(uint32_t total_alloc_bytes, const dl_msg_tb_info& tb_info)
{
  dl_sum_alloc_bytes += total_alloc_bytes;
}

void scheduler_time_qos::ue_ctxt::save_ul_alloc(unsigned alloc_bytes)
{
  if (alloc_bytes == 0) {
    return;
  }
  ul_sum_alloc_bytes += alloc_bytes;
}
