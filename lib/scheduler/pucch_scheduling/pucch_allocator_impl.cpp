/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_allocator_impl.h"
#include "../support/csi_report_helpers.h"
#include "../support/pucch/pucch_default_resource.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/srslog/srslog.h"
#include <algorithm>

//////////////     Helper functions       //////////////

using namespace srsran;

static unsigned get_n_id0_scrambling(const ue_cell_configuration& ue_cell_cfg, unsigned cell_pci)
{
  // As per TS 38.211, Section 6.4.1.3.2.1, "N_{ID}^0 is given by the higher-layer parameter scramblingID0 in the
  // DMRS-UplinkConfig IE if provided and by N_{ID}^{cell} otherwise. If a UE is configured with both
  // dmrs-UplinkForPUSCH-MappingTypeA and dmrs-UplinkForPUSCH-MappingTypeB, scramblingID0 is obtained from
  // dmrs-UplinkForPUSCH-MappingTypeB.

  // Check \c scrambling_id0 in \c dmrs-UplinkForPUSCH-MappingTypeB, first
  if (ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_b_dmrs.has_value() and
      ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_b_dmrs.value()
          .trans_precoder_disabled.has_value() and
      ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_b_dmrs.value()
          .trans_precoder_disabled.value()
          .scrambling_id0.has_value()) {
    return ue_cell_cfg.cfg_dedicated()
        .ul_config.value()
        .init_ul_bwp.pusch_cfg.value()
        .pusch_mapping_type_b_dmrs.value()
        .trans_precoder_disabled.value()
        .scrambling_id0.value();
  }
  // Else, check \c scrambling_id0 in \c dmrs-UplinkForPUSCH-MappingTypeA.
  if (ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_a_dmrs.has_value() and
      ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_a_dmrs.value()
          .trans_precoder_disabled.has_value() and
      ue_cell_cfg.cfg_dedicated()
          .ul_config.value()
          .init_ul_bwp.pusch_cfg.value()
          .pusch_mapping_type_a_dmrs.value()
          .trans_precoder_disabled.value()
          .scrambling_id0.has_value()) {
    return ue_cell_cfg.cfg_dedicated()
        .ul_config.value()
        .init_ul_bwp.pusch_cfg.value()
        .pusch_mapping_type_a_dmrs.value()
        .trans_precoder_disabled.value()
        .scrambling_id0.value();
  }

  return cell_pci;
}

//////////////    Public functions       //////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_,
                                           unsigned                  max_pucchs_per_slot,
                                           unsigned                  max_ul_grants_per_slot_) :
  cell_cfg(cell_cfg_),
  max_pucch_grants_per_slot(max_pucchs_per_slot),
  max_ul_grants_per_slot(max_ul_grants_per_slot_),
  garbage_collector(resource_manager),
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

std::optional<unsigned> pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
                                                                             rnti_t                      tcrnti,
                                                                             unsigned                    k0,
                                                                             unsigned                    k1,
                                                                             const pdcch_dl_information& dci_info)
{
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = slot_alloc[k0 + k1 + slot_alloc.cfg.ntn_cs_koffset];

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
          get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size())) or
      pucch_common_alloc_grid[slot_alloc[k0 + k1 + slot_alloc.cfg.ntn_cs_koffset].slot.to_uint()].full()) {
    return std::nullopt;
  }

  if (not cell_cfg.is_fully_ul_enabled(pucch_slot_alloc.slot)) {
    return std::nullopt;
  }

  // If there are existing PUCCH grants that are either F2 for CSI or F1 for SR, allocate the PUCCH common grant anyway
  // without multiplexing it with the existing one. Otherwise, if the existing grant is F1 for HARQ-ACK, do not allocate
  // on the same slot.
  const bool has_existing_pucch_f1_grants = std::find_if(pucch_slot_alloc.result.ul.pucchs.begin(),
                                                         pucch_slot_alloc.result.ul.pucchs.end(),
                                                         [tcrnti](const pucch_info& pucch) {
                                                           return tcrnti == pucch.crnti and
                                                                  pucch.format == pucch_format::FORMAT_1 and
                                                                  pucch.format_1.harq_ack_nof_bits != 0;
                                                         }) != pucch_slot_alloc.result.ul.pucchs.end();
  if (has_existing_pucch_f1_grants or has_common_pucch_f1_grant(tcrnti, pucch_slot_alloc.slot)) {
    logger.debug("tc-rnti={}: PUCCH common not allocated for slot={}. Cause: a PUCCH F1 grant with HARQ-ACK bits "
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
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().first_hop_res);
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().second_hop_res);

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_info, tcrnti, pucch_res.value());
  unsigned pucch_res_indicator = pucch_res.value().pucch_res_indicator;

  pucch_common_alloc_grid[slot_alloc[k0 + k1 + slot_alloc.cfg.ntn_cs_koffset].slot.to_uint()].emplace_back(tcrnti);

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

  existing_pucch_grants existing_grants =
      get_existing_pucch_grants(pucch_slot_alloc.result.ul.pucchs, rnti, pucch_slot);

  // NOTE: this function is called by the UE fallback scheduler, which iterates over several PDCCH slots and different
  // k1 values. It can happen that the UE fallback scheduler attempts to allocate a grant on a slot where it previously
  // allocated another grant. If that is the case, quit the PUCCH allocation.
  if (existing_grants.format1_harq_common_grant != nullptr or existing_grants.format1_harq_grant != nullptr) {
    logger.debug("rnti={}: PUCCH HARQ-ACK for slot={} not allocated. Cause: another F1 PUCCH grant with HARQ-ACK bits "
                 "already exists for the same UE for the same slot",
                 rnti,
                 pucch_slot);
    return std::nullopt;
  }

  srsran_assert(not(existing_grants.format1_sr_grant != nullptr and existing_grants.format2_grant != nullptr),
                "It is expected that there are either no grants, or at most 1 PUCCH grant (F1 SR and F2 for CSI)");

  // If a F2 PUCCH grant with HARQ-ACK bits exists, then there must be as well a common PUCCH F1 grant (with 1 HARQ-ACK
  // bit); in that case, the function should have returned already in the previous "if" check.
  srsran_assert(existing_grants.format2_grant == nullptr or
                    existing_grants.format2_grant->format_2.harq_ack_nof_bits == 0,
                "If the existing PUCCH grant is F2, it must be for CSI or CSI/SR reporting only");

  pucch_info* current_existing_grant = nullptr;
  // If there are no existing grants or if the existing one is F1 (for SR), we need to add 2 additional PUCCH grants: 1
  // on common resources and 1 on dedicated resources (for HARQ-ACK bit).
  unsigned extra_pucch_grants_to_allocate = 2;
  if (existing_grants.format1_sr_grant != nullptr) {
    current_existing_grant = existing_grants.format1_sr_grant;
  } else if (existing_grants.format2_grant != nullptr) {
    current_existing_grant = existing_grants.format2_grant;
    // If the existing PUCCH grant is F2 (for CSI), we need to allocate only 1 additional PUCCH grant on common
    // resource; the CSI + additional HARQ-ACK bit will be allocated on a F2 resource which will replace the existing F2
    // resource for CSI.
    extra_pucch_grants_to_allocate = 1;
  }

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() + extra_pucch_grants_to_allocate >
          pucch_slot_alloc.result.ul.pucchs.capacity() or
      pucch_slot_alloc.result.ul.pucchs.size() + extra_pucch_grants_to_allocate >
          get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size())) or
      pucch_common_alloc_grid[pucch_slot.to_uint()].full()) {
    return std::nullopt;
  }

  // Find a couple of PUCCH resources (1 common, 1 dedicated) that are (i) are available and that (ii) have the same
  // PUCCH resource indicator.
  std::optional<pucch_com_ded_res> harq_res_cfgs =
      find_common_and_ded_harq_res_available(pucch_slot_alloc, current_existing_grant, rnti, ue_cell_cfg, dci_info.ctx);

  // If both PUCCH common and dedicated resources are available, allocate them. If it is not possible to allocate the
  // dedicated resource (this can only happen if the UCI bits exceeds the PUCCH F2 capacity), then the function will
  // abort the allocation. The caller will attempt a new allocation in the next UL slot.
  if (harq_res_cfgs.has_value()) {
    return exec_common_and_ded_res_alloc(pucch_slot_alloc,
                                         current_existing_grant,
                                         rnti,
                                         ue_cell_cfg,
                                         harq_res_cfgs.value().pucch_common_info,
                                         harq_res_cfgs.value().pucch_ded_cfg);
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
  // TS 38.213, Section 9.2.3, explains the UE's procedure to multiplex HARQ-ACK reporting of multiple slot and for
  // different cells.
  // "The PUCCH resource determination is based on a PUCCH resource indicator field [5, TS 38.212] in a last DCI
  // format 1_0 or DCI format 1_1, among the DCI formats 1_0 or DCI formats 1_1 that have a value of a
  // PDSCH-to-HARQ_feedback timing indicator field indicating a same slot for the PUCCH transmission, that the UE
  // detects and for which the UE transmits corresponding HARQ-ACK information in the PUCCH where, for PUCCH resource
  // determination, detected DCI formats are first indexed in an ascending order across serving cells indexes for a
  // same PDCCH monitoring occasion and are then indexed in an ascending order across PDCCH monitoring occasion
  // indexes". As a result of this, and depending on whether there is any scheduled SRs, the PUCCH allocator can
  // either allocate a new PUCCH grant or update an existing one by changing the number of HARQ-ACK bits to be
  // reported.

  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];

  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH grants.
  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti, pucch_slot_alloc.slot);

  // [Implementation-defined] Multiplexing of common and dedicated PUCCH grants are not yet supported.
  if (existing_grants.format1_harq_common_grant != nullptr) {
    logger.debug(
        "rnti={}: PUCCH HARQ-ACK for slot={} not allocated. Cause: Multiplexing of common and dedicated PUCCH grants "
        "are not supported",
        crnti,
        pucch_slot_alloc.slot);
    return std::nullopt;
  }

  const unsigned harq_ack_bits_increment = 1;

  // Case 1) If there is a PUCCH format 2 grant, update it.
  if (existing_grants.format2_grant != nullptr) {
    // Case 1-A) If the allocated resource is the one specific for CSI, change resource and allocate grant.
    if (existing_grants.format2_grant->format_2.harq_ack_nof_bits == 0 and
        existing_grants.format2_grant->format_2.csi_part1_bits > 0) {
      return change_format2_resource(
          pucch_slot_alloc, *existing_grants.format2_grant, crnti, ue_cell_cfg, harq_ack_bits_increment, {});
    }

    // Case 1-B) If the allocated resource is for HARQ too, just update the resource.
    return add_harq_bits_to_harq_f2_grant(
        *existing_grants.format2_grant, pucch_slot_alloc.slot, crnti, ue_cell_cfg, harq_ack_bits_increment);
  }

  // Case 2) An HARQ Format 1 is already scheduled. Update the existing HARQ grant and the SR grant, if present.
  if (existing_grants.format1_harq_grant != nullptr) {
    if (existing_grants.format1_sr_grant != nullptr) {
      srsran_sanity_check(existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits ==
                              existing_grants.format1_sr_grant->format_1.harq_ack_nof_bits,
                          "Mismatch HARQ bits mismatch between SR and HARQ grants");
    }

    // Case 2-A) If the existing grant is for at least 2 HARQ-ACK bits, then the PUCCH needs to be converted to
    // format 2.
    if (existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits > 1) {
      return convert_to_format2_harq(pucch_slot_alloc,
                                     *existing_grants.format1_harq_grant,
                                     existing_grants.format1_sr_grant,
                                     crnti,
                                     ue_cell_cfg,
                                     harq_ack_bits_increment);
    }

    // Case 2-B)
    return add_harq_ack_bit_to_format1_grant(
        *existing_grants.format1_harq_grant,
        existing_grants.format1_sr_grant,
        crnti,
        pucch_slot_alloc.slot,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  }

  // Case C) If there is no existing HARQ_ACK grant, allocate a new one and update the SR grant, if present.
  return allocate_new_format1_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg, existing_grants.format1_sr_grant);
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
    logger.info("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: scheduler cache full",
                crnti,
                pucch_slot_alloc.slot);
    return;
  }

  // NOTE: This check can be removed in future refactors, it's not required by the SR allocator. At the moment, we
  // schedule the SRs before anything else, therefore we don't expect to find any existing PUCCH grant.
  const auto* existing_grant_it = std::find_if(pucch_grants_alloc_grid[sl_tx.to_uint()].begin(),
                                               pucch_grants_alloc_grid[sl_tx.to_uint()].end(),
                                               [crnti](const ue_pucch_bits& ue) { return ue.rnti != crnti; });
  if (existing_grant_it == pucch_grants_alloc_grid[sl_tx.to_uint()].end()) {
    logger.info("No PUCCH grants are expected before allocating a new SR grant", crnti, pucch_slot_alloc.slot);
    return;
  }

  // Get the index of the PUCCH resource to be used for SR.
  const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("rnti={}: SR occasion allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  const pucch_format format = pucch_sr_res->format;

  // Allocate PUCCH SR grant only.
  const unsigned harq_ack_bits_increment = 0U;
  if (format == pucch_format::FORMAT_0) {
    srsran_assertion_failure("PUCCH Format 0 is not yet supported for SR");
  } else {
    fill_pucch_ded_format1_grant(pucch_slot_alloc.result.ul.pucchs.emplace_back(),
                                 crnti,
                                 *pucch_sr_res,
                                 harq_ack_bits_increment,
                                 sr_nof_bits::one);
  }

  // Save the info in the scheduler list of PUCCH grants.
  auto& sr_pucch_grant = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_pucch_bits{.rnti = crnti});
  sr_pucch_grant.pucch_grants.sr_resource.emplace(
      pucch_grant{.type = pucch_grant_type::sr, .format = format, .pucch_res_cfg = pucch_sr_res});
  sr_pucch_grant.pucch_grants.sr_resource.value().bits.sr_bits = sr_nof_bits::one;
}

