/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_srb0_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "srsran/ran/sch/tbs_calculator.h"

using namespace srsran;

ue_srb0_scheduler::ue_srb0_scheduler(const scheduler_ue_expert_config& expert_cfg_,
                                     const cell_configuration&         cell_cfg_,
                                     pdcch_resource_allocator&         pdcch_sch_,
                                     pucch_allocator&                  pucch_alloc_,
                                     ue_repository&                    ues_) :
  expert_cfg(expert_cfg_),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  pucch_alloc(pucch_alloc_),
  ues(ues_),
  initial_active_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params),
  ss_cfg(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
             .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id]),
  cs_cfg(cell_cfg.get_common_coreset(ss_cfg.get_coreset_id())),
  logger(srslog::fetch_basic_logger("SCHED"))
{
}

void ue_srb0_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  slot_indication();

  if (ues.empty()) {
    return;
  }

  const cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    return;
  }

  // Schedule SRB0 and SRB1 retransmissions, if any.
  auto next_ue_retx = ongoing_ues.begin();
  while (next_ue_retx != ongoing_ues.end()) {
    // TODO: Add a stopping condition in this loop, so next_ue won't iterate over all the UEs and HARQ-processes if
    // there are
    //       no resources.

    // Check if UE exists.
    if (not ues.contains(next_ue_retx->ue_index)) {
      next_ue_retx = ongoing_ues.erase(next_ue_retx);
      continue;
    }
    auto& u = *ues.find(next_ue_retx->ue_index);

    for (auto& h_dl_rext : next_ue_retx->ongoing_h_procs) {
      if (h_dl_rext.h_dl->has_pending_retx()) {
        if (h_dl_rext.is_srb0) {
          schedule_srb0(res_alloc, u, h_dl_rext.h_dl);
        } else {
          // TODO: Change this into SRB1_retx.
          schedule_srb1(res_alloc, u);
        }
      }
    }
    ++next_ue_retx;
  }

  // Schedule SRB0 messages.
  auto next_ue = pending_ues_srb0.begin();
  while (next_ue != pending_ues_srb0.end()) {
    // Check if UE exists.
    if (not ues.contains(*next_ue)) {
      next_ue = pending_ues_srb0.erase(next_ue);
      continue;
    }

    auto& u = ues[*next_ue];
    if (u.has_pending_dl_newtx_bytes(LCID_SRB0) and schedule_srb0(res_alloc, u)) {
      next_ue = pending_ues_srb0.erase(next_ue);
    } else {
      ++next_ue;
    }
  }

  // Keep SRB1 with lower priority than SRB0.
  next_ue = pending_ues_srb1.begin();
  while (next_ue != pending_ues_srb1.end()) {
    // Check if UE exists.
    if (not ues.contains(*next_ue)) {
      next_ue = pending_ues_srb1.erase(next_ue);
      continue;
    }

    auto& u = ues[*next_ue];
    if (u.has_pending_dl_newtx_bytes(LCID_SRB1) and schedule_srb1(res_alloc, u)) {
      next_ue = pending_ues_srb1.erase(next_ue);
    } else {
      ++next_ue;
    }
  }
}

void ue_srb0_scheduler::handle_dl_buffer_state_indication_srb(du_ue_index_t ue_index, bool is_srb0)
{
  is_srb0 ? pending_ues_srb0.push_back(ue_index) : pending_ues_srb1.push_back(ue_index);
}

static unsigned get_next_srb_slot(const cell_configuration& cell_cfg, slot_point sl_tx)
{
  if (not cell_cfg.is_tdd()) {
    // In FDD, we advance by 1 slot.
    return 1U;
  }
  const auto&        tdd_cfg              = cell_cfg.tdd_cfg_common.value();
  const unsigned     tdd_period_nof_slots = nof_slots_per_tdd_period(tdd_cfg);
  optional<unsigned> next_dl_slot_offset =
      find_next_tdd_full_dl_slot(cell_cfg.tdd_cfg_common.value(), sl_tx.slot_index() % tdd_period_nof_slots);
  if (not next_dl_slot_offset.has_value()) {
    // This returns the offset from the current slot to the next DL slot within the next TDD period.
    next_dl_slot_offset = tdd_period_nof_slots - sl_tx.slot_index() % tdd_period_nof_slots;
  }
  return next_dl_slot_offset.value();
}

