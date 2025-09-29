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

#include "pucch_allocator_impl.h"
#include "../support/pucch/pucch_default_resource.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/srslog/srslog.h"
#include <algorithm>
#include <variant>

//////////////     Helper functions       //////////////

using namespace srsran;

static unsigned get_n_id0_scrambling(const ue_cell_configuration& ue_cell_cfg, unsigned cell_pci)
{
  // As per TS 38.211, Section 6.4.1.3.2.1, N_{ID}^0 is given by the higher-layer parameter scramblingID0 in the
  // DMRS-UplinkConfig IE if provided and by N_{ID}^{cell} otherwise. If a UE is configured with both
  // dmrs-UplinkForPUSCH-MappingTypeA and dmrs-UplinkForPUSCH-MappingTypeB, scramblingID0 is obtained from
  // dmrs-UplinkForPUSCH-MappingTypeB.

  // Check \c scrambling_id0 in \c dmrs-UplinkForPUSCH-MappingTypeB, first
  const auto& pusch_cfg = *ue_cell_cfg.init_bwp().ul_ded->pusch_cfg;
  if (pusch_cfg.pusch_mapping_type_b_dmrs.has_value() and
      pusch_cfg.pusch_mapping_type_b_dmrs.value().trans_precoder_disabled.has_value() and
      pusch_cfg.pusch_mapping_type_b_dmrs.value().trans_precoder_disabled.value().scrambling_id0.has_value()) {
    return pusch_cfg.pusch_mapping_type_b_dmrs.value().trans_precoder_disabled.value().scrambling_id0.value();
  }
  // Else, check \c scrambling_id0 in \c dmrs-UplinkForPUSCH-MappingTypeA.
  if (pusch_cfg.pusch_mapping_type_a_dmrs.has_value() and
      pusch_cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_disabled.has_value() and
      pusch_cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_disabled.value().scrambling_id0.has_value()) {
    return pusch_cfg.pusch_mapping_type_a_dmrs.value().trans_precoder_disabled.value().scrambling_id0.value();
  }

  return cell_pci;
}

static pucch_resource* get_sr_pucch_res_cfg(const pucch_config& pucch_cfg)
{
  const auto&    pucch_res_list  = pucch_cfg.pucch_res_list;
  const unsigned sr_pucch_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;
  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [sr_pucch_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == sr_pucch_res_id;
      });

  return res_cfg != pucch_res_list.end() ? const_cast<pucch_resource*>(res_cfg) : nullptr;
}

static void mark_pucch_in_resource_grid(cell_slot_resource_allocator&    pucch_slot_alloc,
                                        const grant_info&                first_hop_grant,
                                        const std::optional<grant_info>& second_hop_grant,
                                        const crb_interval&              ul_bwp_crbs,
                                        const scheduler_expert_config&   expert_cfg)
{
  const unsigned guard_rbs = expert_cfg.ue.min_pucch_pusch_prb_distance;

  if (guard_rbs == 0) {
    pucch_slot_alloc.ul_res_grid.fill(first_hop_grant);
    if (second_hop_grant.has_value()) {
      pucch_slot_alloc.ul_res_grid.fill(second_hop_grant.value());
    }
    return;
  }

  // Add guard band to the allocated grid resources to minimize cross PUCCH-PUSCH interference.
  auto     grant_extended = first_hop_grant;
  unsigned start          = first_hop_grant.crbs.start() >= guard_rbs ? first_hop_grant.crbs.start() - guard_rbs : 0;
  grant_extended.crbs     = {start, first_hop_grant.crbs.stop() + guard_rbs};
  grant_extended.crbs.intersect(ul_bwp_crbs);
  pucch_slot_alloc.ul_res_grid.fill(grant_extended);

  if (second_hop_grant.has_value()) {
    start               = second_hop_grant->crbs.start() >= guard_rbs ? second_hop_grant->crbs.start() - guard_rbs : 0;
    grant_extended      = *second_hop_grant;
    grant_extended.crbs = {start, second_hop_grant->crbs.stop() + guard_rbs};
    grant_extended.crbs.intersect(ul_bwp_crbs);
    pucch_slot_alloc.ul_res_grid.fill(grant_extended);
  }
}

static std::pair<grant_info, std::optional<grant_info>>
pucch_resource_to_grant_info(const bwp_configuration& init_ul_bwp, const pucch_resource& pucch_res)
{
  unsigned nof_prbs = 1;
  if (const auto* format_2_3 = std::get_if<pucch_format_2_3_cfg>(&pucch_res.format_params)) {
    nof_prbs = format_2_3->nof_prbs;
  }

  if (pucch_res.second_hop_prb.has_value()) {
    crb_interval first_hop_crbs = prb_to_crb(init_ul_bwp, {pucch_res.starting_prb, pucch_res.starting_prb + nof_prbs});
    ofdm_symbol_range first_hop_symbols{pucch_res.starting_sym_idx,
                                        pucch_res.starting_sym_idx + pucch_res.nof_symbols / 2};
    crb_interval      second_hop_crbs =
        prb_to_crb(init_ul_bwp, {pucch_res.second_hop_prb.value(), pucch_res.second_hop_prb.value() + nof_prbs});
    ofdm_symbol_range second_hop_symbols{pucch_res.starting_sym_idx + pucch_res.nof_symbols / 2,
                                         pucch_res.starting_sym_idx + pucch_res.nof_symbols};
    return {grant_info{init_ul_bwp.scs, first_hop_symbols, first_hop_crbs},
            grant_info{init_ul_bwp.scs, second_hop_symbols, second_hop_crbs}};
  }
  ofdm_symbol_range symbols{pucch_res.starting_sym_idx, pucch_res.starting_sym_idx + pucch_res.nof_symbols};
  crb_interval      crbs = prb_to_crb(init_ul_bwp, {pucch_res.starting_prb, pucch_res.starting_prb + nof_prbs});
  return {grant_info{init_ul_bwp.scs, symbols, crbs}, std::nullopt};
}

static void mark_pucch_in_resource_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                        const pucch_resource&         pucch_res,
                                        const ue_cell_configuration&  ue_cell_cfg)
{
  const auto& init_ul_bwp = ue_cell_cfg.init_bwp().ul_common.value()->generic_params;
  const auto  grants      = pucch_resource_to_grant_info(init_ul_bwp, pucch_res);

  // Fill Slot grid.
  mark_pucch_in_resource_grid(
      pucch_slot_alloc, grants.first, grants.second, init_ul_bwp.crbs, ue_cell_cfg.cell_cfg_common.expert_cfg);
}

//////////////    Public functions       //////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_,
                                           unsigned                  max_pucchs_per_slot,
                                           unsigned                  max_ul_grants_per_slot_) :
  cell_cfg(cell_cfg_),
  max_pucch_grants_per_slot(max_pucchs_per_slot),
  max_ul_grants_per_slot(max_ul_grants_per_slot_),
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

std::optional<unsigned> pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    res_alloc,
                                                                             rnti_t                      tcrnti,
                                                                             unsigned                    k0,
                                                                             unsigned                    k1,
                                                                             const pdcch_dl_information& dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc  = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  auto&                         pucch_grants_slot = pucch_grants_alloc_grid[pucch_slot_alloc.slot.to_uint()];

  auto* grants_ue_it = std::find_if(pucch_grants_slot.begin(),
                                    pucch_grants_slot.end(),
                                    [tcrnti](const ue_grants& grants) { return grants.rnti == tcrnti; });
  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
          get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size())) or
      (grants_ue_it == pucch_grants_slot.end() and pucch_grants_slot.full())) {
    return std::nullopt;
  }

  if (not cell_cfg.is_fully_ul_enabled(pucch_slot_alloc.slot)) {
    return std::nullopt;
  }

  // If there are existing PUCCH grants that are either F2/F3/F4 for CSI or F0/F1 for SR, allocate the PUCCH common
  // grant anyway without multiplexing it with the existing one. Otherwise, if the existing grant is F0/F1 for HARQ-ACK,
  // do not allocate on the same slot.
  const bool has_existing_ded_harq_grants =
      grants_ue_it != pucch_grants_slot.end() and grants_ue_it->pucch_grants.harq_resource.has_value();
  const bool has_existing_common_grants = grants_ue_it != pucch_grants_slot.end() and grants_ue_it->has_common_pucch;
  if (has_existing_ded_harq_grants or has_existing_common_grants) {
    logger.debug("tc-rnti={}: PUCCH common not allocated for slot={}. Cause: A PUCCH grant with HARQ-ACK bits "
                 "already exists in the same slot",
                 tcrnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  // Get the PUCCH resources from default tables.
  std::optional<pucch_res_alloc_cfg> pucch_res = alloc_pucch_common_res_harq(pucch_slot_alloc, dci_info.ctx);

  // No resources available for PUCCH.
  if (not pucch_res.has_value()) {
    logger.debug("tc-rnti={}: PUCCH not allocated due to resources not available", tcrnti);
    return std::nullopt;
  }

  // Fill Slot grid.
  mark_pucch_in_resource_grid(pucch_slot_alloc,
                              pucch_res->first_hop_res,
                              pucch_res->second_hop_res,
                              cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs,
                              cell_cfg.expert_cfg);

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_info, tcrnti, pucch_res.value());
  unsigned pucch_res_indicator = pucch_res.value().pucch_res_indicator;

  if (grants_ue_it != pucch_grants_slot.end()) {
    grants_ue_it->has_common_pucch = true;
  } else {
    pucch_grants_slot.emplace_back(ue_grants{.rnti = tcrnti, .has_common_pucch = true});
  }

  return pucch_res_indicator;
}

std::optional<unsigned> pucch_allocator_impl::alloc_common_and_ded_harq_res(cell_resource_allocator&     res_alloc,
                                                                            rnti_t                       rnti,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            unsigned                     k0,
                                                                            unsigned                     k1,
                                                                            const pdcch_dl_information&  dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];
  slot_point                    pucch_slot       = pucch_slot_alloc.slot;

  if (not cell_cfg.is_fully_ul_enabled(pucch_slot)) {
    return std::nullopt;
  }

  auto& pucch_grants = pucch_grants_alloc_grid[pucch_slot.to_uint()];
  auto* ue_grants_it = std::find_if(
      pucch_grants.begin(), pucch_grants.end(), [rnti](const ue_grants& grants) { return grants.rnti == rnti; });

  // NOTE: this function is called by the UE fallback scheduler, which iterates over several PDCCH slots and different
  // k1 values. It can happen that the UE fallback scheduler attempts to allocate a grant on a slot where it previously
  // allocated another grant. If that is the case, quit the PUCCH allocation.
  const bool has_existing_ded_harq_grants =
      ue_grants_it != pucch_grants.end() and ue_grants_it->pucch_grants.harq_resource.has_value();
  const bool has_existing_common_grants = ue_grants_it != pucch_grants.end() and ue_grants_it->has_common_pucch;
  if (has_existing_ded_harq_grants or has_existing_common_grants) {
    logger.debug("rnti={}: PUCCH HARQ-ACK for slot={} not allocated. Cause: another PUCCH grant with HARQ-ACK bits "
                 "already exists for the same UE for the same slot",
                 rnti,
                 pucch_slot);
    return std::nullopt;
  }

  srsran_assert(not(ue_grants_it != pucch_grants.end() and ue_grants_it->pucch_grants.sr_resource.has_value() and
                    ue_grants_it->pucch_grants.csi_resource.has_value()),
                "It is expected that there are either no grants, or at most 1 PUCCH grant (SR grant or CSI grant)");

  // If there are no existing grants or if the existing one is for SR with Format 0/1, we need to add 2 additional PUCCH
  // grants: 1 on common resources and 1 on dedicated resources (for HARQ-ACK bit).
  // In the case of CSI, the number of PUCCH additional grants depends on the PUCCH resources configuration, but it will
  // never be more than 2. The exact number cannot be known at this point.
  unsigned extra_pucch_grants_to_allocate = 2;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() + extra_pucch_grants_to_allocate >
          pucch_slot_alloc.result.ul.pucchs.capacity() or
      pucch_slot_alloc.result.ul.pucchs.size() + extra_pucch_grants_to_allocate >
          get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size())) or
      (ue_grants_it == pucch_grants.end() and pucch_grants.full())) {
    return std::nullopt;
  }

  const bool new_ue_grant_added = ue_grants_it == pucch_grants.end();

  // Keep track of whether a new ue_grant has been added; in case the function fails, remove it before exiting.
  if (new_ue_grant_added) {
    pucch_grants.emplace_back(ue_grants{.rnti = rnti});
  }
  ue_grants& current_grants = new_ue_grant_added ? pucch_grants.back() : *ue_grants_it;

  // Find a couple of PUCCH resources (1 common, 1 dedicated) that are (i) are available and that (ii) have the same
  // PUCCH resource indicator.
  std::optional<pucch_common_params> pucch_common_info =
      find_common_and_ded_harq_res_available(pucch_slot_alloc, current_grants, rnti, ue_cell_cfg, dci_info.ctx);

  if (pucch_common_info.has_value()) {
    compute_pucch_common_params_and_alloc(pucch_slot_alloc, rnti, pucch_common_info.value());
    return pucch_common_info.value().pucch_res_indicator;
  }

  if (new_ue_grant_added) {
    pucch_grants.pop_back();
  }

  logger.debug(
      "rnti={}: PUCCH HARQ-ACK for slot={} not allocated. Cause: no res_indicator available for both common and "
      "ded. PUCCH resources",
      rnti,
      pucch_slot_alloc.slot);
  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                                          rnti_t                       crnti,
                                                                          const ue_cell_configuration& ue_cell_cfg,
                                                                          unsigned                     k0,
                                                                          unsigned                     k1)
{
  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];

  // The PUCCH allocation may result in a temporary reservation of PUCCH resources, which need to be released in case of
  // failure or in case the multiplexing results in a different final PUCCH resource. If we don't reset the previous
  // record, we could release the resources that have been allocated for other UEs of allocated for this UE in a
  // different slot.
  resource_manager.reset_latest_reserved_res_tracker();

  slot_point sl_ack = pucch_slot_alloc.slot;

  auto& ue_pucchs = pucch_grants_alloc_grid[sl_ack.to_uint()];

  auto* existing_grant_it =
      std::find_if(ue_pucchs.begin(), ue_pucchs.end(), [crnti](const ue_grants& pucch) { return pucch.rnti == crnti; });

  // Allocate PUCCH HARQ-ACK grant depending on whether there existing PUCCH grants.
  if (existing_grant_it != ue_pucchs.end()) {
    // Multiplexing of multiple HARQ-ACK bits in a PUCCH common grant is not allowed.
    if (existing_grant_it->has_common_pucch) {
      logger.debug(
          "rnti={}: PUCCH HARQ-ACK for slot={} not allocated. Cause: The UE has a PUCCH common grant at the same slot",
          crnti,
          sl_ack);
      return std::nullopt;
    }

    const pucch_uci_bits current_uci_bits = existing_grant_it->pucch_grants.get_uci_bits();
    pucch_uci_bits       new_bits         = current_uci_bits;
    ++new_bits.harq_ack_nof_bits;

    // As per TS 38.213, Section 9.2.1, we can infer that, when the HARQ-ACK bit count changes from 1 to 2, or when it
    // changes from 3 to more than 3, the PUCCH resource for the HARQ-ACK is the same as the previous one; this means
    // that the multiplexing would yield the same result as in the last UE's allocation; in this case, we skip the
    // multiplexing. Refer to paragraph "If the UE transmits O_UCI UCI information bits, that include HARQ-ACK
    // information bits, the UE determines a PUCCH resource set to be ...".
    const bool is_multiplexing_needed =
        not((current_uci_bits.harq_ack_nof_bits == 1U and new_bits.harq_ack_nof_bits == 2U) or
            (current_uci_bits.harq_ack_nof_bits == 3U and new_bits.harq_ack_nof_bits > 3U));

    std::optional<unsigned> pucch_res_ind =
        is_multiplexing_needed
            ? multiplex_and_allocate_pucch(pucch_slot_alloc, new_bits, *existing_grant_it, ue_cell_cfg, std::nullopt)
            : allocate_without_multiplexing(pucch_slot_alloc, new_bits, *existing_grant_it, ue_cell_cfg);

    if (not pucch_res_ind) {
      resource_manager.cancel_last_ue_res_reservations(sl_ack, crnti, ue_cell_cfg);
    }
    return pucch_res_ind;
  }
  return allocate_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg);
}