void pucch_allocator_impl::pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          unsigned                      csi_part1_nof_bits)
{
  const slot_point sl_tx = pucch_slot_alloc.slot;

  garbage_collector.reset();

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
                                         [crnti](const ue_pucch_bits& ue) { return ue.rnti != crnti; });

  // Handle case of no existing PUCCH grant.
  if (existing_grant_it == pucch_grants_alloc_grid[sl_tx.to_uint()].end()) {
    allocate_csi_grant(pucch_slot_alloc, crnti, ue_cell_cfg, csi_part1_nof_bits);
    return;
  }

  // Handle case of existing PUCCHs with possible multiplexing.
  uci_bits bits_for_uci = existing_grant_it->pucch_grants.get_uci_bits();
  srsran_assert(bits_for_uci.csi_part1_bits == 0, "PUCCH grant for CSI already been allocated");
  bits_for_uci.csi_part1_bits = csi_part1_nof_bits;
  auto alloc_outcome = multiplex_and_allocate_pucch(pucch_slot_alloc, bits_for_uci, *existing_grant_it, ue_cell_cfg);
  if (not alloc_outcome.has_value()) {
    garbage_collector.release_resource(sl_tx, crnti, ue_cell_cfg);
  }
}

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                                              rnti_t                        crnti,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  pucch_uci_bits removed_uci_info;

  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove HARQ-ACK grant first.
  auto* it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and
           ((pucch.format == pucch_format::FORMAT_1 and pucch.format_1.sr_bits == sr_nof_bits::no_sr and
             pucch.format_1.harq_ack_nof_bits > 0) or
            (pucch.format == pucch_format::FORMAT_2));
  });

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  if (it != pucchs.end()) {
    // Search for Format 2 first; if present, then remove only that resource and exit.
    if (it->format == pucch_format::FORMAT_2) {
      removed_uci_info.harq_ack_nof_bits = it->format_2.harq_ack_nof_bits;
      removed_uci_info.sr_bits           = it->format_2.sr_bits;
      removed_uci_info.csi_part1_bits    = it->format_2.csi_part1_bits;
      pucchs.erase(it);
      resource_manager.release_harq_f2_resource(slot_alloc.slot, crnti, pucch_cfg);
      if (removed_uci_info.csi_part1_bits > 0) {
        resource_manager.release_csi_resource(slot_alloc.slot, crnti, ue_cell_cfg);
      }
      // If there is a PUCCH resource Format 2, then no Format 1 should be present.
      return removed_uci_info;
    }
    // Proceed with Format 1.
    // Only remove HARQ-ACK grant, handle SR grant separately.
    removed_uci_info.harq_ack_nof_bits = it->format_1.harq_ack_nof_bits;
    pucchs.erase(it);
    resource_manager.release_harq_f1_resource(slot_alloc.slot, crnti, pucch_cfg);
  }

  // Remove SR grant, if any.
  it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format == pucch_format::FORMAT_1 and
           pucch.format_1.sr_bits == sr_nof_bits::one;
  });

  if (it != pucchs.end()) {
    removed_uci_info.sr_bits = it->format_1.sr_bits;
    pucchs.erase(it);
    resource_manager.release_sr_resource(slot_alloc.slot, crnti, pucch_cfg);
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

  // Clear previous slot PUCCH common allocations.
  pucch_common_alloc_grid[(sl_tx - 1).to_uint()].clear();
}

