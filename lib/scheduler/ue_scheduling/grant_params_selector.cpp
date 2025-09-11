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

#include "grant_params_selector.h"
#include "../slicing/slice_ue_repository.h"
#include "../support/csi_report_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "../support/rb_helper.h"
#include "../ue_context/ue_cell.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"

using namespace srsran;
using namespace sched_helper;

namespace {

/// Estimation of the number of PRBs and MCS to use for a given number of pending bytes and channel state.
struct mcs_prbs_selection {
  /// Recommended MCS to use.
  sch_mcs_index mcs;
  /// Number of recommended PRBs for the PDSCH grant given the number of pending bytes and chosen MCS.
  unsigned nof_prbs;
};

} // namespace

static std::optional<mcs_prbs_selection> compute_newtx_required_mcs_and_prbs(const pdsch_config_params& pdsch_cfg,
                                                                             const ue_cell&             ue_cc,
                                                                             unsigned                   pending_bytes,
                                                                             interval<unsigned>         nof_rb_lims)
{
  // Note: At this point, CQI must be higher than 0, so MCS is valid.
  const sch_mcs_index       mcs = ue_cc.link_adaptation_controller().calculate_dl_mcs(pdsch_cfg.mcs_table).value();
  const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_cfg.mcs_table, mcs);

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  pdsch_cfg.symbols.length(),
                                                                  calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                                  pdsch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pdsch_cfg.nof_layers},
                                       nof_rb_lims.stop());
  srsran_sanity_check(prbs_tbs.nof_prbs <= nof_rb_lims.stop(), "Error in RB computation");
  if (prbs_tbs.nof_prbs == 0) {
    return std::nullopt;
  }

  // Apply min RB grant size limits (max was applied before).
  prbs_tbs.nof_prbs = std::max(prbs_tbs.nof_prbs, nof_rb_lims.start());

  // [Implementation-defined] In case of partial slots and nof. PRBs allocated equals to 1 probability of KO is
  // high due to code not being able to cope with interference. So the solution is to increase the PRB allocation
  // to greater than 1 PRB.
  if (prbs_tbs.nof_prbs == 1 and pdsch_cfg.symbols.length() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP) {
    prbs_tbs.nof_prbs = 2;
  }

  return mcs_prbs_selection{mcs.value(), prbs_tbs.nof_prbs};
}

/// Compute PUSCH grant parameters for a newTx/reTx given the UE state, DCI type and PUSCH time-domain resource.
static pusch_config_params compute_pusch_config_params(const ue_cell&                               ue_cc,
                                                       dci_ul_rnti_config_type                      dci_type,
                                                       unsigned                                     nof_layers,
                                                       const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                       unsigned                                     uci_bits,
                                                       bool                                         is_csi_report_slot)
{
  const ue_cell_configuration& ue_cell_cfg = ue_cc.cfg();
  const cell_configuration&    cell_cfg    = ue_cc.cfg().cell_cfg_common;
  const bwp_uplink_common&     bwp_ul_cmn  = *ue_cell_cfg.bwp(ue_cc.active_bwp_id()).ul_common.value();

  pusch_config_params pusch_cfg;
  switch (dci_type) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0:
      pusch_cfg = get_pusch_config_f0_0_tc_rnti(cell_cfg, pusch_td_cfg);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_0:
      pusch_cfg =
          get_pusch_config_f0_0_c_rnti(cell_cfg, &ue_cell_cfg, bwp_ul_cmn, pusch_td_cfg, uci_bits, is_csi_report_slot);
      break;
    case dci_ul_rnti_config_type::c_rnti_f0_1:
      pusch_cfg = get_pusch_config_f0_1_c_rnti(ue_cell_cfg, pusch_td_cfg, nof_layers, uci_bits, is_csi_report_slot);
      break;
    default:
      report_fatal_error("Unsupported PDCCH DCI UL format");
  }

  return pusch_cfg;
}

