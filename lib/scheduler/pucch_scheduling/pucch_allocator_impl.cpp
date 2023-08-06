/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/pucch/pucch_info.h"

//////////////     Helper functions       //////////////

using namespace srsran;

struct existing_pucch_grants {
  pucch_info* format1_sr_grant{nullptr};
  pucch_info* format1_harq_grant{nullptr};
  pucch_info* format2_grant{nullptr};
};

// Retrieve the existing PUCCH grants for the current RNTI.
static existing_pucch_grants get_existing_pucch_grants(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs,
                                                       rnti_t                                              rnti)
{
  existing_pucch_grants grants;
  for (auto& pucch : pucchs) {
    if (pucch.crnti == rnti) {
      // First look for first for Format 2; if present, this is the only PUCCH resource allocated to the UE.
      if (pucch.format == srsran::pucch_format::FORMAT_2) {
        grants.format2_grant = &pucch;
        // If there is a grant for PUCCH format 2, then no PUCCH format-1 grants are expected.
        break;
      }
      // Second, look for first for Format 1.
      else if (pucch.format == srsran::pucch_format::FORMAT_1) {
        if (pucch.format_1.sr_bits == sr_nof_bits::one) {
          grants.format1_sr_grant = &pucch;
        } else if (pucch.format_1.harq_ack_nof_bits > 0 and pucch.format_1.sr_bits == sr_nof_bits::no_sr) {
          grants.format1_harq_grant = &pucch;
        }
      }
    }
  }

  return grants;
}

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

// \brief Helper that updates number of bits to be reported in the PUCCH Format 2 grant.
// The number of bits are computed so that not to exceed the max_code_rate for the PUCCH Format 2.
// The priority is given to HARQ-ACK bits, followed by SR and CSI.
static void update_format2_uci_bits(pucch_info&           existing_f2_grant,
                                    const pucch_resource& res_cfg,
                                    float                 max_code_rate,
                                    unsigned              nof_harq_ack_bits,
                                    sr_nof_bits           nof_sr_bits,
                                    unsigned              nof_csi_part1_bits)
{
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(res_cfg.format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(res_cfg.format_params).nof_symbols,
                                    max_code_rate);

  const unsigned sr_bits = sr_nof_bits_to_uint(existing_f2_grant.format_2.sr_bits) + sr_nof_bits_to_uint(nof_sr_bits);
  const unsigned harq_ack_bits      = existing_f2_grant.format_2.harq_ack_nof_bits + nof_harq_ack_bits;
  const unsigned csi_part1_bits     = existing_f2_grant.format_2.csi_part1_bits + nof_csi_part1_bits;
  unsigned       candidate_uci_bits = harq_ack_bits + sr_bits + csi_part1_bits;

  // If the additional UCI bits can be allocated, update the UCI bits and exit.
  if (candidate_uci_bits <= max_payload) {
    existing_f2_grant.format_2.harq_ack_nof_bits += nof_harq_ack_bits;
    existing_f2_grant.format_2.sr_bits = existing_f2_grant.format_2.sr_bits + nof_sr_bits;
    existing_f2_grant.format_2.csi_part1_bits += nof_csi_part1_bits;
    return;
  }

  // Attempt to allocate additional HARQ bits.
  if (harq_ack_bits > max_payload) {
    // We no additional HARQ can be added, it means that the PUCCH grant had already reached its maximum capacity in
    // terms of bits.
    existing_f2_grant.format_2.sr_bits        = srsran::sr_nof_bits::no_sr;
    existing_f2_grant.format_2.csi_part1_bits = 0;
    return;
  }
  existing_f2_grant.format_2.harq_ack_nof_bits += nof_harq_ack_bits;
  candidate_uci_bits = harq_ack_bits;

  // Attempt to allocate old SR bits.
  // NOTE: for the time being, there should not be more than 1 SR bit.
  if (candidate_uci_bits + sr_nof_bits_to_uint(existing_f2_grant.format_2.sr_bits) > max_payload) {
    // TODO: Log an error message and exit.
    existing_f2_grant.format_2.sr_bits        = srsran::sr_nof_bits::no_sr;
    existing_f2_grant.format_2.csi_part1_bits = 0;
    return;
  }
  candidate_uci_bits += sr_nof_bits_to_uint(existing_f2_grant.format_2.sr_bits);

  // Attempt to allocate new SR bits.
  if (candidate_uci_bits + sr_nof_bits_to_uint(nof_sr_bits) > max_payload) {
    // TODO: Log an error message and exit.
    existing_f2_grant.format_2.csi_part1_bits = 0;
    return;
  }
  candidate_uci_bits += sr_nof_bits_to_uint(nof_sr_bits);
  existing_f2_grant.format_2.sr_bits = existing_f2_grant.format_2.sr_bits + nof_sr_bits;

  // Attempt to allocate old CSI bits.
  if (candidate_uci_bits + existing_f2_grant.format_2.csi_part1_bits > max_payload) {
    // TODO: Log an error message and exit.
    existing_f2_grant.format_2.csi_part1_bits = 0;
    return;
  }
}