//////////////    Private functions       //////////////

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_res_alloc_cfg>
pucch_allocator_impl::alloc_pucch_common_res_harq(cell_slot_resource_allocator&  pucch_alloc,
                                                  const dci_context_information& dci_info)
{
  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  const unsigned max_d_pri = 7;
  // As per Section 9.2.1, TS 38.213, r_pucch can take values within {0,...,15}.
  const unsigned r_pucch_invalid = 16;

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

std::optional<unsigned> pucch_allocator_impl::exec_common_and_ded_res_alloc(cell_slot_resource_allocator& pucch_alloc,
                                                                            pucch_info*                  existing_grant,
                                                                            rnti_t                       rnti,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            pucch_res_alloc_cfg          common_res_cfg,
                                                                            const pucch_resource&        ded_res_cfg)
{
  const unsigned pucch_res_indicator          = common_res_cfg.pucch_res_indicator;
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;

  // Allocate the dedicated PUCCH HARQ-ACK resource first; this is because the allocation can may and, in that case, we
  // can quit the allocation without the need to remove the PUCCH common grant.
  if (existing_grant == nullptr) {
    pucch_info& pucch_pdu = pucch_alloc.result.ul.pucchs.emplace_back();
    fill_pucch_ded_format1_grant(pucch_pdu, rnti, ded_res_cfg, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  } else if (existing_grant->format == pucch_format::FORMAT_1) {
    // Update the HARQ-ACK bits in the PUCCH resource for SR.
    ++existing_grant->format_1.harq_ack_nof_bits;

    // Allocate the new grant on ded. PUCCH F1 resource for HARQ-ACK.
    pucch_info& pucch_pdu = pucch_alloc.result.ul.pucchs.emplace_back();
    fill_pucch_ded_format1_grant(pucch_pdu, rnti, ded_res_cfg, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  } else {
    // Change the existing PUCCH F2 grant for CSI into one for HARQ-ACK bits, if available.
    std::optional<unsigned> result = change_format2_resource(
        pucch_alloc,
        *existing_grant,
        rnti,
        ue_cell_cfg,
        HARQ_BITS_IN_NEW_PUCCH_GRANT,
        pucch_harq_resource_alloc_record{.pucch_res = &ded_res_cfg, .pucch_res_indicator = pucch_res_indicator});
    if (not result.has_value()) {
      return std::nullopt;
    }
  }

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_info, rnti, common_res_cfg);

  // Allocate common HARQ-ACK resource.
  pucch_alloc.ul_res_grid.fill(common_res_cfg.first_hop_res);
  pucch_alloc.ul_res_grid.fill(common_res_cfg.second_hop_res);

  pucch_common_alloc_grid[pucch_alloc.slot.to_uint()].emplace_back(rnti);

  logger.debug("rnti={}: PUCCH on common {}resource with res_ind={} allocated for slot={}",
               rnti,
               existing_grant != nullptr and existing_grant->format == pucch_format::FORMAT_2 ? "" : "and ded. ",
               pucch_res_indicator,
               pucch_alloc.slot);

  return pucch_res_indicator;
}

void pucch_allocator_impl::fill_pucch_harq_common_grant(pucch_info&                pucch_info,
                                                        rnti_t                     rnti,
                                                        const pucch_res_alloc_cfg& pucch_res)
{
  pucch_info.crnti                     = rnti;
  pucch_info.format                    = pucch_res.format;
  pucch_info.bwp_cfg                   = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_info.resources.prbs            = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.first_hop_res.crbs);
  pucch_info.resources.second_hop_prbs = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.second_hop_res.crbs);
  pucch_info.resources.symbols =
      ofdm_symbol_range{pucch_res.first_hop_res.symbols.start(), pucch_res.second_hop_res.symbols.stop()};

  switch (pucch_res.format) {
    case pucch_format::FORMAT_0: {
      pucch_info.format_0.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_info.format_0.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                              : static_cast<unsigned>(cell_cfg.pci);
      // \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
      pucch_info.format_0.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.format_0.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_info.format_0.harq_ack_nof_bits = 1;
      break;
    }
    case pucch_format::FORMAT_1: {
      pucch_info.format_1.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_info.format_1.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                              : static_cast<unsigned>(cell_cfg.pci);
      pucch_info.format_1.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.format_1.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_info.format_1.harq_ack_nof_bits = 1;
      // This option can be configured with Dedicated PUCCH resources.
      pucch_info.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
      // As per TS 38.213, Section 9.2.1, OCC with index 0 is used for PUCCH resources in Table 9.2.1-1.
      pucch_info.format_1.time_domain_occ = 0;
      break;
    }
    default:
      srsran_assert(false, "PUCCH Format must from 0 to 4, but only 0 and 1 are currently supported");
  }
}

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional
// if no resource is available.
std::optional<pucch_allocator_impl::pucch_com_ded_res>
pucch_allocator_impl::find_common_and_ded_harq_res_available(cell_slot_resource_allocator&  pucch_alloc,
                                                             pucch_info*                    existing_grant,
                                                             rnti_t                         rnti,
                                                             const ue_cell_configuration&   ue_cell_cfg,
                                                             const dci_context_information& dci_info)
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource pucch_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  const unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  const unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per Section 9.2.1, TS 38.213, this is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  const unsigned max_d_pri = 7;
  for (unsigned d_pri = 0; d_pri != max_d_pri + 1; ++d_pri) {
    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    const unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);
    srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");

    // Look for an available PUCCH common resource.
    if (not resource_manager.is_common_resource_available(pucch_alloc.slot, r_pucch)) {
      continue;
    }

    // Look for an available PUCCH dedicated resource with the same PUCCH resource indicator as the common's.
    const pucch_config&   pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
    const pucch_resource* ded_resource =
        existing_grant == nullptr or existing_grant->format == pucch_format::FORMAT_1
            ? resource_manager.reserve_f1_res_by_res_indicator(pucch_alloc.slot, rnti, d_pri, pucch_cfg)
            : resource_manager.reserve_f2_res_by_res_indicator(pucch_alloc.slot, rnti, d_pri, pucch_cfg);
    if (ded_resource == nullptr) {
      continue;
    }

    // Compute the PUCCH resource common configuration parameters.

    // As per TS 38.211, Section 6.3.2.1, the first floor(N_symb_PUCCH/2) are for the first hop, the remaining ones for
    // the second hop.
    const ofdm_symbol_range first_hop_symbols{pucch_res.first_symbol_index,
                                              pucch_res.first_symbol_index + pucch_res.nof_symbols / 2};
    const ofdm_symbol_range second_hop_symbols{pucch_res.first_symbol_index + pucch_res.nof_symbols / 2,
                                               pucch_res.first_symbol_index + pucch_res.nof_symbols};

    const bwp_configuration& init_ul_bwp_param = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

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

    return pucch_com_ded_res{pucch_res_alloc_cfg{.pucch_res_indicator = d_pri,
                                                 .first_hop_res       = first_hop_grant,
                                                 .second_hop_res      = second_hop_grant,
                                                 .cs                  = cyclic_shift,
                                                 .format              = pucch_res.format},
                             *ded_resource};
  };
  return std::nullopt;
}

std::optional<unsigned>
pucch_allocator_impl::allocate_new_format1_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                      rnti_t                        crnti,
                                                      const ue_cell_configuration&  ue_cell_cfg,
                                                      pucch_info*                   existing_sr_grant)
{
  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info("rnti={}: HARQ-ACK allocation on PUCCH Format1 for slot={} skipped. Cause: UL grants reached",
                crnti,
                pucch_slot_alloc.slot);
    return std::nullopt;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_f1_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                 crnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  // Update the number of HARQ-ACK bits in the SR grant, if present.
  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }

  // Allocate the new grant on PUCCH F1 resources for HARQ-ACK bits (without SR).
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_format1_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  const auto pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  return pucch_res_indicator;
}

std::optional<unsigned> pucch_allocator_impl::allocate_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                  rnti_t                        crnti,
                                                                  const ue_cell_configuration&  ue_cell_cfg)
{
  slot_point sl_tx = pucch_slot_alloc.slot;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info("rnti={}: HARQ-ACK allocation on PUCCH Format1 for slot={} skipped. Cause: UL grants reached",
                crnti,
                pucch_slot_alloc.slot);
    return std::nullopt;
  }

  if (pucch_grants_alloc_grid[sl_tx.to_uint()].full()) {
    logger.info("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: scheduler cache full",
                crnti,
                pucch_slot_alloc.slot);
    return std::nullopt;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_f1_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                 crnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  // Allocate the new grant on PUCCH F1 resources for HARQ-ACK bits (without SR).
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_format1_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  const auto pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  // Save the info in the scheduler list of PUCCH grants.
  auto& harq_pucch_grant = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_pucch_bits{.rnti = crnti});
  harq_pucch_grant.pucch_grants.harq_resource.emplace(pucch_grant{.type          = pucch_grant_type::harq_ack,
                                                                  .format        = pucch_format::FORMAT_1,
                                                                  .pucch_res_cfg = pucch_harq_res_info.pucch_res});
  harq_pucch_grant.pucch_grants.harq_resource.value().harq_id.pucch_set_idx = pucch_res_set_idx::set_0;
  harq_pucch_grant.pucch_grants.harq_resource.value().harq_id.pucch_res_ind = pucch_res_indicator;
  harq_pucch_grant.pucch_grants.harq_resource.value().bits.harq_ack_bits    = HARQ_BITS_IN_NEW_PUCCH_GRANT;

  return pucch_res_indicator;
}