/// Compute PUSCH grant parameters for a newTx given the UE state, DCI type and PUSCH time-domain resource.
static pusch_config_params compute_newtx_pusch_config_params(const ue_cell&                               ue_cc,
                                                             dci_ul_rnti_config_type                      dci_type,
                                                             const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                             unsigned                                     uci_bits,
                                                             bool is_csi_report_slot)
{
  return compute_pusch_config_params(
      ue_cc, dci_type, ue_cc.channel_state_manager().get_nof_ul_layers(), pusch_td_cfg, uci_bits, is_csi_report_slot);
}

/// Compute PUSCH grant parameters for a reTx given the UE state, DCI type and PUSCH time-domain resource.
pusch_config_params static compute_retx_pusch_config_params(const ue_cell&                               ue_cc,
                                                            const ul_harq_process_handle&                h_ul,
                                                            const pusch_time_domain_resource_allocation& pusch_td_cfg,
                                                            unsigned                                     uci_bits,
                                                            bool is_csi_report_slot)
{
  return compute_pusch_config_params(ue_cc,
                                     h_ul.get_grant_params().dci_cfg_type,
                                     h_ul.get_grant_params().nof_layers,
                                     pusch_td_cfg,
                                     uci_bits,
                                     is_csi_report_slot);
}

/// Derive recommended MCS and number of PRBs for a newTx PUSCH grant.
static std::optional<mcs_prbs_selection> compute_newtx_required_mcs_and_prbs(const pusch_config_params& pusch_cfg,
                                                                             const ue_cell&             ue_cc,
                                                                             unsigned                   pending_bytes,
                                                                             interval<unsigned>         nof_rb_lims)
{
  sch_mcs_index mcs =
      ue_cc.link_adaptation_controller().calculate_ul_mcs(pusch_cfg.mcs_table, pusch_cfg.use_transform_precoder);
  sch_mcs_description mcs_config =
      pusch_mcs_get_config(pusch_cfg.mcs_table, mcs, pusch_cfg.use_transform_precoder, pusch_cfg.tp_pi2bpsk_present);

  const auto nof_symbols = pusch_cfg.symbols.length();

  sch_prbs_tbs prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                                  nof_symbols,
                                                                  calculate_nof_dmrs_per_rb(pusch_cfg.dmrs),
                                                                  pusch_cfg.nof_oh_prb,
                                                                  mcs_config,
                                                                  pusch_cfg.nof_layers},
                                       nof_rb_lims.stop());

  // Apply minimum grant size.
  unsigned nof_prbs = std::max(prbs_tbs.nof_prbs, nof_rb_lims.start());

  // We need to adjust the number of PRBs to the PHR, to prevent the UE from reducing the nominal TX power to meet the
  // max TX power.
  nof_prbs = ue_cc.get_pusch_power_controller().adapt_pusch_prbs_to_phr(nof_prbs);

  // Ensure code rate and UCI is valid. If not, increase the number of PRBs.
  // Note: We make the pessimistic assumption that the DC intersects the newTx grant.
  static constexpr bool contains_dc = true;
  while (nof_prbs <= nof_rb_lims.stop() and
         not is_pusch_effective_rate_valid(pusch_cfg, ue_cc.active_bwp(), mcs, nof_prbs, contains_dc)) {
    ++nof_prbs;
  }
  if (nof_prbs > nof_rb_lims.stop()) {
    // No valid MCS-PRB allocation.
    return std::nullopt;
  }

  if (mcs == 5U and nof_prbs == 1U and nof_prbs < nof_rb_lims.stop()) {
    // [Implementation-defined] In our tests, we have seen that MCS 5 with 1 PRB can lead (depending on the
    // configuration) to a non-valid MCS-PRB allocation; therefore, we increase the number of 1 PRBs.
    // TODO: Remove this part and handle the problem with a loop that is general for any configuration.
    ++nof_prbs;
  }

  // Ensure the number of PRB is valid if the transform precoder is used. The condition the PUSCH bandwidth with
  // transform precoder is defined in TS 38.211 Section 6.1.3.
  // The number of PRB must be lower than or equal to current number of PRB.
  if (pusch_cfg.use_transform_precoder) {
    nof_prbs = transform_precoding::get_nof_prbs_lower_bound(nof_prbs).value_or(nof_prbs);
  }

  if (nof_prbs == 0) {
    // No valid MCS-PRB allocation.
    return std::nullopt;
  }
  return mcs_prbs_selection{mcs, nof_prbs};
}

