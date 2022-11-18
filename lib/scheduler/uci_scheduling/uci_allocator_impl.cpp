/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_allocator_impl.h"
#include "../support/pucch/pucch_default_resource.h"

using namespace srsgnb;

uci_allocator_impl::uci_allocator_impl(pucch_allocator& pucch_alloc_) : pucch_alloc{pucch_alloc_} {}

uci_allocator_impl::~uci_allocator_impl() = default;

void uci_allocator_impl::allocate_uci_harq_on_pusch(ul_sched_info&      pusch_grant,
                                                    const uci_on_pusch& uci_cfg,
                                                    unsigned            harq_ack_nof_bits)
{
  uci_info& uci         = pusch_grant.uci;
  uci.harq_ack_nof_bits = harq_ack_nof_bits;

  // If UCI is already present, no need to do anything else.
  if (uci.uci_present) {
    return;
  }

  // Set parameters of UCI-HARQ.
  uci.uci_present = true;
  uci.alpha       = uci_cfg.scaling;

  const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg);

  // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
  if (uci.harq_ack_nof_bits <= 2) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_1.value;
  }
  if (uci.harq_ack_nof_bits > 2 and uci.harq_ack_nof_bits <= 11) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_2.value;
  } else {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_3.value;
  }

  // TODO: verify whether CSI beta offset need to be set for HARQ reporting only.
  if (uci.csi_part1_nof_bits <= 11) {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value;
  } else {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value;
  }

  if (uci.csi_part2_nof_bits <= 11) {
    uci.beta_offset_csi_2 = beta_offsets.beta_offset_csi_p2_idx_1.value;
  } else {
    uci.beta_offset_csi_2 = beta_offsets.beta_offset_csi_p2_idx_2.value;
  }
}

uci_allocation uci_allocator_impl::alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                                     rnti_t                       crnti,
                                                     const ue_cell_configuration& ue_cell_cfg,
                                                     unsigned                     pdsch_time_domain_resource,
                                                     unsigned                     k1)
{
  uci_allocation uci_output;

  // Retreive the scheduling results for slot = k0 + k1;
  cell_slot_resource_allocator& slot_alloc = res_alloc[pdsch_time_domain_resource + k1];

  // Get existing PUSCH grant, if any, for a given UE's RNTI.
  auto           puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  bool has_pusch_grants = existing_pusch != slot_alloc.result.ul.puschs.end();

  // Allocate UCI on PUSCH if any PUSCH grants already exist for the UE; else, allocate UCI on PUCCH.
  if (has_pusch_grants) {
    // We assume we only report the HARQ-ACK for a single layer (i.e., 1 bit); increment by 1 only the current number of
    // HARQ-ACK bits.
    unsigned nof_harq_ack_bits = existing_pusch->uci.harq_ack_nof_bits + 1;
    allocate_uci_harq_on_pusch(
        *existing_pusch,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
        nof_harq_ack_bits);
    // This will remove the UCI only if the UE with CRNTI has some PUCCH grants.
    pucch_alloc.remove_ue_uci_from_pucch(slot_alloc, crnti);
    uci_output.alloc_successful = true;
  } else {
    uci_output.pucch_grant =
        pucch_alloc.alloc_ded_pucch_harq_ack_ue(res_alloc, crnti, ue_cell_cfg, pdsch_time_domain_resource, k1);
    uci_output.alloc_successful = uci_output.pucch_grant.pucch_pdu != nullptr;
  }

  return uci_output;
}

void uci_allocator_impl::move_uci_from_pucch_to_pusch(ul_sched_info&                pusch,
                                                      cell_slot_resource_allocator& slot_alloc,
                                                      const ue_cell_configuration&  ue_cell_cfg,
                                                      rnti_t                        crnti)
{
  pucch_uci_bits pucch_uci = pucch_alloc.remove_ue_uci_from_pucch(slot_alloc, crnti);

  // In case there are no UCI bits from PUCCH, nothing else to be done.
  if (pucch_uci.harq_ack_nof_bits == 0 and pucch_uci.csi_part1_bits == 0) {
    return;
  }

  // Add the HARQ-ACK (PUCCH) UCI bits to the existing HARQ-ACK PUSCH UCI bits.
  unsigned nof_harq_ack_bits = pusch.uci.harq_ack_nof_bits + pucch_uci.harq_ack_nof_bits;
  allocate_uci_harq_on_pusch(
      pusch,
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
      nof_harq_ack_bits);
}