std::optional<unsigned> pucch_allocator_impl::convert_to_format2_harq(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                      pucch_info&                   existing_harq_grant,
                                                                      pucch_info*                   existing_sr_grant,
                                                                      rnti_t                        rnti,
                                                                      const ue_cell_configuration&  ue_cell_cfg,
                                                                      unsigned int harq_ack_bits_increment)
{
  const unsigned    curr_harq_bits = existing_harq_grant.format_1.harq_ack_nof_bits;
  const sr_nof_bits sr_bits = existing_sr_grant != nullptr ? existing_sr_grant->format_1.sr_bits : sr_nof_bits::no_sr;

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Get a PUCCH Format 2 resource.
  const pucch_harq_resource_alloc_record format2_res =
      resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);

  if (format2_res.pucch_res == nullptr) {
    logger.debug("rnti={}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource "
                 "not available",
                 rnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  const unsigned candidate_uci_bits = curr_harq_bits + harq_ack_bits_increment + sr_nof_bits_to_uint(sr_bits);

  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                    std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  if (max_payload < candidate_uci_bits) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is "
                 "insufficient for "
                 "{} candidate UCI bits",
                 rnti,
                 pucch_slot_alloc.slot,
                 max_payload,
                 candidate_uci_bits);
    resource_manager.release_harq_f2_resource(pucch_slot_alloc.slot, rnti, pucch_cfg);
    return std::nullopt;
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-1 resources.
  remove_pucch_format1_from_grants(
      pucch_slot_alloc, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info("rnti={}: HARQ-ACK allocation on PUCCH Format2 for slot={} skipped. Cause: UL grants reached",
                rnti,
                pucch_slot_alloc.slot);
    return std::nullopt;
  }

  pucch_info&    pucch_pdu               = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned csi1_nof_bits_only_harq = 0U;
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           curr_harq_bits + harq_ack_bits_increment,
                           sr_bits,
                           csi1_nof_bits_only_harq);

  return format2_res.pucch_res_indicator;
}

std::optional<unsigned>
pucch_allocator_impl::change_format2_resource(cell_slot_resource_allocator&                   pucch_slot_alloc,
                                              pucch_info&                                     existing_grant,
                                              rnti_t                                          rnti,
                                              const ue_cell_configuration&                    ue_cell_cfg,
                                              unsigned                                        harq_ack_bits_increment,
                                              std::optional<pucch_harq_resource_alloc_record> harq_f2_res)
{
  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const pucch_harq_resource_alloc_record format2_res =
      harq_f2_res.has_value()
          ? harq_f2_res.value()
          : resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);

  if (format2_res.pucch_res == nullptr) {
    logger.debug("rnti={}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource "
                 "not available",
                 rnti,
                 pucch_slot_alloc.slot);
    return std::nullopt;
  }

  // This function would only be called in case CSI and SR gets allocated before the HARQ. In that case, if there are
  // SR bits or CSI bits to be carried by the PUCCH F2 grant, they would have already been allocated and there is no
  // need to check if the slot is a CSI or SR opportunity.
  const sr_nof_bits sr_bits_to_report  = existing_grant.format_2.sr_bits;
  const unsigned    csi_bits_to_report = existing_grant.format_2.csi_part1_bits;

  const unsigned candidate_uci_bits =
      harq_ack_bits_increment + sr_nof_bits_to_uint(sr_bits_to_report) + csi_bits_to_report;

  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                    std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  if (max_payload < candidate_uci_bits) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is "
                 "insufficient for {} candidate UCI bits",
                 rnti,
                 pucch_slot_alloc.slot,
                 max_payload,
                 candidate_uci_bits);
    // The allocation will be aborted, we need to deallocate the resource that was reserved at the beginning of the
    // function.
    resource_manager.release_harq_f2_resource(pucch_slot_alloc.slot, rnti, pucch_cfg);
    return std::nullopt;
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 std::get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-2 resource.
  // TODO: instead of removing the grant, change the data according to the new resource used.
  remove_format2_csi_from_grants(pucch_slot_alloc, rnti, ue_cell_cfg);

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info("rnti={}: HARQ-ACK allocation on PUCCH Format2 for slot={} skipped. Cause: UL grants reached",
                rnti,
                pucch_slot_alloc.slot);
    return std::nullopt;
  }

  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           harq_ack_bits_increment,
                           sr_bits_to_report,
                           csi_bits_to_report);

  return static_cast<unsigned>(format2_res.pucch_res_indicator);
}

std::optional<unsigned> pucch_allocator_impl::add_harq_ack_bit_to_format1_grant(pucch_info&         existing_harq_grant,
                                                                                pucch_info*         existing_sr_grant,
                                                                                rnti_t              rnti,
                                                                                slot_point          sl_tx,
                                                                                const pucch_config& pucch_cfg)
{
  const int pucch_res_idx = resource_manager.fetch_f1_pucch_res_indic(sl_tx, rnti, pucch_cfg);
  if (pucch_res_idx < 0) {
    srsran_assert(pucch_res_idx >= 0, "PUCCH resource index should not be negative");
    return std::nullopt;
  }
  srsran_sanity_check(existing_harq_grant.format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for HARQ");
  // Update the SR, if present.
  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }
  // Update the HARQ, if present.
  existing_harq_grant.format_1.harq_ack_nof_bits++;
  const auto pucch_res_indicator = static_cast<unsigned>(pucch_res_idx);

  return pucch_res_indicator;
}

void pucch_allocator_impl::remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                                            rnti_t                        crnti,
                                                            const pucch_config&           pucch_cfg)
{
  auto&      pucchs = slot_alloc.result.ul.pucchs;
  slot_point sl_tx  = slot_alloc.slot;

  // Remove dedicated HARQ-ACK grant first.
  auto* it_harq = std::find_if(pucchs.begin(), pucchs.end(), [crnti, sl_tx, this](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format == pucch_format::FORMAT_1 and
           pucch.format_1.sr_bits == sr_nof_bits::no_sr and pucch.format_1.harq_ack_nof_bits > 0 and
           not has_common_pucch_f1_grant(crnti, sl_tx);
  });

  if (it_harq != pucchs.end()) {
    pucchs.erase(it_harq);
    resource_manager.release_harq_f1_resource(slot_alloc.slot, crnti, pucch_cfg);
  }

  // Remove SR grant, if any.
  auto* it_sr = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format == pucch_format::FORMAT_1 and
           pucch.format_1.sr_bits == sr_nof_bits::one;
  });

  if (it_sr != pucchs.end()) {
    pucchs.erase(it_sr);
    resource_manager.release_sr_resource(slot_alloc.slot, crnti, pucch_cfg);
  }
}

void pucch_allocator_impl::remove_format2_csi_from_grants(cell_slot_resource_allocator& slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg)
{
  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove PUCCH Format2 resource specific for CSI.
  auto* it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format == pucch_format::FORMAT_2 and pucch.format_2.harq_ack_nof_bits == 0 and
           pucch.format_2.csi_part1_bits > 0;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    resource_manager.release_csi_resource(slot_alloc.slot, crnti, ue_cell_cfg);
  }
}

void pucch_allocator_impl::allocate_csi_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                              rnti_t                        crnti,
                                              const ue_cell_configuration&  ue_cell_cfg,
                                              unsigned                      csi_part1_bits)
{
  srsran_assert(csi_part1_bits != 0, "This function can only be called to allocate a PUCCH F2 resource for CSI");
  const slot_point sl_tx = pucch_slot_alloc.slot;

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: CSI allocation on PUCCH Format2 for slot={} skipped. Cause: UL grants reached",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  if (pucch_grants_alloc_grid[sl_tx.to_uint()].full()) {
    logger.info("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: scheduler cache full",
                crnti,
                pucch_slot_alloc.slot);
    return;
  }

  // Get the F2 resource specific for with CSI.
  const pucch_resource* csi_f2_res = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, crnti, ue_cell_cfg);

  if (csi_f2_res == nullptr) {
    logger.warning(
        "rnti={}: CSI could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource not available",
        crnti,
        pucch_slot_alloc.slot);
    return;
  }
  // When this function is called, it means that there are no SR grants to be multiplexed with CSI; thus, the CSI bits
  // are the only UCI bits to be considered.
  // It's the validator that should make sure the CSI bits fit into a PUCCH Format 2 resource.
  const unsigned max_payload =
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().get_max_payload(csi_f2_res->format);
  srsran_assert(csi_part1_bits <= max_payload,
                "rnti={}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                crnti,
                max_payload,
                csi_part1_bits);

  // Allocate a PUCCH PDU in the list and fill it with the parameters.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  // Neither HARQ-ACK bits
  const unsigned    harq_ack_bits_only_csi = 0U;
  const sr_nof_bits sr_bits_only_csi       = sr_nof_bits::no_sr;
  fill_pucch_format2_grant(pucch_pdu,
                           crnti,
                           *csi_f2_res,
                           ue_cell_cfg,
                           std::get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_prbs,
                           harq_ack_bits_only_csi,
                           sr_bits_only_csi,
                           csi_part1_bits);

  // Save the info in the scheduler list of PUCCH grants.
  auto& csi_pucch_grant = pucch_grants_alloc_grid[sl_tx.to_uint()].emplace_back(ue_pucch_bits{.rnti = crnti});
  csi_pucch_grant.pucch_grants.csi_resource.emplace(
      pucch_grant{.type = pucch_grant_type::sr, .format = csi_f2_res->format, .pucch_res_cfg = csi_f2_res});
  csi_pucch_grant.pucch_grants.csi_resource.value().bits.sr_bits = sr_nof_bits::one;
}

