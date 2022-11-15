/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_srb0_scheduler.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"
#include "../support/tbs_calculator.h"
#include "ue_dci_builder.h"
#include "ue_sch_pdu_builder.h"

using namespace srsgnb;

ue_srb0_scheduler::ue_srb0_scheduler(const cell_configuration& cell_cfg_,
                                     pdcch_resource_allocator& pdcch_sch_,
                                     pucch_allocator&          pucch_alloc_,
                                     ue_list&                  ues_,
                                     unsigned                  max_msg4_mcs_index_) :
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  pucch_alloc(pucch_alloc_),
  ues(ues_),
  max_msg4_mcs_index(max_msg4_mcs_index_),
  initial_active_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params),
  logger(srslog::fetch_basic_logger("MAC"))
{
  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
    // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB is the size of CORESET 0 for TC-RNTI.
    initial_active_dl_bwp.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  }
}

void ue_srb0_scheduler::run_slot(cell_resource_allocator& res_alloc)
{
  if (ues.empty()) {
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
  // Search valid PDSCH time domain resource.
  const auto& bwp_cfg_common = cell_cfg.dl_cfg_common.init_dl_bwp;
  // See 3GPP TS 38.213, clause 10.1,
  // A UE monitors PDCCH candidates in one or more of the following search spaces sets
  //  - a Type1-PDCCH CSS set configured by ra-SearchSpace in PDCCH-ConfigCommon for a DCI format with
  //    CRC scrambled by a RA-RNTI, a MsgB-RNTI, or a TC-RNTI on the primary cell.
  const search_space_configuration& ss =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
          .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id];
  for (unsigned time_res_idx = 0; time_res_idx != bwp_cfg_common.pdsch_common.pdsch_td_alloc_list.size();
       ++time_res_idx) {
    if (schedule_srb0(u, res_alloc, time_res_idx, ss)) {
      return true;
    }
  }

  // No resource found in UE's carriers and Search spaces.
  logger.warning("SCHED: Not enough PDSCH space for SRB0 message. Will re-try in next slot");
  return false;
}