bool ue_srb0_scheduler::schedule_srb0(cell_resource_allocator& res_alloc, ue& u, dl_harq_process* h_dl_retx)
{
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // Search valid PDSCH time domain resource.

  unsigned offset = 0;
  for (unsigned offset_cnt = 0; offset_cnt != max_dl_slots_ahead_sched;
       ++offset_cnt, offset = get_next_srb_slot(cell_cfg, res_alloc[0 + offset].slot)) {
    for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);

      if (h_dl_retx != nullptr and h_dl_retx->last_alloc_params().nof_symbols != pdsch_td_cfg.symbols.length()) {
        continue;
      }

      // Fetch PDSCH resource grid allocators.
      const cell_slot_resource_allocator& pdsch_alloc = res_alloc[offset + pdsch_td_cfg.k0];

      // We do not support multiplexing of PDSCH for SRB0 with CSI-RS.
      if (not pdsch_alloc.result.dl.csi_rs.empty()) {
        continue;
      }

      if (not cell_cfg.is_dl_enabled(pdsch_alloc.slot)) {
        continue;
      }

      // Check whether PDSCH time domain resource does not overlap with CORESET.
      if (pdsch_td_cfg.symbols.start() < ss_cfg.get_first_symbol_index() + cs_cfg.duration) {
        continue;
      }

      // Check whether PDSCH time domain resource fits in DL symbols of the slot.
      if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(pdsch_alloc.slot)) {
        continue;
      }

      if (h_dl_retx != nullptr) {
        return schedule_srb0_retx(u, res_alloc, time_res_idx, offset, *h_dl_retx);
      } else {
        dl_harq_process* candidate_h_dl = schedule_srb0(u, res_alloc, time_res_idx, offset);
        if (candidate_h_dl != nullptr) {
          cache_ue_srb_tx(u.ue_index, candidate_h_dl, /* is_srb0*/ true);
          return true;
        }
      }
    }
  }

  // No resource found in UE's carriers and Search spaces.
  slot_point pdcch_slot = res_alloc[0].slot;
  logger.debug("rnti={}: Not enough PDSCH space for SRB0 message found in any of the slots:[{},{}).",
               u.crnti,
               pdcch_slot,
               pdcch_slot + max_dl_slots_ahead_sched);
  return false;
}

bool ue_srb0_scheduler::schedule_srb1(cell_resource_allocator& res_alloc, ue& u)
{
  // TODO:
  return true;
}

dl_harq_process* ue_srb0_scheduler::schedule_srb0(ue&                      u,
                                                  cell_resource_allocator& res_alloc,
                                                  unsigned                 pdsch_time_res,
                                                  unsigned                 slot_offset)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.debug("rnti={}: Failed to allocate PDSCH for SRB0. Cause: No space available in scheduler output list.",
                 u.crnti);
    return nullptr;
  }

  // Search for empty HARQ.
  dl_harq_process* h_dl = ue_pcell.harqs.find_empty_dl_harq();
  if (h_dl == nullptr) {
    logger.warning("rnti={}: UE must have empty HARQs during SRB0 PDU allocation", u.crnti);
    return nullptr;
  }

  // Find available symbol x RB resources.
  const unsigned pending_bytes = u.pending_dl_srb0_newtx_bytes();

  pdsch_config_params pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);

  prb_bitmap used_crbs = pdsch_alloc.dl_res_grid.used_crbs(
      initial_active_dl_bwp.scs,
      pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id()),
      pdsch_cfg.symbols);
  const unsigned starting_crb_idx = 0;
  crb_interval   unused_crbs      = rb_helper::find_next_empty_interval(used_crbs, starting_crb_idx, used_crbs.size());
  // Try to find least MCS to fit SRB0 message.
  // See 38.214, table 5.1.3.1-1: MCS index table 1 for PDSCH.
  sch_mcs_index mcs_idx = 0;
  // Assumption.
  // TODO: As per Section 5.1.3.2, TS 38.214, need to derive xOverhead from PDSCH-ServingCellconfig.
  // Assumed to be not configured hence set to 0 as per spec.
  sch_prbs_tbs prbs_tbs{};
  while (mcs_idx <= expert_cfg.max_msg4_mcs) {
    // See 38.214, clause 5.1.3.1 - the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm)
    // and Target code rate (R) used in the physical downlink shared channel.
    const sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, mcs_idx);
    prbs_tbs                             = get_nof_prbs(prbs_calculator_sch_config{pending_bytes,
                                                       static_cast<unsigned>(pdsch_cfg.symbols.length()),
                                                       calculate_nof_dmrs_per_rb(pdsch_cfg.dmrs),
                                                       pdsch_cfg.nof_oh_prb,
                                                       mcs_config,
                                                       pdsch_cfg.nof_layers});
    if (unused_crbs.length() >= prbs_tbs.nof_prbs) {
      break;
    }
    ++mcs_idx;
  }

  if (prbs_tbs.tbs_bytes < pending_bytes) {
    logger.debug(
        "rnti={}: SRB0 PDU size ({}) exceeds TBS calculated ({}).", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
    return nullptr;
  }

  if (mcs_idx > expert_cfg.max_msg4_mcs) {
    logger.debug("rnti={}: MCS index chosen ({}) for SRB0 exceeds maximum allowed MCS index ({}).",
                 u.crnti,
                 mcs_idx,
                 expert_cfg.max_msg4_mcs);
    return nullptr;
  }

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return nullptr;
  }

  // Allocate PDCCH resources.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: No space in PDCCH.", u.crnti);
    return nullptr;
  }

  // Allocate PUCCH resources.
  unsigned k1 = 4;
  // Minimum k1 value supported is 4.
  static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7, 8};
  optional<unsigned>                  pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
        res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug("Failed to allocate PDSCH for SRB0. Cause: No space in PUCCH.");
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return nullptr;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_srb0_grant(u,
                  pdsch_alloc.slot,
                  *h_dl,
                  *pdcch,
                  pdsch_alloc.result.dl.ue_grants.emplace_back(),
                  pucch_res_indicator.value(),
                  pdsch_time_res,
                  k1,
                  mcs_idx,
                  ue_grant_crbs,
                  pdsch_cfg,
                  prbs_tbs.tbs_bytes);

  return h_dl;
}