std::optional<unsigned> pucch_allocator_impl::add_harq_bits_to_harq_f2_grant(pucch_info& existing_f2_grant,
                                                                             slot_point  sl_tx,
                                                                             rnti_t      crnti,
                                                                             const ue_cell_configuration& ue_cell_cfg,
                                                                             unsigned harq_ack_bits_increment)
{
  const unsigned    current_csi_part1_bits = existing_f2_grant.format_2.csi_part1_bits;
  const unsigned    current_harq_ack_bits  = existing_f2_grant.format_2.harq_ack_nof_bits;
  const sr_nof_bits current_sr_bits        = existing_f2_grant.format_2.sr_bits;

  // This function cannot be called if the resource of for CSI and needs to be converted into HARQ-ACK.
  srsran_sanity_check(current_harq_ack_bits != 0 and harq_ack_bits_increment != 0,
                      "This PUCCH grant is expected to have HARQ-ACK bits to report");

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const pucch_harq_resource_alloc_record pucch_f2_harq_cfg =
      resource_manager.fetch_allocated_f2_harq_resource(sl_tx, existing_f2_grant.crnti, pucch_cfg);

  if (pucch_f2_harq_cfg.pucch_res == nullptr) {
    srsran_assertion_failure(
        "PUCCH F2 resource previously allocated for HARQ-ACK not found in the PUCCH resource manager");
    return std::nullopt;
  }

  const unsigned candidate_uci_bits =
      current_harq_ack_bits + harq_ack_bits_increment + sr_nof_bits_to_uint(current_sr_bits) + current_csi_part1_bits;

  // NOTE: The process of (i) checking the constraint max_payload >= candidate_uci_bits and (ii) the number of required
  // PRBs and (iii) updating the PUCCH grant PRBs values needs to be repeated every time we add HARQ bits; this is
  // because the required number of PRBs can increase as a result of adding HARQ-ACK bits.

  // Check if the number of PRBs is sufficient for the number of bits to be acked.
  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);

  const auto&    f2_params    = std::get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params);
  const unsigned max_nof_prbs = f2_params.nof_prbs;
  const unsigned nof_symbols  = f2_params.nof_symbols;
  const unsigned max_payload  = get_pucch_format2_max_payload(max_nof_prbs, nof_symbols, max_pucch_code_rate);

  if (max_payload < candidate_uci_bits) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is "
                 "insufficient for {} candidate UCI bits",
                 crnti,
                 sl_tx,
                 max_payload,
                 candidate_uci_bits);

    // No need to release the resource, as it was previously allocated for other HARQ processes.
    return std::nullopt;
  }

  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits, max_nof_prbs, nof_symbols, max_pucch_code_rate);
  // NOTE: there is no need to check if the code rate is within the limit, as the UCI bits are computed so that not to
  // exceed the code rate.
  existing_f2_grant.resources.prbs.set(pucch_f2_harq_cfg.pucch_res->starting_prb,
                                       pucch_f2_harq_cfg.pucch_res->starting_prb + nof_prbs);
  if (pucch_f2_harq_cfg.pucch_res->second_hop_prb.has_value()) {
    existing_f2_grant.resources.second_hop_prbs.set(pucch_f2_harq_cfg.pucch_res->second_hop_prb.value(),
                                                    pucch_f2_harq_cfg.pucch_res->second_hop_prb.value() + nof_prbs);
  }

  existing_f2_grant.format_2.harq_ack_nof_bits += harq_ack_bits_increment;

  return pucch_f2_harq_cfg.pucch_res_indicator;
}

void pucch_allocator_impl::fill_pucch_ded_format1_grant(pucch_info&           pucch_pdu,
                                                        rnti_t                crnti,
                                                        const pucch_resource& pucch_ded_res_cfg,
                                                        unsigned              harq_ack_bits,
                                                        sr_nof_bits           sr_bits)
{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_pdu.format  = pucch_format::FORMAT_1;

  // Set PRBs and symbols, first.
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  const auto& res_f1 = std::get<pucch_format_1_cfg>(pucch_ded_res_cfg.format_params);
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb,
                               pucch_ded_res_cfg.starting_prb + PUCCH_FORMAT_1_NOF_PRBS);
  pucch_pdu.resources.symbols.set(res_f1.starting_sym_idx, res_f1.starting_sym_idx + res_f1.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() + PUCCH_FORMAT_1_NOF_PRBS);
  }
  // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
  pucch_pdu.format_1.group_hopping        = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  pucch_pdu.format_1.n_id_hopping         = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                                ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                                : cell_cfg.pci;
  pucch_pdu.format_1.initial_cyclic_shift = res_f1.initial_cyclic_shift;
  pucch_pdu.format_1.time_domain_occ      = res_f1.time_domain_occ;
  // For PUCCH Format 1, only 1 SR bit.
  pucch_pdu.format_1.sr_bits           = sr_bits;
  pucch_pdu.format_1.harq_ack_nof_bits = harq_ack_bits;
  // [Implementation-defined] We do not implement PUCCH over several slots.
  pucch_pdu.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
}

void pucch_allocator_impl::fill_pucch_format2_grant(pucch_info&                  pucch_pdu,
                                                    rnti_t                       crnti,
                                                    const pucch_resource&        pucch_ded_res_cfg,
                                                    const ue_cell_configuration& ue_cell_cfg,
                                                    unsigned                     nof_prbs,
                                                    unsigned                     harq_ack_bits,
                                                    sr_nof_bits                  sr_bits,
                                                    unsigned                     csi_part1_bits)
{
  pucch_pdu.crnti   = crnti;
  pucch_pdu.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_pdu.format  = pucch_format::FORMAT_2;

  // Set PRBs and symbols, first.
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  pucch_pdu.resources.prbs.set(pucch_ded_res_cfg.starting_prb, pucch_ded_res_cfg.starting_prb + nof_prbs);
  const auto& res_f2 = std::get<pucch_format_2_3_cfg>(pucch_ded_res_cfg.format_params);
  pucch_pdu.resources.symbols.set(res_f2.starting_sym_idx, res_f2.starting_sym_idx + res_f2.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_pdu.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                            pucch_ded_res_cfg.second_hop_prb.value() + nof_prbs);
  }

  pucch_pdu.format_2.sr_bits           = sr_bits;
  pucch_pdu.format_2.harq_ack_nof_bits = harq_ack_bits;
  pucch_pdu.format_2.csi_part1_bits    = csi_part1_bits;
  // \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  pucch_pdu.format_2.n_id_scambling =
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.has_value()
          ? ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.value()
          : cell_cfg.pci;
  // \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  pucch_pdu.format_2.n_id_0_scrambling = get_n_id0_scrambling(ue_cell_cfg, cell_cfg.pci);
  pucch_pdu.format_2.max_code_rate     = ue_cell_cfg.cfg_dedicated()
                                         .ul_config.value()
                                         .init_ul_bwp.pucch_cfg.value()
                                         .format_2_common_param.value()
                                         .max_c_rate;

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (csi_part1_bits > 0) {
    pucch_pdu.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
  }
}

pucch_allocator_impl::existing_pucch_grants
pucch_allocator_impl::get_existing_pucch_grants(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs,
                                                rnti_t                                              rnti,
                                                slot_point                                          sl_ack)
{
  existing_pucch_grants grants;
  for (auto& pucch : pucchs) {
    if (pucch.crnti == rnti) {
      // First look for first for Format 2; if present, this is the only PUCCH resource allocated to the UE.
      if (pucch.format == srsran::pucch_format::FORMAT_2) {
        grants.format2_grant = &pucch;
      } else if (pucch.format == srsran::pucch_format::FORMAT_1) {
        if (pucch.format_1.sr_bits != sr_nof_bits::no_sr) {
          grants.format1_sr_grant = &pucch;
        }
        // In the following, we need to check whether the PUCCH grant found in the scheduler output is a common or
        // dedicated resource.
        else if (pucch.format_1.harq_ack_nof_bits > 0) {
          auto* pucch_common_it = std::find(
              pucch_common_alloc_grid[sl_ack.to_uint()].begin(), pucch_common_alloc_grid[sl_ack.to_uint()].end(), rnti);
          if (pucch_common_it != pucch_common_alloc_grid[sl_ack.to_uint()].end()) {
            grants.format1_harq_common_grant = &pucch;
          } else {
            grants.format1_harq_grant = &pucch;
          }
        } else {
          logger.error("rnti={}: unexpected PUCCH grant found in slot={}", rnti, sl_ack);
        }
      }
    }
  }

  return grants;
}

bool pucch_allocator_impl::has_common_pucch_f1_grant(rnti_t rnti, slot_point sl_tx) const
{
  return std::find(pucch_common_alloc_grid[sl_tx.to_uint()].begin(),
                   pucch_common_alloc_grid[sl_tx.to_uint()].end(),
                   rnti) != pucch_common_alloc_grid[sl_tx.to_uint()].end();
}