static std::optional<dl_sched_context> get_dl_sched_context(const slice_ue&               u,
                                                            slot_point                    pdcch_slot,
                                                            slot_point                    pdsch_slot,
                                                            bool                          interleaving_enabled,
                                                            const dl_harq_process_handle* h_dl,
                                                            unsigned                      pending_bytes)
{
  const ue_cell& ue_cc = u.get_cc();

  if (not ue_cc.is_pdsch_enabled(pdcch_slot, pdsch_slot) or ue_cc.is_in_fallback_mode()) {
    // The UE cannot be scheduled in the provided slots.
    return std::nullopt;
  }

  const ue_cell_configuration& ue_cell_cfg      = ue_cc.cfg();
  const cell_configuration&    cell_cfg         = ue_cell_cfg.cell_cfg_common;
  unsigned                     slot_nof_symbols = cell_cfg.get_nof_dl_symbol_per_slot(pdsch_slot);

  // TODO: Support more search spaces.
  static constexpr search_space_id ue_ded_ss_id = to_search_space_id(2);
  const search_space_info&         ss           = ue_cc.cfg().search_space(ue_ded_ss_id);

  if (h_dl != nullptr) {
    // ReTx case.
    if (slot_nof_symbols < h_dl->get_grant_params().nof_symbols) {
      // Early exit if there are not enough symbols in the slot for the retransmission.
      return std::nullopt;
    }
    srsran_assert(ss.get_dl_dci_format() == get_dci_format(h_dl->get_grant_params().dci_cfg_type),
                  "DCI type cannot change across reTxs");
  }

  // Determine RB allocation limits.
  const auto crb_lims = (cell_cfg.expert_cfg.ue.pdsch_crb_limits & ss.dl_crb_lims).convert_to<crb_interval>();
  const interval<unsigned> nof_rb_lims =
      cell_cfg.expert_cfg.ue.pdsch_nof_rbs &
      ue_cell_cfg.rrm_cfg().pdsch_grant_size_limits.convert_to<interval<unsigned>>() &
      interval<unsigned>{0, crb_lims.length()};
  if (nof_rb_lims.empty()) {
    // Invalid RB allocation range.
    return std::nullopt;
  }

  for (unsigned pdsch_td_index = 0, e = ss.pdsch_time_domain_list.size(); pdsch_td_index != e; ++pdsch_td_index) {
    const pdsch_time_domain_resource_allocation& pdsch_td_res = ss.pdsch_time_domain_list[pdsch_td_index];

    // Check that k0 matches the chosen PDSCH slot
    if (pdcch_slot + pdsch_td_res.k0 != pdsch_slot) {
      continue;
    }

    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    if (h_dl != nullptr and pdsch_td_res.symbols.length() != h_dl->get_grant_params().nof_symbols) {
      continue;
    }

    // Check whether PDSCH time domain resource last symbol is lower than the total number of DL symbols of the slot.
    if (slot_nof_symbols < pdsch_td_res.symbols.stop()) {
      continue;
    }

    // Check whether PDSCH time domain resource does not overlap with CORESET.
    if (pdsch_td_res.symbols.start() < ss.cfg->get_first_symbol_index() + ss.coreset->duration) {
      continue;
    }

    // Compute recommended number of layers, MCS and PRBs.
    unsigned      nof_layers;
    unsigned      nof_rbs;
    sch_mcs_index mcs;
    if (h_dl == nullptr) {
      // NewTx Case.
      nof_layers                           = ue_cc.channel_state_manager().get_nof_dl_layers();
      const pdsch_config_params& pdsch_cfg = ss.get_pdsch_config(pdsch_td_index, nof_layers);
      auto mcs_prbs_sel = compute_newtx_required_mcs_and_prbs(pdsch_cfg, ue_cc, pending_bytes, nof_rb_lims);
      if (not mcs_prbs_sel.has_value()) {
        // Note: No point in carrying on.
        return std::nullopt;
      }
      mcs     = mcs_prbs_sel.value().mcs;
      nof_rbs = mcs_prbs_sel.value().nof_prbs;
    } else {
      // ReTx Case.
      nof_layers = h_dl->get_grant_params().nof_layers;
      mcs        = h_dl->get_grant_params().mcs;
      nof_rbs    = h_dl->get_grant_params().rbs.type1().length();
      if (nof_rbs > nof_rb_lims.stop()) {
        return std::nullopt;
      }
    }

    dl_sched_context ctxt;
    ctxt.ss_id              = ss.cfg->get_id();
    ctxt.pdsch_td_res_index = pdsch_td_index;
    ctxt.recommended_mcs    = mcs;
    ctxt.recommended_ri     = nof_layers;
    ctxt.expected_nof_rbs   = nof_rbs;
    return ctxt;
  }

  return std::nullopt;
}