void pucch_allocator_impl::pucch_allocate_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                         rnti_t                        crnti,
                                                         const ue_cell_configuration&  ue_cell_cfg)
{
  const slot_point sl_tx = pucch_slot_alloc.slot;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: SR occasion allocation for slot={} skipped. Cause: max number of UL grants reached",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  if (pucch_grants_alloc_grid[sl_tx.to_uint()].full()) {
    logger.info("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH allocator grant list is full",
                crnti,
                pucch_slot_alloc.slot);
    return;
  }

  const auto* existing_grant_it = std::find_if(pucch_grants_alloc_grid[sl_tx.to_uint()].begin(),
                                               pucch_grants_alloc_grid[sl_tx.to_uint()].end(),
                                               [crnti](const ue_grants& ue) { return ue.rnti == crnti; });
  if (existing_grant_it != pucch_grants_alloc_grid[sl_tx.to_uint()].end()) {
    // Allocation of dedicated + common resources are handled by allocating PUCCH common on existing SR, not the other
    // way around. If the function enters the path, it means it too early to start scheduling the SR.
    if (existing_grant_it->has_common_pucch) {
      logger.info("rnti={}: SR occasion allocation for slot={} skipped. Cause: existing PUCCH common grant",
                  crnti,
                  pucch_slot_alloc.slot);
      return;
    }
    // NOTE: This check can be removed in future refactors, it's not required by the SR allocator. At the moment, we
    // schedule the SRs before anything else, therefore we don't expect to find any existing PUCCH grant.
    if (not existing_grant_it->pucch_grants.is_empty()) {
      logger.info("No PUCCH grants are expected before allocating a new SR grant", crnti, pucch_slot_alloc.slot);
      return;
    }
  }

  // Get the index of the PUCCH resource to be used for SR.
  const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("rnti={}: SR occasion allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  const pucch_format format = pucch_sr_res->format;

  // Allocate PUCCH SR grant only.
  static constexpr unsigned harq_ack_bits_increment = 0U;
  if (format == pucch_format::FORMAT_0) {
    fill_pucch_ded_format0_grant(pucch_slot_alloc.result.ul.pucchs.emplace_back(),
                                 crnti,
                                 *pucch_sr_res,
                                 harq_ack_bits_increment,
                                 sr_nof_bits::one);
  } else if (format == pucch_format::FORMAT_1) {
    fill_pucch_ded_format1_grant(pucch_slot_alloc.result.ul.pucchs.emplace_back(),
                                 crnti,
                                 *pucch_sr_res,
                                 harq_ack_bits_increment,
                                 sr_nof_bits::one);
  } else {
    srsran_assertion_failure("Only PUCCH Format 0 and Format 1 are supported for SR dedicated grant");
  }

  // TODO: unmark on multiplexing (take into account CS/OCC).
  mark_pucch_in_resource_grid(pucch_slot_alloc, *pucch_sr_res, ue_cell_cfg);

  // Save the info in the scheduler list of PUCCH grants.
  auto& sr_pucch_grant = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_grants{.rnti = crnti});
  sr_pucch_grant.pucch_grants.sr_resource.emplace(pucch_grant{.type = pucch_grant_type::sr});
  sr_pucch_grant.pucch_grants.sr_resource.value().set_res_config(*pucch_sr_res);
  sr_pucch_grant.pucch_grants.sr_resource.value().bits.sr_bits = sr_nof_bits::one;
}

void pucch_allocator_impl::pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          unsigned                      csi_part1_nof_bits)
{
  const slot_point sl_tx = pucch_slot_alloc.slot;

  // The PUCCH allocation may result in a temporary reservation of PUCCH resources, which need to be released in case of
  // failure or in case the multiplexing results in a different final PUCCH resource. If we don't reset the previous
  // record, we could release the resources that have been allocated for other UEs of allocated for this UE in a
  // different slot.
  resource_manager.reset_latest_reserved_res_tracker();

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: CSI occasion allocation for slot={} skipped. Cause: max number of UL grants reached",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  auto* existing_grant_it = std::find_if(pucch_grants_alloc_grid[sl_tx.to_uint()].begin(),
                                         pucch_grants_alloc_grid[sl_tx.to_uint()].end(),
                                         [crnti](const ue_grants& ue) { return ue.rnti == crnti; });

  // Handle case of no existing PUCCH grant.
  if (existing_grant_it == pucch_grants_alloc_grid[sl_tx.to_uint()].end()) {
    allocate_csi_grant(pucch_slot_alloc, crnti, ue_cell_cfg, csi_part1_nof_bits);
    return;
  }

  if (existing_grant_it->has_common_pucch) {
    // Allocation of dedicated + common resources are handled by allocating PUCCH common on existing CSI, not the
    // other way around. If the function enters the path, it means it too early to start scheduling the CSI.
    logger.info("rnti={}: CSI occasion allocation for slot={} skipped. Cause: There is a PUCCH common grant"
                "allocated at this slot",
                crnti,
                pucch_slot_alloc.slot);
    return;
  }

  // Handle case of existing PUCCHs with possible multiplexing.
  pucch_uci_bits bits_for_uci = existing_grant_it->pucch_grants.get_uci_bits();
  srsran_assert(bits_for_uci.csi_part1_nof_bits == 0, "PUCCH grant for CSI has already been allocated");
  bits_for_uci.csi_part1_nof_bits = csi_part1_nof_bits;
  auto alloc_outcome =
      multiplex_and_allocate_pucch(pucch_slot_alloc, bits_for_uci, *existing_grant_it, ue_cell_cfg, std::nullopt);
  if (not alloc_outcome.has_value()) {
    resource_manager.cancel_last_ue_res_reservations(sl_tx, crnti, ue_cell_cfg);
  }
}

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                                              rnti_t                        crnti,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  pucch_uci_bits removed_uci_info;

  slot_point          sl_uci    = slot_alloc.slot;
  const pucch_config& pucch_cfg = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value();

  // Get the PUCCH grants for the slot.
  auto& ue_pucchs = pucch_grants_alloc_grid[sl_uci.to_uint()];

  auto* grant_it = std::find_if(
      ue_pucchs.begin(), ue_pucchs.end(), [crnti](const ue_grants& grants) { return grants.rnti == crnti; });

  auto& pucch_pdus = slot_alloc.result.ul.pucchs;

  auto* pdu = std::find_if(
      pucch_pdus.begin(), pucch_pdus.end(), [crnti](const pucch_info& info) { return info.crnti == crnti; });
  if ((grant_it != ue_pucchs.end()) != (pdu != pucch_pdus.end())) {
    logger.error("rnti={}: Inconsistent PUCCH state: PUCCH grant list and PUCCH PDUs list are not in sync", crnti);
  }

  // Get the bits from the PUCCH grants and remove the item from the list.
  if (grant_it != ue_pucchs.end()) {
    // This function can be called for UE with dedicated PUCCH grants only.
    if (grant_it->has_common_pucch) {
      logger.error("rnti={}: unexpected call of this function for UE with PUCCH common grant allocated", crnti);
    }

    // Get the UCI bits.
    removed_uci_info = grant_it->pucch_grants.get_uci_bits();

    // Release the resources used in the PUCCH resource manager first.
    if (grant_it->pucch_grants.harq_resource.has_value()) {
      if (grant_it->pucch_grants.harq_resource.value().harq_id.pucch_set_idx == pucch_res_set_idx::set_0) {
        resource_manager.release_harq_set_0_resource(slot_alloc.slot, crnti, pucch_cfg);
      } else {
        resource_manager.release_harq_set_1_resource(slot_alloc.slot, crnti, pucch_cfg);
      }
    }
    if (grant_it->pucch_grants.sr_resource.has_value()) {
      resource_manager.release_sr_resource(slot_alloc.slot, crnti, pucch_cfg);
    }
    if (grant_it->pucch_grants.csi_resource.has_value()) {
      resource_manager.release_csi_resource(slot_alloc.slot, crnti, ue_cell_cfg);
    }

    ue_pucchs.erase(grant_it);
  }

  // Start from the (first) PUCCH PDU that was found above.
  for (auto* pdu_it = pdu; pdu_it != pucch_pdus.end();) {
    if (pdu_it->crnti == crnti) {
      pdu_it = pucch_pdus.erase(pdu_it);
    } else {
      ++pdu_it;
    }
  }

  return removed_uci_info;
}

void pucch_allocator_impl::slot_indication(slot_point sl_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  srsran_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  resource_manager.slot_indication(sl_tx);

  // Clear previous slot PUCCH grants allocations.
  pucch_grants_alloc_grid[(sl_tx - 1).to_uint()].clear();
}

void pucch_allocator_impl::stop()
{
  resource_manager.stop();
  for (auto& grants : pucch_grants_alloc_grid) {
    grants.clear();
  }
  last_sl_ind = {};
}

//////////////     Sub-class definitions       //////////////

void pucch_allocator_impl::pucch_grant::set_res_config(const pucch_resource& res_cfg)
{
  pucch_res_cfg = &res_cfg;
  format        = res_cfg.format;
  symbols.set(res_cfg.starting_sym_idx, res_cfg.starting_sym_idx + res_cfg.nof_symbols);
}

pucch_uci_bits pucch_allocator_impl::pucch_grant_list::get_uci_bits() const
{
  pucch_uci_bits bits;

  // If there is an SR grant, then the SR bits are retrieved from it. Otherwise, the SR bits are retrieved from the
  // HARQ or CSI grants; as HARQ and CSI grants can separate, we need to take the SR value that is non-zero, if any.
  if (sr_resource.has_value()) {
    bits.sr_bits = sr_resource->bits.sr_bits;
  } else {
    if (harq_resource.has_value()) {
      bits.sr_bits = harq_resource->bits.sr_bits;
    }
    if (csi_resource.has_value() and bits.sr_bits == sr_nof_bits::no_sr) {
      bits.sr_bits = csi_resource->bits.sr_bits;
    }
  }

  // If there is a CSI grant, then the CSI part 1 bits are retrieved from it. Otherwise, the HARQ bits are retrieved
  // from the HARQ grant (it can be zero).
  if (csi_resource.has_value()) {
    bits.csi_part1_nof_bits = csi_resource.value().bits.csi_part1_nof_bits;
  } else if (harq_resource.has_value()) {
    bits.csi_part1_nof_bits = harq_resource.value().bits.csi_part1_nof_bits;
  }

  // If there is an HARQ grant, then the HARQ bits are retrieved from it. Otherwise, they are retrieved from the
  // SR or CSI grants; as SR and CSI grants can separate, we need to take the HARQ bits value that is non-zero, if any.
  if (harq_resource.has_value()) {
    bits.harq_ack_nof_bits = harq_resource.value().bits.harq_ack_nof_bits;
  } else {
    if (sr_resource.has_value()) {
      bits.harq_ack_nof_bits = sr_resource.value().bits.harq_ack_nof_bits;
    }
    if (csi_resource.has_value() and bits.harq_ack_nof_bits == 0U) {
      bits.harq_ack_nof_bits = csi_resource.value().bits.harq_ack_nof_bits;
    }
  }

  return bits;
}

bool pucch_allocator_impl::pucch_grant_list::is_empty() const
{
  return not sr_resource.has_value() and not harq_resource.has_value() and not csi_resource.has_value();
}

unsigned pucch_allocator_impl::pucch_grant_list::get_nof_grants() const
{
  unsigned nof_grants = 0;
  if (harq_resource.has_value()) {
    ++nof_grants;
  }
  if (sr_resource.has_value()) {
    ++nof_grants;
  }
  if (csi_resource.has_value()) {
    ++nof_grants;
  }
  return nof_grants;
}

// Utility used by existing_pucch_pdus_handler to check if a PUCCH PDU is for SR.
static bool sr_id_match(const pucch_resource& pucch_res_cfg_lhs, const pucch_info& rhs)
{
  const bool prb_match =
      pucch_res_cfg_lhs.starting_prb == rhs.resources.prbs.start() and
      ((not pucch_res_cfg_lhs.second_hop_prb.has_value() and rhs.resources.second_hop_prbs.empty()) or
       (pucch_res_cfg_lhs.second_hop_prb.has_value() and pucch_res_cfg_lhs.second_hop_prb.value() and
        pucch_res_cfg_lhs.second_hop_prb == rhs.resources.second_hop_prbs.start()));
  const bool symb_match = pucch_res_cfg_lhs.starting_sym_idx == rhs.resources.symbols.start() and
                          pucch_res_cfg_lhs.nof_symbols == rhs.resources.symbols.length();
  const auto& f1_cfg       = std::get<pucch_format_1_cfg>(pucch_res_cfg_lhs.format_params);
  const auto& rhs_format_1 = std::get<pucch_format_1>(rhs.format_params);
  return prb_match && symb_match && f1_cfg.initial_cyclic_shift == rhs_format_1.initial_cyclic_shift &&
         f1_cfg.time_domain_occ == rhs_format_1.time_domain_occ;
}

namespace {

/// Contains the existing PUCCH grants currently allocated to a given UE.
class existing_pucch_pdus_handler
{
public:
  existing_pucch_pdus_handler(rnti_t crnti, span<pucch_info> pucchs, const pucch_resource* pucch_res_cfg);

  [[nodiscard]] bool     is_empty() const { return pdus_cnt == 0; }
  [[nodiscard]] unsigned get_nof_unallocated_pdu() const { return pdus_cnt; }
  pucch_info*            get_next_grant(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs);
  void remove_unused_pdus(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs, rnti_t rnti) const;
  void update_sr_pdu_bits(sr_nof_bits sr_bits, unsigned harq_ack_bits);
  void update_csi_pdu_bits(unsigned csi_part1_bits, sr_nof_bits sr_bits);
  void update_harq_pdu_bits(unsigned                                       harq_ack_bits,
                            sr_nof_bits                                    sr_bits,
                            unsigned                                       csi_part1_bits,
                            const pucch_resource&                          pucch_res_cfg,
                            const std::optional<pucch_common_all_formats>& common_params);

  pucch_info* sr_pdu{nullptr};
  pucch_info* harq_pdu{nullptr};
  pucch_info* csi_pdu{nullptr};

private:
  unsigned pdus_cnt = 0;
  unsigned pdu_id   = 0;
};

} // namespace

