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

#include "pucch_allocator_impl.h"
#include "../support/csi_report_helpers.h"
#include "../support/pucch/pucch_default_resource.h"
#include "../support/sr_helper.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/pucch/pucch_info.h"

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
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

optional<unsigned> pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
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
    return nullopt;
  }

  if (not cell_cfg.is_fully_ul_enabled(pucch_slot_alloc.slot)) {
    return nullopt;
  }

  // Get the PUCCH resources, either from default tables.
  optional<pucch_res_alloc_cfg> pucch_res = alloc_pucch_common_res_harq(pucch_slot_alloc, dci_info.ctx);

  // No resources available for PUCCH.
  if (not pucch_res.has_value()) {
    logger.debug("tc-rnti={}: PUCCH not allocated due to resources not available", tcrnti);
    return nullopt;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().first_hop_res);
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().second_hop_res);

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_info, tcrnti, pucch_res.value());
  unsigned pucch_res_indicator = pucch_res.value().pucch_res_indicator;

  pucch_common_alloc_grid[slot_alloc[k0 + k1 + slot_alloc.cfg.ntn_cs_koffset].slot.to_uint()].emplace_back(tcrnti);

  logger.debug("tc-rnti={}: PUCCH HARQ-ACK common with res_ind={} allocated for slot={}",
               tcrnti,
               pucch_res_indicator,
               pucch_slot_alloc.slot);

  return pucch_res_indicator;
}

optional<unsigned> pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
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

  const unsigned harq_ack_bits_increment = 1;

  // Case 1) If there is a PUCCH format 2 grant, update it.
  if (existing_grants.format2_grant != nullptr) {
    // Case 1-A) If the allocated resource is the one specific for CSI, change resource and allocate grant.
    if (existing_grants.format2_grant->format_2.harq_ack_nof_bits == 0 and
        existing_grants.format2_grant->format_2.csi_part1_bits > 0) {
      return change_format2_resource(
          pucch_slot_alloc, *existing_grants.format2_grant, crnti, ue_cell_cfg, harq_ack_bits_increment);
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
  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: SR occasion allocation for slot={} skipped. Cause: max number of UL grants reached",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  const existing_pucch_grants existing_grants =
      get_existing_pucch_grants(pucch_slot_alloc.result.ul.pucchs, crnti, pucch_slot_alloc.slot);
  srsran_assert(
      existing_grants.format1_harq_grant == nullptr and existing_grants.format1_sr_grant == nullptr and
          existing_grants.format2_grant == nullptr,
      "The SR is the first dedicated PUCCH grant that is expected to be allocated; no grants expected at this point.");

  // Get the index of the PUCCH resource to be used for SR.
  const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("rnti={}: SR occasion allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  // Allocate PUCCH SR grant only.
  const unsigned harq_ack_bits_increment = 0U;
  fill_pucch_ded_format1_grant(pucch_slot_alloc.result.ul.pucchs.emplace_back(),
                               crnti,
                               *pucch_sr_res,
                               harq_ack_bits_increment,
                               sr_nof_bits::one);
}

void pucch_allocator_impl::pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          unsigned                      csi_part1_nof_bits)
{
  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti, pucch_slot_alloc.slot);
  srsran_assert(existing_grants.format2_grant == nullptr and existing_grants.format1_harq_grant == nullptr,
                "The CSI is the first dedicated PUCCH grant that is expected to be allocated.");

  if (existing_grants.format1_sr_grant != nullptr) {
    return convert_to_format2_csi(
        pucch_slot_alloc, *existing_grants.format1_sr_grant, crnti, ue_cell_cfg, csi_part1_nof_bits);
  }

  return allocate_new_csi_grant(pucch_slot_alloc, crnti, ue_cell_cfg, csi_part1_nof_bits);
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
    else {
      // Only remove HARQ-ACK grant, handle SR grant separately.
      removed_uci_info.harq_ack_nof_bits = it->format_1.harq_ack_nof_bits;
      pucchs.erase(it);
      resource_manager.release_harq_f1_resource(slot_alloc.slot, crnti, pucch_cfg);
    }
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
optional<pucch_allocator_impl::pucch_res_alloc_cfg>
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
      return optional<pucch_allocator_impl::pucch_res_alloc_cfg>{candidate_pucch_resource};
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
    return optional<pucch_allocator_impl::pucch_res_alloc_cfg>{candidate_pucch_resource};
  }

  // This is the case in which there exists no available resource.
  return nullopt;
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