// \brief Helper that computes number of bits to be reported a new PUCCH Format 2 grant.
// The number of bits are computed so that not to exceed the max_code_rate for the PUCCH Format 2.
// The priority is given to HARQ-ACK bits, followed by SR and CSI.
static pucch_uci_bits compute_format2_uci_bits(const pucch_resource& res_cfg,
                                               float                 max_code_rate,
                                               unsigned              nof_harq_ack_bits,
                                               sr_nof_bits           nof_sr_bits,
                                               unsigned              nof_csi_part1_bits)
{
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(res_cfg.format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(res_cfg.format_params).nof_symbols,
                                    max_code_rate);

  const unsigned candidate_uci_bits = nof_harq_ack_bits + sr_nof_bits_to_uint(nof_sr_bits) + nof_csi_part1_bits;

  // If the additional UCI bits can be allocated, update the UCI bits and exit.
  if (candidate_uci_bits <= max_payload) {
    return pucch_uci_bits{
        .harq_ack_nof_bits = nof_harq_ack_bits, .sr_bits = nof_sr_bits, .csi_part1_bits = nof_csi_part1_bits};
  }

  // Attempt to allocate the HARQ bits.
  if (nof_harq_ack_bits > max_payload) {
    return pucch_uci_bits{};
  }

  // Attempt to allocate SR bits.
  if (nof_harq_ack_bits + sr_nof_bits_to_uint(nof_sr_bits) > max_payload) {
    // TODO: Log an error message and exit.
    return pucch_uci_bits{.harq_ack_nof_bits = nof_harq_ack_bits};
  }

  return pucch_uci_bits{.harq_ack_nof_bits = nof_harq_ack_bits, .sr_bits = nof_sr_bits};
}

//////////////    Public functions       //////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

pucch_harq_ack_grant pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator& slot_alloc,
                                                                          rnti_t                   tcrnti,
                                                                          unsigned pdsch_time_domain_resource,
                                                                          unsigned k1,
                                                                          const pdcch_dl_information& dci_info)
{
  // PUCCH output.
  pucch_harq_ack_grant pucch_harq_ack_output{};

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = slot_alloc[pdsch_time_domain_resource + k1];

  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    return pucch_harq_ack_output;
  }

  if (not cell_cfg.is_fully_ul_enabled(pucch_slot_alloc.slot)) {
    return pucch_harq_ack_output;
  }

  // Get the PUCCH resources, either from default tables.
  optional<pucch_res_alloc_cfg> pucch_res = alloc_pucch_common_res_harq(pucch_slot_alloc, dci_info.ctx);

  // No resources available for PUCCH.
  if (not pucch_res.has_value()) {
    logger.debug("PUCCH for TC-RNTI={:#x} not allocated due to resources not available", tcrnti);
    return pucch_harq_ack_output;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().first_hop_res);
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.value().second_hop_res);

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_common_grant(pucch_info, tcrnti, pucch_res.value());
  pucch_harq_ack_output.pucch_pdu           = &pucch_info;
  pucch_harq_ack_output.pucch_res_indicator = pucch_res.value().pucch_res_indicator;

  logger.debug("PUCCH for TC-RNTI={:#x} allocated for slot={}.", tcrnti, pucch_slot_alloc.slot);

  return pucch_harq_ack_output;
}