std::optional<dl_sched_context> sched_helper::get_newtx_dl_sched_context(const slice_ue& u,
                                                                         slot_point      pdcch_slot,
                                                                         slot_point      pdsch_slot,
                                                                         bool            interleaving_enabled,
                                                                         unsigned        pending_bytes)
{
  return get_dl_sched_context(u, pdcch_slot, pdsch_slot, interleaving_enabled, nullptr, pending_bytes);
}

std::optional<dl_sched_context> sched_helper::get_retx_dl_sched_context(const slice_ue& u,
                                                                        slot_point      pdcch_slot,
                                                                        slot_point      pdsch_slot,
                                                                        bool            interleaving_enabled,
                                                                        const dl_harq_process_handle& h_dl)
{
  return get_dl_sched_context(u, pdcch_slot, pdsch_slot, interleaving_enabled, &h_dl, 0);
}

static vrb_interval
find_available_vrbs(const dl_sched_context& space_cfg, const vrb_bitmap& used_vrbs, unsigned max_rbs = MAX_NOF_PRBS)
{
  // Compute recommended number of layers, MCS and PRBs.
  unsigned nof_rbs = std::min(space_cfg.expected_nof_rbs, max_rbs);

  // Compute PRB allocation interval.
  return rb_helper::find_empty_interval_of_length(used_vrbs, nof_rbs);
}

vrb_interval sched_helper::compute_newtx_dl_vrbs(const dl_sched_context& decision_ctxt,
                                                 const vrb_bitmap&       used_vrbs,
                                                 unsigned                max_nof_rbs)
{
  return find_available_vrbs(decision_ctxt, used_vrbs, max_nof_rbs);
}

vrb_interval sched_helper::compute_retx_dl_vrbs(const dl_sched_context& decision_ctxt, const vrb_bitmap& used_vrbs)
{
  vrb_interval vrbs = find_available_vrbs(decision_ctxt, used_vrbs, decision_ctxt.expected_nof_rbs);
  if (vrbs.length() != decision_ctxt.expected_nof_rbs) {
    // In case of Retx, the #CRBs need to stay the same.
    return {};
  }
  return vrbs;
}