unsigned pucch_allocator_impl::get_max_pucch_grants(unsigned currently_allocated_puschs)
{
  return std::min(max_pucch_grants_per_slot, max_ul_grants_per_slot - currently_allocated_puschs);
}

void pucch_allocator_impl::remove_unsed_pucch_res(slot_point                   sl_tx,
                                                  pucch_grant_list             grants_to_tx,
                                                  ue_pucch_bits&               existing_pucchs,
                                                  const ue_cell_configuration& ue_cell_cfg)
{
  // Remove the PUCCH resources by evaluating the difference between the previously allocated resources and the current
  // ones.
  if (existing_pucchs.pucch_grants.csi_resource.has_value() and not grants_to_tx.csi_resource.has_value()) {
    resource_manager.release_csi_resource(sl_tx, existing_pucchs.rnti, ue_cell_cfg);
  }
  if (existing_pucchs.pucch_grants.sr_resource.has_value() and not grants_to_tx.sr_resource.has_value()) {
    resource_manager.release_sr_resource(
        sl_tx, existing_pucchs.rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  }

  if (existing_pucchs.pucch_grants.harq_resource.has_value() and
      (not grants_to_tx.harq_resource.has_value() or
       (grants_to_tx.sr_resource.has_value() and
        existing_pucchs.pucch_grants.harq_resource->format != grants_to_tx.sr_resource->format))) {
    if (existing_pucchs.pucch_grants.harq_resource.value().harq_id.pucch_set_idx == pucch_res_set_idx::set_0) {
      resource_manager.release_harq_f1_resource(
          sl_tx, existing_pucchs.rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
    } else {
      resource_manager.release_harq_f2_resource(
          sl_tx, existing_pucchs.rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
    }
  }
}

std::optional<pucch_allocator_impl::pucch_grant_list>
pucch_allocator_impl::get_pucch_res_pre_multiplexing(slot_point                   sl_tx,
                                                     uci_bits                     new_bits,
                                                     ue_pucch_bits                ue_current_grants,
                                                     const ue_cell_configuration& ue_cell_cfg)
{
  pucch_grant_list candidate_resources;

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Save HARQ resource

  if (new_bits.harq_ack_bits > 0) {
    // Case HARQ ACK bits 1 or 2, resource to be chosen from PUCCH resource set 0; else, pick from PUCCH resource set 1.
    const pucch_res_set_idx pucch_set_idx =
        new_bits.harq_ack_bits <= 2U ? pucch_res_set_idx::set_0 : pucch_res_set_idx::set_1;

    candidate_resources.harq_resource.emplace(pucch_grant{.type = pucch_grant_type::harq_ack});
    pucch_grant& harq_candidate_grant = candidate_resources.harq_resource.value();

    // There is already a PUCCH resource for HARQ-ACK; if so, we use the info and configuration from this resource.
    if (ue_current_grants.pucch_grants.harq_resource.has_value() and
        ue_current_grants.pucch_grants.harq_resource.value().harq_id.pucch_set_idx == pucch_set_idx) {
      harq_candidate_grant = ue_current_grants.pucch_grants.harq_resource.value();
    }
    // Get a new PUCCH resource for HARQ-ACK from the correct PUCCH resource set.
    else {
      // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits before multiplexing.
      pucch_harq_resource_alloc_record harq_resource =
          pucch_set_idx == pucch_res_set_idx::set_0
              ? resource_manager.reserve_next_f1_harq_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg)
              : resource_manager.reserve_next_f2_harq_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg);
      // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
      if (pucch_set_idx == pucch_res_set_idx::set_0) {
        garbage_collector.harq_set_0 = true;
      } else {
        garbage_collector.harq_set_1 = true;
      }
      if (harq_resource.pucch_res == nullptr) {
        return std::nullopt;
      }
      harq_candidate_grant.harq_id.pucch_set_idx = pucch_res_set_idx::set_0,
      harq_candidate_grant.harq_id.pucch_res_ind = static_cast<uint8_t>(harq_resource.pucch_res_indicator);
      harq_candidate_grant.pucch_res_cfg         = harq_resource.pucch_res;
    }
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources still
    // need to be multiplexed.
    harq_candidate_grant.bits.harq_ack_bits  = new_bits.harq_ack_bits;
    harq_candidate_grant.bits.sr_bits        = sr_nof_bits::no_sr;
    harq_candidate_grant.bits.csi_part1_bits = 0U;
  }

  if (new_bits.sr_bits != sr_nof_bits::no_sr) {
    candidate_resources.sr_resource.emplace(pucch_grant{.type = pucch_grant_type::sr});
    pucch_grant& sr_candidate_grant = candidate_resources.sr_resource.value();

    // There is already a PUCCH resource for SR; if so, we use the info and configuration from this resource.
    if (ue_current_grants.pucch_grants.sr_resource.has_value()) {
      sr_candidate_grant = ue_current_grants.pucch_grants.sr_resource.value();
    }
    // Get the new resource from the resource manager; the UCI bits will be added later.
    else {
      // TODO: handle case in which the resource is already used by the same UE.
      const pucch_resource* sr_resource =
          resource_manager.reserve_sr_res_available(sl_tx, ue_current_grants.rnti, pucch_cfg);
      // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
      garbage_collector.sr = true;
      if (sr_resource == nullptr) {
        return std::nullopt;
      }
      sr_candidate_grant.pucch_res_cfg = sr_resource;
    }
    // Only copy the SR bits, as at this stage we only need to consider the UCI bits assuming the resources still
    // need to be multiplexed.
    sr_candidate_grant.bits.harq_ack_bits  = 0U;
    sr_candidate_grant.bits.sr_bits        = new_bits.sr_bits;
    sr_candidate_grant.bits.csi_part1_bits = 0U;
  }

  if (new_bits.csi_part1_bits != 0U) {
    candidate_resources.csi_resource.emplace(pucch_grant{.type = pucch_grant_type::csi});
    pucch_grant& csi_candidate_grant = candidate_resources.csi_resource.value();

    // There is already a PUCCH resource for SR; if so, we use the info and configuration from this resource.
    if (ue_current_grants.pucch_grants.csi_resource.has_value()) {
      csi_candidate_grant = ue_current_grants.pucch_grants.csi_resource.value();
    }
    // Get the new resource from the resource manager; the UCI bits will be added later.
    else {
      // TODO: handle case in which the resource is already used by the same UE.
      const pucch_resource* csi_resource =
          resource_manager.reserve_csi_resource(sl_tx, ue_current_grants.rnti, ue_cell_cfg);
      // Save the resources that have been generated; if at some point the allocation fails, we need to release them.
      garbage_collector.csi = true;
      if (csi_resource == nullptr) {
        return std::nullopt;
      }
      csi_candidate_grant.pucch_res_cfg = csi_resource;
    }
    // Only copy the HARQ-ACK bits, as at this stage we only need to consider the UCI bits assuming the resources still
    // need to be multiplexed.
    csi_candidate_grant.bits.harq_ack_bits  = 0U;
    csi_candidate_grant.bits.sr_bits        = new_bits.sr_bits;
    csi_candidate_grant.bits.csi_part1_bits = 0U;
  }

  // TODO: handle the failure case, in which the resources that had been reserved are not used and need to be released.

  return candidate_resources;
}

std::optional<pucch_allocator_impl::pucch_grant>
pucch_allocator_impl::merge_pucch_resources(span<const pucch_allocator_impl::pucch_grant> resources_to_merge,
                                            slot_point                                    slot_harq,
                                            rnti_t                                        crnti,
                                            const pucch_config&                           pucch_cfg)
{
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
      srsran_assert(r_0.format == r_1.format, "The two resources must have the same format");
      if (r_0.format == pucch_format::FORMAT_0 or r_0.format == pucch_format::FORMAT_1) {
        // TODO: Multiplexing of SR + HARQ on F0 needs to be checked, as the TS is not clear about it.
        pucch_grant        new_resource;
        const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
        const pucch_grant& r_sr   = r_0.type == pucch_grant_type::sr ? r_0 : r_1;
        new_resource              = r_harq;
        // Copy the SR bits in the HARQ resource.
        new_resource.bits.sr_bits = r_sr.bits.sr_bits;

        return new_resource;
      } else {
        return std::nullopt;
      }
    }

    // SR and CSI only.
    if ((r_0.type == pucch_grant_type::sr and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::sr)) {
      //  We don't support SR with Format 0 on the same slot as CSI.
      srsran_assert(r_0.format == pucch_format::FORMAT_0 or r_1.format == pucch_format::FORMAT_0,
                    "SR with Format 0 is not supported on the same slot as CSI");
      // Apply F2 CSI merging rule: SR and CSI PUCCH resources will be multiplexed in the CSI PUCCH resource.
      pucch_grant        new_resource{.type = pucch_grant_type::csi};
      const pucch_grant& r_csi = r_0.type == pucch_grant_type::csi ? r_0 : r_1;
      const pucch_grant& r_sr  = r_0.type == pucch_grant_type::sr ? r_0 : r_1;
      // Copy the SR bits in the CSI resource.
      new_resource              = r_csi;
      new_resource.bits.sr_bits = r_sr.bits.sr_bits;

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() <= pucch_cfg.get_max_payload(new_resource.format)) {
        return new_resource;
      } else {
        return std::nullopt;
      }
    }

    // HARQ and CSI only.
    if ((r_0.type == pucch_grant_type::harq_ack and r_1.type == pucch_grant_type::csi) or
        (r_0.type == pucch_grant_type::csi and r_1.type == pucch_grant_type::harq_ack)) {
      // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res set 1.
      pucch_grant        new_resource;
      const pucch_grant& r_harq = r_0.type == pucch_grant_type::harq_ack ? r_0 : r_1;
      const pucch_grant& r_csi  = r_0.type == pucch_grant_type::csi ? r_0 : r_1;

      // A HARQ resource from PUCCH resource set idx 1 already exits. Use that one.
      if (r_harq.format != pucch_format::FORMAT_0 and r_harq.format != pucch_format::FORMAT_1) {
        new_resource = r_harq;

        return new_resource;
      }
      // Get a new HARQ resource (from PUCCH resource set idx 1) from the resource manager.
      else {
        pucch_harq_resource_alloc_record res_alloc =
            resource_manager.reserve_next_f2_harq_res_available(slot_harq, crnti, pucch_cfg);
        if (res_alloc.pucch_res != nullptr) {
          return std::nullopt;
        }

        new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
        new_resource.harq_id.pucch_res_ind = res_alloc.pucch_res_indicator;
        new_resource.pucch_res_cfg         = res_alloc.pucch_res;
      }

      new_resource.bits.harq_ack_bits  = r_harq.bits.harq_ack_bits;
      new_resource.bits.csi_part1_bits = r_csi.bits.csi_part1_bits;
      // SR bits, if present, can be in either HARQ or CSI, but not in both.
      if (r_harq.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_harq.bits.sr_bits;
      } else if (r_csi.bits.sr_bits != sr_nof_bits::no_sr) {
        new_resource.bits.sr_bits = r_csi.bits.sr_bits;
      }

      // Check if the UCI payload fits in the PUCCH resource.
      if (new_resource.bits.get_total_bits() <= pucch_cfg.get_max_payload(new_resource.format)) {
        return new_resource;
      } else {
        return std::nullopt;
      }

      return new_resource;
    }
  }

  if (resources_to_merge.size() == 3) {
    // Apply F2 HARQ merging rule: all PUCCH resources will be multiplexed in a PUCCH resource from PUCCH res set 1.
    pucch_grant        new_resource;
    const pucch_grant* r_harq_ptr = nullptr;
    const pucch_grant* r_sr_ptr   = nullptr;
    const pucch_grant* r_csi_ptr  = nullptr;
    for (const auto& grant : resources_to_merge) {
      if (grant.type == pucch_grant_type::harq_ack) {
        r_harq_ptr = &grant;
      } else if (grant.type == pucch_grant_type::sr) {
        r_sr_ptr = &grant;
      } else if (grant.type == pucch_grant_type::csi) {
        r_csi_ptr = &grant;
      }
    }

    srsran_assert(r_harq_ptr != nullptr and r_sr_ptr != nullptr and r_csi_ptr != nullptr,
                  "The three resources must be present");
    if (r_sr_ptr->format == pucch_format::FORMAT_0) {
      // SR and CSI are not supported on the same slot if SR uses Format 0.
      return std::nullopt;
    }

    if (r_harq_ptr->format != pucch_format::FORMAT_0 and r_harq_ptr->format != pucch_format::FORMAT_1) {
      new_resource = *r_harq_ptr;
    } else {
      pucch_harq_resource_alloc_record res_alloc =
          resource_manager.reserve_next_f2_harq_res_available(slot_harq, crnti, pucch_cfg);
      if (res_alloc.pucch_res != nullptr) {
        return std::nullopt;
      }

      new_resource.harq_id.pucch_set_idx = pucch_res_set_idx::set_1;
      new_resource.harq_id.pucch_res_ind = res_alloc.pucch_res_indicator;
      new_resource.pucch_res_cfg         = res_alloc.pucch_res;
    }
    new_resource.bits.harq_ack_bits  = r_harq_ptr->bits.harq_ack_bits;
    new_resource.bits.sr_bits        = r_sr_ptr->bits.sr_bits;
    new_resource.bits.csi_part1_bits = r_csi_ptr->bits.csi_part1_bits;

    // Check if the UCI payload fits in the PUCCH resource.
    if (new_resource.bits.get_total_bits() <= pucch_cfg.get_max_payload(new_resource.format)) {
      return new_resource;
    } else {
      return std::nullopt;
    }

    return new_resource;
  }

  return std::nullopt;
}