existing_pucch_pdus_handler::existing_pucch_pdus_handler(rnti_t                crnti,
                                                         span<pucch_info>      pucchs,
                                                         const pucch_resource* pucch_res_cfg)
{
  pdu_id = 0;
  for (auto& pucch : pucchs) {
    if (pucch.crnti == crnti and not pucch.pdu_context.is_common) {
      pucch.pdu_context.id = MAX_PUCCH_PDUS_PER_SLOT;

      if (pucch.format() == pucch_format::FORMAT_0) {
        // With Format 0, when there are both HARQ bits and SR bits in the same PDU (this means that the PUCCH HARQ
        // resource and SR resource have overlapping symbols), we only use the HARQ-ACK resource; the only case when the
        // SR PUCCH F0 is used is when there are only SR bits.
        if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr and pucch.uci_bits.harq_ack_nof_bits == 0U) {
          sr_pdu = &pucch;
          ++pdus_cnt;
        } else if (pucch.uci_bits.harq_ack_nof_bits != 0U and pucch.uci_bits.harq_ack_nof_bits <= 2U) {
          harq_pdu = &pucch;
          ++pdus_cnt;
        } else {
          srsran_assertion_failure("Invalid HARQ/SR bits for PUCCH Format 0");
        }
      } else if (pucch.format() == pucch_format::FORMAT_1) {
        if (pucch.uci_bits.sr_bits == sr_nof_bits::one and pucch_res_cfg != nullptr and
            sr_id_match(*pucch_res_cfg, pucch)) {
          sr_pdu = &pucch;
        } else {
          harq_pdu = &pucch;
        }
        ++pdus_cnt;
      } else {
        // Formats 2, 3 and 4.
        if (pucch.uci_bits.csi_part1_nof_bits != 0U and pucch.uci_bits.harq_ack_nof_bits == 0U) {
          csi_pdu = &pucch;
        } else {
          harq_pdu = &pucch;
        }
        ++pdus_cnt;
      }
    }
  }
}

pucch_info* existing_pucch_pdus_handler::get_next_grant(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs)
{
  if (is_empty()) {
    srsran_assert(not pucchs.full(), "PUCCH grants list is full");
    auto* new_pdu           = &pucchs.emplace_back();
    new_pdu->pdu_context.id = pdu_id++;
    return new_pdu;
  }
  pucch_info* ret_grant = nullptr;
  if (csi_pdu != nullptr) {
    ret_grant                 = csi_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  } else if (sr_pdu != nullptr) {
    ret_grant                 = sr_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  } else if (harq_pdu != nullptr) {
    ret_grant                 = harq_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  }
  // NOTE: this cannot be nullptr, otherwise the function would have exited at the previous return.
  return ret_grant;
}

void existing_pucch_pdus_handler::remove_unused_pdus(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs,
                                                     rnti_t                                              rnti) const
{
  if (pdus_cnt == 0) {
    return;
  }
  auto* it = pucchs.begin();
  while (it != pucchs.end()) {
    if (it->crnti == rnti and not it->pdu_context.is_common and it->pdu_context.id >= MAX_PUCCH_PDUS_PER_SLOT) {
      it = pucchs.erase(it);
    } else {
      ++it;
    }
  }
}

void existing_pucch_pdus_handler::update_sr_pdu_bits(sr_nof_bits sr_bits, unsigned harq_ack_bits)
{
  if (sr_pdu == nullptr) {
    return;
  }
  srsran_assert(sr_pdu->format() == pucch_format::FORMAT_0 or sr_pdu->format() == pucch_format::FORMAT_1,
                "Only PUCCH Formats 0 or 1 can be used for SR grant");

  sr_pdu->uci_bits.sr_bits           = sr_bits;
  sr_pdu->uci_bits.harq_ack_nof_bits = harq_ack_bits;
  sr_pdu->pdu_context.id             = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  sr_pdu = nullptr;
  --pdus_cnt;
}

void existing_pucch_pdus_handler::update_csi_pdu_bits(unsigned csi_part1_bits, sr_nof_bits sr_bits)
{
  srsran_assert(csi_pdu->format() == pucch_format::FORMAT_2 or csi_pdu->format() == pucch_format::FORMAT_3 or
                    csi_pdu->format() == pucch_format::FORMAT_4,
                "Only PUCCH Formats 2, 3 and 4 can be used for CSI grant");

  csi_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
  csi_pdu->uci_bits.sr_bits            = sr_bits;
  csi_pdu->pdu_context.id              = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  csi_pdu = nullptr;
  --pdus_cnt;
}

void existing_pucch_pdus_handler::update_harq_pdu_bits(unsigned                                       harq_ack_bits,
                                                       sr_nof_bits                                    sr_bits,
                                                       unsigned                                       csi_part1_bits,
                                                       const pucch_resource&                          pucch_res_cfg,
                                                       const std::optional<pucch_common_all_formats>& common_params)
{
  switch (harq_pdu->format()) {
    case pucch_format::FORMAT_2: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
      // After updating the UCI bits, we need to recompute the number of PRBs for PUCCH format 2, as per TS 38.213,
      // Section 9.2.5.2.
      const auto&    f2_cfg = std::get<pucch_format_2_3_cfg>(pucch_res_cfg.format_params);
      const unsigned nof_prbs =
          get_pucch_format2_nof_prbs(harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits,
                                     f2_cfg.nof_prbs,
                                     pucch_res_cfg.nof_symbols,
                                     to_max_code_rate_float(common_params->max_c_rate));
      harq_pdu->resources.prbs.set(pucch_res_cfg.starting_prb, pucch_res_cfg.starting_prb + nof_prbs);
      if (pucch_res_cfg.second_hop_prb.has_value()) {
        harq_pdu->resources.second_hop_prbs.set(pucch_res_cfg.second_hop_prb.value(),
                                                pucch_res_cfg.second_hop_prb.value() + nof_prbs);
      }
    } break;
    case pucch_format::FORMAT_3: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
      // After updating the UCI bits, we need to recompute the number of PRBs for PUCCH format 3, as per TS 38.213,
      // Section 9.2.5.2.
      const auto&    f3_cfg = std::get<pucch_format_2_3_cfg>(pucch_res_cfg.format_params);
      const unsigned nof_prbs =
          get_pucch_format3_nof_prbs(harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits,
                                     f3_cfg.nof_prbs,
                                     pucch_res_cfg.nof_symbols,
                                     to_max_code_rate_float(common_params->max_c_rate),
                                     pucch_res_cfg.second_hop_prb.has_value(),
                                     common_params->additional_dmrs,
                                     common_params->pi_2_bpsk);
      harq_pdu->resources.prbs.set(pucch_res_cfg.starting_prb, pucch_res_cfg.starting_prb + nof_prbs);
      if (pucch_res_cfg.second_hop_prb.has_value()) {
        harq_pdu->resources.second_hop_prbs.set(pucch_res_cfg.second_hop_prb.value(),
                                                pucch_res_cfg.second_hop_prb.value() + nof_prbs);
      }
    } break;
    case pucch_format::FORMAT_4: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
    } break;
    default:
      break;
  }
  harq_pdu->uci_bits.harq_ack_nof_bits = harq_ack_bits;
  harq_pdu->uci_bits.sr_bits           = sr_bits;
  harq_pdu->pdu_context.id             = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  harq_pdu = nullptr;
  --pdus_cnt;
}

//////////////    Private functions       //////////////

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_res_alloc_cfg>
pucch_allocator_impl::alloc_pucch_common_res_harq(const cell_slot_resource_allocator& pucch_alloc,
                                                  const dci_context_information&      dci_info)
{
  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  static constexpr unsigned max_d_pri = 7;
  // As per Section 9.2.1, TS 38.213, r_pucch can take values within {0,...,15}.
  static constexpr unsigned r_pucch_invalid = 16;

  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource pucch_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per TS 38.211, Section 6.3.2.1, the first floor(N_symb_PUCCH/2) are for the first hop, the remaining ones for
  // the second hop.
  const ofdm_symbol_range first_hop_symbols{pucch_res.first_symbol_index,
                                            pucch_res.first_symbol_index + pucch_res.nof_symbols / 2};
  const ofdm_symbol_range second_hop_symbols{pucch_res.first_symbol_index + pucch_res.nof_symbols / 2,
                                             pucch_res.first_symbol_index + pucch_res.nof_symbols};

  const bwp_configuration& init_ul_bwp_param = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  pucch_res_alloc_cfg candidate_pucch_resource{};
  // Initialize r_pucch candidate with an invalid value. This is to verify whether an available resource exists at the
  // end of the loop.
  unsigned candidate_r_pucch = r_pucch_invalid;
  // Flag to check if the backup resource (i.e., an available resource that collides with the grid) has been
  // initialized.
  bool backup_res_initialized = false;

  // The scope of the loop below is to allocate the PUCCH common resource while pursuing the following objectives:
  // - Avoiding the common PUCCH resources that are already allocated to other UEs. If there is no resource available,
  // then the allocation fails.
  // - Trying to allocate the common PUCCH resource in grid REs that are free. Should this not be possible (as the
  // UE-specific PUCCH guardbands can take quite some space on the band), the PUCCH allocator will choose the first
  // available common PUCCH resource (i.e., not used by other UEs), even though this will result in collision with the
  // grid.
  //
  // Loop over the values of \Delta_PRI to find an available common PUCCH resource that possibly doesn't collide with
  // the UL grid.
  for (unsigned d_pri = 0; d_pri != max_d_pri + 1; ++d_pri) {
    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
    srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");

    if (not resource_manager.is_common_resource_available(pucch_alloc.slot, r_pucch)) {
      continue;
    }

    // Compute PRB_first_hop and PRB_second_hop as per Section 9.2.1, TS 38.213.
    auto prbs = get_pucch_default_prb_index(r_pucch, pucch_res.rb_bwp_offset, pucch_res.cs_indexes.size(), size_ul_bwp);

    // With the default PUCCH resource configs, Format is either 0 or 1, which only occupy 1 RB.
    const unsigned   crb_first_hop = prb_to_crb(init_ul_bwp_param, prbs.first);
    const grant_info first_hop_grant{
        init_ul_bwp_param.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + 1}};
    const unsigned   crb_second_hop = prb_to_crb(init_ul_bwp_param, prbs.second);
    const grant_info second_hop_grant{
        init_ul_bwp_param.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + 1}};

    // Compute CS index as per Section 9.2.1, TS 38.213.
    const size_t cs_idx = r_pucch < 8 ? static_cast<size_t>(r_pucch) % pucch_res.cs_indexes.size()
                                      : static_cast<size_t>(r_pucch - 8) % pucch_res.cs_indexes.size();
    srsran_assert(cs_idx < pucch_res.cs_indexes.size(), "CS index exceeds static vector size");
    const uint8_t cyclic_shift = pucch_res.cs_indexes[cs_idx];

    // If both 1st and 2nd hop grants do not collide with any UL grants, then the allocator chooses this PUCCH
    // resource.
    if (not pucch_alloc.ul_res_grid.collides(first_hop_grant) &&
        not pucch_alloc.ul_res_grid.collides(second_hop_grant)) {
      // Set outputs before exiting the function.
      candidate_pucch_resource.first_hop_res       = first_hop_grant;
      candidate_pucch_resource.cs                  = cyclic_shift;
      candidate_pucch_resource.format              = pucch_res.format;
      candidate_pucch_resource.second_hop_res      = second_hop_grant;
      candidate_pucch_resource.pucch_res_indicator = d_pri;
      resource_manager.reserve_common_resource(pucch_alloc.slot, r_pucch);
      return std::optional<pucch_allocator_impl::pucch_res_alloc_cfg>{candidate_pucch_resource};
    }

    // Save the first available common PUCCH resource. If no other resource not colliding with the grid, then we
    // allocate this back-up resource at the end of the loop.
    if (not backup_res_initialized) {
      backup_res_initialized                       = true;
      candidate_pucch_resource.first_hop_res       = first_hop_grant;
      candidate_pucch_resource.cs                  = cyclic_shift;
      candidate_pucch_resource.format              = pucch_res.format;
      candidate_pucch_resource.second_hop_res      = second_hop_grant;
      candidate_pucch_resource.pucch_res_indicator = d_pri;
      candidate_r_pucch                            = r_pucch;
    }
  }

  // This is the case in which the only common PUCCH resources available collides with the grid.
  if (candidate_r_pucch < r_pucch_invalid) {
    resource_manager.reserve_common_resource(pucch_alloc.slot, candidate_r_pucch);
    return std::optional<pucch_allocator_impl::pucch_res_alloc_cfg>{candidate_pucch_resource};
  }

  // This is the case in which there exists no available resource.
  return std::nullopt;
}

static std::pair<grant_info, grant_info> get_common_pucch_grants(const bwp_configuration&      init_ul_bwp_params,
                                                                 const pucch_default_resource& pucch_res,
                                                                 unsigned                      r_pucch)
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = init_ul_bwp_params.crbs.length();

  // As per TS 38.211, Section 6.3.2.1, the first floor(N_symb_PUCCH/2) are for the first hop, the remaining ones for
  // the second hop.
  const ofdm_symbol_range first_hop_symbols{pucch_res.first_symbol_index,
                                            pucch_res.first_symbol_index + pucch_res.nof_symbols / 2};
  const ofdm_symbol_range second_hop_symbols{pucch_res.first_symbol_index + pucch_res.nof_symbols / 2,
                                             pucch_res.first_symbol_index + pucch_res.nof_symbols};

  // Compute PRB_first_hop and PRB_second_hop as per Section 9.2.1, TS 38.213.
  auto prbs = get_pucch_default_prb_index(r_pucch, pucch_res.rb_bwp_offset, pucch_res.cs_indexes.size(), size_ul_bwp);

  // With the default PUCCH resource configs, Format is either 0 or 1, which only occupy 1 RB.
  const unsigned   crb_first_hop = prb_to_crb(init_ul_bwp_params, prbs.first);
  const grant_info first_hop_grant{
      init_ul_bwp_params.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + 1}};
  const unsigned   crb_second_hop = prb_to_crb(init_ul_bwp_params, prbs.second);
  const grant_info second_hop_grant{
      init_ul_bwp_params.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + 1}};

  return {first_hop_grant, second_hop_grant};
}

void pucch_allocator_impl::compute_pucch_common_params_and_alloc(cell_slot_resource_allocator& pucch_alloc,
                                                                 rnti_t                        rnti,
                                                                 pucch_common_params           pucch_params)
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource pucch_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Compute the PUCCH resource common configuration parameters.
  const auto grant_infos =
      get_common_pucch_grants(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, pucch_res, pucch_params.r_pucch);

  // Compute CS index as per Section 9.2.1, TS 38.213.
  const size_t cs_idx = pucch_params.r_pucch < 8
                            ? static_cast<size_t>(pucch_params.r_pucch) % pucch_res.cs_indexes.size()
                            : static_cast<size_t>(pucch_params.r_pucch - 8) % pucch_res.cs_indexes.size();
  srsran_assert(cs_idx < pucch_res.cs_indexes.size(), "CS index exceeds static vector size");
  const uint8_t cyclic_shift = pucch_res.cs_indexes[cs_idx];

  // Fill scheduler output.
  pucch_info& pucch_pdu_common = pucch_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_pdu_common,
                               rnti,
                               pucch_res_alloc_cfg{.pucch_res_indicator = pucch_params.pucch_res_indicator,
                                                   .first_hop_res       = grant_infos.first,
                                                   .second_hop_res      = grant_infos.second,
                                                   .cs                  = cyclic_shift,
                                                   .format              = pucch_res.format});

  // Allocate common HARQ-ACK resource.
  mark_pucch_in_resource_grid(pucch_alloc,
                              grant_infos.first,
                              grant_infos.second,
                              cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs,
                              cell_cfg.expert_cfg);

  // Update the PUCCH grants with the common resource.
  auto& pucch_grants = pucch_grants_alloc_grid[pucch_alloc.slot.to_uint()];
  auto* ue_grants_it = std::find_if(
      pucch_grants.begin(), pucch_grants.end(), [rnti](const ue_grants& grants) { return grants.rnti == rnti; });
  srsran_assert(ue_grants_it != pucch_grants.end(), "UE grants allocation failed at some point");
  ue_grants_it->has_common_pucch = true;

  // TODO: once the code has been properly tested, remove the debug message.
  logger.debug("rnti={}: PUCCH on common and ded. resource with res_ind={} allocated for slot={}",
               rnti,
               pucch_params.pucch_res_indicator,
               pucch_alloc.slot);
}

