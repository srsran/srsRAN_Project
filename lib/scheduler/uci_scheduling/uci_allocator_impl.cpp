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

////////////    C-tors and d-tors    ////////////

uci_allocator_impl::uci_allocator_impl(pucch_allocator& pucch_alloc_) :
  pucch_alloc{pucch_alloc_}, logger(srslog::fetch_basic_logger("MAC"))
{
}

uci_allocator_impl::~uci_allocator_impl() = default;

////////////    Private functions    ////////////

void uci_allocator_impl::allocate_uci_harq_on_pusch(ul_sched_info&      pusch_grant,
                                                    const uci_on_pusch& uci_cfg,
                                                    unsigned            harq_ack_nof_bits)
{
  srsgnb_assert(not pusch_grant.uci.has_value(),
                "Unexpected event: in the current slot UCI was found on PUSCH for RNTI {:#x}",
                pusch_grant.pusch_cfg.rnti);

  pusch_grant.uci.emplace();
  uci_info& uci         = pusch_grant.uci.value();
  uci.harq_ack_nof_bits = harq_ack_nof_bits;

  // Set parameters of UCI-HARQ.
  uci.alpha = uci_cfg.scaling;

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

////////////    Public functions    ////////////

uci_allocation uci_allocator_impl::alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                                     rnti_t                       crnti,
                                                     const ue_cell_configuration& ue_cell_cfg,
                                                     unsigned                     pdsch_time_domain_resource,
                                                     unsigned                     k1)
{
  uci_allocation uci_output;

  // Retrieve the scheduling results for slot = k0 + k1;
  cell_slot_resource_allocator& slot_alloc = res_alloc[pdsch_time_domain_resource + k1];

  // Get existing PUSCH grant, if any, for a given UE's RNTI.
  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // Allocate UCI on PUSCH if any PUSCH grants already exist for the UE; else, allocate UCI on PUCCH.
  if (has_pusch_grants) {
    // This will remove the UCI only if the UE with CRNTI has some PUCCH grants.
    pucch_uci_bits pucch_uci = pucch_alloc.remove_ue_uci_from_pucch(slot_alloc, crnti);
    // We assume we only report the HARQ-ACK for a single layer (i.e., 1 bit); increment by 1 only the current number of
    // HARQ-ACK bits.
    unsigned nof_harq_ack_bits = pucch_uci.harq_ack_nof_bits + 1;
    allocate_uci_harq_on_pusch(
        *existing_pusch,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
        nof_harq_ack_bits);
    uci_output.alloc_successful = true;
  } else {
    uci_output.pucch_grant =
        pucch_alloc.alloc_ded_pucch_harq_ack_ue(res_alloc, crnti, ue_cell_cfg, pdsch_time_domain_resource, k1);
    uci_output.alloc_successful = uci_output.pucch_grant.pucch_pdu != nullptr;
  }

  return uci_output;
}

void uci_allocator_impl::multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                                                cell_slot_resource_allocator& slot_alloc,
                                                const ue_cell_configuration&  ue_cell_cfg,
                                                rnti_t                        crnti)
{
  pucch_uci_bits pucch_uci = pucch_alloc.remove_ue_uci_from_pucch(slot_alloc, crnti);

  // In case there are no UCI bits from PUCCH, then there is no UCI to be multiplexed on the PUSCH.
  if (pucch_uci.harq_ack_nof_bits == 0 and pucch_uci.csi_part1_bits == 0) {
    return;
  }

  // We assume that at this point, there are no existing UCI grants in the PUSCH.
  allocate_uci_harq_on_pusch(
      pusch_grant,
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
      pucch_uci.harq_ack_nof_bits);
}

void uci_allocator_impl::uci_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                     rnti_t                        crnti,
                                                     const ue_cell_configuration&  ue_cell_cfg)
{
  // Retrieve the scheduling results for slot = k0 + k1;
  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // If there is a PUSCH allocated for this UE, do not allocate any PUCCH SR grants.
  if (has_pusch_grants) {
    logger.debug("SCHED: SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", crnti);
    return;
  }

  pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, crnti, ue_cell_cfg);
}