pucch_allocator_impl::pucch_grant_list
pucch_allocator_impl::multiplex_resources(slot_point                   sl_tx,
                                          rnti_t                       crnti,
                                          pucch_grant_list             candidate_grants,
                                          const ue_cell_configuration& ue_cell_cfg)
{
  pucch_grant_list mplexed_grants;

  std::vector<pucch_grant> resource_set_q;

  // Build the resource set Q.
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
  std::sort(resource_set_q.begin(), resource_set_q.end(), [](const pucch_grant& lhs_res, const pucch_grant& rhs_res) {
    return lhs_res.get_symbols().start() < rhs_res.get_symbols().start() or
           (lhs_res.get_symbols().start() == rhs_res.get_symbols().start() and
            lhs_res.get_symbols().length() > rhs_res.get_symbols().length());
  });

  // This is the implementation of the sudo code for multiplexing the resources provided in Section 9.2.5, TS 38.213.
  unsigned o_cnt = 0;
  unsigned j_cnt = 0;
  while (j_cnt < resource_set_q.size()) {
    if (j_cnt < resource_set_q.size() - 1 and
        resource_set_q[j_cnt - o_cnt].get_symbols().overlaps(resource_set_q[j_cnt + 1].get_symbols())) {
      ++j_cnt;
      ++o_cnt;
    } else {
      if (o_cnt > 0U) {
        // Merge the overlapping resources.
        std::optional<pucch_grant> new_res =
            merge_pucch_resources(span<const pucch_grant>(&resource_set_q[j_cnt - o_cnt], o_cnt),
                                  sl_tx,
                                  crnti,
                                  ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
        if (not new_res.has_value()) {
          return {};
        }
        // Remove the old resources that got merged from the set.
        resource_set_q.erase(resource_set_q.begin() + j_cnt - o_cnt, resource_set_q.begin() + j_cnt + 1);

        // Add the new resource (resulting from the previous merge) to the set.
        resource_set_q.push_back(new_res.value());

        // Sort the resources in the set based on the first symbol position and number of symbols.
        std::sort(
            resource_set_q.begin(), resource_set_q.end(), [](const pucch_grant& lhs_res, const pucch_grant& rhs_res) {
              return lhs_res.get_symbols().start() < rhs_res.get_symbols().start() or
                     (lhs_res.get_symbols().start() == rhs_res.get_symbols().start() and
                      lhs_res.get_symbols().length() > rhs_res.get_symbols().length());
            });

        // Reset the counter and start from the beginning.
        j_cnt = 0;
        o_cnt = 0;
      } else {
        ++j_cnt;
      }
    }
  }

  // The PUCCH resource multiplexing algorithm above is specified for the UE. In the GNB, we need to add an extra
  // resource Format 1 if slot there is a SR opportunity and HARQ bits to be reported with PUCCH Format 1.
  if (resource_set_q.size() == 1 and resource_set_q.front().format == pucch_format::FORMAT_1 and
      resource_set_q.front().bits.harq_ack_bits != 0 and resource_set_q.front().bits.sr_bits != sr_nof_bits::no_sr) {
    const pucch_resource* sr_res = resource_manager.reserve_sr_res_available(
        sl_tx, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
    if (sr_res == nullptr) {
      logger.error("This is not expected");
    }
    uci_bits bits = {.harq_ack_bits  = resource_set_q.front().bits.harq_ack_bits,
                     .sr_bits        = resource_set_q.front().bits.sr_bits,
                     .csi_part1_bits = 0};
    resource_set_q.emplace_back(pucch_grant{
        .type = pucch_grant_type::sr, .format = pucch_format::FORMAT_1, .bits = bits, .pucch_res_cfg = sr_res});
  }

  // Build the final list of PUCCH resources that need to be transmitted.
  for (const auto& mplex_res : resource_set_q) {
    if (mplex_res.type == pucch_grant_type::harq_ack) {
      mplexed_grants.harq_resource.emplace(mplex_res);
      ++mplexed_grants.nof_grants;
    } else if (mplex_res.type == pucch_grant_type::sr) {
      mplexed_grants.sr_resource.emplace(mplex_res);
      ++mplexed_grants.nof_grants;
    } else if (mplex_res.type == pucch_grant_type::csi) {
      mplexed_grants.csi_resource.emplace(mplex_res);
      ++mplexed_grants.nof_grants;
    }
  }
  return mplexed_grants;
}

std::optional<unsigned> pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue_1(cell_resource_allocator&     res_alloc,
                                                                            rnti_t                       crnti,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            unsigned                     k0,
                                                                            unsigned                     k1)
{
  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];

  garbage_collector.reset();

  slot_point sl_ack = pucch_slot_alloc.slot;

  auto& ue_pucchs = pucch_grants_alloc_grid[sl_ack.to_uint()];

  auto* existing_grant_it = std::find_if(
      ue_pucchs.begin(), ue_pucchs.end(), [crnti](const ue_pucch_bits& pucch) { return pucch.rnti == crnti; });

  // Allocate PUCCH HARQ-ACK grant depending on whether there existing PUCCH grants.
  if (existing_grant_it != ue_pucchs.end()) {
    uci_bits new_bits = existing_grant_it->pucch_grants.get_uci_bits();
    ++new_bits.harq_ack_bits;

    std::optional<unsigned> pucch_res_ind =
        multiplex_and_allocate_pucch(pucch_slot_alloc, new_bits, *existing_grant_it, ue_cell_cfg);
    if (not pucch_res_ind) {
      garbage_collector.release_resource(sl_ack, crnti, ue_cell_cfg);
    }
    return pucch_res_ind;
  } else {
    return allocate_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg);
  }
}