void pucch_allocator_impl::fill_pucch_harq_common_grant(pucch_info&                pucch_info,
                                                        rnti_t                     rnti,
                                                        const pucch_res_alloc_cfg& pucch_res) const
{
  pucch_info.crnti = rnti;
  pucch_info.set_format(pucch_res.format);
  pucch_info.bwp_cfg                   = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_info.resources.prbs            = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.first_hop_res.crbs);
  pucch_info.resources.second_hop_prbs = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.second_hop_res.crbs);
  pucch_info.resources.symbols =
      ofdm_symbol_range{pucch_res.first_hop_res.symbols.start(), pucch_res.second_hop_res.symbols.stop()};
  pucch_info.pdu_context.is_common = true;

  switch (pucch_res.format) {
    case pucch_format::FORMAT_0: {
      auto& format_0         = std::get<pucch_format_0>(pucch_info.format_params);
      format_0.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      format_0.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                   ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                   : static_cast<unsigned>(cell_cfg.pci);
      // \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
      format_0.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.uci_bits.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_info.uci_bits.harq_ack_nof_bits = 1;
      break;
    }
    case pucch_format::FORMAT_1: {
      auto& format_1                = std::get<pucch_format_1>(pucch_info.format_params);
      format_1.group_hopping        = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      format_1.n_id_hopping         = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                          ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                          : static_cast<unsigned>(cell_cfg.pci);
      format_1.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.uci_bits.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_info.uci_bits.harq_ack_nof_bits = 1;
      // This option can be configured with Dedicated PUCCH resources.
      format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
      // As per TS 38.213, Section 9.2.1, OCC with index 0 is used for PUCCH resources in Table 9.2.1-1.
      format_1.time_domain_occ = 0;
      break;
    }
    default:
      srsran_assert(false, "Only PUCCH Format 0 and 1 can be used for PUCCH common resources");
  }
}

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_common_params>
pucch_allocator_impl::find_common_and_ded_harq_res_available(cell_slot_resource_allocator&  pucch_alloc,
                                                             ue_grants&                     existing_grants,
                                                             rnti_t                         rnti,
                                                             const ue_cell_configuration&   ue_cell_cfg,
                                                             const dci_context_information& dci_info)
{
  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  static constexpr unsigned max_d_pri = 7;

  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource pucch_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  for (unsigned d_pri = 0; d_pri != max_d_pri + 1; ++d_pri) {
    // The PUCCH allocation may result in a temporary reservation of PUCCH resources, which need to be released in
    // case of failure or in case the multiplexing results in a different final PUCCH resource. If we don't reset the
    // previous record, we could release the resources that have been allocated for other UEs of allocated for this
    // UE in a different slot. Reset at each iteration, as a new iteration indicates that the previous allocation
    // failed.
    resource_manager.reset_latest_reserved_res_tracker();

    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
    srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");

    // Look for an available PUCCH common resource.
    if (not resource_manager.is_common_resource_available(pucch_alloc.slot, r_pucch)) {
      continue;
    }

    const pucch_config&     pucch_cfg = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value();
    std::optional<unsigned> pucch_res_ind;

    // Look for an available PUCCH dedicated resource with the same PUCCH resource indicator as the common's.
    const pucch_resource* ded_resource =
        resource_manager.reserve_set_0_res_by_res_indicator(pucch_alloc.slot, rnti, d_pri, pucch_cfg);
    if (ded_resource == nullptr) {
      continue;
    }
    resource_manager.set_new_resource_allocation(rnti, pucch_resource_usage::HARQ_SET_0);

    // In the bit to transmit, we have 1 HARQ-ACK bit, plus the SR and CSI bits if any existing grants.
    pucch_uci_bits bits_for_uci{.harq_ack_nof_bits = 1U};
    bits_for_uci.sr_bits            = existing_grants.pucch_grants.sr_resource.has_value()
                                          ? existing_grants.pucch_grants.sr_resource.value().bits.sr_bits
                                          : sr_nof_bits::no_sr;
    bits_for_uci.csi_part1_nof_bits = existing_grants.pucch_grants.csi_resource.has_value()
                                          ? existing_grants.pucch_grants.csi_resource.value().bits.csi_part1_nof_bits
                                          : 0U;

    // Compute the PUCCH resource common configuration parameters.
    const auto common_grants =
        get_common_pucch_grants(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params, pucch_res, r_pucch);
    // The PUCCH resource indicator must match the one used for the common resource.
    pucch_res_ind =
        multiplex_and_allocate_pucch(pucch_alloc, bits_for_uci, existing_grants, ue_cell_cfg, d_pri, common_grants);

    // In the case of PUCCH Format 0, the PUCCH (dedicated) resources that can be used when there are SR/CSI
    // pre-allocated are constrained in the PUCCH resource indicator. Therefore, the \ref multiplex_and_allocate_pucch
    // function might not preserve the requested PUCCH resource indicator \ref d_pri.
    const bool is_format0 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0;
    if (not pucch_res_ind.has_value() or (is_format0 and pucch_res_ind.value() != d_pri)) {
      resource_manager.cancel_last_ue_res_reservations(pucch_alloc.slot, rnti, ue_cell_cfg);
      continue;
    }
    srsran_assert(pucch_res_ind.value() == d_pri, "PUCCH resource indicator must match the one given as an input");

    return pucch_common_params{.pucch_res_indicator = d_pri, .r_pucch = r_pucch};
  };
  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::allocate_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                  rnti_t                        crnti,
                                                                  const ue_cell_configuration&  ue_cell_cfg)
{
  slot_point sl_tx = pucch_slot_alloc.slot;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.debug("rnti={}: HARQ-ACK allocation on PUCCH Format1 for slot={} skipped. Cause: UL grants reached",
                 crnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  if (pucch_grants_alloc_grid[sl_tx.to_uint()].full()) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH allocator grant list is full",
                 crnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_set_0_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                 crnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  // Allocate the new grant on PUCCH F1 resources for HARQ-ACK bits (without SR).
  pucch_info&               pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  static constexpr unsigned harq_bits_in_new_pucch_grant = 1;
  if (pucch_harq_res_info.pucch_res->format == pucch_format::FORMAT_0) {
    fill_pucch_ded_format0_grant(
        pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, harq_bits_in_new_pucch_grant, sr_nof_bits::no_sr);
  } else if (pucch_harq_res_info.pucch_res->format == pucch_format::FORMAT_1) {
    fill_pucch_ded_format1_grant(
        pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, harq_bits_in_new_pucch_grant, sr_nof_bits::no_sr);
  } else {
    srsran_assertion_failure("Only PUCCH Format 0 or 1 can be used for UCI with only 1 HARQ-ACK bit");
  }

  // Save the info in the scheduler list of PUCCH grants.
  const auto pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);
  auto&      grants              = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_grants{.rnti = crnti});
  grants.pucch_grants.harq_resource.emplace(pucch_grant{.type = pucch_grant_type::harq_ack});
  grants.pucch_grants.harq_resource.value().set_res_config(*pucch_harq_res_info.pucch_res);
  grants.pucch_grants.harq_resource.value().harq_id.pucch_set_idx  = pucch_res_set_idx::set_0;
  grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind  = pucch_res_indicator;
  grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = harq_bits_in_new_pucch_grant;

  // TODO: unmark on multiplexing (take into account CS/OCC).
  mark_pucch_in_resource_grid(pucch_slot_alloc, *pucch_harq_res_info.pucch_res, ue_cell_cfg);

  return pucch_res_indicator;
}

void pucch_allocator_impl::allocate_csi_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                              rnti_t                        crnti,
                                              const ue_cell_configuration&  ue_cell_cfg,
                                              unsigned                      csi_part1_bits)
{
  srsran_assert(csi_part1_bits != 0, "This function can only be called to allocate a PUCCH F2/F3/F4 resource for CSI");
  const slot_point sl_tx = pucch_slot_alloc.slot;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: CSI allocation on PUCCH F2/F3/F4 for slot={} skipped. Cause: UL grants reached",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  if (pucch_grants_alloc_grid[sl_tx.to_uint()].full()) {
    logger.debug("rnti={}: PUCCH CSI allocation for slot={} skipped. Cause: PUCCH allocator grant list is full",
                 crnti,
                 pucch_slot_alloc.slot);
    return;
  }

  // Get the F2/F3/F4 resource specific for with CSI.
  const pucch_resource* csi_f2_f3_f4_res =
      resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, crnti, ue_cell_cfg);

  if (csi_f2_f3_f4_res == nullptr) {
    logger.warning("rnti={}: CSI could not be allocated for slot={}. Cause: PUCCH F2/F3/F4 resource not available",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }
  // When this function is called, it means that there are no SR grants to be multiplexed with CSI; thus, the CSI bits
  // are the only UCI bits to be considered.
  // It's the validator that should make sure the CSI bits fit into a PUCCH Format 2/3/4 resource.
  const unsigned max_payload =
      ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().get_max_payload(csi_f2_f3_f4_res->format);
  srsran_assert(csi_part1_bits <= max_payload,
                "rnti={}: PUCCH F2/F3/F4 max payload {} is insufficient for {} candidate UCI bits",
                crnti,
                max_payload,
                csi_part1_bits);

  // Allocate a PUCCH PDU in the list and fill it with the parameters.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  // No HARQ-ACK bits.
  static constexpr unsigned    harq_ack_bits_only_csi = 0U;
  static constexpr sr_nof_bits sr_bits_only_csi       = sr_nof_bits::no_sr;
  switch (csi_f2_f3_f4_res->format) {
    case pucch_format::FORMAT_2:
      fill_pucch_format2_grant(pucch_pdu,
                               crnti,
                               *csi_f2_f3_f4_res,
                               ue_cell_cfg,
                               std::get<pucch_format_2_3_cfg>(csi_f2_f3_f4_res->format_params).nof_prbs,
                               harq_ack_bits_only_csi,
                               sr_bits_only_csi,
                               csi_part1_bits);
      break;
    case pucch_format::FORMAT_3:
      fill_pucch_format3_grant(pucch_pdu,
                               crnti,
                               *csi_f2_f3_f4_res,
                               ue_cell_cfg,
                               std::get<pucch_format_2_3_cfg>(csi_f2_f3_f4_res->format_params).nof_prbs,
                               harq_ack_bits_only_csi,
                               sr_bits_only_csi,
                               csi_part1_bits);
      break;
    case pucch_format::FORMAT_4:
      fill_pucch_format4_grant(
          pucch_pdu, crnti, *csi_f2_f3_f4_res, ue_cell_cfg, harq_ack_bits_only_csi, sr_bits_only_csi, csi_part1_bits);
      break;
    default:
      srsran_assertion_failure("PUCCH resource for CSI must be of Formats 2, 3 or 4");
  }

  // TODO: unmark on multiplexing (take into account CS/OCC).
  mark_pucch_in_resource_grid(pucch_slot_alloc, *csi_f2_f3_f4_res, ue_cell_cfg);

  // Save the info in the scheduler list of PUCCH grants.
  auto& csi_pucch_grant = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_grants{.rnti = crnti});
  csi_pucch_grant.pucch_grants.csi_resource.emplace(pucch_grant{.type = pucch_grant_type::csi});
  csi_pucch_grant.pucch_grants.csi_resource.value().set_res_config(*csi_f2_f3_f4_res);
  csi_pucch_grant.pucch_grants.csi_resource.value().bits.csi_part1_nof_bits = csi_part1_bits;
}

void pucch_allocator_impl::fill_pucch_ded_format0_grant(pucch_info&           pucch_pdu,
                                                        rnti_t                crnti,
                                                        const pucch_resource& pucch_ded_res_cfg,
                                                        unsigned              harq_ack_bits,
                                                        sr_nof_bits           sr_bits) const

{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  auto& format_0    = pucch_pdu.format_params.emplace<pucch_format_0>();

  // Set PRBs and symbols, first.
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb,
                               +pucch_ded_res_cfg.starting_prb + pucch_constants::FORMAT0_1_4_MAX_NPRB);
  pucch_pdu.resources.symbols.set(pucch_ded_res_cfg.starting_sym_idx,
                                  pucch_ded_res_cfg.starting_sym_idx + pucch_ded_res_cfg.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() +
                                                pucch_constants::FORMAT0_1_4_MAX_NPRB);
  }
  // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
  format_0.group_hopping        = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  format_0.n_id_hopping         = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                      ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                      : cell_cfg.pci;
  const auto& res_f0            = std::get<pucch_format_0_cfg>(pucch_ded_res_cfg.format_params);
  format_0.initial_cyclic_shift = res_f0.initial_cyclic_shift;
  // For PUCCH Format 0, only 1 SR bit.
  pucch_pdu.uci_bits.sr_bits           = sr_bits;
  pucch_pdu.uci_bits.harq_ack_nof_bits = harq_ack_bits;
}

void pucch_allocator_impl::fill_pucch_ded_format1_grant(pucch_info&           pucch_pdu,
                                                        rnti_t                crnti,
                                                        const pucch_resource& pucch_ded_res_cfg,
                                                        unsigned              harq_ack_bits,
                                                        sr_nof_bits           sr_bits) const
{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  auto& format_1    = pucch_pdu.format_params.emplace<pucch_format_1>();

  // Set PRBs and symbols, first.
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb,
                               pucch_ded_res_cfg.starting_prb + pucch_constants::FORMAT0_1_4_MAX_NPRB);
  pucch_pdu.resources.symbols.set(pucch_ded_res_cfg.starting_sym_idx,
                                  pucch_ded_res_cfg.starting_sym_idx + pucch_ded_res_cfg.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() +
                                                pucch_constants::FORMAT0_1_4_MAX_NPRB);
  }
  // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
  format_1.group_hopping        = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  format_1.n_id_hopping         = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                      ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                      : cell_cfg.pci;
  const auto& res_f1            = std::get<pucch_format_1_cfg>(pucch_ded_res_cfg.format_params);
  format_1.initial_cyclic_shift = res_f1.initial_cyclic_shift;
  format_1.time_domain_occ      = res_f1.time_domain_occ;
  // For PUCCH Format 1, only 1 SR bit.
  pucch_pdu.uci_bits.sr_bits           = sr_bits;
  pucch_pdu.uci_bits.harq_ack_nof_bits = harq_ack_bits;
  // [Implementation-defined] We do not implement PUCCH over several slots.
  format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
}