pucch_harq_ack_grant pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                                       rnti_t                       crnti,
                                                                       const ue_cell_configuration& ue_cell_cfg,
                                                                       unsigned pdsch_time_domain_resource,
                                                                       unsigned k1)
{
  // TS 38.213, Section 9.2.3, explains the UE's procedure to multiplex HARQ-ACK reporting of multiple slot and for
  // different cells.
  // "The PUCCH resource determination is based on a PUCCH resource indicator field [5, TS 38.212] in a last DCI format
  // 1_0 or DCI format 1_1, among the DCI formats 1_0 or DCI formats 1_1 that have a value of a PDSCH-to-HARQ_feedback
  // timing indicator field indicating a same slot for the PUCCH transmission, that the UE detects and for which the UE
  // transmits corresponding HARQ-ACK information in the PUCCH where, for PUCCH resource determination, detected DCI
  // formats are first indexed in an ascending order across serving cells indexes for a same PDCCH monitoring occasion
  // and are then indexed in an ascending order across PDCCH monitoring occasion indexes".
  // As a result of this, and depending on whether there is any scheduled SRs, the PUCCH allocator can either allocate
  // a new PUCCH grant or update an existing one by changing the number of HARQ-ACK bits to be reported.

  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[pdsch_time_domain_resource + k1];

  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH grants.
  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti);

  const unsigned    harq_ack_bits  = 1;
  const sr_nof_bits sr_bits        = sr_nof_bits::no_sr;
  const unsigned    csi_part1_bits = 0;

  // Case 1) If there is a PUCCH format 2 grant, update it.
  if (existing_grants.format2_grant != nullptr) {
    // Case 1-A) If the allocated resource is the one specific for CSI, change resource and allocate grant.
    if (existing_grants.format2_grant->format_2.harq_ack_nof_bits == 0 and
        existing_grants.format2_grant->format_2.csi_part1_bits > 0) {
      return change_format2_resource(
          pucch_slot_alloc, *existing_grants.format2_grant, crnti, ue_cell_cfg, harq_ack_bits);
    }

    // Case 1-B) If the allocated resource is for HARQ too, just update the resource.
    return update_format2_grant(
        *existing_grants.format2_grant, pucch_slot_alloc.slot, ue_cell_cfg, harq_ack_bits, sr_bits, csi_part1_bits);
  }

  // Case 2) An HARQ Format 1 is already scheduled. Update the existing HARQ grant and the SR grant, if present.
  if (existing_grants.format1_harq_grant != nullptr) {
    if (existing_grants.format1_sr_grant != nullptr) {
      srsran_sanity_check(existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits ==
                              existing_grants.format1_sr_grant->format_1.harq_ack_nof_bits,
                          "Mismatch HARQ bits mismatch between SR and HARQ grants");
    }

    // Case 2-A) If the existing grant is for at lest 2 HARQ-ACK bits, then the PUCCH needs to be converted to format 2.
    if (existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits > 1) {
      const unsigned csi_part1_nof_bits = 0;
      return convert_to_format2(pucch_slot_alloc,
                                existing_grants.format1_harq_grant,
                                existing_grants.format1_sr_grant,
                                crnti,
                                ue_cell_cfg,
                                harq_ack_bits,
                                csi_part1_nof_bits);
    }

    // Case 2-B)
    return update_existing_pucch_harq_grant(
        *existing_grants.format1_harq_grant,
        existing_grants.format1_sr_grant,
        crnti,
        pucch_slot_alloc.slot,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  }

  // Case C) If there is no existing HARQ_ACK grant, allocate a new one and update the SR grant, if present.
  return allocate_new_pucch_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg, existing_grants.format1_sr_grant);
}

