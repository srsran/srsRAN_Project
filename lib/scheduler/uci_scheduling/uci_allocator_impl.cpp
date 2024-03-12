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

#include "uci_allocator_impl.h"
#include "../support/csi_report_helpers.h"
#include "../support/pucch/pucch_default_resource.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "srsran/ran/csi_report/csi_report_pusch_size.h"

using namespace srsran;

////////////    C-tors and d-tors    ////////////

uci_allocator_impl::uci_allocator_impl(pucch_allocator& pucch_alloc_) :
  pucch_alloc{pucch_alloc_}, logger(srslog::fetch_basic_logger("SCHED"))
{
}

uci_allocator_impl::~uci_allocator_impl() = default;

////////////    Static functions    ////////////

static void update_uci_on_pusch_harq_offsets(uci_info::harq_info& uci_harq, const uci_on_pusch& uci_cfg)
{
  // We assume the configuration contains the values for beta_offsets.
  const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

  // The values of \c beta_offsets are set according to Section 9.3, TS38.213.
  const uint16_t harq_ack_nof_bits = uci_harq.harq_ack_nof_bits;
  if (harq_ack_nof_bits <= 2) {
    uci_harq.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_1.value();
  } else if (harq_ack_nof_bits > 2 and harq_ack_nof_bits <= 11) {
    uci_harq.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_2.value();
  } else {
    uci_harq.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_3.value();
  }
}

static void add_csi_to_uci_on_pusch(uci_info::csi_info& uci_csi, const ue_cell_configuration& ue_cell_cfg)
{
  uci_csi.csi_rep_cfg = create_csi_report_configuration(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value());

  // NOTE: The CSI size depends on whether the CSI is configured on PUSCH or PUCCH, as per Section 5.2.3, TS 38.214:
  // "For both Type I and Type II reports configured for PUCCH but transmitted on PUSCH, the determination of the
  // payload for CSI part 1 and CSI part 2 follows that of PUCCH as described in clause 5.2.4."
  if (is_pusch_configured(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value())) {
    csi_report_pusch_size csi_size = get_csi_report_pusch_size(uci_csi.csi_rep_cfg);
    uci_csi.csi_part1_nof_bits     = csi_size.part1_size.value();

    const auto& uci_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value();

    // We assume the configuration contains the values for beta_offsets.
    const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

    // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
    if (uci_csi.csi_part1_nof_bits <= 11) {
      uci_csi.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value();
    } else {
      uci_csi.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value();
    }

    if (csi_size.part2_min_size.value() > 0U) {
      srsran_assert(csi_size.part2_max_size.value() <= 11U, "CSI Part 2 on UCI-PUSCH is only supported up to 11 bits");
      uci_csi.beta_offset_csi_2.emplace(beta_offsets.beta_offset_csi_p2_idx_1.value());
    }

  } else {
    uci_csi.csi_part1_nof_bits = get_csi_report_pucch_size(uci_csi.csi_rep_cfg).value();
    // NOTE: with PUCCH-configured CSI report, CSI patrt 2 is not supported.

    const auto& uci_cfg = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value();

    // We assume the configuration contains the values for beta_offsets.
    const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

    // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
    if (uci_csi.csi_part1_nof_bits <= 11) {
      uci_csi.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value();
    } else {
      uci_csi.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value();
    }
  }
}

////////////    Private functions    ////////////