optional<unsigned> pucch_allocator_impl::allocate_new_format1_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                         rnti_t                        crnti,
                                                                         const ue_cell_configuration&  ue_cell_cfg,
                                                                         pucch_info* existing_sr_grant)
{
  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.info("rnti={}: HARQ-ACK allocation on PUCCH Format1 for slot={} skipped. Cause: UL grants reached",
                crnti,
                pucch_slot_alloc.slot);
    return nullopt;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_f1_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug("rnti={}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
                 crnti,
                 pucch_slot_alloc.slot);
    return nullopt;
  }

  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_format1_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  const unsigned pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  logger.debug("rnti={}: PUCCH HARQ-ACK allocation on F1 with res_ind={} for slot={} completed",
               crnti,
               pucch_res_indicator,
               pucch_slot_alloc.slot);
  return pucch_res_indicator;
}

void pucch_allocator_impl::convert_to_format2_csi(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  pucch_info&                   existing_sr_grant,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      csi_part1_nof_bits)
{
  // Get a PUCCH Format 2 resource.
  const pucch_resource* pucch_res = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, rnti, ue_cell_cfg);
  if (pucch_res == nullptr) {
    logger.warning(
        "rnti={}: CSI could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource not available",
        rnti,
        pucch_slot_alloc.slot);
    return;
  }

  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);

  // This function can be only be called after the SR gets allocated. The \c sr_bits_to_report will be passed on the
  // PUCCH grant for CSI. The same SR bits will be used to compute the expected payload to be carried by the PUCCH F2
  // CSI-specific resource.
  const sr_nof_bits sr_bits_to_report  = existing_sr_grant.format_1.sr_bits;
  const unsigned    candidate_uci_bits = sr_nof_bits_to_uint(sr_bits_to_report) + csi_part1_nof_bits;

  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(pucch_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(pucch_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  // It's the config validator that should ensure this is verified.
  srsran_assert(max_payload >= candidate_uci_bits,
                "rnti={}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                rnti,
                max_payload,
                candidate_uci_bits);

  // Remove the previously allocated PUCCH format-1 resources.
  remove_pucch_format1_from_grants(
      pucch_slot_alloc, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: UCI allocation on PUCCH Format2 for slot={} skipped. Cause: UL grants reached",
                   rnti,
                   pucch_slot_alloc.slot);
    return;
  }

  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("rnti={}: UCI could not be allocated on PUCCH Format2 for slot={}. Cause: List still full after "
                   "removing PUCCH F1 grant",
                   rnti,
                   pucch_slot_alloc.slot);
    return;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info&    pucch_pdu          = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned harq_bits_only_csi = 0U;
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *pucch_res,
                           ue_cell_cfg,
                           variant_get<pucch_format_2_3_cfg>(pucch_res->format_params).nof_prbs,
                           harq_bits_only_csi,
                           sr_bits_to_report,
                           csi_part1_nof_bits);
}

optional<unsigned> pucch_allocator_impl::convert_to_format2_harq(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                 pucch_info&                   existing_harq_grant,
                                                                 pucch_info*                   existing_sr_grant,
                                                                 rnti_t                        rnti,
                                                                 const ue_cell_configuration&  ue_cell_cfg,
                                                                 unsigned int                  harq_ack_bits_increment)
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
    return nullopt;
  }

  const unsigned candidate_uci_bits = curr_harq_bits + harq_ack_bits_increment + sr_nof_bits_to_uint(sr_bits);

  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
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
    return nullopt;
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
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
    return nullopt;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
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

  logger.debug("rnti={}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits with res_ind={} for "
               "slot={} completed",
               rnti,
               curr_harq_bits + harq_ack_bits_increment,
               sr_nof_bits_to_uint(sr_bits),
               csi1_nof_bits_only_harq,
               format2_res.pucch_res_indicator,
               pucch_slot_alloc.slot

  );

  return format2_res.pucch_res_indicator;
}