void pucch_allocator_impl::pucch_allocate_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                         rnti_t                        crnti,
                                                         const ue_cell_configuration&  ue_cell_cfg)
{
  // Get the index of the PUCCH resource to be used for SR.
  // NOTEs: (i) This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  //        (ii) reserve_sr_res_available() should be a function of sr_res; however, to simplify the
  //        implementation, as assume sr_resource_cfg_list only has 1 element.
  // TODO: extend sr_resource_cfg_list to multiple resource and reserve_sr_res_available() so that it becomes a
  //       func of sr_res.

  const unsigned HARQ_BITS_WITH_NO_HARQ_REPORTING = 0;
  const unsigned CSI_PART1_BITS                   = 0;

  // Retrieve the existing PUCCH grants.
  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucch_slot_alloc.result.ul.pucchs, crnti);

  // If there is a PUCCH Format 2 grant, allocate SR request on that grant and exit.
  if (existing_grants.format2_grant != nullptr) {
    logger.debug("SR occasion for RNTI {:#x} for slot={} scheduled on existing PUCCH Format 2 grant.",
                 crnti,
                 pucch_slot_alloc.slot);
    update_format2_grant(*existing_grants.format2_grant,
                         pucch_slot_alloc.slot,
                         ue_cell_cfg,
                         HARQ_BITS_WITH_NO_HARQ_REPORTING,
                         sr_nof_bits::one,
                         CSI_PART1_BITS);
    return;
  }

  const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("SR allocation skipped for RNTI {:#x} for slot={} due to PUCCH ded. resource not available.",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("SR occasion allocation for RNTI {:#x} for slot={} skipped. CAUSE: no more PUCCH grants available.",
                   crnti,
                   pucch_slot_alloc.slot);
    return;
  }

  const unsigned nof_harq_ack_bits = existing_grants.format1_harq_grant != nullptr
                                         ? existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits
                                         : HARQ_BITS_WITH_NO_HARQ_REPORTING;

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  fill_pucch_ded_format1_grant(
      pucch_slot_alloc.result.ul.pucchs.emplace_back(), crnti, *pucch_sr_res, nof_harq_ack_bits, sr_nof_bits::one);
}

void pucch_allocator_impl::pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          unsigned                      csi_part1_nof_bits)
{
  const unsigned    HARQ_ACK_NOF_BITS = 0;
  const sr_nof_bits SR_BITS           = sr_nof_bits::no_sr;

  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti);

  // Case A) There are no existing PUCCH grants, allocate a new one for CSI.
  if (existing_grants.format1_harq_grant == nullptr and existing_grants.format1_sr_grant == nullptr and
      existing_grants.format2_grant == nullptr) {
    // Allocate new resource Format 2.
    allocate_new_format2_grant(pucch_slot_alloc, crnti, ue_cell_cfg, HARQ_ACK_NOF_BITS, SR_BITS, csi_part1_nof_bits);
  }

  // Case B) There is a PUCCH Format 2 grant.
  if (existing_grants.format2_grant != nullptr) {
    update_format2_grant(*existing_grants.format2_grant,
                         pucch_slot_alloc.slot,
                         ue_cell_cfg,
                         HARQ_ACK_NOF_BITS,
                         SR_BITS,
                         csi_part1_nof_bits);
    return;
  }
  // Case C) There are existing PUCCH Format 1 grants; convert to Format 2
  else if (existing_grants.format1_harq_grant != nullptr or existing_grants.format1_sr_grant != nullptr) {
    convert_to_format2(pucch_slot_alloc,
                       existing_grants.format1_harq_grant,
                       existing_grants.format1_sr_grant,
                       crnti,
                       ue_cell_cfg,
                       HARQ_ACK_NOF_BITS,
                       csi_part1_nof_bits);
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
      resource_manager.release_csi_resource(slot_alloc.slot, crnti, ue_cell_cfg);
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
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::one;
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
}

//////////////    Private functions       //////////////

// The function returns an available common PUCCH resource (i.e., not used by other UEs); it returns a null optional if
// no resource is available.
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
  unsigned candiadate_r_pucch = r_pucch_invalid;
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

    // If both 1st and 2nd hop grants do not collide with any UL grants, then the allocator chooses this PUCCH resource.
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
      candiadate_r_pucch                           = r_pucch;
    }
  }

  // This is the case in which the only common PUCCH resources available collides with the grid.
  if (candiadate_r_pucch < r_pucch_invalid) {
    resource_manager.reserve_common_resource(pucch_alloc.slot, candiadate_r_pucch);
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
      srsran_assert(false, "PUCCH Format must from 0 to 4, but only 0 and 1 are currently supported.");
  }
}

