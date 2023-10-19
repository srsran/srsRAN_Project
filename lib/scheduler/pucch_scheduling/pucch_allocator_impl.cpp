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
#include "../support/csi_report_helpers.h"
#include "../support/pucch/pucch_default_resource.h"
#include "../support/sr_helper.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/pucch/pucch_info.h"

//////////////     Helper functions       //////////////

using namespace srsran;

struct existing_pucch_grants {
  pucch_info* format1_sr_grant{nullptr};
  pucch_info* format1_harq_grant{nullptr};
  pucch_info* format2_grant{nullptr};
};

struct existing_pucch_grants_in_fallback {
  pucch_info* format1_sr_grant{nullptr};
  pucch_info* format1_harq_grant{nullptr};
  pucch_info* format2_harq_grant{nullptr};
  pucch_info* format2_csi_grant{nullptr};
};

// Retrieve the existing PUCCH grants for the current RNTI. If present, we expect at most 1 PUCCH F2 grant, which can
// carry HARQ-ACK, CSI and SR bits, of any combination of them.
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
      } else if (pucch.format == srsran::pucch_format::FORMAT_1) {
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

// Retrieve the existing PUCCH grants for the current RNTI during fallback mode. Note that in fallback mode, there is no
// multiplexing of HARQ-ACK grants with CSI/SR grants, therefore there can be 2 different F2 grants, one for HARQ-ACK
// and one for the CSI/SR. The HARQ-ACK grants with Format 2 are those that are scheduled before the UE enters fallback
// mode; therefore, we don't want to mix them with the CSI/SR grants. The HARQ-ACK grant will be likely ignored by
// the UE, while we need to preserve the CSI/SR grants, which are those that terminate the UE's fallback mode.
static existing_pucch_grants_in_fallback
get_existing_pucch_grants_fallback(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs, rnti_t rnti)
{
  existing_pucch_grants_in_fallback grants;
  for (auto& pucch : pucchs) {
    if (pucch.crnti == rnti) {
      // First look for Format 2 grants. There can be 1 or 2 of them.
      if (pucch.format == srsran::pucch_format::FORMAT_2) {
        srsran_assert(
            (pucch.format_2.harq_ack_nof_bits != 0 and pucch.format_2.csi_part1_bits == 0) or
                (pucch.format_2.harq_ack_nof_bits == 0 and pucch.format_2.csi_part1_bits != 0),
            "During fallback, HARQ and CSI are not expected to be multiplexed within the same PUCCH F2 grant");
        if (pucch.format_2.harq_ack_nof_bits != 0) {
          grants.format2_harq_grant = &pucch;
        } else if (pucch.format_2.csi_part1_bits != 0) {
          grants.format2_csi_grant = &pucch;
        }
      } else if (pucch.format == srsran::pucch_format::FORMAT_1) {
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

//////////////    Public functions       //////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

pucch_harq_ack_grant pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
                                                                          rnti_t                      tcrnti,
                                                                          unsigned                    k0,
                                                                          unsigned                    k1,
                                                                          const pdcch_dl_information& dci_info)
{
  // PUCCH output.
  pucch_harq_ack_grant pucch_harq_ack_output{};

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = slot_alloc[k0 + k1];

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
    logger.debug("tc-rnti={:#x}: PUCCH not allocated due to resources not available", tcrnti);
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

  logger.debug("tc-rnti={:#x}: PUCCH HARQ-ACK common with res_ind={} allocated for slot={}",
               tcrnti,
               pucch_harq_ack_output.pucch_res_indicator,
               pucch_slot_alloc.slot);

  return pucch_harq_ack_output;
}

pucch_harq_ack_grant pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                                       rnti_t                       crnti,
                                                                       const ue_cell_configuration& ue_cell_cfg,
                                                                       unsigned                     k0,
                                                                       unsigned                     k1)
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
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[k0 + k1];

  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH grants.
  const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti);

  const unsigned    harq_ack_bits_increment  = 1;
  const sr_nof_bits sr_bits_increment        = sr_nof_bits::no_sr;
  const unsigned    csi_part1_bits_increment = 0;

  // Case 1) If there is a PUCCH format 2 grant, update it.
  if (existing_grants.format2_grant != nullptr) {
    // Case 1-A) If the allocated resource is the one specific for CSI, change resource and allocate grant.
    if (existing_grants.format2_grant->format_2.harq_ack_nof_bits == 0 and
        existing_grants.format2_grant->format_2.csi_part1_bits > 0) {
      return change_format2_resource(
          pucch_slot_alloc, *existing_grants.format2_grant, crnti, ue_cell_cfg, harq_ack_bits_increment);
    }

    // Case 1-B) If the allocated resource is for HARQ too, just update the resource.
    return add_uci_bits_to_harq_f2_grant(*existing_grants.format2_grant,
                                         pucch_slot_alloc.slot,
                                         crnti,
                                         ue_cell_cfg,
                                         harq_ack_bits_increment,
                                         sr_bits_increment,
                                         csi_part1_bits_increment);
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
      return convert_to_format2_harq(
          pucch_slot_alloc, existing_grants.format1_harq_grant, crnti, ue_cell_cfg, harq_ack_bits_increment);
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
                                                         const ue_cell_configuration&  ue_cell_cfg,
                                                         bool                          is_fallback_mode)
{
  // Get the index of the PUCCH resource to be used for SR.
  // NOTEs: (i) This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  //        (ii) reserve_sr_res_available() should be a function of sr_res; however, to simplify the
  //        implementation, as assume sr_resource_cfg_list only has 1 element.
  // TODO: extend sr_resource_cfg_list to multiple resource and reserve_sr_res_available() so that it becomes a
  //       func of sr_res.

  const unsigned harq_ack_bits_increment = 0U;
  const unsigned csi_bits_increment      = 0U;

  if (is_fallback_mode) {
    // In fallback mode, we only multiplex the SR on a PUCCH F2 grant if this grant is for CSI only. This is because,
    // during multiplexing, we stop multiplexing HARQ-ACK bits along with SR or CSI on PUCCH Format 2 grants.
    // NOTEs:
    // (i) CSI and SR are still multiplexed within the same F2 grant, as CSI and SR are the grants that disable the
    // fallback mode. We don't know exactly when the user will start using the dedicated resources, therefore we keep
    // multiplexing CSI and SR even in fallback mode to guarantee the correct decoding of CSI and/or SR.
    // (ii) by disabling multiplexing HARQ-ACK during fallback mode, the scheduler could allocate 2 separate PUCCH F2
    // grants for HARQ-ACK and CSI. E.g., it can happen that HARQ-ACK grant gets allocated at slot 0
    // for k1=8 (F1), at slot 1 for k1=7 (F1) and at slot 2 for k1=6 (F2), then at slot 4 the UE enters in fallback
    // mode, and the CSI is due at slot 8. Normally, the CSI and HARQ-ACK bits should be multiplexed within the same
    // resource, but we don't do this during fallback mode. Although this is not according to the TS, the UE won't use
    // the dedicated resources for multiplexing, as it doesn't have access to dedicated configuration in fallback mode.

    // Retrieve the existing PUCCH grants.
    const existing_pucch_grants_in_fallback existing_grants =
        get_existing_pucch_grants_fallback(pucch_slot_alloc.result.ul.pucchs, crnti);

    if (existing_grants.format2_csi_grant != nullptr) {
      return add_sr_bits_to_csi_f2_grant(
          *existing_grants.format2_csi_grant, pucch_slot_alloc.slot, crnti, ue_cell_cfg, sr_nof_bits::one);
    }

    // If there are no PUCCH Format 2 grants, then we allocate the SR on a dedicated F1 grant.
    const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
        pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
    if (pucch_sr_res == nullptr) {
      logger.warning("rnti={:#x}: SR allocation skipped for slot={}. Cause: PUCCH F1 ded. resource not available",
                     crnti,
                     pucch_slot_alloc.slot);
      return;
    }

    // Allocate PUCCH SR grant only.
    if (pucch_slot_alloc.result.ul.pucchs.full()) {
      logger.warning("rnti={:#x}: SR occasion allocation for slot={} skipped. Cause: no more PUCCH grants available",
                     crnti,
                     pucch_slot_alloc.slot);
      return;
    }

    const unsigned nof_harq_ack_bits = existing_grants.format1_harq_grant != nullptr
                                           ? existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits
                                           : harq_ack_bits_increment;

    // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
    fill_pucch_ded_format1_grant(
        pucch_slot_alloc.result.ul.pucchs.emplace_back(), crnti, *pucch_sr_res, nof_harq_ack_bits, sr_nof_bits::one);
    logger.debug(
        "rnti={:#x}: SR occasion allocation for slot={} on PUCCH Format 1 completed", crnti, pucch_slot_alloc.slot);
  }
  // Non-fallback mode.
  else {
    const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucch_slot_alloc.result.ul.pucchs, crnti);

    // If there is a PUCCH Format 2 grant, allocate SR request on that grant and exit.
    if (existing_grants.format2_grant != nullptr) {
      srsran_assert(existing_grants.format2_grant->format_2.harq_ack_nof_bits != 0 or
                        existing_grants.format2_grant->format_2.csi_part1_bits != 0,
                    "This PUCCH F2 grant is expected to have at least HARQ-ACK or CSI bits to be reported");
      if (existing_grants.format2_grant->format_2.harq_ack_nof_bits != 0) {
        add_uci_bits_to_harq_f2_grant(*existing_grants.format2_grant,
                                      pucch_slot_alloc.slot,
                                      crnti,
                                      ue_cell_cfg,
                                      harq_ack_bits_increment,
                                      sr_nof_bits::one,
                                      csi_bits_increment);
        return;
      } else {
        return add_sr_bits_to_csi_f2_grant(
            *existing_grants.format2_grant, pucch_slot_alloc.slot, crnti, ue_cell_cfg, sr_nof_bits::one);
      }
    }

    const pucch_resource* pucch_sr_res = resource_manager.reserve_sr_res_available(
        pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
    if (pucch_sr_res == nullptr) {
      logger.warning(
          "rnti={:#x}: SR occasion allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
          crnti,
          pucch_slot_alloc.slot);
      return;
    }

    // Allocate PUCCH SR grant only.
    if (pucch_slot_alloc.result.ul.pucchs.full()) {
      logger.warning("rnti={:#x}: SR occasion allocation for slot={} skipped. Cause: no more PUCCH grants available in "
                     "the scheduler",
                     crnti,
                     pucch_slot_alloc.slot);
      return;
    }

    const unsigned nof_harq_ack_bits = existing_grants.format1_harq_grant != nullptr
                                           ? existing_grants.format1_harq_grant->format_1.harq_ack_nof_bits
                                           : harq_ack_bits_increment;

    // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
    fill_pucch_ded_format1_grant(
        pucch_slot_alloc.result.ul.pucchs.emplace_back(), crnti, *pucch_sr_res, nof_harq_ack_bits, sr_nof_bits::one);
    logger.debug(
        "rnti={:#x}: SR occasion allocation for slot={} on PUCCH Format 1 completed", crnti, pucch_slot_alloc.slot);
  }
}

void pucch_allocator_impl::pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          unsigned                      csi_part1_nof_bits,
                                                          bool                          is_fallback_mode)
{
  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  if (is_fallback_mode) {
    // In fallback mode, we don't multiplex HARQ-ACK bits with CSI and/or SR. This is because the HARQ-ACK reports are
    // expected to be using the common PUCCH resources and we don't support multiplexing across dedicated and common
    // resources.
    // NOTEs:
    // (i) CSI and SR are still multiplexed within the same F2 grant, as CSI and SR are the grants that disable the
    // fallback mode. We don't know exactly when the user will start using the dedicated resources, therefore we keep
    // multiplexing CSI and SR even in fallback mode to guarantee the correct decoding of CSI and/or SR.
    // (ii) by disabling multiplexing HARQ-ACK during fallback mode, the scheduler could allocate 2 separate PUCCH F2
    // grants for HARQ-ACK and CSI. E.g., it can happen that HARQ-ACK grant gets allocated at slot 0
    // for k1=8 (F1), at slot 1 for k1=7 (F1) and at slot 2 for k1=6 (F2), then at slot 4 the UE enters in fallback
    // mode, and the CSI is due at slot 8. Normally, the CSI and HARQ-ACK bits should be multiplexed within the same
    // resource, but we don't do this during fallback mode. Although this is not according to the TS, the UE won't use
    // the dedicated resources for multiplexing, as it doesn't have access to dedicated configuration in fallback mode.
    const existing_pucch_grants_in_fallback existing_grants = get_existing_pucch_grants_fallback(pucchs, crnti);

    srsran_assert(existing_grants.format2_csi_grant == nullptr,
                  "No PUCCH F2 grants for CSI are expected at this point");

    if (existing_grants.format1_sr_grant == nullptr) {
      return allocate_new_csi_grant(pucch_slot_alloc, crnti, ue_cell_cfg, csi_part1_nof_bits);
    } else {
      return convert_to_format2_csi(pucch_slot_alloc,
                                    nullptr,
                                    existing_grants.format1_sr_grant,
                                    crnti,
                                    ue_cell_cfg,
                                    csi_part1_nof_bits,
                                    is_fallback_mode);
    }
  }
  // Non-fallback mode.
  else {
    const existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti);

    // Case A) There are no existing PUCCH grants, allocate a new one for CSI.
    if (existing_grants.format1_harq_grant == nullptr and existing_grants.format1_sr_grant == nullptr and
        existing_grants.format2_grant == nullptr) {
      // Allocate new resource Format 2.
      return allocate_new_csi_grant(pucch_slot_alloc, crnti, ue_cell_cfg, csi_part1_nof_bits);
    }

    // Case B) There is a PUCCH Format 2 grant.
    if (existing_grants.format2_grant != nullptr) {
      srsran_assert(existing_grants.format2_grant->format_2.harq_ack_nof_bits != 0,
                    "This grant is expected to have HARQ-ACK bits");
      const unsigned    harq_ack_bits_increment = 0;
      const sr_nof_bits sr_bits_increment       = sr_nof_bits::no_sr;
      add_uci_bits_to_harq_f2_grant(*existing_grants.format2_grant,
                                    pucch_slot_alloc.slot,
                                    crnti,
                                    ue_cell_cfg,
                                    harq_ack_bits_increment,
                                    sr_bits_increment,
                                    csi_part1_nof_bits);
      return;
    }
    // Case C) There are existing PUCCH Format 1 grants; convert to Format 2.
    else if (existing_grants.format1_harq_grant != nullptr or existing_grants.format1_sr_grant != nullptr) {
      convert_to_format2_csi(pucch_slot_alloc,
                             existing_grants.format1_harq_grant,
                             existing_grants.format1_sr_grant,
                             crnti,
                             ue_cell_cfg,
                             csi_part1_nof_bits,
                             is_fallback_mode);
    }
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

pucch_harq_ack_grant
pucch_allocator_impl::allocate_new_format1_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                      rnti_t                        crnti,
                                                      const ue_cell_configuration&  ue_cell_cfg,
                                                      pucch_info*                   existing_sr_grant)
{
  pucch_harq_ack_grant pucch_harq_ack_output;
  // Check if there is space for one more additional PUCCH HARQ grant.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("rnti={:#x}: PUCCH HARQ-ACK grant for slot={} not allocated. Cause: no more PUCCH grants available "
                   "in the scheduler",
                   crnti,
                   pucch_slot_alloc.slot);
    return pucch_harq_ack_output;
  }

  const pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.reserve_next_f1_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.debug(
        "rnti={:#x}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F1 ded. resource not available",
        crnti,
        pucch_slot_alloc.slot);
    return pucch_harq_ack_output;
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
  pucch_harq_ack_output.pucch_pdu           = &pucch_pdu;
  pucch_harq_ack_output.pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  logger.debug("rnti={:#x}: PUCCH HARQ-ACK allocation on F1 with res_ind={} for slot={} completed",
               crnti,
               pucch_harq_ack_output.pucch_res_indicator,
               pucch_slot_alloc.slot);
  return pucch_harq_ack_output;
}