bool ue_srb0_scheduler::schedule_srb0_retx(ue&                      u,
                                           cell_resource_allocator& res_alloc,
                                           unsigned                 pdsch_time_res,
                                           unsigned                 slot_offset,
                                           dl_harq_process&         h_dl_retx)
{
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[slot_offset];
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[slot_offset + pdsch_td_cfg.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.debug("rnti={}: Failed to allocate PDSCH for SRB0. Cause: No space available in scheduler output list.",
                 u.crnti);
    return false;
  }

  pdsch_config_params pdsch_cfg = get_pdsch_config_f1_0_tc_rnti(cell_cfg, pdsch_td_cfg);

  prb_bitmap used_crbs = pdsch_alloc.dl_res_grid.used_crbs(
      initial_active_dl_bwp.scs,
      pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, ss_cfg.get_id()),
      pdsch_cfg.symbols);

  // See 38.214, table 5.1.3.1-1: MCS index table 1 for PDSCH.
  sch_mcs_index mcs_idx = h_dl_retx.last_alloc_params().tb[0]->mcs;
  // Assumption.
  // TODO: As per Section 5.1.3.2, TS 38.214, need to derive xOverhead from PDSCH-ServingCellconfig.
  // Assumed to be not configured hence set to 0 as per spec.
  sch_prbs_tbs prbs_tbs{h_dl_retx.last_alloc_params().rbs.type1().length(),
                        h_dl_retx.last_alloc_params().tb[0].value().tbs_bytes};

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return false;
  }

  // Allocate PDCCH resources.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={}: Postponed SRB0 PDU scheduling. Cause: No space in PDCCH.", u.crnti);
    return false;
  }

  // Allocate PUCCH resources.
  unsigned k1 = 4;
  // Minimum k1 value supported is 4.
  static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7, 8};
  optional<unsigned>                  pucch_res_indicator;
  for (const auto k1_candidate : dci_1_0_k1_values) {
    pucch_res_indicator = pucch_alloc.alloc_common_pucch_harq_ack_ue(
        res_alloc, u.crnti, slot_offset + pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_res_indicator.has_value()) {
      k1 = k1_candidate;
      break;
    }
  }
  if (not pucch_res_indicator.has_value()) {
    logger.debug("Failed to allocate PDSCH for SRB0. Cause: No space in PUCCH.");
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  // NOTE: the caller of this function has verified that the number of symbols is the same as the original transmission.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_srb0_grant(u,
                  pdsch_alloc.slot,
                  h_dl_retx,
                  *pdcch,
                  pdsch_alloc.result.dl.ue_grants.emplace_back(),
                  pucch_res_indicator.value(),
                  pdsch_time_res,
                  k1,
                  mcs_idx,
                  ue_grant_crbs,
                  pdsch_cfg,
                  prbs_tbs.tbs_bytes,
                  &h_dl_retx);

  return true;
}