pucch_harq_ack_grant pucch_allocator_impl::allocate_new_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                         rnti_t                        crnti,
                                                                         const ue_cell_configuration&  ue_cell_cfg,
                                                                         pucch_info* existing_sr_grant)
{
  pucch_harq_ack_grant pucch_harq_ack_output;
  // Check if there is space for one more additional PUCCH HARQ grant.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "PUCCH HARQ-ACK grant for RNTI {:#x} for slot={} not allocated. Cause: no more PUCCH grants available.",
        crnti,
        pucch_slot_alloc.slot);
    return pucch_harq_ack_output;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_f1_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug("PUCCH HARQ-ACK allocation for RNTI {:#x} for slot={} skipped due to PUCCH ded. resources "
                 "not available.",
                 crnti,
                 pucch_slot_alloc.slot);
    return pucch_harq_ack_output;
  }

  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR.");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_format1_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  pucch_harq_ack_output.pucch_pdu           = &pucch_pdu;
  pucch_harq_ack_output.pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  return pucch_harq_ack_output;
}

pucch_harq_ack_grant pucch_allocator_impl::convert_to_format2(cell_slot_resource_allocator& pucch_slot_alloc,
                                                              pucch_info*                   existing_harq_grant,
                                                              pucch_info*                   existing_sr_grant,
                                                              rnti_t                        rnti,
                                                              const ue_cell_configuration&  ue_cell_cfg,
                                                              unsigned                      harq_ack_nof_bits,
                                                              unsigned                      csi_part1_nof_bits)
{
  srsran_assert(existing_harq_grant != nullptr or existing_sr_grant != nullptr,
                "Pointers for existing resources are null.");
  pucch_harq_ack_grant output;

  const unsigned curr_harq_bits = existing_harq_grant != nullptr ? existing_harq_grant->format_1.harq_ack_nof_bits : 0;

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Get a PUCCH Format 2 resource.
  pucch_harq_resource_alloc_record format2_res{.pucch_res = nullptr, .pucch_res_indicator = 0};
  // Case A) If for CSI report and NOT for HARQ-ACK, get the resource specific for with CSI.
  if (csi_part1_nof_bits > 0 and curr_harq_bits == 0 and harq_ack_nof_bits == 0) {
    format2_res.pucch_res = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, rnti, ue_cell_cfg);
  }
  // Case B) If there is a Format 1 present with HARQ, get the PUCCH F2 resource with the same PUCCH resource
  // indicator as for existing F1.
  else if (csi_part1_nof_bits > 0 and curr_harq_bits > 0) {
    const int f1_pucch_res_ind = resource_manager.fetch_f1_pucch_res_indic(pucch_slot_alloc.slot, rnti, pucch_cfg);
    if (f1_pucch_res_ind >= 0) {
      format2_res.pucch_res = resource_manager.reserve_specific_format2_res(
          pucch_slot_alloc.slot,
          rnti,
          static_cast<unsigned>(f1_pucch_res_ind),
          ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
      format2_res.pucch_res_indicator = static_cast<unsigned>(f1_pucch_res_ind);
    }
  }
  // Case C) In any other case, just get any available PUCCH resource 2.
  else {
    format2_res = resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);
  }

  if (format2_res.pucch_res == nullptr) {
    logger.debug(
        "No available PUCCH Format2 resources to allocate UCI for RNTI {:#x} on slot={}.", rnti, pucch_slot_alloc.slot);
    return output;
  }

  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  // Compute the number of and which UCI bits that can be reported so as not to exceed the Max Code Rate of PUCCH
  // Format 2.
  const sr_nof_bits curr_sr_bits =
      existing_sr_grant != nullptr ? existing_sr_grant->format_1.sr_bits : sr_nof_bits::no_sr;
  const pucch_uci_bits uci_bits = compute_format2_uci_bits(*format2_res.pucch_res,
                                                           max_pucch_code_rate,
                                                           curr_harq_bits + harq_ack_nof_bits,
                                                           curr_sr_bits,
                                                           csi_part1_nof_bits);

  const unsigned max_bits =
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits;
  const unsigned curr_bits =
      curr_harq_bits + harq_ack_nof_bits + sr_nof_bits_to_uint(curr_sr_bits) + csi_part1_nof_bits;
  if (max_bits < curr_bits) {
    logger.warning("PUCCH F2 max payload of {} is smaller than the required UCI bits {}. Consider increasing the PUCCH "
                   "F2 payload.",
                   max_bits,
                   curr_bits);
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs = get_pucch_format2_nof_prbs(
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
      max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-1 resources.
  remove_pucch_format1_from_grants(
      pucch_slot_alloc, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "List still full after removing PUCCH f1 grant for RNTI {:#x} for slot={}.", rnti, pucch_slot_alloc.slot);
    return output;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           uci_bits.harq_ack_nof_bits,
                           uci_bits.sr_bits,
                           uci_bits.csi_part1_bits);
  logger.debug(
      "PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for RNTI {:#x} for slot={} completed.",
      uci_bits.harq_ack_nof_bits,
      uci_bits.sr_bits,
      uci_bits.csi_part1_bits,
      rnti,
      pucch_slot_alloc.slot);
  output.pucch_pdu           = &pucch_pdu;
  output.pucch_res_indicator = static_cast<unsigned>(format2_res.pucch_res_indicator);

  return output;
}