optional<unsigned> pucch_allocator_impl::change_format2_resource(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                 pucch_info&                   existing_grant,
                                                                 rnti_t                        rnti,
                                                                 const ue_cell_configuration&  ue_cell_cfg,
                                                                 unsigned                      harq_ack_bits_increment)
{
  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const pucch_harq_resource_alloc_record format2_res =
      resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);

  if (format2_res.pucch_res == nullptr) {
    logger.debug("rnti={}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource "
                 "not available",
                 rnti,
                 pucch_slot_alloc.slot);
    return nullopt;
  }

  // This function would only be called in case CSI and SR gets allocated before the HARQ. In that case, if there are
  // SR bits or CSI bits to be carried by the PUCCH F2 grant, they would have already been allocated and there is no
  // need to check if the slot is an CSI or SR opportunity.
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
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
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
    return nullopt;
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
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
    return nullopt;
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
  logger.debug("rnti={}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for slot={} completed",
               rnti,
               harq_ack_bits_increment,
               sr_nof_bits_to_uint(sr_bits_to_report),
               csi_bits_to_report,
               pucch_slot_alloc.slot);

  return static_cast<unsigned>(format2_res.pucch_res_indicator);
}

optional<unsigned> pucch_allocator_impl::add_harq_ack_bit_to_format1_grant(pucch_info&         existing_harq_grant,
                                                                           pucch_info*         existing_sr_grant,
                                                                           rnti_t              rnti,
                                                                           slot_point          sl_tx,
                                                                           const pucch_config& pucch_cfg)
{
  const int pucch_res_idx = resource_manager.fetch_f1_pucch_res_indic(sl_tx, rnti, pucch_cfg);
  if (pucch_res_idx < 0) {
    srsran_assert(pucch_res_idx >= 0, "PUCCH resource index should not be negative");
    return nullopt;
  }
  srsran_sanity_check(existing_harq_grant.format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for HARQ");
  // Update the SR, if present.
  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }
  // Update the HARQ, if present.
  existing_harq_grant.format_1.harq_ack_nof_bits++;
  const unsigned pucch_res_indicator = static_cast<unsigned>(pucch_res_idx);

  logger.debug(
      "rnti={}: HARQ-ACK mltplxd on existing PUCCH F1 with res_ind={} for slot={}", rnti, pucch_res_indicator, sl_tx);
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
           not is_pucch_f1_grant_common(crnti, sl_tx);
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

void pucch_allocator_impl::allocate_new_csi_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  rnti_t                        crnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      csi_part1_bits)
{
  srsran_assert(csi_part1_bits != 0, "This function can only be called to allocate a PUCCH F2 resource for CSI");

  // [Implementation-defined] We only allow a max number of PUCCH + PUSCH grants per slot.
  if (pucch_slot_alloc.result.ul.pucchs.size() >=
      get_max_pucch_grants(static_cast<unsigned>(pucch_slot_alloc.result.ul.puschs.size()))) {
    logger.warning("rnti={}: CSI allocation on PUCCH Format2 for slot={} skipped. Cause: UL grants reached",
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

  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  // When this function is called, it means that there are no SR grants to be multiplexed with CSI; thus, the CSI bits
  // are the only UCI bits to be considered.
  // It's the validator that should make sure the CSI bits fit into a PUCCH Format 2 resource.
  srsran_assert(max_payload >= csi_part1_bits,
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
                           variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_prbs,
                           harq_ack_bits_only_csi,
                           sr_bits_only_csi,
                           csi_part1_bits);
}

optional<unsigned> pucch_allocator_impl::add_harq_bits_to_harq_f2_grant(pucch_info&                  existing_f2_grant,
                                                                        slot_point                   sl_tx,
                                                                        rnti_t                       crnti,
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
    return nullopt;
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

  const auto&    f2_params    = variant_get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params);
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
    return nullopt;
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
  logger.debug("rnti={}: HARQ-ACK multiplexed on existing PUCCH F2 with res_ind={} for slot={}",
               existing_f2_grant.crnti,
               pucch_f2_harq_cfg.pucch_res_indicator,
               sl_tx);

  return pucch_f2_harq_cfg.pucch_res_indicator;
}

void pucch_allocator_impl::fill_pucch_ded_format1_grant(pucch_info&           pucch_grant,
                                                        rnti_t                crnti,
                                                        const pucch_resource& pucch_ded_res_cfg,
                                                        unsigned              harq_ack_bits,
                                                        sr_nof_bits           sr_bits)
{
  pucch_grant.crnti   = crnti;
  pucch_grant.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_grant.format  = pucch_format::FORMAT_1;

  // Set PRBs and symbols, first.º
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  const pucch_format_1_cfg& res_f1 = variant_get<pucch_format_1_cfg>(pucch_ded_res_cfg.format_params);
  pucch_grant.resources.prbs.set(pucch_ded_res_cfg.starting_prb,
                                 pucch_ded_res_cfg.starting_prb + PUCCH_FORMAT_1_NOF_PRBS);
  pucch_grant.resources.symbols.set(res_f1.starting_sym_idx, res_f1.starting_sym_idx + res_f1.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_grant.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                              pucch_ded_res_cfg.second_hop_prb.value() + PUCCH_FORMAT_1_NOF_PRBS);
  }
  // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
  pucch_grant.format_1.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
  pucch_grant.format_1.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                           ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                           : cell_cfg.pci;
  pucch_grant.format_1.initial_cyclic_shift = res_f1.initial_cyclic_shift;
  pucch_grant.format_1.time_domain_occ      = res_f1.time_domain_occ;
  // For PUCCH Format 1, only 1 SR bit.
  pucch_grant.format_1.sr_bits           = sr_bits;
  pucch_grant.format_1.harq_ack_nof_bits = harq_ack_bits;
  // [Implementation-defined] We do not implement PUCCH over several slots.
  pucch_grant.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
}