uci_allocator_impl::slot_alloc_list::ue_uci* uci_allocator_impl::get_uci_alloc(slot_point uci_slot, rnti_t rnti)
{
  auto& ucis = uci_alloc_grid[uci_slot.to_uint()].ucis;
  auto* it   = std::find_if(ucis.begin(), ucis.end(), [rnti](const auto& uci) { return uci.rnti == rnti; });
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

optional<uci_allocation> uci_allocator_impl::alloc_uci_harq_ue_helper(cell_resource_allocator&     res_alloc,
                                                                      rnti_t                       crnti,
                                                                      const ue_cell_configuration& ue_cell_cfg,
                                                                      unsigned                     k0,
                                                                      unsigned                     k1,
                                                                      const pdcch_dl_information*  fallback_dci_info)
{
  cell_slot_resource_allocator& slot_alloc = res_alloc[k0 + k1 + res_alloc.cfg.ntn_cs_koffset];

  // Get existing PUSCH grant, if any, for a given UE's RNTI.
  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  const bool has_pusch_grants = existing_pusch != slot_alloc.result.ul.puschs.end();

  // [Implementation-defined] Skip allocation of UCI if any existing PUSCH grants.
  if (has_pusch_grants) {
    return {};
  }

  const bool fallback_mode = fallback_dci_info != nullptr;

  optional<unsigned> pucch_res_indicator;
  if (fallback_mode) {
    const auto* pucch_harq_grant_it = std::find_if(
        slot_alloc.result.ul.pucchs.begin(), slot_alloc.result.ul.pucchs.end(), [crnti](const pucch_info& pucch_grant) {
          srsran_assert(pucch_grant.format == pucch_format::FORMAT_1 or pucch_grant.format == pucch_format::FORMAT_2,
                        "Only PUCCH Format 1 and Format 2 are supported");
          const bool has_harq_ack_bits = pucch_grant.format == pucch_format::FORMAT_1
                                             ? pucch_grant.format_1.harq_ack_nof_bits != 0U
                                             : pucch_grant.format_2.harq_ack_nof_bits != 0U;
          return pucch_grant.crnti == crnti and has_harq_ack_bits;
        });

    // Do not allocate PUCCH on common resources if there is any pre-allocated grant using PUCCH dedicated resource
    // with HARQ-ACK bits, as we don't support multiplexing of bits across common and dedicated resources.
    if (pucch_harq_grant_it == slot_alloc.result.ul.pucchs.end()) {
      pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(res_alloc, crnti, k0, k1, *fallback_dci_info);
    }
  }
  // PUCCH, Non-fallback mode.
  else {
    pucch_res_indicator = pucch_alloc.alloc_ded_pucch_harq_ack_ue(res_alloc, crnti, ue_cell_cfg, k0, k1);
  }
  return pucch_res_indicator.has_value()
             ? optional<uci_allocation>{uci_allocation{.pucch_res_indicator = pucch_res_indicator}}
             : nullopt;
}

////////////    Public functions    ////////////

void uci_allocator_impl::slot_indication(slot_point sl_tx)
{
  // Clear previous slot UCI allocations.
  uci_alloc_grid[(sl_tx - 1).to_uint()].ucis.clear();
}

optional<uci_allocation> uci_allocator_impl::alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                                               rnti_t                       crnti,
                                                               const ue_cell_configuration& ue_cell_cfg,
                                                               unsigned                     k0,
                                                               span<const uint8_t>          k1_list,
                                                               const pdcch_dl_information*  fallback_dci_info)
{
  // [Implementation-defined] We restrict the number of HARQ bits per PUCCH that are expected to carry CSI reporting to
  // 2 , until the PUCCH allocator supports more than this.
  static const uint8_t max_harq_bits_per_uci = 2U;

  const slot_point          pdsch_slot = res_alloc[k0].slot;
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
    cell_slot_resource_allocator& slot_alloc = res_alloc[k0 + k1_candidate + res_alloc.cfg.ntn_cs_koffset];
    const slot_point              uci_slot   = slot_alloc.slot;

    // Check whether UCI slot is UL enabled.
    if (not cell_cfg.is_fully_ul_enabled(uci_slot)) {
      continue;
    }

    if (uci_alloc_grid[slot_alloc.slot.to_uint()].ucis.full()) {
      logger.info(
          "rnti={}: UCI allocation for slot={} skipped due to UCI cache full. Attempting this allocation for the next "
          "k1 candidate, if any k1 available",
          crnti,
          slot_alloc.slot);
      continue;
    }

    if (csi_helper::is_csi_reporting_slot(ue_cell_cfg.cfg_dedicated(), uci_slot)) {
      // NOTE: This is only to avoid allocating more than 2 HARQ bits in PUCCH that are expected to carry CSI reporting.
      // TODO: Remove this when the PUCCH allocator handle properly more than 2 HARQ-ACK bits + CSI.
      if (get_scheduled_pdsch_counter_in_ue_uci(slot_alloc, crnti) >= max_harq_bits_per_uci) {
        continue;
      }
    }

    // Step 2: Try to allocate UCI HARQ ACK for UE, either on PUSCH or PUCCH.
    optional<uci_allocation> uci_output =
        alloc_uci_harq_ue_helper(res_alloc, crnti, ue_cell_cfg, k0, k1_candidate, fallback_dci_info);

    // Register new UCI allocation in the respective grid slot entry and derive DAI.
    if (uci_output.has_value()) {
      auto* uci = get_uci_alloc(slot_alloc.slot, crnti);
      if (uci == nullptr) {
        uci                             = &uci_alloc_grid[slot_alloc.slot.to_uint()].ucis.emplace_back();
        uci->rnti                       = crnti;
        uci->scheduled_dl_pdcch_counter = 0;
      }
      uci_output.value().harq_bit_idx = uci->scheduled_dl_pdcch_counter;
      ++uci->scheduled_dl_pdcch_counter;
      uci_output.value().k1 = k1_candidate;

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
  uci.alpha     = ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value().scaling;

  if (pucch_uci.csi_part1_bits != 0) {
    // The number of bits is computed based on the CSI report configuration.
    add_csi_to_uci_on_pusch(uci.csi.emplace(), ue_cell_cfg);
  }

  if (pucch_uci.harq_ack_nof_bits != 0) {
    uci.harq.emplace();
    uci.harq.value().harq_ack_nof_bits = pucch_uci.harq_ack_nof_bits;
    update_uci_on_pusch_harq_offsets(
        uci.harq.value(), ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value());
  }

  logger.debug("rnti={}: UCI mltplxd on PUSCH for slot={}", crnti, slot_alloc.slot);
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
    logger.debug("rnti={}: SR allocation skipped due to PUSCH grant allocated.", crnti);
    return;
  }

  pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, crnti, ue_cell_cfg);
}