void pucch_allocator_impl::convert_to_format2_csi(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  pucch_info*                   existing_harq_grant,
                                                  pucch_info*                   existing_sr_grant,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      csi_part1_nof_bits,
                                                  bool                          is_fallback_mode)
{
  srsran_assert(existing_harq_grant != nullptr or existing_sr_grant != nullptr,
                "Pointers for existing resources are null");

  const unsigned curr_harq_bits = existing_harq_grant != nullptr ? existing_harq_grant->format_1.harq_ack_nof_bits : 0;

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Get a PUCCH Format 2 resource.
  pucch_harq_resource_alloc_record format2_res{.pucch_res = nullptr, .pucch_res_indicator = 0};
  // If there are (previously allocate) HARQ-ACK bits to be reported, when we allocate a PUCCH F2 resource with CSI
  // bits, we need to keep the previous PUCCH resource indicator (this was sent previously in a DCI to the UE). Without
  // HARQ-ACK bits, we allocate the CSI-specific PUCCH resource, which doesn't use the PUCCH resource indicator.
  if (curr_harq_bits != 0 and not is_fallback_mode) {
    const int pucch_res = resource_manager.fetch_f1_pucch_res_indic(pucch_slot_alloc.slot, rnti, pucch_cfg);
    if (pucch_res < 0) {
      logger.debug("rnti={:#x}: Previously allocated PUCCH F1 resource can't be found in the PUCCH resource manager. "
                   "CSI allocation on F2 is aborted",
                   rnti,
                   pucch_slot_alloc.slot);
      return;
    }
    format2_res.pucch_res_indicator = static_cast<unsigned>(pucch_res);
    format2_res.pucch_res           = resource_manager.reserve_specific_format2_res(
        pucch_slot_alloc.slot, rnti, format2_res.pucch_res_indicator, pucch_cfg);
  } else {
    format2_res.pucch_res           = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, rnti, ue_cell_cfg),
    format2_res.pucch_res_indicator = 0;
  }

  if (format2_res.pucch_res == nullptr) {
    logger.warning(
        "rnti={:#x}: CSI could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource not available",
        rnti,
        pucch_slot_alloc.slot);
    return;
  }

  // This function can be potentially be called before of after the SR gets allocated.
  // - If it is called after, then the \c sr_bits_to_report will be passed on the PUCCH grant for CSI. The same SR bits
  //   will be used to compute the expected payload to be carried by the PUCCH F2 CSI-specific resource.
  // - If it is called before the SR allocation, then we need to take into account the possible SR bits that will be
  //   added later on by the SR allocator. In this case, \c sr_bits_for_payload_computation will only be used to compute
  //   the expected payload to be carried by the PUCCH F2 CSI-specific resource, but will NOT be passed on to the
  //   PUCCH grant for CSI.
  const sr_nof_bits sr_bits_to_report =
      existing_sr_grant != nullptr ? existing_sr_grant->format_1.sr_bits : sr_nof_bits::no_sr;
  const sr_nof_bits sr_bits_for_payload_computation =
      sr_bits_to_report != sr_nof_bits::no_sr
          ? sr_bits_to_report
          : (sr_helper::is_sr_opportunity_slot(pucch_cfg, pucch_slot_alloc.slot) ? existing_sr_grant->format_1.sr_bits
                                                                                 : sr_nof_bits::no_sr);

  const unsigned candidate_uci_bits =
      curr_harq_bits + sr_nof_bits_to_uint(sr_bits_for_payload_computation) + csi_part1_nof_bits;

  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  srsran_assert(max_payload >= candidate_uci_bits,
                "rnti={:#x}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                rnti,
                max_payload,
                candidate_uci_bits);

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-1 resources.
  remove_pucch_format1_from_grants(pucch_slot_alloc,
                                   rnti,
                                   ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value(),
                                   is_fallback_mode);

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("rnti={:#x}: UCI could not be allocated on PUCCH Format2 for slot={}. Cause: List still full after "
                   "removing PUCCH F1 grant",
                   rnti,
                   pucch_slot_alloc.slot);
    return;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           curr_harq_bits,
                           sr_bits_to_report,
                           csi_part1_nof_bits);
  logger.debug(
      "rnti={:#x}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for for slot={} completed",
      rnti,
      curr_harq_bits,
      sr_bits_to_report,
      csi_part1_nof_bits,
      pucch_slot_alloc.slot);
}

