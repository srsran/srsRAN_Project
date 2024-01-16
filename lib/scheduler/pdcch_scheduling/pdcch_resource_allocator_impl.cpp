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

#include "pdcch_resource_allocator_impl.h"
#include "../support/pdcch/pdcch_mapping.h"
#include "../support/pdcch/pdcch_pdu_parameters.h"
#include "pdcch_slot_resource_allocator.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"
#include "srsran/ran/pdcch/pdcch_candidates.h"

using namespace srsran;

pdcch_resource_allocator_impl::pdcch_resource_allocator_impl(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_)
{
  for (const search_space_configuration& ss : cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
    pdcch_common_candidates.emplace(ss.get_id());
    const coreset_configuration& cs_cfg = (ss.get_coreset_id() == to_coreset_id(0))
                                              ? (*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0)
                                              : cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();
    for (unsigned lidx = 0; lidx != NOF_AGGREGATION_LEVELS; ++lidx) {
      const aggregation_level aggr_lvl            = aggregation_index_to_level(lidx);
      const unsigned          nof_candidates      = ss.get_nof_candidates()[lidx];
      pdcch_candidate_info&   aggr_lvl_candidates = pdcch_common_candidates[ss.get_id()][lidx];

      aggr_lvl_candidates.candidates = pdcch_candidates_common_ss_get_lowest_cce(
          pdcch_candidates_common_ss_configuration{aggr_lvl, nof_candidates, cs_cfg.get_nof_cces()});
      aggr_lvl_candidates.candidate_crbs.resize(aggr_lvl_candidates.candidates.size());
      for (unsigned i = 0; i != aggr_lvl_candidates.candidates.size(); ++i) {
        aggr_lvl_candidates.candidate_crbs[i] = pdcch_helper::cce_to_prb_mapping(
            cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, cs_cfg, cell_cfg.pci, aggr_lvl, i);

        // Convert PRBs to CRBs.
        for (uint16_t& prb_idx : aggr_lvl_candidates.candidate_crbs[i]) {
          prb_idx = crb_to_prb(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params, prb_idx);
        }
      }
    }
  }

  for (unsigned i = 0; i < SLOT_ALLOCATOR_RING_SIZE; ++i) {
    slot_records[i] = std::make_unique<pdcch_slot_allocator>();
  }
}

pdcch_resource_allocator_impl::~pdcch_resource_allocator_impl() = default;

void pdcch_resource_allocator_impl::slot_indication(slot_point sl_tx)
{
  srsran_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  // Clear old records.
  slot_records[(last_sl_ind - 1).to_uint() % slot_records.size()]->clear();
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                           rnti_t                        rnti,
                                                                           search_space_id               ss_id,
                                                                           aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[(size_t)ss_id];
  const coreset_configuration* cs_cfg = nullptr;
  if (ss_cfg.get_coreset_id() == to_coreset_id(0)) {
    cs_cfg = &(*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0);
  } else {
    cs_cfg = &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();
  }

  return alloc_dl_pdcch_helper(slot_alloc,
                               rnti,
                               bwp_cfg,
                               *cs_cfg,
                               ss_cfg,
                               aggr_lvl,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidates,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidate_crbs);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                                                           rnti_t                        rnti,
                                                                           search_space_id               ss_id,
                                                                           aggregation_level             aggr_lvl)
{
  // Find Common BWP and CORESET configurations.
  const bwp_configuration&          bwp_cfg = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  const search_space_configuration& ss_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces[(size_t)ss_id];
  const coreset_configuration* cs_cfg = (ss_cfg.get_coreset_id() == to_coreset_id(0))
                                            ? &(*cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0)
                                            : &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value();

  return alloc_ul_pdcch_helper(slot_alloc,
                               rnti,
                               bwp_cfg,
                               *cs_cfg,
                               ss_cfg,
                               aggr_lvl,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidates,
                               pdcch_common_candidates[ss_id][to_aggregation_level_index(aggr_lvl)].candidate_crbs);
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const search_space_info&   ss_cfg         = user.search_space(ss_id);
  const bwp_configuration&   bwp_cfg        = ss_cfg.bwp->dl_common->generic_params;
  span<const uint8_t>        candidates     = ss_cfg.get_pdcch_candidates(aggr_lvl, slot_alloc.slot);
  span<const crb_index_list> candidate_crbs = ss_cfg.get_crb_list_of_pdcch_candidates(aggr_lvl, slot_alloc.slot);

  return alloc_dl_pdcch_helper(
      slot_alloc, rnti, bwp_cfg, *ss_cfg.coreset, *ss_cfg.cfg, aggr_lvl, candidates, candidate_crbs);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                                                       rnti_t                        rnti,
                                                                       const ue_cell_configuration&  user,
                                                                       search_space_id               ss_id,
                                                                       aggregation_level             aggr_lvl)
{
  // Find Common or UE-specific BWP and CORESET configurations.
  const search_space_info&         ss_cfg         = user.search_space(ss_id);
  const bwp_configuration&         bwp_cfg        = ss_cfg.bwp->ul_common->generic_params;
  span<const pdcch_candidate_type> candidates     = ss_cfg.get_pdcch_candidates(aggr_lvl, slot_alloc.slot);
  span<const crb_index_list>       candidate_crbs = ss_cfg.get_crb_list_of_pdcch_candidates(aggr_lvl, slot_alloc.slot);

  return alloc_ul_pdcch_helper(
      slot_alloc, rnti, bwp_cfg, *ss_cfg.coreset, *ss_cfg.cfg, aggr_lvl, candidates, candidate_crbs);
}