void uci_allocator_impl::uci_allocate_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                      rnti_t                        crnti,
                                                      const ue_cell_configuration&  ue_cell_cfg)
{
  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  const bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // TODO: check if this case is possible. The first PUSCH that is allocated outside the RAR grant is will be allocated
  //       after the UE objected gets created with the UE dedicated configuration; this means the CSI scheduler should
  //       start allocating CSI grants before any PUSCH grants (except the PUSCH scheduled in the RAR).
  if (has_pusch_grants) {
    // If there is UCI grant allocated, allocate it.
    srsran_assert(not existing_pusch->uci.has_value(), "UCI on PUSCH grant for CSI cannot be already allocated");
    existing_pusch->uci.emplace();
    existing_pusch->uci.value().alpha =
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value().scaling;

    add_csi_to_uci_on_pusch(existing_pusch->uci.value().csi.emplace(), ue_cell_cfg);

    logger.debug("rnti={} UCI with 0 H-ACK, {} CSI-p1 and {} CSI-p2 bits for slot={} allocated on PUSCH",
                 crnti,
                 existing_pusch->uci.value().csi.value().csi_part1_nof_bits,
                 existing_pusch->uci.value().csi.value().beta_offset_csi_2.has_value() ? "up to 11" : "0",
                 slot_alloc.slot);
    return;
  }

  // Else, allocate the CSI on the PUCCH.
  const auto& csi_report_cfg = create_csi_report_configuration(ue_cell_cfg.cfg_dedicated().csi_meas_cfg.value());
  pucch_alloc.pucch_allocate_csi_opportunity(
      slot_alloc, crnti, ue_cell_cfg, get_csi_report_pucch_size(csi_report_cfg).value());
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

bool uci_allocator_impl::has_uci_harq_on_common_pucch_res(rnti_t rnti, slot_point sl_tx)
{
  return pucch_alloc.has_common_pucch_f1_grant(rnti, sl_tx);
}