void pucch_allocator_impl::fill_pucch_format2_grant(pucch_info&                  pucch_grant,
                                                    rnti_t                       crnti,
                                                    const pucch_resource&        pucch_ded_res_cfg,
                                                    const ue_cell_configuration& ue_cell_cfg,
                                                    unsigned                     nof_prbs,
                                                    unsigned                     harq_ack_bits,
                                                    sr_nof_bits                  sr_bits,
                                                    unsigned                     csi_part1_bits)
{
  pucch_grant.crnti   = crnti;
  pucch_grant.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_grant.format  = pucch_format::FORMAT_2;

  // Set PRBs and symbols, first.º
  // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
  pucch_grant.resources.prbs.set(pucch_ded_res_cfg.starting_prb, pucch_ded_res_cfg.starting_prb + nof_prbs);
  const pucch_format_2_3_cfg& res_f2 = variant_get<pucch_format_2_3_cfg>(pucch_ded_res_cfg.format_params);
  pucch_grant.resources.symbols.set(res_f2.starting_sym_idx, res_f2.starting_sym_idx + res_f2.nof_symbols);
  if (pucch_ded_res_cfg.second_hop_prb.has_value()) {
    pucch_grant.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb.value(),
                                              pucch_ded_res_cfg.second_hop_prb.value() + nof_prbs);
  }

  pucch_grant.format_2.sr_bits           = sr_bits;
  pucch_grant.format_2.harq_ack_nof_bits = harq_ack_bits;
  pucch_grant.format_2.csi_part1_bits    = csi_part1_bits;
  // \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  pucch_grant.format_2.n_id_scambling =
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.has_value()
          ? ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().data_scrambling_id_pusch.value()
          : cell_cfg.pci;
  // \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  pucch_grant.format_2.n_id_0_scrambling = get_n_id0_scrambling(ue_cell_cfg, cell_cfg.pci);
  pucch_grant.format_2.max_code_rate     = ue_cell_cfg.cfg_dedicated()
                                           .ul_config.value()
                                           .init_ul_bwp.pucch_cfg.value()
                                           .format_2_common_param.value()
                                           .max_c_rate;

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (csi_part1_bits > 0) {
    pucch_grant.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
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

bool pucch_allocator_impl::is_pucch_f1_grant_common(rnti_t rnti, slot_point sl_tx) const
{
  return std::find(pucch_common_alloc_grid[sl_tx.to_uint()].begin(),
                   pucch_common_alloc_grid[sl_tx.to_uint()].end(),
                   rnti) != pucch_common_alloc_grid[sl_tx.to_uint()].end();
}

unsigned pucch_allocator_impl::get_max_pucch_grants(unsigned currently_allocated_puschs)
{
  return std::min(max_pucch_grants_per_slot, max_ul_grants_per_slot - currently_allocated_puschs);
}