static std::optional<ul_sched_context> get_ul_sched_context(const slice_ue&               u,
                                                            slot_point                    pdcch_slot,
                                                            slot_point                    pusch_slot,
                                                            unsigned                      uci_nof_harq_bits,
                                                            const ul_harq_process_handle* h_ul,
                                                            unsigned                      pending_bytes)
{
  const ue_cell& ue_cc = u.get_cc();

  if (not ue_cc.is_pusch_enabled(pdcch_slot, pusch_slot) or ue_cc.is_in_fallback_mode()) {
    // The UE cannot be scheduled in the provided slots.
    return std::nullopt;
  }

  const ue_cell_configuration& ue_cell_cfg      = ue_cc.cfg();
  const cell_configuration&    cell_cfg         = ue_cell_cfg.cell_cfg_common;
  unsigned                     slot_nof_symbols = cell_cfg.get_nof_ul_symbol_per_slot(pusch_slot);

  // TODO: Support more search spaces.
  static constexpr search_space_id ue_ded_ss_id = to_search_space_id(2);
  const search_space_info&         ss           = ue_cc.cfg().search_space(ue_ded_ss_id);

  if (h_ul != nullptr) {
    // ReTx case.
    if (slot_nof_symbols < h_ul->get_grant_params().nof_symbols) {
      // Early exit if there are not enough symbols in the slot for the retransmission.
      return std::nullopt;
    }
    srsran_assert(ss.get_ul_dci_format() == get_dci_format(h_ul->get_grant_params().dci_cfg_type),
                  "DCI type cannot change across reTxs");
  }

  // Determine RB allocation limits.
  interval<unsigned> nof_rb_lims = cell_cfg.expert_cfg.ue.pusch_nof_rbs &
                                   ue_cell_cfg.rrm_cfg().pusch_grant_size_limits.convert_to<interval<unsigned>>();
  const auto crb_lims = cell_cfg.expert_cfg.ue.pusch_crb_limits & ss.ul_crb_lims;
  const auto prb_lims = crb_to_prb(ss.ul_crb_lims, crb_lims);
  const auto vrb_lims = prb_lims.convert_to<vrb_interval>();
  nof_rb_lims         = nof_rb_lims & interval<unsigned>{0, vrb_lims.length()};
  if (nof_rb_lims.empty()) {
    // Invalid RB allocation range.
    return std::nullopt;
  }

  for (unsigned pusch_td_index = 0, e = ss.pusch_time_domain_list.size(); pusch_td_index != e; ++pusch_td_index) {
    const pusch_time_domain_resource_allocation& pusch_td_res = ss.pusch_time_domain_list[pusch_td_index];

    // Check that k2 matches the chosen PUSCH slot
    if (pdcch_slot + pusch_td_res.k2 + cell_cfg.ntn_cs_koffset != pusch_slot) {
      continue;
    }

    // If it is a retx, we need to ensure we use a time_domain_resource with the same number of symbols as used for
    // the first transmission.
    if (h_ul != nullptr and pusch_td_res.symbols.length() != h_ul->get_grant_params().nof_symbols) {
      continue;
    }

    // Check whether PUSCH time domain resource fits in UL symbols of the slot.
    if (cell_cfg.tdd_cfg_common.has_value() and
        not get_active_tdd_ul_symbols(cell_cfg.tdd_cfg_common.value(), pusch_slot.slot_index(), cyclic_prefix::NORMAL)
                .contains(pusch_td_res.symbols)) {
      continue;
    }

    // Compute recommended number of layers, MCS and PRBs.
    sch_mcs_index       mcs;
    pusch_config_params pusch_cfg;
    unsigned            nof_rbs;
    const bool          is_csi_report_slot = ue_cell_cfg.csi_meas_cfg() != nullptr and
                                    csi_helper::is_csi_reporting_slot(*ue_cell_cfg.csi_meas_cfg(), pusch_slot);
    if (h_ul == nullptr) {
      // NewTx Case.
      dci_ul_rnti_config_type dci_type = ss.get_ul_dci_format() == dci_ul_format::f0_0
                                             ? dci_ul_rnti_config_type::c_rnti_f0_0
                                             : dci_ul_rnti_config_type::c_rnti_f0_1;
      // Note: We assume k2 <= k1, which means that all the HARQ bits are set at this point for this UL slot and UE.
      pusch_cfg =
          compute_newtx_pusch_config_params(ue_cc, dci_type, pusch_td_res, uci_nof_harq_bits, is_csi_report_slot);
      auto mcs_prbs_sel = compute_newtx_required_mcs_and_prbs(pusch_cfg, ue_cc, pending_bytes, nof_rb_lims);
      if (not mcs_prbs_sel.has_value()) {
        return std::nullopt;
      }
      mcs     = mcs_prbs_sel->mcs;
      nof_rbs = mcs_prbs_sel->nof_prbs;
    } else {
      // ReTx Case.
      mcs     = h_ul->get_grant_params().mcs;
      nof_rbs = h_ul->get_grant_params().rbs.type1().length();
      if (nof_rbs > nof_rb_lims.stop()) {
        continue;
      }
      pusch_cfg = compute_retx_pusch_config_params(ue_cc, *h_ul, pusch_td_res, uci_nof_harq_bits, is_csi_report_slot);

      // Compute if effective code rate does not go over the limit for this reTx, for instance, due to presence of UCI.
      // Note: We take the conservative approach of assuming the reTx will intersect the DC.
      static constexpr bool contains_dc = true;
      auto                  tbs_res     = compute_ul_tbs(pusch_cfg, ue_cc.active_bwp(), mcs, nof_rbs, contains_dc);
      if (not tbs_res.has_value() or tbs_res.value() != h_ul->get_grant_params().tbs_bytes) {
        // Unable to keep the same TBS for PUSCH reTx.
        continue;
      }
    }

    // Successful selection of grant parameters.
    ul_sched_context ctxt;
    ctxt.ss_id              = ss.cfg->get_id();
    ctxt.pusch_td_res_index = pusch_td_index;
    ctxt.vrb_lims           = vrb_lims;
    ctxt.nof_rb_lims        = nof_rb_lims;
    ctxt.recommended_mcs    = mcs;
    ctxt.expected_nof_rbs   = nof_rbs;
    ctxt.pusch_cfg          = pusch_cfg;
    return ctxt;
  }

  return std::nullopt;
}