void pucch_allocator_impl::fill_pucch_format2_grant(pucch_info&                  pucch_pdu,
                                                    rnti_t                       crnti,
                                                    const pucch_resource&        pucch_ded_res_cfg,
                                                    const ue_cell_configuration& ue_cell_cfg,
                                                    unsigned                     nof_prbs,
                                                    unsigned                     harq_ack_bits,
                                                    sr_nof_bits                  sr_bits,
                                                    unsigned                     csi_part1_bits) const
{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  auto& format_2    = pucch_pdu.format_params.emplace<pucch_format_2>();

  // Set PRBs and symbols, first.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb, pucch_ded_res_cfg.starting_prb + nof_prbs);
  pucch_pdu.resources.symbols.set(pucch_ded_res_cfg.starting_sym_idx,
                                  pucch_ded_res_cfg.starting_sym_idx + pucch_ded_res_cfg.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() + nof_prbs);
  }

  pucch_pdu.uci_bits.sr_bits            = sr_bits;
  pucch_pdu.uci_bits.harq_ack_nof_bits  = harq_ack_bits;
  pucch_pdu.uci_bits.csi_part1_nof_bits = csi_part1_bits;
  // \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  const auto& init_ul_bwp = ue_cell_cfg.init_bwp().ul_ded.value();
  format_2.n_id_scambling = init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.has_value()
                                ? init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.value()
                                : cell_cfg.pci;
  // \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  format_2.n_id_0_scrambling = get_n_id0_scrambling(ue_cell_cfg, cell_cfg.pci);
  format_2.max_code_rate     = init_ul_bwp.pucch_cfg.value().format_2_common_param.value().max_c_rate;

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (csi_part1_bits > 0) {
    pucch_pdu.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.csi_meas_cfg());
  }
}

void pucch_allocator_impl::fill_pucch_format3_grant(pucch_info&                  pucch_pdu,
                                                    rnti_t                       crnti,
                                                    const pucch_resource&        pucch_ded_res_cfg,
                                                    const ue_cell_configuration& ue_cell_cfg,
                                                    unsigned                     nof_prbs,
                                                    unsigned                     harq_ack_bits,
                                                    sr_nof_bits                  sr_bits,
                                                    unsigned                     csi_part1_bits) const
{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  auto& format_3    = pucch_pdu.format_params.emplace<pucch_format_3>();

  // Set PRBs and symbols, first.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb, pucch_ded_res_cfg.starting_prb + nof_prbs);
  pucch_pdu.resources.symbols.set(pucch_ded_res_cfg.starting_sym_idx,
                                  pucch_ded_res_cfg.starting_sym_idx + pucch_ded_res_cfg.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() + nof_prbs);
  }

  pucch_pdu.uci_bits.sr_bits            = sr_bits;
  pucch_pdu.uci_bits.harq_ack_nof_bits  = harq_ack_bits;
  pucch_pdu.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  format_3.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  format_3.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                               ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                               : cell_cfg.pci;
  // [Implementation-defined] We do not implement PUCCH over several slots.
  format_3.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  // \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  const auto& init_ul_bwp    = ue_cell_cfg.init_bwp().ul_ded.value();
  format_3.n_id_scrambling   = init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.has_value()
                                   ? init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.value()
                                   : cell_cfg.pci;
  format_3.n_id_0_scrambling = get_n_id0_scrambling(ue_cell_cfg, cell_cfg.pci);
  format_3.pi_2_bpsk         = init_ul_bwp.pucch_cfg.value().format_3_common_param.value().pi_2_bpsk;
  format_3.additional_dmrs   = init_ul_bwp.pucch_cfg.value().format_3_common_param.value().additional_dmrs;
  // \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  format_3.max_code_rate = init_ul_bwp.pucch_cfg.value().format_3_common_param.value().max_c_rate;

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (csi_part1_bits > 0) {
    pucch_pdu.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.csi_meas_cfg());
  }
}

void pucch_allocator_impl::fill_pucch_format4_grant(pucch_info&                  pucch_pdu,
                                                    rnti_t                       crnti,
                                                    const pucch_resource&        pucch_ded_res_cfg,
                                                    const ue_cell_configuration& ue_cell_cfg,
                                                    unsigned                     harq_ack_bits,
                                                    sr_nof_bits                  sr_bits,
                                                    unsigned                     csi_part1_bits) const
{
  constexpr unsigned nof_prbs_f4 = 1U;
  pucch_pdu.crnti                = crnti;
  pucch_pdu.bwp_cfg              = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  auto& format_4                 = pucch_pdu.format_params.emplace<pucch_format_4>();

  // Set PRBs and symbols, first.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb, pucch_ded_res_cfg.starting_prb + nof_prbs_f4);
  pucch_pdu.resources.symbols.set(pucch_ded_res_cfg.starting_sym_idx,
                                  pucch_ded_res_cfg.starting_sym_idx + pucch_ded_res_cfg.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() + nof_prbs_f4);
  }

  pucch_pdu.uci_bits.sr_bits            = sr_bits;
  pucch_pdu.uci_bits.harq_ack_nof_bits  = harq_ack_bits;
  pucch_pdu.uci_bits.csi_part1_nof_bits = csi_part1_bits;

  format_4.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  format_4.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                               ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                               : cell_cfg.pci;
  // [Implementation-defined] We do not implement PUCCH over several slots.
  format_4.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  // \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  const auto& init_ul_bwp    = ue_cell_cfg.init_bwp().ul_ded.value();
  format_4.n_id_scrambling   = init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.has_value()
                                   ? init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.value()
                                   : cell_cfg.pci;
  format_4.n_id_0_scrambling = get_n_id0_scrambling(ue_cell_cfg, cell_cfg.pci);
  format_4.pi_2_bpsk         = init_ul_bwp.pucch_cfg.value().format_4_common_param.value().pi_2_bpsk;
  format_4.additional_dmrs   = init_ul_bwp.pucch_cfg.value().format_4_common_param.value().additional_dmrs;
  // \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  format_4.max_code_rate  = init_ul_bwp.pucch_cfg.value().format_4_common_param.value().max_c_rate;
  const auto& res_f4      = std::get<pucch_format_4_cfg>(pucch_ded_res_cfg.format_params);
  format_4.orthog_seq_idx = static_cast<unsigned>(res_f4.occ_index);
  format_4.n_sf_pucch_f4  = static_cast<pucch_format_4_sf>(res_f4.occ_length);

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (csi_part1_bits > 0) {
    pucch_pdu.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.csi_meas_cfg());
  }
}

bool pucch_allocator_impl::has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const
{
  return std::find_if(pucch_grants_alloc_grid[sl_tx.to_uint()].begin(),
                      pucch_grants_alloc_grid[sl_tx.to_uint()].end(),
                      [rnti](const ue_grants& grant) { return grant.has_common_pucch and grant.rnti == rnti; }) !=
         pucch_grants_alloc_grid[sl_tx.to_uint()].end();
}

unsigned pucch_allocator_impl::get_max_pucch_grants(unsigned currently_allocated_puschs) const
{
  return std::min(max_pucch_grants_per_slot, max_ul_grants_per_slot - currently_allocated_puschs);
}

void pucch_allocator_impl::remove_unused_pucch_res(slot_point                   sl_tx,
                                                   const pucch_grant_list&      grants_to_tx,
                                                   const ue_grants&             existing_pucchs,
                                                   const ue_cell_configuration& ue_cell_cfg)
{
  const auto& init_ul_bwp = ue_cell_cfg.init_bwp().ul_ded.value();

  // Remove the PUCCH resources by evaluating the difference between the previously allocated resources and the current
  // ones.
  if (existing_pucchs.pucch_grants.csi_resource.has_value() and not grants_to_tx.csi_resource.has_value()) {
    resource_manager.release_csi_resource(sl_tx, existing_pucchs.rnti, ue_cell_cfg);
  }
  if (existing_pucchs.pucch_grants.sr_resource.has_value() and not grants_to_tx.sr_resource.has_value()) {
    resource_manager.release_sr_resource(sl_tx, existing_pucchs.rnti, init_ul_bwp.pucch_cfg.value());
  }

  if (existing_pucchs.pucch_grants.harq_resource.has_value() and
      (not grants_to_tx.harq_resource.has_value() or
       existing_pucchs.pucch_grants.harq_resource->format != grants_to_tx.harq_resource->format)) {
    if (existing_pucchs.pucch_grants.harq_resource.value().harq_id.pucch_set_idx == pucch_res_set_idx::set_0) {
      resource_manager.release_harq_set_0_resource(sl_tx, existing_pucchs.rnti, init_ul_bwp.pucch_cfg.value());
    } else {
      resource_manager.release_harq_set_1_resource(sl_tx, existing_pucchs.rnti, init_ul_bwp.pucch_cfg.value());
    }
  }

  // This is a special case, in which the PUCCH from resource set 0 is first reserved, but later it is converted into a
  // PUCCH from resource set 1 due to the multiplexing process.
  if (resource_manager.is_resource_allocated(existing_pucchs.rnti, pucch_resource_usage::HARQ_SET_1) and
      resource_manager.is_resource_allocated(existing_pucchs.rnti, pucch_resource_usage::HARQ_SET_0)) {
    resource_manager.release_harq_set_0_resource(sl_tx, existing_pucchs.rnti, init_ul_bwp.pucch_cfg.value());
  }
}

static bool check_common_and_ded_grant_collisions(const std::pair<grant_info, grant_info>&                common_grants,
                                                  const std::pair<grant_info, std::optional<grant_info>>& ded_grants)
{
  if (common_grants.first.overlaps(ded_grants.first) or common_grants.second.overlaps(ded_grants.first)) {
    return true;
  }
  if (ded_grants.second.has_value() and
      (common_grants.first.overlaps(*ded_grants.second) or common_grants.second.overlaps(*ded_grants.second))) {
    return true;
  }
  return false;
}

std::optional<unsigned>
pucch_allocator_impl::multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                   pucch_uci_bits                new_bits,
                                                   ue_grants&                    current_grants,
                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                   std::optional<uint8_t>        preserve_res_indicator,
                                                   std::optional<std::pair<grant_info, grant_info>> common_grants)
{
  // NOTE: In this function, the \c candidate_grants report the data about the grants BEFORE the multiplexing is
  // applied. Each grant contains only one UCI type (HARQ grant contains HARQ bits, SR grant contains SR bits and so
  // on); on the contrary, \c grants_to_tx contains the grants AFTER the multiplexing; this means that 1 grant can
  // contain more than 1 UCI bit type.

  slot_point sl_ack = pucch_slot_alloc.slot;

  // Find the grants/resources needed for the UCI bits to be reported, assuming the resources are not multiplexed.
  std::optional<pucch_grant_list> candidate_grants =
      get_pucch_res_pre_multiplexing(sl_ack, new_bits, current_grants, ue_cell_cfg);
  if (not candidate_grants.has_value()) {
    return std::nullopt;
  }

  pucch_grant_list grants_to_tx =
      multiplex_resources(sl_ack, current_grants.rnti, candidate_grants.value(), ue_cell_cfg, preserve_res_indicator);

  if (grants_to_tx.is_empty()) {
    return std::nullopt;
  }

  // If the common grants are provided, make sure the grants_to_tx don't collide with them.
  if (common_grants.has_value()) {
    const auto& init_ul_bwp = ue_cell_cfg.init_bwp().ul_common.value()->generic_params;
    if (grants_to_tx.csi_resource.has_value()) {
      const auto ded_grants =
          pucch_resource_to_grant_info(init_ul_bwp, *grants_to_tx.csi_resource.value().pucch_res_cfg);
      if (check_common_and_ded_grant_collisions(*common_grants, ded_grants)) {
        return std::nullopt;
      }
    }
    if (grants_to_tx.sr_resource.has_value()) {
      const auto ded_grants =
          pucch_resource_to_grant_info(init_ul_bwp, *grants_to_tx.sr_resource.value().pucch_res_cfg);
      if (check_common_and_ded_grant_collisions(*common_grants, ded_grants)) {
        return std::nullopt;
      }
    }
    if (grants_to_tx.harq_resource.has_value()) {
      const auto ded_grants =
          pucch_resource_to_grant_info(init_ul_bwp, *grants_to_tx.harq_resource.value().pucch_res_cfg);
      if (check_common_and_ded_grant_collisions(*common_grants, ded_grants)) {
        return std::nullopt;
      }
    }
  }

  // Allocate the grants.
  return allocate_grants(pucch_slot_alloc, current_grants, current_grants.rnti, grants_to_tx, ue_cell_cfg);
}

// TODO: check how to handle F0+F3/F4.
static unsigned get_pucch_resource_ind_f0_sr_csi(pucch_uci_bits bits, const pucch_config& pucch_cfg)
{
  // With Format 0, with HARQ-ACK bits <= 2, pick a resource from PUCCH resource set 0.

  if (bits.harq_ack_nof_bits <= 2U) {
    // At position (PUCCH resource set 0 size - 1U) the resource coincides with the SR resource.
    if (bits.sr_bits != sr_nof_bits::no_sr) {
      return pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_0)].pucch_res_id_list.size() - 1U;
    }
    // NOTE: Either CSI or SR bits are non-zero, but not both.
    // At position (PUCCH resource set 0 size - 2U) the resource is of Format 0, but set on the same PRBs/symbols as the
    // CSI resource.
    return pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_0)].pucch_res_id_list.size() - 2U;
  }

  // This if for bits.harq_ack_nof_bits > 2U.

  // At position (PUCCH resource set 1 size - 1U) the resource is of Format 2, but set on the same PRBs/symbols as the
  // SR resource.
  if (bits.sr_bits != sr_nof_bits::no_sr) {
    return pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_1)].pucch_res_id_list.size() - 1U;
  }
  // NOTE: Either CSI or SR bits are non-zero, but not both.
  // At position (PUCCH resource set 1 size - 2U) the resource coincides with the CSI resource.
  return pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(pucch_res_set_idx::set_1)].pucch_res_id_list.size() - 2U;
}