pucch_harq_ack_grant pucch_allocator_impl::change_format2_resource(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                   pucch_info&                   existing_grant,
                                                                   rnti_t                        rnti,
                                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                                   unsigned harq_ack_bits_increment)
{
  pucch_harq_ack_grant output;

  const pucch_harq_resource_alloc_record format2_res = resource_manager.reserve_next_f2_harq_res_available(
      pucch_slot_alloc.slot, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  if (format2_res.pucch_res == nullptr) {
    remove_format2_csi_from_grants(pucch_slot_alloc, rnti, ue_cell_cfg);
    logger.debug(
        "No available PUCCH Format2 resources to allocate UCI for RNTI {:#x} on slot={}.", rnti, pucch_slot_alloc.slot);
    return output;
  }

  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  // Compute the number of and which UCI bits that can be reported so as not to exceed the Max Code Rate of PUCCH
  // Format 2.
  const pucch_uci_bits uci_bits =
      compute_format2_uci_bits(*format2_res.pucch_res,
                               max_pucch_code_rate,
                               existing_grant.format_2.harq_ack_nof_bits + harq_ack_bits_increment,
                               existing_grant.format_2.sr_bits,
                               existing_grant.format_2.csi_part1_bits);

  const unsigned max_bits =
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits;
  const unsigned curr_bits = existing_grant.format_2.harq_ack_nof_bits + harq_ack_bits_increment +
                             sr_nof_bits_to_uint(existing_grant.format_2.sr_bits) +
                             existing_grant.format_2.csi_part1_bits;
  if (max_bits < curr_bits) {
    logger.warning("PUCCH F2 max payload of {} is smaller than the required UCI bits {}. Consider increasing the PUCCH "
                   "F2 payload.",
                   max_bits,
                   curr_bits);
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs = get_pucch_format2_nof_prbs(
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
      max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-1 resource.
  remove_format2_csi_from_grants(pucch_slot_alloc, rnti, ue_cell_cfg);

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "List still full after removing PUCCH f1 grant for RNTI {:#x} for slot={}.", rnti, pucch_slot_alloc.slot);
    return output;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           uci_bits.harq_ack_nof_bits,
                           uci_bits.sr_bits,
                           uci_bits.csi_part1_bits);
  logger.debug(
      "PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for RNTI {:#x} for slot={} completed.",
      uci_bits.harq_ack_nof_bits,
      uci_bits.sr_bits,
      uci_bits.csi_part1_bits,
      rnti,
      pucch_slot_alloc.slot);
  output.pucch_pdu           = &pucch_pdu;
  output.pucch_res_indicator = static_cast<unsigned>(format2_res.pucch_res_indicator);

  return output;
}

pucch_harq_ack_grant pucch_allocator_impl::update_existing_pucch_harq_grant(pucch_info&         existing_harq_grant,
                                                                            pucch_info*         existing_sr_grant,
                                                                            rnti_t              rnti,
                                                                            slot_point          sl_tx,
                                                                            const pucch_config& pucch_cfg)
{
  pucch_harq_ack_grant output;

  const int pucch_res_idx = resource_manager.fetch_f1_pucch_res_indic(sl_tx, rnti, pucch_cfg);
  if (pucch_res_idx < 0) {
    srsran_assert(pucch_res_idx >= 0, "PUCCH resource index should not be negative.");
    return output;
  }
  srsran_sanity_check(existing_harq_grant.format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for HARQ.");
  // Update the SR, if present.
  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR.");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }
  // Update the HARQ, if present.
  existing_harq_grant.format_1.harq_ack_nof_bits++;
  output.pucch_pdu           = &existing_harq_grant;
  output.pucch_res_indicator = pucch_res_idx;

  logger.debug("ue={:#x}'s HARQ-ACK mltplxd on existing PUCCH for slot={}", rnti, sl_tx);
  return output;
}

void pucch_allocator_impl::remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                                            rnti_t                        crnti,
                                                            const pucch_config&           pucch_cfg)
{
  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove HARQ-ACK grant first.
  auto* it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::no_sr and
           pucch.format_1.harq_ack_nof_bits > 0;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    resource_manager.release_harq_f1_resource(slot_alloc.slot, crnti, pucch_cfg);
  }

  // Remove SR grant, if any.
  it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::one;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
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
    return pucch.crnti == crnti and pucch.format_2.harq_ack_nof_bits == 0 and pucch.format_2.csi_part1_bits > 0;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    resource_manager.release_csi_resource(slot_alloc.slot, crnti, ue_cell_cfg);
  }
}