pucch_harq_ack_grant pucch_allocator_impl::convert_to_format2_harq(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                   pucch_info*                   existing_harq_grant,
                                                                   rnti_t                        rnti,
                                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                                   unsigned int harq_ack_bits_increment)
{
  srsran_assert(existing_harq_grant != nullptr, "Pointers for existing resources are null");

  const unsigned curr_harq_bits = existing_harq_grant != nullptr ? existing_harq_grant->format_1.harq_ack_nof_bits : 0;

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Get a PUCCH Format 2 resource.
  const pucch_harq_resource_alloc_record format2_res =
      resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);

  if (format2_res.pucch_res == nullptr) {
    logger.debug("rnti={:#x}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource "
                 "not available",
                 rnti,
                 pucch_slot_alloc.slot);
    return {};
  }

  // For SR bits, check if the candidate slot is a SR opportunity and, in that case, consider 1 bit for SR.
  const sr_nof_bits sr_bits_for_payload_computation =
      sr_helper::is_sr_opportunity_slot(pucch_cfg, pucch_slot_alloc.slot) ? sr_nof_bits::one : sr_nof_bits::no_sr;

  unsigned csi1_bits_for_payload_computation = 0U;
  if (csi_helper::is_csi_reporting_slot(ue_cell_cfg.cfg_dedicated(), pucch_slot_alloc.slot)) {
    const auto csi_report_cfg         = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
    const auto csi_report_size        = get_csi_report_pucch_size(csi_report_cfg);
    csi1_bits_for_payload_computation = csi_report_size.value();
  }

  const unsigned candidate_uci_bits = curr_harq_bits + harq_ack_bits_increment +
                                      sr_nof_bits_to_uint(sr_bits_for_payload_computation) +
                                      csi1_bits_for_payload_computation;

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
    logger.debug(
        "rnti={:#x}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is insufficient for "
        "{} candidate UCI bits",
        rnti,
        pucch_slot_alloc.slot,
        max_payload,
        candidate_uci_bits);
    resource_manager.release_harq_f2_resource(pucch_slot_alloc.slot, rnti, pucch_cfg);
    return {};
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-1 resources.
  const bool is_fallback_mode = false;
  remove_pucch_format1_from_grants(pucch_slot_alloc,
                                   rnti,
                                   ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value(),
                                   is_fallback_mode);

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("rnti={:#x}: UCI could not be allocated on PUCCH Format2 for slot={}. Cause: List still full after "
                   "removing PUCCH F1 grant",
                   rnti,
                   pucch_slot_alloc.slot);
    return {};
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
                           sr_nof_bits::no_sr,
                           csi1_nof_bits_only_harq);

  logger.debug("rnti={:#x}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits with res_ind={} for "
               "slot={} completed",
               rnti,
               curr_harq_bits + harq_ack_bits_increment,
               sr_nof_bits::no_sr,
               csi1_nof_bits_only_harq,
               format2_res.pucch_res_indicator,
               pucch_slot_alloc.slot

  );

  return pucch_harq_ack_grant{.pucch_res_indicator = format2_res.pucch_res_indicator, .pucch_pdu = &pucch_pdu};
}