std::optional<pucch_allocator_impl::pucch_grant_list>
pucch_allocator_impl::get_pucch_res_pre_multiplexing(slot_point                   sl_tx,
                                                     pucch_uci_bits               new_bits,
                                                     const ue_grants&             ue_current_grants,
                                                     const ue_cell_configuration& ue_cell_cfg)
{
  pucch_grant_list candidate_resources;

  const pucch_config& pucch_cfg = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value();

  if (new_bits.sr_bits != sr_nof_bits::no_sr) {
    candidate_resources.sr_resource.emplace(pucch_grant{.type = pucch_grant_type::sr});
    pucch_grant& sr_candidate_grant = candidate_resources.sr_resource.value();

    // Get the resource from the resource manager; the UCI bits will be added later.
    // NOTE: if the SR resource was already assigned to this UE, the resource manager will only return the PUCCH
    // config that was reserved previously.
    const pucch_resource* sr_resource =
        resource_manager.reserve_sr_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg);
    // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
    resource_manager.set_new_resource_allocation(ue_current_grants.rnti, pucch_resource_usage::SR);
    if (sr_resource == nullptr) {
      srsran_assertion_failure("rnti={}: PUCCH SR resource previously reserved not available anymore",
                               ue_current_grants.rnti);
      return std::nullopt;
    }
    sr_candidate_grant.set_res_config(*sr_resource);
    // Only copy the SR bits, as at this stage we only need to consider the UCI bits assuming the resources still
    // need to be multiplexed.
    sr_candidate_grant.bits.harq_ack_nof_bits  = 0U;
    sr_candidate_grant.bits.sr_bits            = new_bits.sr_bits;
    sr_candidate_grant.bits.csi_part1_nof_bits = 0U;
  }

  if (new_bits.csi_part1_nof_bits != 0U) {
    candidate_resources.csi_resource.emplace(pucch_grant{.type = pucch_grant_type::csi});
    pucch_grant& csi_candidate_grant = candidate_resources.csi_resource.value();

    // Get the resource from the resource manager; the UCI bits will be added later.
    // NOTE: if the CSI resource was already assigned to this UE, the resource manager will only return the PUCCH
    // config that was reserved previously.
    const pucch_resource* csi_resource =
        resource_manager.reserve_csi_resource(sl_tx, ue_current_grants.rnti, ue_cell_cfg);
    // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
    resource_manager.set_new_resource_allocation(ue_current_grants.rnti, pucch_resource_usage::CSI);
    if (csi_resource == nullptr) {
      srsran_assertion_failure("rnti={}: PUCCH CSI resource previously reserved not available anymore",
                               ue_current_grants.rnti);
      return std::nullopt;
    }
    csi_candidate_grant.set_res_config(*csi_resource);
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources
    // still need to be multiplexed.
    csi_candidate_grant.bits.harq_ack_nof_bits  = 0U;
    csi_candidate_grant.bits.sr_bits            = sr_nof_bits::no_sr;
    csi_candidate_grant.bits.csi_part1_nof_bits = new_bits.csi_part1_nof_bits;
  }

  if (new_bits.harq_ack_nof_bits > 0) {
    // Case HARQ ACK bits 1 or 2, resource to be chosen from PUCCH resource set 0; else, pick from PUCCH resource
    // set 1.
    const pucch_res_set_idx pucch_set_idx =
        new_bits.harq_ack_nof_bits <= 2U ? pucch_res_set_idx::set_0 : pucch_res_set_idx::set_1;
    // NOTE: Not all UEs are capable of transmitting more than 1 PUCCH; this would be the case in which the UE has
    // Format 0 resources, and it needs to transmit HARQ-ACK bits + SR or CSI in the same slot, and the resource symbols
    // for HARQ and SR/CSI do not overlap. In these slots, we force, the UE to use a PUCCH resource for HARQ-ACK that is
    // guaranteed to overlap (in symbols) with the SR or CSI resource.
    const bool has_format_0 =
        std::find_if(pucch_cfg.pucch_res_list.begin(), pucch_cfg.pucch_res_list.end(), [](const auto& pucch_res) {
          return pucch_res.format == pucch_format::FORMAT_0;
        }) != pucch_cfg.pucch_res_list.end();
    const bool has_format_2 =
        std::find_if(pucch_cfg.pucch_res_list.begin(), pucch_cfg.pucch_res_list.end(), [](const auto& pucch_res) {
          return pucch_res.format == pucch_format::FORMAT_2;
        }) != pucch_cfg.pucch_res_list.end();
    const bool ue_with_f0_sr_csi_allocation =
        has_format_0 and has_format_2 and (new_bits.sr_bits != sr_nof_bits::no_sr or new_bits.csi_part1_nof_bits != 0U);

    candidate_resources.harq_resource.emplace(pucch_grant{.type = pucch_grant_type::harq_ack});
    pucch_grant& harq_candidate_grant = candidate_resources.harq_resource.value();

    // Handle these 2 cases: (i) There is already a PUCCH resource for HARQ-ACK; if so, we use the info and
    // configuration from this resource; (ii) the UE has Format 0 resources, and it needs to transmit HARQ-ACK bits + SR
    // or CSI in the same slot.
    if ((ue_current_grants.pucch_grants.harq_resource.has_value() and
         ue_current_grants.pucch_grants.harq_resource.value().harq_id.pucch_set_idx == pucch_set_idx) or
        ue_with_f0_sr_csi_allocation) {
      // NOTE: If the UE has Format 0 resources, and it needs to transmit HARQ-ACK bits + SR or CSI in the same slot,
      // use the HARQ-ACK resource that has highest PUCCH resource indicator; the UE's dedicated PUCCH config has been
      // constructed in such a way that this resource overlaps with the SR or CSI resource.
      const unsigned pucch_res_ind = ue_with_f0_sr_csi_allocation
                                         ? get_pucch_resource_ind_f0_sr_csi(new_bits, pucch_cfg)
                                         : ue_current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind;

      const pucch_resource* pucch_res = pucch_set_idx == pucch_res_set_idx::set_0
                                            ? resource_manager.reserve_set_0_res_by_res_indicator(
                                                  sl_tx, ue_current_grants.rnti, pucch_res_ind, pucch_cfg)
                                            : resource_manager.reserve_set_1_res_by_res_indicator(
                                                  sl_tx, ue_current_grants.rnti, pucch_res_ind, pucch_cfg);
      if (pucch_res == nullptr) {
        if (ue_with_f0_sr_csi_allocation) {
          srsran_assertion_failure("rnti={}: PUCCH HARQ-ACK that should be reserved for this UE is not available",
                                   ue_current_grants.rnti);
        } else {
          srsran_assertion_failure("rnti={}: PUCCH HARQ-ACK resource previously reserved not available anymore",
                                   ue_current_grants.rnti);
        }

        return std::nullopt;
      }
      harq_candidate_grant.harq_id.pucch_set_idx = pucch_set_idx;
      harq_candidate_grant.harq_id.pucch_res_ind = static_cast<uint8_t>(pucch_res_ind);
      harq_candidate_grant.set_res_config(*pucch_res);
    }
    // Get a new PUCCH resource for HARQ-ACK from the correct PUCCH resource set.
    else {
      // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits before multiplexing.
      pucch_harq_resource_alloc_record harq_resource =
          pucch_set_idx == pucch_res_set_idx::set_0
              ? resource_manager.reserve_next_set_0_harq_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg)
              : resource_manager.reserve_next_set_1_harq_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg);
      // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
      if (pucch_set_idx == pucch_res_set_idx::set_0) {
        resource_manager.set_new_resource_allocation(ue_current_grants.rnti, pucch_resource_usage::HARQ_SET_0);
      } else {
        resource_manager.set_new_resource_allocation(ue_current_grants.rnti, pucch_resource_usage::HARQ_SET_1);
      }
      if (harq_resource.pucch_res == nullptr) {
        return std::nullopt;
      }
      harq_candidate_grant.harq_id.pucch_set_idx = pucch_set_idx;
      harq_candidate_grant.harq_id.pucch_res_ind = static_cast<uint8_t>(harq_resource.pucch_res_indicator);
      harq_candidate_grant.set_res_config(*harq_resource.pucch_res);
    }
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources
    // still need to be multiplexed.
    harq_candidate_grant.bits.harq_ack_nof_bits  = new_bits.harq_ack_nof_bits;
    harq_candidate_grant.bits.sr_bits            = sr_nof_bits::no_sr;
    harq_candidate_grant.bits.csi_part1_nof_bits = 0U;
  }

  // TODO: handle the failure case, in which the resources that had been reserved are not used and need to be released.

  return candidate_resources;
}

std::optional<unsigned>
pucch_allocator_impl::allocate_without_multiplexing(cell_slot_resource_allocator& pucch_slot_alloc,
                                                    pucch_uci_bits                new_bits,
                                                    ue_grants&                    current_grants,
                                                    const ue_cell_configuration&  ue_cell_cfg)
{
  slot_point sl_tx = pucch_slot_alloc.slot;

  const pucch_config& pucch_cfg = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value();

  auto& pucch_pdus = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH PDUs.
  existing_pucch_pdus_handler existing_pdus(current_grants.rnti, pucch_pdus, get_sr_pucch_res_cfg(pucch_cfg));

  srsran_assert(existing_pdus.harq_pdu != nullptr and current_grants.pucch_grants.harq_resource.has_value(),
                "rnti={}: expected HARQ-ACK PUCCH grant and PDU not found",
                current_grants.rnti);

  pucch_uci_bits current_bits = current_grants.pucch_grants.get_uci_bits();

  // If the HARQ PDU uses F0, there can be 1 HARQ PDU + an optional SR (F0) or CSI (F2). In any case, we only need to
  // update the HARQ-ACK bits in the HARQ-ACK PDU.
  // TODO: handle F0+F3/F4.
  static constexpr unsigned csi_bits_format_0_and_1 = 0U;
  if (existing_pdus.harq_pdu->format() == pucch_format::FORMAT_0) {
    const pucch_resource* pucch_res_cfg = resource_manager.reserve_set_0_res_by_res_indicator(
        sl_tx, current_grants.rnti, current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind, pucch_cfg);
    srsran_assert(pucch_res_cfg != nullptr, "rnti={}: PUCCH expected resource not available", current_grants.rnti);
    logger.debug("rnti={}: PUCCH PDU on F0 HARQ resource updated: slot={} p_ind={} prbs={} sym={} {}",
                 current_grants.rnti,
                 pucch_slot_alloc.slot,
                 current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind,
                 existing_pdus.harq_pdu->resources.prbs,
                 existing_pdus.harq_pdu->resources.symbols,
                 new_bits);
    existing_pdus.update_harq_pdu_bits(new_bits.harq_ack_nof_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.sr_bits,
                                       csi_bits_format_0_and_1,
                                       *pucch_res_cfg,
                                       std::nullopt);
    // Update the current grant with the new UCI (HARQ) bits.
    current_grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
  }
  // If the HARQ PDU uses F1, there can be 1 HARQ PDU + an optional SR (F1). In the latter case, we need to update the
  // HARQ-ACK bits in the SR PDU as well.
  else if (existing_pdus.harq_pdu->format() == pucch_format::FORMAT_1) {
    const pucch_resource* pucch_res_cfg = resource_manager.reserve_set_0_res_by_res_indicator(
        sl_tx, current_grants.rnti, current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind, pucch_cfg);
    srsran_assert(pucch_res_cfg != nullptr, "rnti={}: PUCCH expected resource not available", current_grants.rnti);
    const auto& harq_format_1 = std::get<pucch_format_1>(existing_pdus.harq_pdu->format_params);
    logger.debug("rnti={}: PUCCH PDU on F1 HARQ resource updated: slot={} p_ind={} prbs={} sym={} cs={} occ={} {}",
                 current_grants.rnti,
                 pucch_slot_alloc.slot,
                 current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind,
                 existing_pdus.harq_pdu->resources.prbs,
                 existing_pdus.harq_pdu->resources.symbols,
                 harq_format_1.initial_cyclic_shift,
                 harq_format_1.time_domain_occ,
                 new_bits);
    const auto& common_params = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_1_common_param;
    existing_pdus.update_harq_pdu_bits(
        new_bits.harq_ack_nof_bits, sr_nof_bits::no_sr, csi_bits_format_0_and_1, *pucch_res_cfg, common_params);
    // Update the current grants with the new UCI (HARQ) bits.
    current_grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
    if (existing_pdus.sr_pdu != nullptr) {
      auto& sr_format_1 = std::get<pucch_format_1>(existing_pdus.sr_pdu->format_params);
      logger.debug(
          "rnti={}: PUCCH PDU on SR F1 resource updated: slot={} prbs={} sym={} cs={} occ={} uci_bits=[H={} S={}]",
          current_grants.rnti,
          pucch_slot_alloc.slot,
          existing_pdus.sr_pdu->resources.prbs,
          existing_pdus.sr_pdu->resources.symbols,
          sr_format_1.initial_cyclic_shift,
          sr_format_1.time_domain_occ,
          new_bits.harq_ack_nof_bits,
          sr_nof_bits_to_uint(current_bits.sr_bits));
      existing_pdus.update_sr_pdu_bits(current_bits.sr_bits, new_bits.harq_ack_nof_bits);
      // Update the current grants with the new UCI (HARQ) bits.
      current_grants.pucch_grants.sr_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
    }

  }
  // If the HARQ PDU uses F2, there can be 1 HARQ PDU + an optional CSI (F2). In any case, we only need to update the
  // HARQ-ACK bits in the HARQ-ACK PDU.
  else if (existing_pdus.harq_pdu->format() == pucch_format::FORMAT_2) {
    if (current_grants.pucch_grants.get_uci_bits().get_total_bits() >=
        ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().get_max_payload(pucch_format::FORMAT_2)) {
      logger.debug("rnti={}: PUCCH allocation (HARQ-ACK) for slot={} skipped. Cause: UCI bits exceed PUCCH payload",
                   current_grants.rnti,
                   sl_tx);
      return std::nullopt;
    }
    const pucch_resource* pucch_res_cfg = resource_manager.reserve_set_1_res_by_res_indicator(
        sl_tx, current_grants.rnti, current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind, pucch_cfg);
    srsran_assert(pucch_res_cfg != nullptr, "rnti={}: PUCCH expected resource not available", current_grants.rnti);
    logger.debug("rnti={}: PUCCH PDU on F2 HARQ resource updated: slot={} p_ind={} prbs={} sym={} {}",
                 current_grants.rnti,
                 pucch_slot_alloc.slot,
                 current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind,
                 existing_pdus.harq_pdu->resources.prbs,
                 existing_pdus.harq_pdu->resources.symbols,
                 existing_pdus.harq_pdu->uci_bits);
    const auto& common_params = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_2_common_param;
    existing_pdus.update_harq_pdu_bits(new_bits.harq_ack_nof_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.sr_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.csi_part1_nof_bits,
                                       *pucch_res_cfg,
                                       common_params);
    current_grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
    // If the HARQ PDU uses F3, there can be 1 HARQ PDU + an optional CSI (F3). In any case, we only need to update the
    // HARQ-ACK bits in the HARQ-ACK PDU.
  } else if (existing_pdus.harq_pdu->format() == pucch_format::FORMAT_3) {
    if (current_grants.pucch_grants.get_uci_bits().get_total_bits() >=
        ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().get_max_payload(pucch_format::FORMAT_3)) {
      logger.debug("rnti={}: PUCCH allocation (HARQ-ACK) for slot={} skipped. Cause: UCI bits exceed PUCCH payload",
                   current_grants.rnti,
                   sl_tx);
      return std::nullopt;
    }
    const pucch_resource* pucch_res_cfg = resource_manager.reserve_set_1_res_by_res_indicator(
        sl_tx, current_grants.rnti, current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind, pucch_cfg);
    srsran_assert(pucch_res_cfg != nullptr, "rnti={}: PUCCH expected resource not available", current_grants.rnti);
    logger.debug("rnti={}: PUCCH PDU on F3 HARQ resource updated: slot={} p_ind={} prbs={} sym={} {}",
                 current_grants.rnti,
                 pucch_slot_alloc.slot,
                 current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind,
                 existing_pdus.harq_pdu->resources.prbs,
                 existing_pdus.harq_pdu->resources.symbols,
                 existing_pdus.harq_pdu->uci_bits);
    const auto& common_params = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_3_common_param;
    existing_pdus.update_harq_pdu_bits(new_bits.harq_ack_nof_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.sr_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.csi_part1_nof_bits,
                                       *pucch_res_cfg,
                                       common_params);
    current_grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
    // If the HARQ PDU uses F4, there can be 1 HARQ PDU + an optional CSI (F4). In any case, we only need to update the
    // HARQ-ACK bits in the HARQ-ACK PDU.
  } else if (existing_pdus.harq_pdu->format() == pucch_format::FORMAT_4) {
    if (current_grants.pucch_grants.get_uci_bits().get_total_bits() >=
        ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().get_max_payload(pucch_format::FORMAT_4)) {
      logger.debug("rnti={}: PUCCH allocation (HARQ-ACK) for slot={} skipped. Cause: UCI bits exceed PUCCH payload",
                   current_grants.rnti,
                   sl_tx);
      return std::nullopt;
    }
    const pucch_resource* pucch_res_cfg = resource_manager.reserve_set_1_res_by_res_indicator(
        sl_tx, current_grants.rnti, current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind, pucch_cfg);
    srsran_assert(pucch_res_cfg != nullptr, "rnti={}: PUCCH expected resource not available", current_grants.rnti);
    const auto& format_4 = std::get<pucch_format_4>(existing_pdus.harq_pdu->format_params);
    logger.debug("rnti={}: PUCCH PDU on F4 HARQ resource updated: slot={} p_ind={} prbs={} sym={} occ={}/{} {}",
                 current_grants.rnti,
                 pucch_slot_alloc.slot,
                 current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind,
                 existing_pdus.harq_pdu->resources.prbs,
                 existing_pdus.harq_pdu->resources.symbols,
                 format_4.orthog_seq_idx,
                 fmt::underlying(format_4.n_sf_pucch_f4),
                 existing_pdus.harq_pdu->uci_bits);
    const auto& common_params = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_4_common_param;
    existing_pdus.update_harq_pdu_bits(new_bits.harq_ack_nof_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.sr_bits,
                                       current_grants.pucch_grants.harq_resource.value().bits.csi_part1_nof_bits,
                                       *pucch_res_cfg,
                                       common_params);
    current_grants.pucch_grants.harq_resource.value().bits.harq_ack_nof_bits = new_bits.harq_ack_nof_bits;
  }

  return current_grants.pucch_grants.harq_resource.value().harq_id.pucch_res_ind;
}