pucch_harq_ack_grant pucch_allocator_impl::allocate_new_format2_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                      rnti_t                        crnti,
                                                                      const ue_cell_configuration&  ue_cell_cfg,
                                                                      unsigned                      harq_ack_bits,
                                                                      sr_nof_bits                   sr_bits,
                                                                      unsigned                      csi_part1_bits)
{
  pucch_harq_ack_grant output;

  // Check if there is space for new PUCCH grants in the scheduler.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "PUCCH Format 2 grant allocation for RNTI {:#x} for slot={} skipped. CAUSE: no more PUCCH grants available.",
        crnti,
        pucch_slot_alloc.slot);
    return output;
  }

  // Get a PUCCH Format 2 resource (if for CSI report, get the resource specific for with CSI).
  pucch_harq_resource_alloc_record format2_res{.pucch_res = nullptr, .pucch_res_indicator = 0};
  if (csi_part1_bits > 0) {
    format2_res.pucch_res = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, crnti, ue_cell_cfg);
  } else {
    format2_res = resource_manager.reserve_next_f2_harq_res_available(
        pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  }

  if (format2_res.pucch_res == nullptr) {
    logger.debug("No available PUCCH Format2 resources to allocate CSI for RNTI {:#x} on slot={}.",
                 crnti,
                 pucch_slot_alloc.slot);
    return output;
  }

  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);

  // Compute the number of and which UCI bits can be reported so as not to exceed the Max Code Rate of PUCCH Format 2.
  const pucch_uci_bits uci_bits =
      compute_format2_uci_bits(*format2_res.pucch_res, max_pucch_code_rate, harq_ack_bits, sr_bits, csi_part1_bits);

  const unsigned max_bits =
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits;
  const unsigned curr_bits = harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits;
  if (max_bits < curr_bits) {
    logger.warning("PUCCH F2 max payload of {} is smaller than the required UCI bits {}. Consider increasing the PUCCH "
                   "F2 payload.",
                   max_bits,
                   curr_bits);
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs = get_pucch_format2_nof_prbs(
      uci_bits.harq_ack_nof_bits + sr_nof_bits_to_uint(uci_bits.sr_bits) + uci_bits.csi_part1_bits,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
      variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
      max_pucch_code_rate);

  // Allocate a PUCCH PDU in the list and fill it with the parameters.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           crnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           uci_bits.harq_ack_nof_bits,
                           uci_bits.sr_bits,
                           uci_bits.csi_part1_bits);

  logger.debug(
      "PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for RNTI {:#x} for slot={} completed.",
      uci_bits.harq_ack_nof_bits,
      uci_bits.sr_bits,
      uci_bits.csi_part1_bits,
      crnti,
      pucch_slot_alloc.slot);
  output.pucch_pdu           = &pucch_pdu;
  output.pucch_res_indicator = static_cast<unsigned>(format2_res.pucch_res_indicator);

  return output;
}

