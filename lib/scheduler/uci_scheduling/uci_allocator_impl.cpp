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

#include "uci_allocator_impl.h"
#include "../support/csi_report_helpers.h"
#include "../support/pucch/pucch_default_resource.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_unpacking.h"

using namespace srsran;

/// Number of possible Downlink Assignment Indexes {0, ..., 3} as per TS38.213 Section 9.1.3.
constexpr static size_t DAI_MOD = 4;

////////////    C-tors and d-tors    ////////////

uci_allocator_impl::uci_allocator_impl(pucch_allocator& pucch_alloc_) :
  pucch_alloc{pucch_alloc_}, logger(srslog::fetch_basic_logger("SCHED"))
{
}

uci_allocator_impl::~uci_allocator_impl() = default;

////////////    Static functions    ////////////

static void
fill_uci_on_pusch(uci_info& uci, const uci_on_pusch& uci_cfg, unsigned harq_ack_nof_bits, unsigned csi_part1_nof_bits)
{
  // [Implementation-defined] CSI Part 2 reporting not supported.
  const unsigned CSI_PART2_NOF_BITS = 0;

  uci.harq_ack_nof_bits  = harq_ack_nof_bits;
  uci.csi_part1_nof_bits = csi_part1_nof_bits;
  uci.csi_part2_nof_bits = CSI_PART2_NOF_BITS;

  // Set parameters of UCI-HARQ.
  uci.alpha = uci_cfg.scaling;

  // We assume the configuration contains the values for beta_offsets.
  const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

  // The values of \c beta_offsets are set according to Section 9.3, TS38.213.
  if (uci.harq_ack_nof_bits <= 2) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_1.value();
  }
  if (uci.harq_ack_nof_bits > 2 and uci.harq_ack_nof_bits <= 11) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_2.value();
  } else {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_3.value();
  }

  // The values of \c beta_offsets are set according to Section 9.3, TS38.213.
  if (uci.csi_part1_nof_bits <= 11) {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value();
  } else {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value();
  }

  // When this allocation is done, there is no CSI Part 2 to be reported. Therefore, as per Section 9.3, TS38.213, the
  // \c beta_offsets values to be used are those with index 1.
  uci.beta_offset_csi_2 = beta_offsets.beta_offset_csi_p2_idx_1.value();
}

////////////    Private functions    ////////////

uci_allocator_impl::slot_alloc_list::ue_uci* uci_allocator_impl::get_uci_alloc(slot_point uci_slot, rnti_t rnti)
{
  auto& ucis = uci_alloc_grid[uci_slot.to_uint()].ucis;
  auto  it   = std::find_if(ucis.begin(), ucis.end(), [rnti](const auto& uci) { return uci.rnti == rnti; });
  return it != ucis.end() ? &*it : nullptr;
}

unsigned uci_allocator_impl::get_min_pdsch_to_ack_slot_distance(slot_point pdsch_slot,
                                                                rnti_t     rnti,
                                                                unsigned   min_k1,
                                                                unsigned   max_k1)
{
  srsran_assert(min_k1 <= max_k1, "Minimum k1 value must be greater than maximum k1 value");
  for (int sl_inc = max_k1; sl_inc >= (int)min_k1; --sl_inc) {
    const slot_point uci_slot = pdsch_slot + sl_inc;
    if (get_uci_alloc(uci_slot, rnti) != nullptr) {
      return sl_inc;
    }
  }
  return min_k1;
}

////////////    Public functions    ////////////

void uci_allocator_impl::slot_indication(slot_point sl_tx)
{
  // Clear previous slot UCI allocations.
  uci_alloc_grid[(sl_tx - 1).to_uint()].ucis.clear();
}