pdcch_ul_information* pdcch_resource_allocator_impl::alloc_ul_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const coreset_configuration&      cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl,
                                                                           span<const pdcch_candidate_type>  candidates,
                                                                           span<const crb_index_list> candidate_crbs)
{
  if (not pdcch_helper::is_pdcch_monitoring_active(slot_alloc.slot, ss_cfg)) {
    // PDCCH monitoring is not active in this slot.
    return nullptr;
  }

  // Verify RNTI is unique.
  for (const pdcch_ul_information& pdcch : slot_alloc.result.dl.ul_pdcchs) {
    if (pdcch.ctx.rnti == rnti) {
      return nullptr;
    }
  }

  // Create PDCCH list element.
  pdcch_ul_information& pdcch = slot_alloc.result.dl.ul_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg;
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0; // Note: Value is set during the PDCCH allocation step.
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg.pci, cs_cfg, ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg, ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg.pci, cs_cfg);
  pdcch.ctx.context.ss_id     = ss_cfg.get_id();
  pdcch.ctx.context.dci_format =
      (ss_cfg.is_common_search_space() ||
       (variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats()) ==
        search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0))
          ? "0_0"
          : "0_1";

  // Allocate a position for UL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, ss_cfg, candidates, candidate_crbs)) {
    slot_alloc.result.dl.ul_pdcchs.pop_back();
    return nullptr;
  }
  return &pdcch;
}

pdcch_dl_information* pdcch_resource_allocator_impl::alloc_dl_pdcch_helper(cell_slot_resource_allocator&     slot_alloc,
                                                                           rnti_t                            rnti,
                                                                           const bwp_configuration&          bwp_cfg,
                                                                           const coreset_configuration&      cs_cfg,
                                                                           const search_space_configuration& ss_cfg,
                                                                           aggregation_level                 aggr_lvl,
                                                                           span<const pdcch_candidate_type>  candidates,
                                                                           span<const crb_index_list> candidate_crbs)
{
  if (not pdcch_helper::is_pdcch_monitoring_active(slot_alloc.slot, ss_cfg)) {
    // PDCCH monitoring is not active in this slot.
    return nullptr;
  }

  // Verify RNTI is unique.
  for (const pdcch_dl_information& pdcch : slot_alloc.result.dl.dl_pdcchs) {
    if (pdcch.ctx.rnti == rnti) {
      return nullptr;
    }
  }

  // Create PDCCH list element.
  pdcch_dl_information& pdcch = slot_alloc.result.dl.dl_pdcchs.emplace_back();
  pdcch.ctx.bwp_cfg           = &bwp_cfg;
  pdcch.ctx.coreset_cfg       = &cs_cfg;
  pdcch.ctx.rnti              = rnti;
  pdcch.ctx.cces.ncce         = 0;
  // [Implementation-defined] We allocate the DCI on the SearchSpace starting from symbols 0.
  pdcch.ctx.starting_symbol   = 0;
  pdcch.ctx.cces.aggr_lvl     = aggr_lvl;
  pdcch.ctx.n_id_pdcch_data   = get_scrambling_n_ID(cell_cfg.pci, cs_cfg, ss_cfg);
  pdcch.ctx.n_rnti_pdcch_data = get_scrambling_n_RNTI(rnti, cs_cfg, ss_cfg);
  pdcch.ctx.n_id_pdcch_dmrs   = get_N_ID_dmrs(cell_cfg.pci, cs_cfg);
  pdcch.ctx.context.ss_id     = ss_cfg.get_id();
  pdcch.ctx.context.dci_format =
      (ss_cfg.is_common_search_space() ||
       (variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats()) ==
        search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0))
          ? "1_0"
          : "1_1";

  // Allocate a position for DL PDCCH in CORESET.
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  if (not pdcch_alloc.alloc_pdcch(pdcch.ctx, slot_alloc, ss_cfg, candidates, candidate_crbs)) {
    slot_alloc.result.dl.dl_pdcchs.pop_back();
    return nullptr;
  }
  return &pdcch;
}

bool pdcch_resource_allocator_impl::cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc)
{
  pdcch_slot_allocator& pdcch_alloc = get_pdcch_slot_alloc(slot_alloc.slot);
  return pdcch_alloc.cancel_last_pdcch(slot_alloc);
}

pdcch_slot_allocator& pdcch_resource_allocator_impl::get_pdcch_slot_alloc(slot_point sl)
{
  srsran_sanity_check(sl < last_sl_ind + SLOT_ALLOCATOR_RING_SIZE, "PDCCH being allocated to far into the future");
  return *slot_records[sl.to_uint() % slot_records.size()];
}