pucch_allocator_impl::pucch_grant_list
pucch_allocator_impl::multiplex_resources(slot_point                   sl_tx,
                                          rnti_t                       crnti,
                                          const pucch_grant_list&      candidate_grants,
                                          const ue_cell_configuration& ue_cell_cfg,
                                          std::optional<uint8_t>       preserve_res_indicator)
{
  // This function implements the multiplexing pseudo-code for PUCCH resources defined in Section 9.2.5, TS 38.213.
  // Refer to paragraph starting as "Set Q to the set of resources for transmission of corresponding PUCCHs in a single
  // slot without repetitions where".
  pucch_grant_list mplexed_grants;

  // The vector should contain at most 1 HARQ-ACK grant, 1 SR grant, 1 CSI grant.
  static constexpr size_t                   max_nof_grant = 3;
  static_vector<pucch_grant, max_nof_grant> resource_set_q;

  // Build the resource set Q. Refer to Section 9.2.5, TS 38.213.
  if (candidate_grants.harq_resource.has_value()) {
    resource_set_q.emplace_back(candidate_grants.harq_resource.value());
  }
  if (candidate_grants.sr_resource.has_value()) {
    resource_set_q.emplace_back(candidate_grants.sr_resource.value());
  }
  if (candidate_grants.csi_resource.has_value()) {
    resource_set_q.emplace_back(candidate_grants.csi_resource.value());
  }

  // Sort the resources in the set based on the number of symbols.
  auto sort_res_set_q = [&resource_set_q]() {
    std::sort(resource_set_q.begin(), resource_set_q.end(), [](const pucch_grant& lhs_res, const pucch_grant& rhs_res) {
      return lhs_res.symbols.start() < rhs_res.symbols.start() or
             (lhs_res.symbols.start() == rhs_res.symbols.start() and
              lhs_res.symbols.length() > rhs_res.symbols.length());
    });
  };

  sort_res_set_q();

  // This is the implementation of the pseudo-code for multiplexing the resources provided in Section 9.2.5, TS 38.213.
  unsigned o_cnt = 0;
  unsigned j_cnt = 0;
  while (j_cnt < resource_set_q.size()) {
    if (j_cnt < resource_set_q.size() - 1 and
        resource_set_q[j_cnt - o_cnt].symbols.overlaps(resource_set_q[j_cnt + 1].symbols)) {
      ++j_cnt;
      ++o_cnt;
    } else {
      if (o_cnt > 0U) {
        // Merge the overlapping resources.
        std::optional<pucch_grant> new_res =
            merge_pucch_resources(span<const pucch_grant>(&resource_set_q[j_cnt - o_cnt], o_cnt + 1),
                                  sl_tx,
                                  crnti,
                                  ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value(),
                                  preserve_res_indicator);
        if (not new_res.has_value()) {
          return {};
        }
        // Remove the old resources that got merged from the set.
        // TODO: check if, by using a different data structure, we can achieve the deletion more efficiently.
        resource_set_q.erase(resource_set_q.begin() + j_cnt - o_cnt, resource_set_q.begin() + j_cnt + 1);

        // Add the new resource (resulting from the previous merge) to the set.
        srsran_assert(not resource_set_q.full(), "PUCCH resource_set_q is full before adding a new resource");
        resource_set_q.push_back(new_res.value());

        // Sort the resources in the set based on the first symbol position and number of symbols.
        sort_res_set_q();

        // Reset the counter and start from the beginning.
        j_cnt = 0;
        o_cnt = 0;
      } else {
        ++j_cnt;
      }
    }
  }

  // The PUCCH resource multiplexing algorithm above is specified from the UE's perspective. In the GNB, we need to add
  // an extra resource Format 1 if slot there is a SR opportunity and HARQ bits to be reported with PUCCH Format 1.
  if (resource_set_q.size() == 1 and resource_set_q.front().format == pucch_format::FORMAT_1 and
      resource_set_q.front().bits.harq_ack_nof_bits != 0 and
      resource_set_q.front().bits.sr_bits != sr_nof_bits::no_sr) {
    const pucch_resource* sr_res =
        resource_manager.reserve_sr_res_available(sl_tx, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
    resource_manager.set_new_resource_allocation(crnti, pucch_resource_usage::SR);
    if (sr_res == nullptr) {
      logger.error("This is not expected");
    }
    // For Format 1, the grant with SR bit is the one with the SR-dedicated resource; in the HARQ-ACK grant we only
    // report the HARQ bits.
    pucch_uci_bits bits        = {.harq_ack_nof_bits  = resource_set_q.front().bits.harq_ack_nof_bits,
                                  .sr_bits            = resource_set_q.front().bits.sr_bits,
                                  .csi_part1_nof_bits = 0};
    auto&          sr_grant_f1 = resource_set_q.emplace_back(pucch_grant{.type = pucch_grant_type::sr, .bits = bits});
    sr_grant_f1.set_res_config(*sr_res);
    resource_set_q.front().bits.sr_bits = sr_nof_bits::no_sr;
  }

  // Build the final list of PUCCH resources that need to be transmitted.
  for (const auto& mplex_res : resource_set_q) {
    if (mplex_res.type == pucch_grant_type::harq_ack) {
      mplexed_grants.harq_resource.emplace(mplex_res);
    } else if (mplex_res.type == pucch_grant_type::sr) {
      mplexed_grants.sr_resource.emplace(mplex_res);
    } else if (mplex_res.type == pucch_grant_type::csi) {
      mplexed_grants.csi_resource.emplace(mplex_res);
    }
  }
  return mplexed_grants;
}

std::optional<pucch_allocator_impl::pucch_grant>
pucch_allocator_impl::merge_pucch_resources(span<const pucch_grant> resources_to_merge,
                                            slot_point              slot_harq,
                                            rnti_t                  crnti,
                                            const pucch_config&     pucch_cfg,
                                            std::optional<uint8_t>  preserve_res_indicator)
{
  // This function implements the merging rules for HARQ-ACK, SR and CSI defined in Section 9.2.5.1 and 9.2.5.2,
  // TS 38.213.

  // This function should only be called if there are 2 or 3 resources.
  if (resources_to_merge.size() == 1U or resources_to_merge.size() > 3U) {
    return std::nullopt;
  }

  if (resources_to_merge.size() == 2) {
    const pucch_grant& r_0 = resources_to_merge[0];
    const pucch_grant& r_1 = resources_to_merge[1];

    // SR and HARQ only.
    if ((r_0.type == pucch_grant_type::sr and r_1.type == pucch_grant_type::harq_ack) or
        (r_0.type == pucch_grant_type::harq_ack and r_1.type == pucch_grant_type::sr)) {
      const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
      const pucch_grant& r_sr   = r_0.type == pucch_grant_type::sr ? r_0 : r_1;

      // When merging SR and HARQ, no matter the format, the resource to be used is the one from HARQ-ACK.
      pucch_grant new_resource{.type = pucch_grant_type::harq_ack};
      // For the case of HARQ-ACK with Format 0 or Format 1, the pre-multiplexing HARQ-ACK grant only has HARQ-ACK
      // bits. NOTE: This rule is defined in Section 9.2.5.1, TS 38.213. For Format 1, in the GNB we need to consider
      // there can be either a resource with positive SR or a resource with negative SR; in this algorithm we assume
      // the positive SR; we'll add the resource for negative SR later on (the algorithm specified in Section 9.2.5
      // would not work with both resources).
      if (r_harq.format == pucch_format::FORMAT_0 or r_harq.format == pucch_format::FORMAT_1) {
        srsran_assert(r_sr.format == r_harq.format, "The two resources must have the same format");
        // TODO: Multiplexing of SR + HARQ on F0 needs to be checked, as the TS is not clear about it.
        new_resource              = r_harq;
        new_resource.bits.sr_bits = r_sr.bits.sr_bits;
        return new_resource;
      }
      // Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a PUCCH resource from
      // PUCCH res set 1. NOTE: This rule is defined in Section 9.2.5.1 or 9.2.5.2 (if any CSI bits), TS 38.213.
      // NOTE: For the case of HARQ-ACK with Format >= 2, the HARQ-ACK resource has HARQ-ACK bits and optionally CSI
      // bits.
      srsran_assert(r_sr.format == pucch_format::FORMAT_0 or r_sr.format == pucch_format::FORMAT_1,
                    "The two resources must have the same format");
      // Apply F2 CSI merging rule: SR and CSI PUCCH resources will be multiplexed in the CSI PUCCH resource.
      // A HARQ resource from PUCCH resource set idx 1 already exits. Use that one.
      if (r_harq.harq_id.pucch_set_idx == pucch_res_set_idx::set_1) {
        new_resource = r_harq;
      }
      // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
      // the PUCCH resource from set idx 0. NOTE: This sub-case is used by the PUCCH common and dedicated
      // allocator.
      else if (preserve_res_indicator.has_value()) {
        const pucch_resource* pucch_res = resource_manager.reserve_set_1_res_by_res_indicator(
            slot_harq, crnti, preserve_res_indicator.value(), pucch_cfg);
        resource_manager.set_new_resource_allocation(crnti, pucch_resource_usage::HARQ_SET_1);
        if (pucch_res != nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
        new_resource.harq_id.pucch_res_ind = r_harq.harq_id.pucch_res_ind;
        new_resource.set_res_config(*pucch_res);
      }
      // Get a new HARQ resource (from PUCCH resource set idx 1) from the resource manager.
      else {
        pucch_harq_resource_alloc_record res_alloc =
            resource_manager.reserve_next_set_1_harq_res_available(slot_harq, crnti, pucch_cfg);
        resource_manager.set_new_resource_allocation(crnti, pucch_resource_usage::HARQ_SET_1);
        if (res_alloc.pucch_res != nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
        new_resource.harq_id.pucch_res_ind = res_alloc.pucch_res_indicator;
        new_resource.set_res_config(*res_alloc.pucch_res);
      }
      // If any CSI bits, these are contained in the HARQ-ACK resource.
      new_resource.bits.harq_ack_nof_bits  = r_harq.bits.harq_ack_nof_bits;
      new_resource.bits.csi_part1_nof_bits = r_harq.bits.csi_part1_nof_bits;
      new_resource.bits.sr_bits            = r_sr.bits.sr_bits;
      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > pucch_cfg.get_max_payload(new_resource.format)) {
        return std::nullopt;
      }
      return new_resource;
    }

    // SR and CSI only. In the case, the SR resource has only SR bits, and the CSI resource has only CSI bits.
    // Apply CSI merging rule for SR + CSI; this is defined in Section 9.2.5.1, TS 38.213.
    if ((r_0.type == pucch_grant_type::sr and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::sr)) {
      // Apply F2 CSI merging rule: SR and CSI PUCCH resources will be multiplexed in the CSI PUCCH resource.
      pucch_grant        new_resource{.type = pucch_grant_type::csi};
      const pucch_grant& r_csi = r_0.type == pucch_grant_type::csi ? r_0 : r_1;
      const pucch_grant& r_sr  = r_0.type == pucch_grant_type::sr ? r_0 : r_1;
      //  We don't support SR with Format 0 on the same slot as CSI.
      srsran_assert(r_sr.format != pucch_format::FORMAT_0, "SR with Format 0 is not supported on the same slot as CSI");
      // Copy the SR bits in the CSI resource.
      new_resource              = r_csi;
      new_resource.bits.sr_bits = r_sr.bits.sr_bits;

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > pucch_cfg.get_max_payload(new_resource.format)) {
        return std::nullopt;
      }
      return new_resource;
    }

    // HARQ and CSI only.  Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a
    // PUCCH resource from PUCCH resource set 1. NOTE: This rule is defined in Section 9.2.5.2, TS 38.213. NOTE: SR
    // bits, if present, can be in either HARQ or CSI, but not in both.
    if ((r_0.type == pucch_grant_type::harq_ack and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::harq_ack)) {
      // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res
      // set 1.
      pucch_grant        new_resource{.type = pucch_grant_type::harq_ack};
      const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
      const pucch_grant& r_csi  = r_0.type == pucch_grant_type::csi ? r_0 : r_1;

      // A HARQ resource from PUCCH resource set idx 1 already exits. Use that one.
      if (r_harq.harq_id.pucch_set_idx == pucch_res_set_idx::set_1) {
        new_resource = r_harq;
      }
      // NOTE: In this case, the PUCCH resource before merging is from PUCCH resource set 0.
      else {
        // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
        // the PUCCH resource from set idx 0.
        // NOTE: If preserve_res_indicator has a value, then the function was called by the PUCCH common and dedicated
        // allocator.
        const unsigned pucch_res_indicator =
            preserve_res_indicator.has_value() ? preserve_res_indicator.value() : r_harq.harq_id.pucch_res_ind;
        const pucch_resource* pucch_res =
            resource_manager.reserve_set_1_res_by_res_indicator(slot_harq, crnti, pucch_res_indicator, pucch_cfg);
        resource_manager.set_new_resource_allocation(crnti, pucch_resource_usage::HARQ_SET_1);
        if (pucch_res == nullptr) {
          return std::nullopt;
        }
        new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
        new_resource.harq_id.pucch_res_ind = r_harq.harq_id.pucch_res_ind;
        new_resource.set_res_config(*pucch_res);
      }

      new_resource.bits.harq_ack_nof_bits  = r_harq.bits.harq_ack_nof_bits;
      new_resource.bits.csi_part1_nof_bits = r_csi.bits.csi_part1_nof_bits;
      // SR bits, if present, can be in either HARQ or CSI, but not in both.
      if (r_harq.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_harq.bits.sr_bits;
      } else if (r_csi.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_csi.bits.sr_bits;
      }

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() > pucch_cfg.get_max_payload(new_resource.format)) {
        return std::nullopt;
      }
      return new_resource;
    }
  }

  // Apply HARQ merging rule for Format >= 2: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH
  // resource set 1.
  // NOTE: This rule is defined in Section 9.2.5.2, TS 38.213.
  if (resources_to_merge.size() == 3) {
    // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res set 1.
    pucch_grant        new_resource{.type = pucch_grant_type::harq_ack};
    const pucch_grant* r_harq = nullptr;
    const pucch_grant* r_sr   = nullptr;
    const pucch_grant* r_csi  = nullptr;
    for (const auto& grant : resources_to_merge) {
      if (grant.type == pucch_grant_type::harq_ack) {
        r_harq = &grant;
      } else if (grant.type == pucch_grant_type::sr) {
        r_sr = &grant;
      } else if (grant.type == pucch_grant_type::csi) {
        r_csi = &grant;
      }
    }

    srsran_assert(r_harq != nullptr and r_sr != nullptr and r_csi != nullptr, "The three resources must be present");
    if (r_sr->format == pucch_format::FORMAT_0) {
      srsran_assertion_failure("This case is not yet supported");
      // SR and CSI are not supported on the same slot if SR uses Format 0.
      return std::nullopt;
    }

    if (r_harq->format != pucch_format::FORMAT_0 and r_harq->format != pucch_format::FORMAT_1) {
      new_resource = *r_harq;
    }
    // NOTE: In this case, the PUCCH resource before merging is from PUCCH resource set 0.
    else {
      // Get a resource from PUCCH resource set idx 1, if available, with the same PUCCH resource indicator as for
      // the PUCCH resource from set idx 0.

      const pucch_resource* pucch_res = resource_manager.reserve_set_1_res_by_res_indicator(
          slot_harq, crnti, r_harq->harq_id.pucch_res_ind, pucch_cfg);
      resource_manager.set_new_resource_allocation(crnti, pucch_resource_usage::HARQ_SET_1);
      if (pucch_res == nullptr) {
        return std::nullopt;
      }
      new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
      new_resource.harq_id.pucch_res_ind = r_harq->harq_id.pucch_res_ind;
      new_resource.set_res_config(*pucch_res);
    }
    new_resource.bits.harq_ack_nof_bits  = r_harq->bits.harq_ack_nof_bits;
    new_resource.bits.sr_bits            = r_sr->bits.sr_bits;
    new_resource.bits.csi_part1_nof_bits = r_csi->bits.csi_part1_nof_bits;

    // Check if the UCI payload fits in the PUCCH resource.
    if (new_resource.bits.get_total_bits() > pucch_cfg.get_max_payload(new_resource.format)) {
      return std::nullopt;
    }
    return new_resource;
  }

  return std::nullopt;
}