uci_allocation uci_allocator_impl::alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                                     rnti_t                       crnti,
                                                     const ue_cell_configuration& ue_cell_cfg,
                                                     unsigned                     pdsch_time_domain_resource,
                                                     span<const uint8_t>          k1_list)
{
  // [Implementation-defined] We restrict the number of HARQ bits per PUCCH that are expected to carry CSI reporting to
  // 2 , until the PUCCH allocator supports more than this.
  static const uint8_t max_harq_bits_per_uci = 2U;

  const slot_point          pdsch_slot = res_alloc[pdsch_time_domain_resource].slot;
  const cell_configuration& cell_cfg   = ue_cell_cfg.cell_cfg_common;
  const unsigned            min_pdsch_to_ack_slot_distance =
      get_min_pdsch_to_ack_slot_distance(pdsch_slot,
                                         crnti,
                                         *std::min_element(k1_list.begin(), k1_list.end()),
                                         *std::max_element(k1_list.begin(), k1_list.end()));

  for (const uint8_t k1_candidate : k1_list) {
    // Step 1: Check for validity of the UCI slot and other restrictions.

    // Check whether the UCI slot to be scheduled is >= last UCI HARQ ACK allocated slot for the UE.
    // NOTE: The reason for having this logic is due to fact that COTS UE sends an invalid ACK bits if newly scheduled
    // PDSCH has its UCI HARQ ACK slot before the (in time) UCI HARQ ACK slot of the previously scheduled PDSCH.
    if (k1_candidate < min_pdsch_to_ack_slot_distance) {
      continue;
    }

    // Retrieve the scheduling results for slot = k0 + k1;
    cell_slot_resource_allocator& slot_alloc = res_alloc[pdsch_time_domain_resource + k1_candidate];
    const slot_point              uci_slot   = slot_alloc.slot;

    // Check whether UCI slot is UL enabled.
    if (not cell_cfg.is_fully_ul_enabled(uci_slot)) {
      continue;
    }

    if (csi_helper::is_csi_reporting_slot(ue_cell_cfg.cfg_dedicated(), uci_slot)) {
      // NOTE: For TX with more than 1 antenna, the reported CSI is 7 bit, so we avoid multiplexing HARQ-ACK with CSI in
      // the slots for CSI.
      if (cell_cfg.dl_carrier.nof_ant > 1U) {
        continue;
      }
      // NOTE: This is only to avoid allocating more than 2 HARQ bits in PUCCH that are expected to carry CSI reporting.
      // TODO: Remove this when the PUCCH allocator handle properly more than 2 HARQ-ACK bits + CSI.
      if (get_scheduled_pdsch_counter_in_ue_uci(slot_alloc, crnti) >= max_harq_bits_per_uci) {
        continue;
      }
    }

    // Step 2: Try to allocate UCI HARQ ACK for UE.
    uci_allocation uci_output;

    // Get existing PUSCH grant, if any, for a given UE's RNTI.
    auto&          puschs         = slot_alloc.result.ul.puschs;
    ul_sched_info* existing_pusch = std::find_if(
        puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

    const bool has_pusch_grants =
        not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

    // Allocate UCI on PUSCH if any PUSCH grants already exist for the UE; else, allocate UCI on PUCCH.
    if (has_pusch_grants) {
      // We assume we only report the HARQ-ACK for a single layer, i.e., 1 bit (plus additional previous bits).
      unsigned nof_harq_ack_bits = 1;
      unsigned nof_csi_part1_bits{0};
      // If we reach this point, we expect no allocated PUCCH grants.
      if (not existing_pusch->uci.has_value()) {
        existing_pusch->uci.emplace();
      } else {
        nof_harq_ack_bits += existing_pusch->uci.value().harq_ack_nof_bits;
        nof_csi_part1_bits += existing_pusch->uci.value().csi_part1_nof_bits;
      }
      fill_uci_on_pusch(existing_pusch->uci.value(),
                        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
                        nof_harq_ack_bits,
                        nof_csi_part1_bits);
      uci_output.alloc_successful = true;
      logger.debug("UCI for HARQ-ACK allocated on PUSCH, for ue={:#x}", crnti);
    } else {
      uci_output.pucch_grant = pucch_alloc.alloc_ded_pucch_harq_ack_ue(
          res_alloc, crnti, ue_cell_cfg, pdsch_time_domain_resource, k1_candidate);
      uci_output.alloc_successful = uci_output.pucch_grant.pucch_pdu != nullptr;
    }

    // Register new UCI allocation in the respective grid slot entry and derive DAI.
    if (uci_output.alloc_successful) {
      auto* uci = get_uci_alloc(slot_alloc.slot, crnti);
      if (uci == nullptr) {
        uci                             = &uci_alloc_grid[slot_alloc.slot.to_uint()].ucis.emplace_back();
        uci->rnti                       = crnti;
        uci->scheduled_dl_pdcch_counter = 0;
      }
      uci_output.dai = uci->scheduled_dl_pdcch_counter % DAI_MOD;
      uci->scheduled_dl_pdcch_counter++;
      uci_output.k1 = k1_candidate;

      return uci_output;
    }
  }
  return {};
}

void uci_allocator_impl::multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                                                cell_slot_resource_allocator& slot_alloc,
                                                const ue_cell_configuration&  ue_cell_cfg,
                                                rnti_t                        crnti)
{
  // Move the bits that are carried by the PUCCH into the PUSCH.
  const pucch_uci_bits pucch_uci = pucch_alloc.remove_ue_uci_from_pucch(slot_alloc, crnti, ue_cell_cfg);

  // In case there are no UCI bits from PUCCH, then there is no UCI to be multiplexed on the PUSCH.
  if (pucch_uci.harq_ack_nof_bits == 0 and pucch_uci.csi_part1_bits == 0) {
    return;
  }

  // We assume that at this point, there are no existing UCI grants in the PUSCH; allocate a new one.
  uci_info& uci = pusch_grant.uci.emplace();
  fill_uci_on_pusch(uci,
                    ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
                    pucch_uci.harq_ack_nof_bits,
                    pucch_uci.csi_part1_bits);

  logger.debug("UCI for ue={:#x} mltplxd on PUSCH for slot={}", crnti, slot_alloc.slot);
}

