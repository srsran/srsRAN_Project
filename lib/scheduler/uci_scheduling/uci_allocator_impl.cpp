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

/// Number of possible Downlink Assignment Indexes {0, ..., 3} as per TS38.213 Section 9.1.3.
constexpr static size_t DAI_MOD = 4;

////////////    C-tors and d-tors    ////////////

uci_allocator_impl::uci_allocator_impl(pucch_allocator& pucch_alloc_) :
  pucch_alloc{pucch_alloc_}, logger(srslog::fetch_basic_logger("SCHED"))
{
}

uci_allocator_impl::~uci_allocator_impl() = default;

////////////    Private functions    ////////////

void uci_allocator_impl::allocate_uci_harq_on_pusch(ul_sched_info&      pusch_grant,
                                                    const uci_on_pusch& uci_cfg,
                                                    unsigned            harq_ack_nof_bits,
                                                    unsigned            csi_part1_nof_bits)
{
  srsgnb_assert(not pusch_grant.uci.has_value(),
                "Unexpected event: in the current slot UCI was found on PUSCH for RNTI {:#x}",
                pusch_grant.pusch_cfg.rnti);

  // [Implementation-defined] CSI part2 reporting not supported.
  const unsigned CSI_PART2_NOF_BITS = 0;

  pusch_grant.uci.emplace();
  uci_info& uci          = pusch_grant.uci.value();
  uci.harq_ack_nof_bits  = harq_ack_nof_bits;
  uci.csi_part1_nof_bits = csi_part1_nof_bits;
  uci.csi_part2_nof_bits = CSI_PART2_NOF_BITS;

  // Set parameters of UCI-HARQ.
  uci.alpha = uci_cfg.scaling;

  // We assume the configuration contains the values for beta_offsets.
  const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

  // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
  if (uci.harq_ack_nof_bits <= 2) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_1.value();
  }
  if (uci.harq_ack_nof_bits > 2 and uci.harq_ack_nof_bits <= 11) {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_2.value();
  } else {
    uci.beta_offset_harq_ack = beta_offsets.beta_offset_ack_idx_3.value();
  }

  // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
  if (uci.csi_part1_nof_bits <= 11) {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value();
  } else {
    uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value();
  }

  // When this allocation is done, there is no CSI_part2 to be reported. Therefore, as per Section 9.3, TS 38.213, the
  // \c beta_offsets values to be used are those with index 1.
  uci.beta_offset_csi_2 = beta_offsets.beta_offset_csi_p2_idx_1.value();
}

