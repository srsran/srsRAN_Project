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

#include "ue_srb0_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/tbs_calculator.h"

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
  if (ues.empty()) {
    return;
  }

  const cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  if (not cell_cfg.is_dl_enabled(pdcch_alloc.slot)) {
    return;
  }
  // Note: Unable at the moment to multiplex CSI and PDSCH.
  if (not res_alloc[0].result.dl.csi_rs.empty()) {
    return;
  }

  // Schedule SRB0 messages.
  auto it = pending_ues.begin();
  while (it != pending_ues.end()) {
    // Check if UE exists.
    if (not ues.contains(*it)) {
      it = pending_ues.erase(it);
      continue;
    }

    auto& u = ues[*it];
    if (u.has_pending_dl_newtx_bytes(LCID_SRB0) and schedule_srb0(res_alloc, u)) {
      it = pending_ues.erase(it);
    } else {
      ++it;
    }
  }
}

void ue_srb0_scheduler::handle_dl_buffer_state_indication(du_ue_index_t ue_index)
{
  pending_ues.push_back(ue_index);
}

bool ue_srb0_scheduler::schedule_srb0(cell_resource_allocator& res_alloc, ue& u)
{
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // Search valid PDSCH time domain resource.
  for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
       ++time_res_idx) {
    const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(time_res_idx);

    // Fetch PDSCH resource grid allocators.
    const cell_slot_resource_allocator& pdsch_alloc = res_alloc[pdsch_td_cfg.k0];

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

    if (schedule_srb0(u, res_alloc, time_res_idx)) {
      return true;
    }
  }

  // No resource found in UE's carriers and Search spaces.
  logger.debug("rnti={:#x}: Not enough PDSCH space for SRB0 message. Will re-try in next slot.", u.crnti);
  return false;
}

bool ue_srb0_scheduler::schedule_srb0(ue& u, cell_resource_allocator& res_alloc, unsigned pdsch_time_res)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  const subcarrier_spacing                     scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[pdsch_td_cfg.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.debug("rnti={:#x}: Failed to allocate PDSCH for SRB0. Cause: No space available in scheduler output list.",
                 u.crnti);
    return false;
  }

  // Search for empty HARQ.
  dl_harq_process* h_dl = ue_pcell.harqs.find_empty_dl_harq();
  srsran_assert(h_dl != nullptr, "UE must have empty HARQs during SRB0 PDU allocation");

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
        "rnti={:#x}: SRB0 PDU size ({}) exceeds TBS calculated ({}).", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
    return false;
  }

  if (mcs_idx > expert_cfg.max_msg4_mcs) {
    logger.debug("rnti={:#x}: MCS index chosen ({}) for SRB0 exceeds maximum allowed MCS index ({}).",
                 u.crnti,
                 mcs_idx,
                 expert_cfg.max_msg4_mcs);
    return false;
  }

  crb_interval ue_grant_crbs = rb_helper::find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  if (ue_grant_crbs.length() < prbs_tbs.nof_prbs) {
    logger.debug("rnti={:#x}: Postponed SRB0 PDU scheduling. Cause: Not enough PRBs ({} < {})",
                 u.crnti,
                 ue_grant_crbs.length(),
                 prbs_tbs.nof_prbs);
    return false;
  }

  // Allocate PDCCH resources.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.get_id(), aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.debug("rnti={:#x}: Postponed SRB0 PDU scheduling. Cause: No space in PDCCH.", u.crnti);
    return false;
  }

  // Allocate PUCCH resources.
  unsigned             k1          = 4;
  pucch_harq_ack_grant pucch_grant = {};
  // Minimum k1 value supported is 4.
  static const std::array<uint8_t, 5> dci_1_0_k1_values = {4, 5, 6, 7, 8};
  for (const auto k1_candidate : dci_1_0_k1_values) {
    pucch_grant = pucch_alloc.alloc_common_pucch_harq_ack_ue(res_alloc, u.crnti, pdsch_td_cfg.k0, k1_candidate, *pdcch);
    if (pucch_grant.pucch_pdu != nullptr) {
      k1 = k1_candidate;
      break;
    }
  }
  if (pucch_grant.pucch_pdu == nullptr) {
    logger.debug("Failed to allocate PDSCH for SRB0. Cause: No space in PUCCH.");
    pdcch_sch.cancel_last_pdcch(pdcch_alloc);
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  fill_srb0_grant(u,
                  pdsch_alloc.slot,
                  *h_dl,
                  *pdcch,
                  pdsch_alloc.result.dl.ue_grants.emplace_back(),
                  pucch_grant,
                  pdsch_time_res,
                  k1,
                  mcs_idx,
                  ue_grant_crbs,
                  pdsch_cfg,
                  prbs_tbs.tbs_bytes);

  return true;
}

void ue_srb0_scheduler::fill_srb0_grant(ue&                        u,
                                        slot_point                 pdsch_slot,
                                        dl_harq_process&           h_dl,
                                        pdcch_dl_information&      pdcch,
                                        dl_msg_alloc&              msg,
                                        pucch_harq_ack_grant&      pucch,
                                        unsigned                   pdsch_time_res,
                                        unsigned                   k1,
                                        sch_mcs_index              mcs_idx,
                                        const crb_interval&        ue_grant_crbs,
                                        const pdsch_config_params& pdsch_params,
                                        unsigned                   tbs_bytes)
{
  static constexpr uint8_t srb0_dai = 0;
  // Allocate DL HARQ.
  h_dl.new_tx(pdsch_slot,
              k1,
              expert_cfg.max_nof_harq_retxs,
              srb0_dai,
              u.get_pcell().channel_state_manager().get_wideband_cqi(),
              u.get_pcell().channel_state_manager().get_nof_dl_layers());

  // Fill DL PDCCH DCI.
  static const uint8_t msg4_rv = 0;
  build_dci_f1_0_tc_rnti(pdcch.dci,
                         cell_cfg.dl_cfg_common.init_dl_bwp,
                         ue_grant_crbs,
                         pdsch_time_res,
                         k1,
                         pucch.pucch_res_indicator,
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