void uci_allocator_impl::uci_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                     rnti_t                        crnti,
                                                     const ue_cell_configuration&  ue_cell_cfg)
{
  // Retrieve the scheduling results for slot = k0 + k1;
  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  const bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // If there is a PUSCH allocated for this UE, do not allocate any PUCCH SR grants.
  if (has_pusch_grants) {
    logger.debug("SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", crnti);
    return;
  }

  pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, crnti, ue_cell_cfg);
}

void uci_allocator_impl::uci_allocate_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                      rnti_t                        crnti,
                                                      const ue_cell_configuration&  ue_cell_cfg)
{
  const auto csi_report_cfg  = create_csi_report_configuration(*ue_cell_cfg.cfg_dedicated().csi_meas_cfg);
  const auto csi_report_size = get_csi_report_pucch_size(csi_report_cfg);

  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  const bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // If there is a PUSCH allocated for this UE, allocate the CSI on the UCI on PUSCH.
  if (has_pusch_grants) {
    // No HARQ bits to be reported when the UCI is initiated by the CSI.
    unsigned nof_harq_ack_bits  = 0;
    unsigned nof_csi_part1_bits = csi_report_size.value();
    // If there is UCI grant allocated, allocate it.
    if (not existing_pusch->uci.has_value()) {
      existing_pusch->uci.emplace();
    }
    // Else, update the number of bits to be reported.
    else {
      nof_harq_ack_bits += existing_pusch->uci.value().harq_ack_nof_bits;
      nof_csi_part1_bits += existing_pusch->uci.value().csi_part1_nof_bits;
    }

    fill_uci_on_pusch(existing_pusch->uci.value(),
                      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
                      nof_harq_ack_bits,
                      nof_csi_part1_bits);
    logger.debug("UCI with {} H-ACK, {} CSI-p1, {} CSI-p2 bits for RNTI {:#x} for slot={} allocated on PUSCH.",
                 existing_pusch->uci.value().harq_ack_nof_bits,
                 existing_pusch->uci.value().csi_part1_nof_bits,
                 existing_pusch->uci.value().csi_part2_nof_bits,
                 crnti,
                 slot_alloc.slot);
    return;
  }

  // Else, allocate the CSI on the PUCCH.
  pucch_alloc.pucch_allocate_csi_opportunity(slot_alloc, crnti, ue_cell_cfg, csi_report_size.value());
}

uint8_t uci_allocator_impl::get_scheduled_pdsch_counter_in_ue_uci(cell_slot_resource_allocator& slot_alloc,
                                                                  rnti_t                        crnti)
{
  auto* uci = get_uci_alloc(slot_alloc.slot, crnti);
  if (uci == nullptr) {
    return 0;
  }
  return uci->scheduled_dl_pdcch_counter;
}