std::optional<unsigned> pucch_allocator_impl::allocate_grants(cell_slot_resource_allocator& pucch_slot_alloc,
                                                              ue_grants&                    existing_pucchs,
                                                              rnti_t                        crnti,
                                                              const pucch_grant_list&       grants_to_tx,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  auto& pucch_pdus = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH PDUs.
  existing_pucch_pdus_handler existing_pdus(
      crnti, pucch_pdus, get_sr_pucch_res_cfg(ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value()));

  // Check if we can fit the new PUCCH PDUs in the output results.
  if ((grants_to_tx.get_nof_grants() >= existing_pdus.get_nof_unallocated_pdu()) and
      (pucch_slot_alloc.result.ul.pucchs.size() +
           (grants_to_tx.get_nof_grants() - existing_pdus.get_nof_unallocated_pdu()) >
       get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size())))) {
    logger.debug(
        "rnti={}: PUCCH allocation for slot={} skipped. Cause: UL grants reached", crnti, pucch_slot_alloc.slot);
    return std::nullopt;
  }

  bool harq_grant_alloc_completed = false;
  bool sr_grant_alloc_completed   = false;
  bool csi_grant_alloc_completed  = false;

  // If there was a CSI grant, re-use the previous one and update the UCI bits with SR.
  if (grants_to_tx.csi_resource.has_value() and existing_pucchs.pucch_grants.csi_resource.has_value() and
      existing_pdus.csi_pdu != nullptr) {
    const auto& csi_res = grants_to_tx.csi_resource.value();
    logger.debug("rnti={}: PUCCH PDU allocated on CSI resource: slot={} prbs={} sym={} uci_bits={}",
                 crnti,
                 pucch_slot_alloc.slot,
                 existing_pdus.csi_pdu->resources.prbs,
                 existing_pdus.csi_pdu->resources.symbols,
                 existing_pdus.csi_pdu->uci_bits);
    existing_pdus.update_csi_pdu_bits(csi_res.bits.csi_part1_nof_bits, csi_res.bits.sr_bits);
    csi_grant_alloc_completed = true;
  }
  // If there was a SR grant, re-use the previous one and update UCI bits with HARQ bits.
  else if (grants_to_tx.sr_resource.has_value() and existing_pucchs.pucch_grants.sr_resource.has_value() and
           existing_pdus.sr_pdu != nullptr) {
    const auto& sr_res = grants_to_tx.sr_resource.value();
    switch (sr_res.format) {
      case pucch_format::FORMAT_0:
        logger.debug("rnti={}: PUCCH PDU allocated on SR FO resource (updated): slot={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     existing_pdus.sr_pdu->resources.prbs,
                     existing_pdus.sr_pdu->resources.symbols,
                     sr_res.bits);
        break;
      case pucch_format::FORMAT_1: {
        const auto& format_1 = std::get<pucch_format_1>(existing_pdus.sr_pdu->format_params);
        logger.debug(
            "rnti={}: PUCCH PDU allocated on SR F1 resource (updated): slot={} prbs={} sym={} cs={} occ={} uci_bits={}",
            crnti,
            pucch_slot_alloc.slot,
            existing_pdus.sr_pdu->resources.prbs,
            existing_pdus.sr_pdu->resources.symbols,
            format_1.initial_cyclic_shift,
            format_1.time_domain_occ,
            sr_res.bits);
      } break;
      default:
        srsran_assertion_failure("Invalid PUCCH Format for SR (should be 0 or 1)");
        break;
    }

    existing_pdus.update_sr_pdu_bits(sr_res.bits.sr_bits, sr_res.bits.harq_ack_nof_bits);
    sr_grant_alloc_completed = true;
  }

  pucch_uci_bits bits = grants_to_tx.get_uci_bits();
  if (grants_to_tx.csi_resource.has_value() and not csi_grant_alloc_completed) {
    const auto& csi_res = grants_to_tx.csi_resource.value();
    pucch_info* grant   = existing_pdus.get_next_grant(pucch_pdus);
    srsran_assert(grant != nullptr and csi_res.pucch_res_cfg != nullptr,
                  "Neither the (CSI) return grant nor the PUCCH res configuration can be nullptr");

    switch (csi_res.format) {
      case pucch_format::FORMAT_2: {
        const unsigned nof_prbs = std::get<pucch_format_2_3_cfg>(csi_res.pucch_res_cfg->format_params).nof_prbs;
        fill_pucch_format2_grant(*grant,
                                 crnti,
                                 *csi_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 nof_prbs,
                                 0U,
                                 csi_res.bits.sr_bits,
                                 csi_res.bits.csi_part1_nof_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on CSI F2 resource: slot={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_3: {
        const unsigned nof_prbs = std::get<pucch_format_2_3_cfg>(csi_res.pucch_res_cfg->format_params).nof_prbs;
        fill_pucch_format3_grant(*grant,
                                 crnti,
                                 *csi_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 nof_prbs,
                                 0U,
                                 csi_res.bits.sr_bits,
                                 csi_res.bits.csi_part1_nof_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on CSI F3 resource: slot={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_4: {
        fill_pucch_format4_grant(*grant,
                                 crnti,
                                 *csi_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 0U,
                                 csi_res.bits.sr_bits,
                                 csi_res.bits.csi_part1_nof_bits);
        const auto& format_4 = std::get<pucch_format_4>(grant->format_params);
        logger.debug("rnti={}: PUCCH PDU allocated on CSI F4 resource: slot={} prbs={} sym={} occ={}/{} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     format_4.orthog_seq_idx,
                     fmt::underlying(format_4.n_sf_pucch_f4),
                     grant->uci_bits);
      } break;
      default:
        srsran_assertion_failure("Invalid PUCCH Format for CSI (should be 2, 3, or 4)");
        break;
    }
  }

  if (grants_to_tx.sr_resource.has_value() and not sr_grant_alloc_completed) {
    const auto& sr_res = grants_to_tx.sr_resource.value();
    pucch_info* grant  = existing_pdus.get_next_grant(pucch_pdus);
    srsran_assert(grant != nullptr and sr_res.pucch_res_cfg != nullptr,
                  "Neither the (SR) return grant nor the PUCCH res configuration can be nullptr");

    switch (sr_res.format) {
      case pucch_format::FORMAT_0: {
        fill_pucch_ded_format0_grant(*grant,
                                     crnti,
                                     *grants_to_tx.sr_resource.value().pucch_res_cfg,
                                     grants_to_tx.sr_resource.value().bits.harq_ack_nof_bits,
                                     grants_to_tx.sr_resource.value().bits.sr_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on SR F0 resource: slot={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_1: {
        fill_pucch_ded_format1_grant(*grant,
                                     crnti,
                                     *grants_to_tx.sr_resource.value().pucch_res_cfg,
                                     grants_to_tx.sr_resource.value().bits.harq_ack_nof_bits,
                                     grants_to_tx.sr_resource.value().bits.sr_bits);
        const auto& format_1 = std::get<pucch_format_1>(grant->format_params);
        logger.debug("rnti={}: PUCCH PDU allocated on SR F1 resource: slot={} prbs={} sym={} cs={} occ={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     format_1.initial_cyclic_shift,
                     format_1.time_domain_occ,
                     grant->uci_bits);
      } break;
      default:
        srsran_assertion_failure("Invalid PUCCH Format for SR (should be 0 or 1)");
        break;
    }
  }

  if (grants_to_tx.harq_resource.has_value() and not harq_grant_alloc_completed) {
    // Allocate HARQ-ACK grant.
    pucch_info* grant    = existing_pdus.get_next_grant(pucch_pdus);
    const auto& harq_res = grants_to_tx.harq_resource.value();
    srsran_assert(harq_res.pucch_res_cfg != nullptr, "The PUCCH res configuration (HARQ-ACK) cannot be nullptr");

    switch (harq_res.format) {
      case pucch_format::FORMAT_0:
        fill_pucch_ded_format0_grant(
            *grant, crnti, *harq_res.pucch_res_cfg, harq_res.bits.harq_ack_nof_bits, harq_res.bits.sr_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on F0 HARQ resource: slot={} p_ind={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     harq_res.harq_id.pucch_res_ind,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
        break;
      case pucch_format::FORMAT_1: {
        fill_pucch_ded_format1_grant(
            *grant, crnti, *harq_res.pucch_res_cfg, harq_res.bits.harq_ack_nof_bits, harq_res.bits.sr_bits);
        const auto& format_1 = std::get<pucch_format_1>(grant->format_params);
        logger.debug("rnti={}: PUCCH PDU allocated on F1 HARQ resource: slot={} p_ind={} prbs={} sym={} cs={} occ={} "
                     "uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     harq_res.harq_id.pucch_res_ind,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     format_1.initial_cyclic_shift,
                     format_1.time_domain_occ,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_2: {
        const auto& f2_cfg              = std::get<pucch_format_2_3_cfg>(harq_res.pucch_res_cfg->format_params);
        const float max_pucch_code_rate = to_max_code_rate_float(
            ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_2_common_param.value().max_c_rate);
        const unsigned nof_prbs = get_pucch_format2_nof_prbs(
            bits.get_total_bits(), f2_cfg.nof_prbs, harq_res.pucch_res_cfg->nof_symbols, max_pucch_code_rate);
        fill_pucch_format2_grant(*grant,
                                 crnti,
                                 *harq_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 nof_prbs,
                                 harq_res.bits.harq_ack_nof_bits,
                                 harq_res.bits.sr_bits,
                                 harq_res.bits.csi_part1_nof_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on F2 HARQ resource: slot={} p_ind={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     harq_res.harq_id.pucch_res_ind,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_3: {
        const auto& f3                  = std::get<pucch_format_2_3_cfg>(harq_res.pucch_res_cfg->format_params);
        const float max_pucch_code_rate = to_max_code_rate_float(
            ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_3_common_param.value().max_c_rate);
        const auto&    common_param = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().format_3_common_param.value();
        const unsigned nof_prbs     = get_pucch_format3_nof_prbs(bits.get_total_bits(),
                                                             f3.nof_prbs,
                                                             harq_res.pucch_res_cfg->nof_symbols,
                                                             max_pucch_code_rate,
                                                             harq_res.pucch_res_cfg->second_hop_prb.has_value(),
                                                             common_param.additional_dmrs,
                                                             common_param.pi_2_bpsk);
        fill_pucch_format3_grant(*grant,
                                 crnti,
                                 *harq_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 nof_prbs,
                                 harq_res.bits.harq_ack_nof_bits,
                                 harq_res.bits.sr_bits,
                                 harq_res.bits.csi_part1_nof_bits);
        logger.debug("rnti={}: PUCCH PDU allocated on F3 HARQ resource: slot={} p_ind={} prbs={} sym={} uci_bits={}",
                     crnti,
                     pucch_slot_alloc.slot,
                     harq_res.harq_id.pucch_res_ind,
                     grant->resources.prbs,
                     grant->resources.symbols,
                     grant->uci_bits);
      } break;
      case pucch_format::FORMAT_4: {
        fill_pucch_format4_grant(*grant,
                                 crnti,
                                 *harq_res.pucch_res_cfg,
                                 ue_cell_cfg,
                                 harq_res.bits.harq_ack_nof_bits,
                                 harq_res.bits.sr_bits,
                                 harq_res.bits.csi_part1_nof_bits);
        const auto& format_4 = std::get<pucch_format_4>(grant->format_params);
        logger.debug(
            "rnti={}: PUCCH PDU allocated on F4 HARQ resource: slot={} p_ind={} prbs={} sym={} occ={}/{} uci_bits={}",
            crnti,
            pucch_slot_alloc.slot,
            harq_res.harq_id.pucch_res_ind,
            grant->resources.prbs,
            grant->resources.symbols,
            format_4.orthog_seq_idx,
            fmt::underlying(format_4.n_sf_pucch_f4),
            grant->uci_bits);
      } break;
      default:
        srsran_assertion_failure("Unexpected PUCCH Format");
        break;
    }
  }

  slot_point sl_tx = pucch_slot_alloc.slot;

  // Remove unused PUCCH PDU, if any.
  existing_pdus.remove_unused_pdus(pucch_pdus, crnti);

  // Remove the previously allocated PUCCH resources which are not needed after the new allocation.
  remove_unused_pucch_res(sl_tx, grants_to_tx, existing_pucchs, ue_cell_cfg);

  // Update the new grants to the UE allocation record.
  existing_pucchs.pucch_grants = grants_to_tx;

  // TODO: unmark on multiplexing.
  if (grants_to_tx.sr_resource.has_value()) {
    srsran_assert(grants_to_tx.sr_resource.value().pucch_res_cfg != nullptr,
                  "SR PUCCH resource cannot have null pointer");
    mark_pucch_in_resource_grid(pucch_slot_alloc, *grants_to_tx.sr_resource.value().pucch_res_cfg, ue_cell_cfg);
  }
  if (grants_to_tx.csi_resource.has_value()) {
    srsran_assert(grants_to_tx.csi_resource.value().pucch_res_cfg != nullptr,
                  "CSI PUCCH resource cannot have null pointer");
    mark_pucch_in_resource_grid(pucch_slot_alloc, *grants_to_tx.csi_resource.value().pucch_res_cfg, ue_cell_cfg);
  }
  if (grants_to_tx.harq_resource.has_value()) {
    srsran_assert(grants_to_tx.harq_resource.value().pucch_res_cfg != nullptr,
                  "HARQ-ACK PUCCH resource cannot have null pointer");
    mark_pucch_in_resource_grid(pucch_slot_alloc, *grants_to_tx.harq_resource.value().pucch_res_cfg, ue_cell_cfg);
  }

  // The return value is only relevant if the allocation was called for a HARQ-ACK grant.
  return grants_to_tx.harq_resource.has_value() ? grants_to_tx.harq_resource.value().harq_id.pucch_res_ind : 0U;
}