void ue_srb0_scheduler::fill_srb0_grant(ue&                        u,
                                        slot_point                 pdsch_slot,
                                        dl_harq_process&           h_dl,
                                        pdcch_dl_information&      pdcch,
                                        dl_msg_alloc&              msg,
                                        unsigned                   pucch_res_indicator,
                                        unsigned                   pdsch_time_res,
                                        unsigned                   k1,
                                        sch_mcs_index              mcs_idx,
                                        const crb_interval&        ue_grant_crbs,
                                        const pdsch_config_params& pdsch_params,
                                        unsigned                   tbs_bytes,
                                        dl_harq_process*           h_dl_retx)
{
  static constexpr uint8_t srb0_dai = 0;
  // Allocate DL HARQ.
  if (h_dl_retx == nullptr) {
    h_dl.new_tx(pdsch_slot,
                k1,
                expert_cfg.max_nof_harq_retxs,
                srb0_dai,
                u.get_pcell().channel_state_manager().get_wideband_cqi(),
                u.get_pcell().channel_state_manager().get_nof_dl_layers());
  } else {
    // We do not multiplex the SRBO PUCCH with existing PUCCH HARQs.
    const unsigned harq_bit_idx = 0U;
    h_dl.new_retx(pdsch_slot, k1, harq_bit_idx);
  }

  // Fill DL PDCCH DCI.
  static const uint8_t msg4_rv = 0;
  build_dci_f1_0_tc_rnti(pdcch.dci,
                         cell_cfg.dl_cfg_common.init_dl_bwp,
                         ue_grant_crbs,
                         pdsch_time_res,
                         k1,
                         pucch_res_indicator,
                         mcs_idx,
                         msg4_rv,
                         h_dl);

  // Fill PDSCH PDU.
  msg.context.ue_index    = u.ue_index;
  msg.context.k1          = k1;
  msg.context.ss_id       = pdcch.ctx.context.ss_id;
  msg.context.nof_retxs   = h_dl.tb(0).nof_retxs;
  msg.context.olla_offset = 0;
  build_pdsch_f1_0_tc_rnti(
      msg.pdsch_cfg, pdsch_params, tbs_bytes, u.crnti, cell_cfg, pdcch.dci.tc_rnti_f1_0, ue_grant_crbs, true);

  // Set MAC logical channels to schedule in this PDU.
  u.build_dl_srb0_transport_block_info(msg.tb_list.emplace_back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(pdcch.dci.type, msg.pdsch_cfg);
}

const pdsch_time_domain_resource_allocation& ue_srb0_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}

void ue_srb0_scheduler::cache_ue_srb_tx(du_ue_index_t ue_index, dl_harq_process* h_dl, bool is_srb0)
{
  auto it_ue = std::find_if(ongoing_ues.begin(), ongoing_ues.end(), [ue_index](const ue_srb_tx_tracker& tracker) {
    return tracker.ue_index == ue_index;
  });

  // There is already a UE with HARQ processes waiting for ACK or with pending retx.
  if (it_ue != ongoing_ues.end()) {
    srsran_assert(it_ue->has_harq_id(h_dl->id), "This HARQ process was already in the list");
    it_ue->add_harq_id(h_dl, is_srb0);
  } else {
    ongoing_ues.emplace_back(ue_index);
    auto& ue_harq_tracker = ongoing_ues.back();
    srsran_assert(ue_harq_tracker.has_harq_id(h_dl->id), "This HARQ process was already in the list");
    ue_harq_tracker.add_harq_id(h_dl, is_srb0);
  }
}

void ue_srb0_scheduler::slot_indication()
{
  if (ongoing_ues.empty()) {
    return;
  }

  // Iterate over the UEs, and check if there are any empty HARQ processes.
  auto it_ue = ongoing_ues.begin();
  while (it_ue != ongoing_ues.end()) {
    // Remove HARQ processes that have been acked for the current UE.
    it_ue->erase_acked_processes();
    // If the UE has no current HARQ process with pending retx or waiting for ack, remove it.
    if (it_ue->ongoing_h_procs.empty()) {
      it_ue = ongoing_ues.erase(it_ue);
      continue;
    }
    ++it_ue;
  }
}