std::optional<ul_sched_context> sched_helper::get_newtx_ul_sched_context(const slice_ue& u,
                                                                         slot_point      pdcch_slot,
                                                                         slot_point      pusch_slot,
                                                                         unsigned        uci_nof_harq_bits,
                                                                         unsigned        pending_bytes)
{
  return get_ul_sched_context(u, pdcch_slot, pusch_slot, uci_nof_harq_bits, nullptr, pending_bytes);
}

std::optional<ul_sched_context> sched_helper::get_retx_ul_sched_context(const slice_ue&               u,
                                                                        slot_point                    pdcch_slot,
                                                                        slot_point                    pusch_slot,
                                                                        unsigned                      uci_nof_harq_bits,
                                                                        const ul_harq_process_handle& h_ul)
{
  return get_ul_sched_context(u, pdcch_slot, pusch_slot, uci_nof_harq_bits, &h_ul, 0);
}

static vrb_interval
find_available_vrbs(const ul_sched_context& sched_ctxt, const vrb_bitmap& used_vrbs, unsigned max_rbs = MAX_NOF_PRBS)
{
  // Compute recommended number of layers, MCS and VRBs.
  unsigned nof_rbs = std::min(sched_ctxt.expected_nof_rbs, max_rbs);
  nof_rbs          = sched_ctxt.nof_rb_lims.clamp(nof_rbs);

  // Compute VRB allocation interval.
  vrb_interval vrbs = rb_helper::find_empty_interval_of_length(used_vrbs, nof_rbs, sched_ctxt.vrb_lims);
  if (vrbs.empty()) {
    return vrb_interval{};
  }

  if (sched_ctxt.pusch_cfg.use_transform_precoder) {
    // At this point we need to ensure a valid number of RBs is selected to be used with transform precoding.
    auto valid_nof_rbs = transform_precoding::get_nof_prbs_lower_bound(vrbs.length());
    if (not valid_nof_rbs.has_value()) {
      return vrb_interval{};
    }
    vrbs.resize(valid_nof_rbs.value());
  }

  // Successful VRB interval derivation.
  return vrbs;
}

vrb_interval sched_helper::compute_newtx_ul_vrbs(const ul_sched_context& decision_ctxt,
                                                 const vrb_bitmap&       used_vrbs,
                                                 unsigned                max_nof_rbs)
{
  return find_available_vrbs(decision_ctxt, used_vrbs, max_nof_rbs);
}

vrb_interval sched_helper::compute_retx_ul_vrbs(const ul_sched_context& decision_ctxt, const vrb_bitmap& used_vrbs)
{
  vrb_interval vrbs = find_available_vrbs(decision_ctxt, used_vrbs, decision_ctxt.expected_nof_rbs);
  if (vrbs.length() != decision_ctxt.expected_nof_rbs) {
    // In case of Retx, the #VRBs need to stay the same.
    return vrb_interval{};
  }
  return vrbs;
}