bool ue_srb0_scheduler::schedule_srb0(ue&                               u,
                                      cell_resource_allocator&          res_alloc,
                                      unsigned                          pdsch_time_res,
                                      const search_space_configuration& ss_cfg)
{
  ue_cell&                                     ue_pcell     = u.get_pcell();
  subcarrier_spacing                           scs          = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = get_pdsch_td_cfg(pdsch_time_res);

  // Fetch PDCCH and PDSCH resource grid allocators.
  cell_slot_resource_allocator& pdcch_alloc = res_alloc[0];
  cell_slot_resource_allocator& pdsch_alloc = res_alloc[pdsch_td_cfg.k0];

  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (pdsch_alloc.result.dl.ue_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full()) {
    logger.warning("Failed to allocate PDSCH. Cause: No space available in scheduler output list");
    return false;
  }

  // Search for empty HARQ.
  dl_harq_process* h_dl = ue_pcell.harqs.find_empty_dl_harq();
  srsgnb_assert(h_dl != nullptr, "UE must have empty HARQs during SRB0 PDU allocation");

  // Find available symbol x RB resources.
  const unsigned   pending_bytes = u.pending_dl_newtx_bytes();
  dmrs_information dmrs_info     = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, pdsch_time_res, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);
  const unsigned nof_symb_sh = pdsch_td_cfg.symbols.length();
  prb_bitmap     used_crbs   = pdsch_alloc.dl_res_grid.used_crbs(initial_active_dl_bwp, pdsch_td_cfg.symbols);
  crb_interval   unused_crbs = find_next_empty_interval(used_crbs, 0, used_crbs.size());
  // Try to find least MCS to fit SRB0 message.
  // See 38.214, table 5.1.3.1-1: MCS index table 1 for PDSCH.
  unsigned mcs_idx = 0;
  // Assumption.
  static const unsigned nof_layers = 1;
  // TODO: As per Section 5.1.3.2, TS 38.214, need to derive xOverhead from PDSCH-ServingCellconfig.
  // Assumed to be not configured hence set to 0 as per spec.
  static const unsigned nof_oh_prb = 0;
  pdsch_prbs_tbs        prbs_tbs{};
  while (mcs_idx <= max_msg4_mcs_index) {
    // See 38.214, clause 5.1.3.1 - the UE shall use I_MCS and Table 5.1.3.1-1 to determine the modulation order (Qm)
    // and Target code rate (R) used in the physical downlink shared channel.
    sch_mcs_description mcs_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, mcs_idx);
    prbs_tbs                       = get_nof_prbs(prbs_calculator_pdsch_config{pending_bytes,
                                                         nof_symb_sh,
                                                         calculate_nof_dmrs_per_rb(dmrs_info),
                                                         nof_oh_prb,
                                                         mcs_config.modulation,
                                                         mcs_config.target_code_rate / 1024.0F,
                                                         nof_layers});
    if (unused_crbs.length() >= prbs_tbs.nof_prbs) {
      break;
    }
    ++mcs_idx;
  }

  if (prbs_tbs.tbs_bytes < pending_bytes) {
    logger.warning(
        "SRB0 PDU size ({}) exceeds TBS calculated ({}) for rnti={:#x}.", pending_bytes, prbs_tbs.tbs_bytes, u.crnti);
    return false;
  }

  if (mcs_idx > max_msg4_mcs_index) {
    logger.warning("SCHED: MCS index chosen ({}) exceeds maximum allowed MCS index ({}) for rnti={:#x}.",
                   mcs_idx,
                   max_msg4_mcs_index,
                   u.crnti);
    return false;
  }

  crb_interval ue_grant_crbs = find_empty_interval_of_length(used_crbs, prbs_tbs.nof_prbs, 0);
  prb_interval ue_grant_prbs = crb_to_prb(initial_active_dl_bwp, ue_grant_crbs);
  if (ue_grant_prbs.length() < prbs_tbs.nof_prbs) {
    logger.warning("SCHED: Postponed SRB0 PDU scheduling for rnti={:#x}. Cause: Not enough PRBs ({} < {})",
                   u.crnti,
                   ue_grant_prbs.length(),
                   prbs_tbs.nof_prbs);
    return false;
  }

  // Allocate PDCCH resources.
  pdcch_dl_information* pdcch = pdcch_sch.alloc_pdcch_common(pdcch_alloc, u.crnti, ss_cfg.id, aggregation_level::n4);
  if (pdcch == nullptr) {
    logger.warning("SCHED: Failed to allocate PDSCH. Cause: No space in PDCCH.");
    return false;
  }

  // Allocate PUCCH resources.
  const unsigned       k1 = 4;
  pucch_harq_ack_grant pucch_grant =
      pucch_alloc.alloc_common_pucch_harq_ack_ue(res_alloc, u.crnti, pdsch_time_res, k1, *pdcch);
  if (pucch_grant.pucch_pdu == nullptr) {
    logger.warning("SCHED: Failed to allocate PDSCH. Cause: No space in PUCCH.");
    // TODO: remove PDCCH allocation.
    return false;
  }

  // Mark resources as occupied in the ResourceGrid.
  pdsch_alloc.dl_res_grid.fill(grant_info{scs, pdsch_td_cfg.symbols, ue_grant_crbs});

  // Add new DL UE Grant.
  pdsch_alloc.result.dl.ue_grants.emplace_back();

  fill_srb0_grant(u,
                  pdsch_alloc.slot,
                  *h_dl,
                  *pdcch,
                  pdsch_alloc.result.dl.ue_grants.back(),
                  pucch_grant,
                  pdsch_time_res,
                  k1,
                  mcs_idx,
                  ue_grant_prbs);

  return true;
}

void ue_srb0_scheduler::fill_srb0_grant(ue&                   u,
                                        slot_point            pdsch_slot,
                                        dl_harq_process&      h_dl,
                                        pdcch_dl_information& pdcch,
                                        dl_msg_alloc&         msg,
                                        pucch_harq_ack_grant& pucch,
                                        unsigned              pdsch_time_res,
                                        unsigned              k1,
                                        sch_mcs_index         mcs_idx,
                                        const prb_interval&   ue_grant_prbs)
{
  // Allocate DL HARQ.
  const static unsigned max_retx = 4; // TODO: Parameterize.
  h_dl.new_tx(pdsch_slot, k1, max_retx);

  // Fill DL PDCCH DCI.
  build_dci_f1_0_tc_rnti(pdcch.dci,
                         cell_cfg.dl_cfg_common.init_dl_bwp,
                         ue_grant_prbs,
                         pdsch_time_res,
                         k1,
                         pucch.pucch_res_indicator,
                         mcs_idx,
                         h_dl);

  // Fill PDSCH PDU.
  build_pdsch_f1_0_tc_rnti(msg.pdsch_cfg, u.crnti, cell_cfg, pdcch.dci.tc_rnti_f1_0);

  // Set MAC logical channels to schedule in this PDU.
  msg.tb_list.emplace_back();
  u.build_dl_transport_block_info(msg.tb_list.back(), msg.pdsch_cfg.codewords[0].tb_size_bytes);

  // Save in HARQ the parameters set for this PDCCH and PDSCH PDUs.
  h_dl.save_alloc_params(pdcch.dci.type, msg.pdsch_cfg);
}

const pdsch_time_domain_resource_allocation& ue_srb0_scheduler::get_pdsch_td_cfg(unsigned pdsch_time_res_idx) const
{
  return cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res_idx];
}