pucch_harq_ack_grant pucch_allocator_impl::update_format2_grant(pucch_info&                  existing_f2_grant,
                                                                slot_point                   sl_tx,
                                                                const ue_cell_configuration& ue_cell_cfg,
                                                                unsigned                     harq_ack_bits_increment,
                                                                sr_nof_bits                  sr_bits_increment,
                                                                unsigned                     csi_part1_bits_increment)
{
  // Retrieving the PUCCH resource configuration depends on whether existing grant is for CSI.
  // If so:  (i) the resource indicator is meaningless; (ii) the PUCCH resource configuration is the CSI specific one.
  // If not: (i) the resource indicator is needed; (ii) the PUCCH resource configuration is retrieved from the resource
  //             indicator.
  const unsigned current_csi_part1_bits = existing_f2_grant.format_2.csi_part1_bits;
  const unsigned current_harq_ack_bits  = existing_f2_grant.format_2.harq_ack_nof_bits;

  // This function cannot be called if the resource of for CSI and needs to be converted into HARQ-ACK.
  srsran_sanity_check(not(current_csi_part1_bits > 0 and current_harq_ack_bits == 0 and harq_ack_bits_increment > 0),
                      "PUCCH resource CSI cannot be converted into HARQ-ACK through this function.");

  const bool use_pucch_f2_csi_res = current_csi_part1_bits > 0 and current_harq_ack_bits == 0;

  const pucch_harq_resource_alloc_record pucch_f2_harq_info =
      use_pucch_f2_csi_res ? pucch_harq_resource_alloc_record{.pucch_res = nullptr}
                           : resource_manager.fetch_allocated_f2_harq_resource(
                                 sl_tx,
                                 existing_f2_grant.crnti,
                                 ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  const pucch_resource* res_cfg =
      use_pucch_f2_csi_res ? resource_manager.fetch_csi_pucch_res_config(sl_tx, existing_f2_grant.crnti, ue_cell_cfg)
                           : pucch_f2_harq_info.pucch_res;

  if (res_cfg == nullptr) {
    srsran_assertion_failure("PUCCH resource previously allocated for UCI not found in the PUCCH resource manager.");
    return pucch_harq_ack_grant{.pucch_res_indicator = 0, .pucch_pdu = nullptr};
  }

  // Check if the number of PRBs is sufficient for the number of bits to be acked.
  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  update_format2_uci_bits(existing_f2_grant,
                          *res_cfg,
                          max_pucch_code_rate,
                          harq_ack_bits_increment,
                          sr_bits_increment,
                          csi_part1_bits_increment);

  const unsigned uci_bits = existing_f2_grant.format_2.harq_ack_nof_bits +
                            sr_nof_bits_to_uint(existing_f2_grant.format_2.sr_bits) +
                            existing_f2_grant.format_2.csi_part1_bits;
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_symbols,
                                 max_pucch_code_rate);
  // NOTE: there is no need to check if the code rate is within the limit, as the UCI bits are computed so that not to
  // exceed the code rate.

  existing_f2_grant.resources.prbs.set(res_cfg->starting_prb, res_cfg->starting_prb + nof_prbs);
  if (res_cfg->second_hop_prb.has_value()) {
    existing_f2_grant.resources.second_hop_prbs.set(res_cfg->second_hop_prb.value(),
                                                    res_cfg->second_hop_prb.value() + nof_prbs);
  }

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (existing_f2_grant.format_2.csi_part1_bits > 0) {
    existing_f2_grant.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
  }

  logger.debug("ue={:#x}'s UCI mltplxd on existing PUCCH F2 for slot={}", existing_f2_grant.crnti, sl_tx);
  // When the chosen resource is for CSI only, the resource indicator won't be used and can be set to any value.
  const unsigned res_indicator = use_pucch_f2_csi_res ? 0U : pucch_f2_harq_info.pucch_res_indicator;
  return pucch_harq_ack_grant{.pucch_res_indicator = static_cast<unsigned>(res_indicator),
                              .pucch_pdu           = &existing_f2_grant};
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
  const pucch_format_1_cfg res_f1 = variant_get<pucch_format_1_cfg>(pucch_ded_res_cfg.format_params);
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
  const pucch_format_2_3_cfg res_f2 = variant_get<pucch_format_2_3_cfg>(pucch_ded_res_cfg.format_params);
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