void uci_allocator_impl::allocate_uci_csi_on_pusch(ul_sched_info&      pusch_grant,
                                                   const uci_on_pusch& uci_cfg,
                                                   unsigned            csi_part1_nof_bits)
{
  // We assume the configuration contains the values for beta_offsets.
  const auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(uci_cfg.beta_offsets_cfg.value());

  if (pusch_grant.uci.has_value()) {
    auto& uci = pusch_grant.uci.value();

    uci.csi_part1_nof_bits = csi_part1_nof_bits;

    // The values of \c beta_offsets are set according to Section 9.3, TS 38.213.
    if (uci.csi_part1_nof_bits <= 11) {
      uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_1.value();
    } else {
      uci.beta_offset_csi_1 = beta_offsets.beta_offset_csi_p1_idx_2.value();
    }

  } else {
    // When this allocation is done, there is only HARQ-ACK to be reported and no CSI.
    const unsigned HARQ_ACK_NOF_BITS = 1;
    allocate_uci_harq_on_pusch(pusch_grant, uci_cfg, HARQ_ACK_NOF_BITS, csi_part1_nof_bits);
  }
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
    // If we reach this point, we expect neither PUCCH grants nor UCI on PUSCH previsouly allocated; this would be the
    // case of a new allocation of HARQ-ACK on PUSCH We assume we only report the HARQ-ACK for a single layer, i.e., 1
    // bit.
    const unsigned nof_harq_ack_bits = 1;
    // When this allocation is done, there is only HARQ-ACK to be reported and no CSI.
    const unsigned CSI_PART1_NOF_BITS = 0;
    allocate_uci_harq_on_pusch(
        *existing_pusch,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
        nof_harq_ack_bits,
        CSI_PART1_NOF_BITS);
    uci_output.alloc_successful = true;
    logger.debug("UCI for HARQ-ACK allocated on PUSCH, for UE={:#x}", crnti);
  } else {
    uci_output.pucch_grant =
        pucch_alloc.alloc_ded_pucch_harq_ack_ue(res_alloc, crnti, ue_cell_cfg, pdsch_time_domain_resource, k1);
    uci_output.alloc_successful = uci_output.pucch_grant.pucch_pdu != nullptr;
  }

  // Register new UCI allocation in the respective grid slot entry and derive DAI.
  if (uci_output.alloc_successful) {
    auto* uci = get_uci_alloc(slot_alloc.slot, crnti);
    if (uci == nullptr) {
      uci                   = &uci_alloc_grid[slot_alloc.slot.to_uint()].ucis.emplace_back();
      uci->rnti             = crnti;
      uci->harq_ack_counter = 0;
    }
    // Set DAI if TDD is enabled.
    if (slot_alloc.cfg.is_tdd()) {
      uci_output.dai = uci->harq_ack_counter % DAI_MOD;
      uci->harq_ack_counter++;
    }
  }

  return uci_output;
}

void uci_allocator_impl::multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                                                cell_slot_resource_allocator& slot_alloc,
                                                const ue_cell_configuration&  ue_cell_cfg,
                                                rnti_t                        crnti)
{
  pucch_uci_bits pucch_uci = pucch_alloc.remove_ue_uci_from_pucch(
      slot_alloc, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // In case there are no UCI bits from PUCCH, then there is no UCI to be multiplexed on the PUSCH.
  if (pucch_uci.harq_ack_nof_bits == 0 and pucch_uci.csi_part1_bits == 0) {
    return;
  }

  // We assume that at this point, there are no existing UCI grants in the PUSCH.
  allocate_uci_harq_on_pusch(
      pusch_grant,
      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
      pucch_uci.harq_ack_nof_bits,
      pucch_uci.csi_part1_bits);

  logger.debug("UCI for UE={:#x} mltplxd on PUSCH for slot={}", crnti, slot_alloc.slot.to_uint());
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
    logger.debug("SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", crnti);
    return;
  }

  pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, crnti, ue_cell_cfg);
}

void uci_allocator_impl::uci_allocate_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                      rnti_t                        crnti,
                                                      const ue_cell_configuration&  ue_cell_cfg)
{
  // [Implementation-defined] This is the only supported CSI format, for the time being.
  const unsigned CSI_PART1_NOF_BITS = 4;

  auto&          puschs         = slot_alloc.result.ul.puschs;
  ul_sched_info* existing_pusch = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });

  bool has_pusch_grants =
      not slot_alloc.result.ul.puschs.empty() and existing_pusch != slot_alloc.result.ul.puschs.end();

  // If there is a PUSCH allocated for this UE, allocate the CSI on the UCI on PUSCH.
  if (has_pusch_grants) {
    return allocate_uci_csi_on_pusch(
        *existing_pusch,
        ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pusch_cfg.value().uci_cfg.value(),
        CSI_PART1_NOF_BITS);
  }

  // Else, allocate the CSI on the PUCCH.
  pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, crnti, ue_cell_cfg);
}

uci_allocator_impl::slot_alloc_list::ue_uci* uci_allocator_impl::get_uci_alloc(slot_point uci_slot, rnti_t rnti)
{
  auto& ucis = uci_alloc_grid[uci_slot.to_uint()].ucis;
  auto  it   = std::find_if(ucis.begin(), ucis.end(), [rnti](const auto& uci) { return uci.rnti == rnti; });
  return it != ucis.end() ? &*it : nullptr;
}