pucch_harq_ack_grant pucch_allocator_impl::change_format2_resource(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                   pucch_info&                   existing_grant,
                                                                   rnti_t                        rnti,
                                                                   const ue_cell_configuration&  ue_cell_cfg,
                                                                   unsigned harq_ack_bits_increment)
{
  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const pucch_harq_resource_alloc_record format2_res =
      resource_manager.reserve_next_f2_harq_res_available(pucch_slot_alloc.slot, rnti, pucch_cfg);

  if (format2_res.pucch_res == nullptr) {
    logger.debug("rnti={:#x}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource "
                 "not available",
                 rnti,
                 pucch_slot_alloc.slot);
    return {};
  }

  // This function would only be called in case CSI and SR gets allocated before the HARQ. In that case, if there are SR
  // bits or CSI bits to be carried by the PUCCH F2 grant, they would have already been allocated and there is no need
  // to check if the slot is an CSI or SR opportunity.
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
    logger.debug(
        "rnti={:#x}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is insufficient for "
        "{} candidate UCI bits",
        rnti,
        pucch_slot_alloc.slot,
        max_payload,
        candidate_uci_bits);
    // The allocation will be aborted, we need to deallocate the resource that was reserved at the beginning of the
    // function.
    resource_manager.release_harq_f2_resource(pucch_slot_alloc.slot, rnti, pucch_cfg);
    return {};
  }

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(format2_res.pucch_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Remove the previously allocated PUCCH format-2 resource.
  remove_format2_csi_from_grants(pucch_slot_alloc, rnti, ue_cell_cfg);

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "rnti={:#x}: HARQ-ACK could not be allocated on PUCCH Format2 for slot={}. Cause: List still full after "
        "removing PUCCH F2 CSI grant",
        rnti,
        pucch_slot_alloc.slot);
    return {};
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_format2_grant(pucch_pdu,
                           rnti,
                           *format2_res.pucch_res,
                           ue_cell_cfg,
                           nof_prbs,
                           harq_ack_bits_increment,
                           sr_bits_to_report,
                           csi_bits_to_report);
  logger.debug("rnti={:#x}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for slot={} completed",
               rnti,
               harq_ack_bits_increment,
               sr_bits_to_report,
               csi_bits_to_report,
               pucch_slot_alloc.slot);

  return pucch_harq_ack_grant{.pucch_res_indicator = static_cast<unsigned>(format2_res.pucch_res_indicator),
                              .pucch_pdu           = &pucch_pdu};
}