std::optional<unsigned>
pucch_allocator_impl::multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                   uci_bits                      new_bits,
                                                   ue_pucch_bits&                current_grants,
                                                   const ue_cell_configuration&  ue_cell_cfg)
{
  slot_point sl_ack = pucch_slot_alloc.slot;

  // Find resource needed for the bits to be reported, assuming the resource is not multiplexed.
  std::optional<pucch_grant_list> candidate_resources =
      get_pucch_res_pre_multiplexing(sl_ack, new_bits, current_grants, ue_cell_cfg);
  if (not candidate_resources.has_value()) {
    return std::nullopt;
  }

  // Multiplex the resources.
  pucch_grant_list multiplexed_grants =
      multiplex_resources(sl_ack, current_grants.rnti, candidate_resources.value(), ue_cell_cfg);

  // Allocate the grants.
  return allocate_grants(pucch_slot_alloc, current_grants, current_grants.rnti, multiplexed_grants, ue_cell_cfg);
}

std::optional<unsigned> pucch_allocator_impl::allocate_grants(cell_slot_resource_allocator& pucch_slot_alloc,
                                                              ue_pucch_bits&                existing_pucchs,
                                                              rnti_t                        crnti,
                                                              pucch_grant_list              grants_to_tx,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  // Retrieve the existing PUCCH PDUs.
  existing_pucch_pdus_handler existing_pdus(
      crnti,
      pucch_slot_alloc.result.ul.pucchs,
      grants_to_tx.sr_resource.has_value() ? grants_to_tx.sr_resource.value().pucch_res_cfg : nullptr);

  // Check if we can fit the new PUCCH PDUs in the output results.
  // TODO: Check if this is correct, or we need to add +/-1.
  if (pucch_slot_alloc.result.ul.pucchs.size() + (grants_to_tx.nof_grants - existing_pdus.grants_cnt) >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info(
        "rnti={}: PUCCH allocation for slot={} skipped. Cause: UL grants reached", crnti, pucch_slot_alloc.slot);
    return std::nullopt;
  }

  bool harq_grant_alloc_completed = false;
  bool sr_grant_alloc_completed   = false;
  bool csi_grant_alloc_completed  = false;
  // If there was a CSI grant, re-use the previous one and update the UCI bits with SR.
  if (grants_to_tx.csi_resource.has_value() and existing_pucchs.pucch_grants.csi_resource.has_value()) {
    existing_pdus.update_csi_pdu_bits(grants_to_tx.csi_resource.value().bits.csi_part1_bits,
                                      grants_to_tx.csi_resource.value().bits.sr_bits);
    csi_grant_alloc_completed = true;
  }
  // If there was a SR grant, re-use the previous one and update UCI bits with HARQ bits.
  else if (grants_to_tx.sr_resource.has_value() and existing_pucchs.pucch_grants.sr_resource.has_value()) {
    // NOTE: the validator is responsible for checking that the is no mix of PUCCH Format 0 and Format 1.
    existing_pdus.update_sr_pdu_bits(grants_to_tx.sr_resource.value().bits.sr_bits,
                                     grants_to_tx.sr_resource.value().bits.harq_ack_bits);
    sr_grant_alloc_completed = true;
  }
  // If there was a HARQ grant of the same PUCCH format, re-use the previous one and update the UCI bits HARQ/CSI/SR
  // bits.
  if (grants_to_tx.harq_resource.has_value() and existing_pucchs.pucch_grants.harq_resource.has_value() and
      grants_to_tx.harq_resource.value().format == existing_pucchs.pucch_grants.harq_resource.value().format) {
    // Update bits;
    existing_pdus.update_harq_pdu_bits(grants_to_tx.harq_resource.value().bits.harq_ack_bits,
                                       grants_to_tx.harq_resource.value().bits.sr_bits,
                                       grants_to_tx.harq_resource.value().bits.csi_part1_bits);
    harq_grant_alloc_completed = true;
  }

  uci_bits bits = grants_to_tx.get_uci_bits();
  if (grants_to_tx.csi_resource.has_value() and not csi_grant_alloc_completed) {
    pucch_info* grant = existing_pdus.get_next_grant();
    if (grant == nullptr) {
      grant = &pucch_slot_alloc.result.ul.pucchs.emplace_back();
    }
    const unsigned nof_prbs =
        std::get<pucch_format_2_3_cfg>(grants_to_tx.csi_resource.value().pucch_res_cfg->format_params).nof_prbs;
    fill_pucch_format2_grant(*grant,
                             crnti,
                             *grants_to_tx.csi_resource.value().pucch_res_cfg,
                             ue_cell_cfg,
                             nof_prbs,
                             0U,
                             grants_to_tx.csi_resource.value().bits.sr_bits,
                             grants_to_tx.csi_resource.value().bits.csi_part1_bits);
  }
  if (grants_to_tx.sr_resource.has_value() and not sr_grant_alloc_completed) {
    pucch_info* grant = existing_pdus.get_next_grant();
    if (grant == nullptr) {
      grant = &pucch_slot_alloc.result.ul.pucchs.emplace_back();
    }
    if (grants_to_tx.sr_resource.value().format == pucch_format::FORMAT_0) {
      // TODO
    } else {
      fill_pucch_ded_format1_grant(*grant,
                                   crnti,
                                   *grants_to_tx.sr_resource.value().pucch_res_cfg,
                                   grants_to_tx.sr_resource.value().bits.harq_ack_bits,
                                   grants_to_tx.sr_resource.value().bits.sr_bits);
    }
  }
  if (grants_to_tx.harq_resource.has_value() and not harq_grant_alloc_completed) {
    pucch_info* grant = existing_pdus.get_next_grant();
    if (grant == nullptr) {
      grant = &pucch_slot_alloc.result.ul.pucchs.emplace_back();
    }
    if (grants_to_tx.harq_resource.value().format == pucch_format::FORMAT_0) {
      // TODO
    } else if (grants_to_tx.harq_resource.value().format == pucch_format::FORMAT_1) {
      fill_pucch_ded_format1_grant(*grant,
                                   crnti,
                                   *grants_to_tx.sr_resource.value().pucch_res_cfg,
                                   grants_to_tx.sr_resource.value().bits.harq_ack_bits,
                                   grants_to_tx.sr_resource.value().bits.sr_bits);
    } else {
      const auto& f2_cfg =
          std::get<pucch_format_2_3_cfg>(grants_to_tx.harq_resource.value().pucch_res_cfg->format_params);
      const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                                   .ul_config.value()
                                                                   .init_ul_bwp.pucch_cfg.value()
                                                                   .format_2_common_param.value()
                                                                   .max_c_rate);
      const unsigned nof_prbs =
          get_pucch_format2_nof_prbs(bits.get_total_bits(), f2_cfg.nof_prbs, f2_cfg.nof_symbols, max_pucch_code_rate);
      fill_pucch_format2_grant(*grant,
                               crnti,
                               *grants_to_tx.harq_resource.value().pucch_res_cfg,
                               ue_cell_cfg,
                               nof_prbs,
                               grants_to_tx.harq_resource.value().bits.harq_ack_bits,
                               grants_to_tx.harq_resource.value().bits.sr_bits,
                               grants_to_tx.harq_resource.value().bits.csi_part1_bits);
    }
  }

  slot_point sl_tx = pucch_slot_alloc.slot;
  // Remove the previously allocated PUCCH resources which are not needed after the new allocation..
  remove_unsed_pucch_res(sl_tx, grants_to_tx, existing_pucchs, ue_cell_cfg);

  // Update the new grants to the UE allocation record.
  existing_pucchs.pucch_grants = grants_to_tx;

  // The return value is only relevant if the allocation was called for a HARQ-ACK grant.
  return grants_to_tx.harq_resource.has_value() ? grants_to_tx.harq_resource.value().harq_id.pucch_res_ind : 0U;
}