pucch_harq_ack_grant pucch_allocator_impl::add_harq_ack_bit_to_format1_grant(pucch_info&         existing_harq_grant,
                                                                             pucch_info*         existing_sr_grant,
                                                                             rnti_t              rnti,
                                                                             slot_point          sl_tx,
                                                                             const pucch_config& pucch_cfg)
{
  pucch_harq_ack_grant output;

  const int pucch_res_idx = resource_manager.fetch_f1_pucch_res_indic(sl_tx, rnti, pucch_cfg);
  if (pucch_res_idx < 0) {
    srsran_assert(pucch_res_idx >= 0, "PUCCH resource index should not be negative");
    return output;
  }
  srsran_sanity_check(existing_harq_grant.format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for HARQ");
  // Update the SR, if present.
  if (existing_sr_grant != nullptr) {
    srsran_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }
  // Update the HARQ, if present.
  existing_harq_grant.format_1.harq_ack_nof_bits++;
  output.pucch_pdu           = &existing_harq_grant;
  output.pucch_res_indicator = pucch_res_idx;

  logger.debug("ue={:#x}: HARQ-ACK mltplxd on existing PUCCH F1 with res_ind={} for slot={}",
               rnti,
               output.pucch_res_indicator,
               sl_tx);
  return output;
}

void pucch_allocator_impl::remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                                            rnti_t                        crnti,
                                                            const pucch_config&           pucch_cfg,
                                                            bool                          is_fallback_mode)
{
  auto& pucchs = slot_alloc.result.ul.pucchs;

  // During fallback mode, the HARQ-ACK grants are not multiplexed with the CSI/SR in a PUCCH Format 2; thus, there is
  // no need to remove the F1 HARQ-ACK grants.
  if (not is_fallback_mode) {
    // Remove HARQ-ACK grant first.
    auto* it_harq = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
      return pucch.crnti == crnti and pucch.format == pucch_format::FORMAT_1 and
             pucch.format_1.sr_bits == sr_nof_bits::no_sr and pucch.format_1.harq_ack_nof_bits > 0;
    });

    if (it_harq != pucchs.end()) {
      pucchs.erase(it_harq);
      resource_manager.release_harq_f1_resource(slot_alloc.slot, crnti, pucch_cfg);
    }
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

  // Check if there is space for new PUCCH grants in the scheduler.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "rnti={:#x}: PUCCH Format 2 grant allocation for slot={} skipped. Cause: scheduler PUCCH grant list is full",
        crnti,
        pucch_slot_alloc.slot);
    return;
  }

  // Get the F2 resource specific for with CSI.
  const pucch_resource* csi_f2_res = resource_manager.reserve_csi_resource(pucch_slot_alloc.slot, crnti, ue_cell_cfg);

  if (csi_f2_res == nullptr) {
    logger.warning(
        "rnti={:#x}: CSI could not be allocated on PUCCH Format2 for slot={}. Cause: PUCCH F2 resource not available",
        crnti,
        pucch_slot_alloc.slot);
    return;
  }

  // This is only to ensure the CSI + any possible SR bits fit into the PUCCH F2 resource. These SR bits will not be
  // passed to the CSI grant, but will only be used to ensure the PUCCH F2 resource can carry both CSI (which will be
  // allocated within this function) and the SR (which will be allocated later on by the SR allocator function).
  const sr_nof_bits sr_bits_for_payload_computation =
      sr_helper::is_sr_opportunity_slot(ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value(),
                                        pucch_slot_alloc.slot)
          ? sr_nof_bits::one
          : sr_nof_bits::no_sr;

  const unsigned candidate_uci_bits = sr_nof_bits_to_uint(sr_bits_for_payload_computation) + csi_part1_bits;

  const float max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);

  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_symbols,
                                    max_pucch_code_rate);

  // It's the validator that should make sure the SR + CSI bits fit into a PUCCH Format 2 resource.
  srsran_assert(max_payload >= candidate_uci_bits,
                "rnti={:#x}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                crnti,
                max_payload,
                candidate_uci_bits);

  // Compute the number of PRBs required for the uci bits computed above.
  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(csi_f2_res->format_params).nof_symbols,
                                 max_pucch_code_rate);

  // Allocate a PUCCH PDU in the list and fill it with the parameters.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  // Neither HARQ-ACK bits
  const unsigned    harq_ack_bits_only_csi = 0U;
  const sr_nof_bits sr_bits_only_csi       = sr_nof_bits::no_sr;
  fill_pucch_format2_grant(
      pucch_pdu, crnti, *csi_f2_res, ue_cell_cfg, nof_prbs, harq_ack_bits_only_csi, sr_bits_only_csi, csi_part1_bits);

  logger.debug(
      "rnti={:#x}: PUCCH Format 2 grant allocation with {} H-ACK, {} SR, {} CSI bits for for slot={} completed",
      crnti,
      harq_ack_bits_only_csi,
      sr_bits_only_csi,
      csi_part1_bits,
      pucch_slot_alloc.slot);
}

void pucch_allocator_impl::add_sr_bits_to_csi_f2_grant(pucch_info&                  existing_f2_grant,
                                                       slot_point                   sl_tx,
                                                       rnti_t                       crnti,
                                                       const ue_cell_configuration& ue_cell_cfg,
                                                       sr_nof_bits                  sr_bits_increment)
{
  const unsigned        current_csi_part1_bits = existing_f2_grant.format_2.csi_part1_bits;
  const pucch_resource* res_cfg =
      resource_manager.fetch_csi_pucch_res_config(sl_tx, existing_f2_grant.crnti, ue_cell_cfg);

  if (res_cfg == nullptr) {
    srsran_assertion_failure("PUCCH resource previously allocated for UCI not found in the PUCCH resource manager");
    return;
  }

  const unsigned candidate_uci_bits = sr_nof_bits_to_uint(sr_bits_increment) + current_csi_part1_bits;

  // Check if the number of PRBs is sufficient for the number of bits to be acked.
  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload =
      get_pucch_format2_max_payload(variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_prbs,
                                    variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_symbols,
                                    max_pucch_code_rate);

  if (max_payload < candidate_uci_bits) {
    // This should not occur, as it should be the validator and the previous scheduler allocations that prevented us
    // from hitting this point.
    srsran_assertion_failure("rnti={:#x}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                             crnti,
                             max_payload,
                             candidate_uci_bits);
  }

  const unsigned nof_prbs =
      get_pucch_format2_nof_prbs(candidate_uci_bits,
                                 variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_prbs,
                                 variant_get<pucch_format_2_3_cfg>(res_cfg->format_params).nof_symbols,
                                 max_pucch_code_rate);

  existing_f2_grant.resources.prbs.set(res_cfg->starting_prb, res_cfg->starting_prb + nof_prbs);
  if (res_cfg->second_hop_prb.has_value()) {
    existing_f2_grant.resources.second_hop_prbs.set(res_cfg->second_hop_prb.value(),
                                                    res_cfg->second_hop_prb.value() + nof_prbs);
  }

  existing_f2_grant.format_2.sr_bits = sr_bits_increment;

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (existing_f2_grant.format_2.csi_part1_bits > 0) {
    existing_f2_grant.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
  }

  logger.debug("rnti={:#x}: SR occasion multiplexed on existing PUCCH F2 for slot={}", existing_f2_grant.crnti, sl_tx);
}

pucch_harq_ack_grant pucch_allocator_impl::add_uci_bits_to_harq_f2_grant(pucch_info&                  existing_f2_grant,
                                                                         slot_point                   sl_tx,
                                                                         rnti_t                       crnti,
                                                                         const ue_cell_configuration& ue_cell_cfg,
                                                                         unsigned    harq_ack_bits_increment,
                                                                         sr_nof_bits sr_bits_increment,
                                                                         unsigned    csi_part1_bits_increment)
{
  const unsigned current_csi_part1_bits = existing_f2_grant.format_2.csi_part1_bits;
  const unsigned current_harq_ack_bits  = existing_f2_grant.format_2.harq_ack_nof_bits;

  // This function cannot be called if the resource of for CSI and needs to be converted into HARQ-ACK.
  srsran_sanity_check(current_harq_ack_bits != 0, "This PUCCH grant is expected to have HARQ-ACK bits to report");

  // This boolean expression is used to check that the function is called to increment only one of the 3 UCI components
  // at once.
  const bool only_one_increment =
      (harq_ack_bits_increment != 0 and sr_bits_increment == sr_nof_bits::no_sr and csi_part1_bits_increment == 0) or
      (harq_ack_bits_increment == 0 and ((sr_bits_increment != sr_nof_bits::no_sr and csi_part1_bits_increment == 0) or
                                         (sr_bits_increment == sr_nof_bits::no_sr and csi_part1_bits_increment != 0)));

  // This function cannot be called if the resource of for CSI and needs to be converted into HARQ-ACK.
  srsran_sanity_check(only_one_increment, "This funcation can only increment one of the 3 UCI components at once");

  srsran_sanity_check(not(current_csi_part1_bits != 0 and csi_part1_bits_increment != 0),
                      "This function cannot be called to allocate CSI on a grant that already has CSI bits");

  const pucch_config& pucch_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value();
  const pucch_harq_resource_alloc_record pucch_f2_harq_cfg =
      resource_manager.fetch_allocated_f2_harq_resource(sl_tx, existing_f2_grant.crnti, pucch_cfg);

  if (pucch_f2_harq_cfg.pucch_res == nullptr) {
    srsran_assertion_failure(
        "PUCCH F2 resource previously allocated for HARQ-ACK not found in the PUCCH resource manager");
    return {};
  }

  // This function can be potentially be called before, during of after the SR gets allocated.
  // - If it is called during or after; this is the case in which there is are bits to be reported in the existing grant
  //   or if an incremental SR bit is passed to the input. The \c sr_bits_to_report will be passed on the PUCCH
  //   grant for CSI. The same SR bits will be used to compute the expected payload to be carried by the PUCCH F2
  //   CSI-specific resource. In this case we'll ignore future possible SR opportunities for the payload computation, as
  //   it is implicit all SR bits have been already consider in the \c sr_bits_to_report.
  // - If it is called before the SR allocation, then we need to take into account the possible SR bits that will be
  //   added later on by the SR allocator. In this case, \c sr_bits_for_payload_computation will only be used to compute
  //   the expected payload to be carried by the PUCCH F2 CSI-specific resource, but will NOT be passed on to the
  //   PUCCH grant for CSI.
  const sr_nof_bits sr_bits_to_report        = existing_f2_grant.format_2.sr_bits + sr_bits_increment;
  const sr_nof_bits sr_bits_for_single_grant = sr_nof_bits::one;
  const sr_nof_bits sr_bits_for_payload_computation =
      sr_bits_to_report != sr_nof_bits::no_sr
          ? sr_bits_to_report
          : (sr_helper::is_sr_opportunity_slot(pucch_cfg, sl_tx) ? sr_bits_for_single_grant : sr_nof_bits::no_sr);

  // This function can be potentially be called before, during of after the CSI gets allocated.
  // - If it is called during or after; this is the case in which there are bits to be reported in the existing grant
  //   or if an incremental CSI bit is passed to the input. The \c csi_bits_to_report will be passed on the PUCCH
  //   grant for CSI. The same CSI bits will be used to compute the expected payload to be carried by the PUCCH F2
  //   CSI-specific resource. In this case we'll ignore future possible CSI opportunities for the payload computation,
  //   as it is implicit that all CSI bits have been already consider in the \c sr_bits_to_report.
  // - If it is called before the CSI allocation, then we need to take into account the possible CSI bits that will be
  //   added later on by the SR allocator. In this case, \c csi1_bits_for_payload_computation will only be used to
  //   compute the expected payload to be carried by the PUCCH F2 resource, but will NOT be passed on to the PUCCH
  //   grant for CSI (as the CSI allocator will take care of that).
  const unsigned csi_bits_to_report                = existing_f2_grant.format_2.csi_part1_bits != 0
                                                         ? existing_f2_grant.format_2.csi_part1_bits
                                                         : csi_part1_bits_increment;
  unsigned       csi1_bits_for_payload_computation = csi_bits_to_report;
  if (csi1_bits_for_payload_computation == 0 and
      csi_helper::is_csi_reporting_slot(ue_cell_cfg.cfg_dedicated(), sl_tx)) {
    const auto csi_report_cfg         = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
    const auto csi_report_size        = get_csi_report_pucch_size(csi_report_cfg);
    csi1_bits_for_payload_computation = csi_report_size.value();
  }

  const unsigned candidate_uci_bits = current_harq_ack_bits + harq_ack_bits_increment +
                                      sr_nof_bits_to_uint(sr_bits_for_payload_computation) +
                                      csi1_bits_for_payload_computation;

  // Check if the number of PRBs is sufficient for the number of bits to be acked.
  const float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                               .ul_config.value()
                                                               .init_ul_bwp.pucch_cfg.value()
                                                               .format_2_common_param.value()
                                                               .max_c_rate);
  const unsigned max_payload         = get_pucch_format2_max_payload(
      variant_get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params).nof_prbs,
      variant_get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params).nof_symbols,
      max_pucch_code_rate);

  if (max_payload < candidate_uci_bits) {
    if (harq_ack_bits_increment != 0) {
      logger.debug("rnti={:#x}: PUCCH HARQ-ACK allocation for slot={} skipped. Cause: PUCCH F2 max payload {} is "
                   "insufficient for {} candidate UCI bits",
                   crnti,
                   sl_tx,
                   max_payload,
                   candidate_uci_bits);

      // No need to release the resource, as it was previously allocated for other HARQ processes.
      return {};
    } else {
      // This should not occur, as it should be the validator and the previous scheduler allocations that prevented us
      // from hitting this point.
      srsran_assertion_failure("rnti={:#x}: PUCCH F2 max payload {} is insufficient for {} candidate UCI bits",
                               crnti,
                               max_payload,
                               candidate_uci_bits);
    }
  }

  const unsigned nof_prbs = get_pucch_format2_nof_prbs(
      candidate_uci_bits,
      variant_get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params).nof_prbs,
      variant_get<pucch_format_2_3_cfg>(pucch_f2_harq_cfg.pucch_res->format_params).nof_symbols,
      max_pucch_code_rate);
  // NOTE: there is no need to check if the code rate is within the limit, as the UCI bits are computed so that not to
  // exceed the code rate.
  existing_f2_grant.resources.prbs.set(pucch_f2_harq_cfg.pucch_res->starting_prb,
                                       pucch_f2_harq_cfg.pucch_res->starting_prb + nof_prbs);
  if (pucch_f2_harq_cfg.pucch_res->second_hop_prb.has_value()) {
    existing_f2_grant.resources.second_hop_prbs.set(pucch_f2_harq_cfg.pucch_res->second_hop_prb.value(),
                                                    pucch_f2_harq_cfg.pucch_res->second_hop_prb.value() + nof_prbs);
  }

  // Generate CSI report configuration if there are CSI bits in UCI.
  if (existing_f2_grant.format_2.csi_part1_bits > 0) {
    existing_f2_grant.csi_rep_cfg = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
  }

  existing_f2_grant.format_2.harq_ack_nof_bits += harq_ack_bits_increment;
  existing_f2_grant.format_2.sr_bits = existing_f2_grant.format_2.sr_bits + sr_bits_increment;
  existing_f2_grant.format_2.csi_part1_bits += csi_part1_bits_increment;

  if (harq_ack_bits_increment != 0) {
    logger.debug("rnti={:#x}: HARQ-ACK multiplexed on existing PUCCH F2 with res_ind={} for slot={}",
                 existing_f2_grant.crnti,
                 pucch_f2_harq_cfg.pucch_res_indicator,
                 sl_tx);
  } else if (sr_bits_increment != sr_nof_bits::no_sr) {
    logger.debug("rnti={:#x}: SR multiplexed on existing PUCCH F2 with res_ind={} for slot={}",
                 existing_f2_grant.crnti,
                 pucch_f2_harq_cfg.pucch_res_indicator,
                 sl_tx);
  } else {
    logger.debug("rnti={:#x}: CSI multiplexed on existing PUCCH F2 with res_ind={} for slot={}",
                 existing_f2_grant.crnti,
                 pucch_f2_harq_cfg.pucch_res_indicator,
                 sl_tx);
  }

  return pucch_harq_ack_grant{.pucch_res_indicator = pucch_f2_harq_cfg.pucch_res_indicator,
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
