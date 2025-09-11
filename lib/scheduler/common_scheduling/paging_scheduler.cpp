/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "paging_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/sch_pdu_builder.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

// (Implementation-defined) limit for maximum number of pending paging indications.
static constexpr size_t PAGING_INFO_QUEUE_SIZE = 128;

paging_scheduler::paging_scheduler(const scheduler_expert_config&                  expert_cfg_,
                                   const cell_configuration&                       cell_cfg_,
                                   pdcch_resource_allocator&                       pdcch_sch_,
                                   const sched_cell_configuration_request_message& msg) :
  expert_cfg(expert_cfg_),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  default_paging_cycle(static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle)),
  nof_pf_per_drx_cycle(static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.nof_pf)),
  paging_frame_offset(cell_cfg.dl_cfg_common.pcch_cfg.paging_frame_offset),
  nof_po_per_pf(static_cast<unsigned>(cell_cfg.dl_cfg_common.pcch_cfg.ns)),
  slot_helper(cell_cfg_, msg),
  new_paging_notifications(PAGING_INFO_QUEUE_SIZE),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  paging_pending_ues.reserve(MAX_NOF_PENDING_PAGINGS);
  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value()) {
    for (const auto& cfg : cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
      if (cfg.get_id() != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value()) {
        continue;
      }
      ss_cfg = &cfg;
      break;
    }

    if (ss_cfg == nullptr) {
      srsran_assertion_failure("Paging Search Space not configured in DL BWP.");
    }

    // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
    bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
    if (ss_cfg->is_common_search_space()) {
      // See TS 38.214, 5.1.2.2.2, Downlink resource allocation type 1.
      if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
        bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
      }
    }

    // See TS 38.214, Table 5.1.2.1.1-1.
    // TODO: Select PDSCH time domain resource allocation to apply based on SS/PBCH and CORESET mux. pattern.
    pdsch_td_alloc_list = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.empty()
                              ? pdsch_default_time_allocations_default_A_table(bwp_cfg.cp, cell_cfg.dmrs_typeA_pos)
                              : cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;

    // Generate an empty vector for each element of pdsch_time_res_idx_to_scheduled_ues_lookup; only then we can reserve
    // the capacity.
    pdsch_time_res_idx_to_scheduled_ues_lookup.assign(pdsch_time_res_idx_to_scheduled_ues_lookup.capacity(),
                                                      std::vector<const sched_paging_information*>{});
    for (auto& sched_paging_ues : pdsch_time_res_idx_to_scheduled_ues_lookup) {
      sched_paging_ues.reserve(MAX_PAGING_RECORDS_PER_PAGING_PDU);
    }

  } else {
    srsran_assertion_failure("Paging Search Space not configured in DL BWP.");
  }
}

void paging_scheduler::run_slot(cell_resource_allocator& res_grid)
{
  // Pop pending Paging notification and process them.
  sched_paging_information new_pg_info;
  while (new_paging_notifications.try_pop(new_pg_info)) {
    // Check whether Paging information is already present or not. i.e. tackle repeated Paging attempt from upper
    // layers.
    if (paging_pending_ues.find(new_pg_info.paging_identity) == paging_pending_ues.end()) {
      if (paging_pending_ues.size() < MAX_NOF_PENDING_PAGINGS) {
        paging_pending_ues.emplace(new_pg_info.paging_identity, ue_paging_info{.info = new_pg_info, .retry_count = 0});
      } else {
        logger.warning("Map of paging pending UEs is full. Dropping paging id={}\n", new_pg_info.paging_identity);
      }
    }
  }

  // NOTE:
  // - [Implementation defined] The pagingSearchSpace (in PDCCH-Common IE) value in UE's active BWP must be taken into
  //   consideration while paging a UE. However, for simplification we consider the value of pagingSearchSpace in UE's
  //   active BWP to be same as in initial DL BWP in this implementation. This is simplification can be applied due to
  //   the fact that DU is responsible for providing the PDCCH-Common configuration for UE.
  //
  // - [Implementation defined] As per Paging Frame (PF) definition, it may contain one or multiple PO(s) or starting
  //   point of a PO. For simplification, we do not support Paging Occasion which starts in PF and extends over multiple
  //   radio frames i.e. DU must ensure paging configuration is set to avoid Paging Occasion spanning multiple radio
  //   frames.
  const auto paging_search_space = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

  const cell_slot_resource_allocator& pdcch_alloc = res_grid[0];
  const auto                          pdcch_slot  = pdcch_alloc.slot;
  // Verify PDCCH slot is DL enabled.
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    return;
  }

  // Check for maximum paging retries.
  auto it = paging_pending_ues.begin();
  while (it != paging_pending_ues.end()) {
    if (it->second.retry_count >= expert_cfg.pg.max_paging_retries) {
      it = paging_pending_ues.erase(it);
    } else {
      ++it;
    }
  }

  // Clear all previous vectors.
  for (auto& sched_paging_ues : pdsch_time_res_idx_to_scheduled_ues_lookup) {
    sched_paging_ues.clear();
  }

  for (const auto& pg_it : paging_pending_ues) {
    const auto&    pg_info   = pg_it.second.info;
    const unsigned drx_cycle = pg_info.paging_drx.has_value() ? pg_info.paging_drx.value() : default_paging_cycle;

    // N value used in equation found at TS 38.304, clause 7.1.
    const unsigned N           = drx_cycle / nof_pf_per_drx_cycle;
    const unsigned t_div_n     = drx_cycle / N;
    const unsigned ue_id_mod_n = pg_info.ue_identity_index_value % N;

    // Check for paging frame.
    // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
    if (((pdcch_slot.sfn() + paging_frame_offset) % drx_cycle) != (t_div_n * ue_id_mod_n)) {
      continue;
    }

    // Index (i_s), indicating the index of the PO.
    // i_s = floor (UE_ID/N) mod Ns.
    const unsigned i_s = (pg_info.ue_identity_index_value / N) % nof_po_per_pf;

    for (unsigned time_res_idx = 0; time_res_idx != pdsch_td_alloc_list.size(); ++time_res_idx) {
      const cell_slot_resource_allocator& paging_alloc = res_grid[pdsch_td_alloc_list[time_res_idx].k0];
      // Verify Paging slot is DL enabled.
      if (not cell_cfg.is_dl_enabled(paging_alloc.slot)) {
        continue;
      }
      // Note: Unable at the moment to multiplex CSI and PDSCH.
      if (not paging_alloc.result.dl.csi_rs.empty()) {
        continue;
      }
      // Verify there is space in PDSCH and PDCCH result lists for new allocations.
      if (paging_alloc.result.dl.paging_grants.full() or pdcch_alloc.result.dl.dl_pdcchs.full() or
          pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx].size() >= MAX_PAGING_RECORDS_PER_PAGING_PDU) {
        continue;
      }
      const auto&                 pdsch_td_cfg = pdsch_td_alloc_list[time_res_idx];
      const coreset_configuration cs_cfg       = cell_cfg.get_common_coreset(ss_cfg->get_coreset_id());
      // Check whether PDSCH time domain resource does not overlap with CORESET.
      if (pdsch_td_cfg.symbols.start() < ss_cfg->get_first_symbol_index() + cs_cfg.duration) {
        continue;
      }
      // Check whether PDSCH time domain resource fits in DL symbols of the slot.
      if (pdsch_td_cfg.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(paging_alloc.slot)) {
        continue;
      }

      if (slot_helper.is_paging_slot(pdcch_slot, i_s)) {
        if (paging_search_space == 0 and
            is_there_space_available_for_paging(
                res_grid,
                time_res_idx,
                get_accumulated_paging_msg_size(pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx]) +
                    (pg_info.paging_type_indicator == paging_identity_type::cn_ue_paging_identity
                         ? RRC_CN_PAGING_ID_RECORD_SIZE
                         : RRC_RAN_PAGING_ID_RECORD_SIZE))) {
          pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx].push_back(&pg_info);
          break;
        }
        if (paging_search_space > 0 and
            is_there_space_available_for_paging(
                res_grid,
                time_res_idx,
                get_accumulated_paging_msg_size(pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx]) +
                    (pg_info.paging_type_indicator == paging_identity_type::cn_ue_paging_identity
                         ? RRC_CN_PAGING_ID_RECORD_SIZE
                         : RRC_RAN_PAGING_ID_RECORD_SIZE))) {
          pdsch_time_res_idx_to_scheduled_ues_lookup[time_res_idx].push_back(&pg_info);
          break;
        }
      }
    }
  }

  // Update paging re-tries counter for all successful allocations.
  for (unsigned pdsch_td_res_idx = 0; pdsch_td_res_idx < pdsch_time_res_idx_to_scheduled_ues_lookup.size();
       pdsch_td_res_idx++) {
    if (not pdsch_time_res_idx_to_scheduled_ues_lookup[pdsch_td_res_idx].empty() and
        allocate_paging(res_grid,
                        pdsch_td_res_idx,
                        pdsch_time_res_idx_to_scheduled_ues_lookup[pdsch_td_res_idx],
                        paging_search_space)) {
      // Allocation successful.
      for (const auto* pg_info : pdsch_time_res_idx_to_scheduled_ues_lookup[pdsch_td_res_idx]) {
        paging_pending_ues.at(pg_info->paging_identity).retry_count++;
      }
    }
  }
}

unsigned paging_scheduler::get_accumulated_paging_msg_size(span<const sched_paging_information*> ues_paging_info)
{
  // Estimate of the number of bytes required for the upper layer header in bytes.
  static constexpr unsigned RRC_HEADER_SIZE_ESTIMATE = 2U;

  unsigned payload_size = 0;
  for (const auto& pg_info : ues_paging_info) {
    if (pg_info->paging_type_indicator == paging_identity_type::cn_ue_paging_identity) {
      payload_size += RRC_CN_PAGING_ID_RECORD_SIZE;
    } else {
      payload_size += RRC_RAN_PAGING_ID_RECORD_SIZE;
    }
  }

  return RRC_HEADER_SIZE_ESTIMATE + payload_size;
}

void paging_scheduler::handle_paging_information(const sched_paging_information& paging_info)
{
  if (not new_paging_notifications.try_push(paging_info)) {
    logger.warning("Discarding paging information for ue ID={}. Cause: Event queue is full",
                   paging_info.ue_identity_index_value);
  }
}

void paging_scheduler::stop()
{
  sched_paging_information paging_info;
  while (new_paging_notifications.try_pop(paging_info)) {
  }
  paging_pending_ues.clear();
}

bool paging_scheduler::is_there_space_available_for_paging(cell_resource_allocator& res_grid,
                                                           unsigned                 pdsch_time_res,
                                                           unsigned                 msg_size)
{
  // NOTE:
  // - [Implementation defined] Need to take into account PDSCH Time Domain Resource Allocation in UE's active DL BWP,
  //   for now only initial DL BWP is considered for simplification in this function.

  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_td_alloc_list[pdsch_time_res];
  static const unsigned                        nof_symb_sh  = pdsch_td_cfg.symbols.length();
  static const unsigned                        nof_layers   = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by P-RNTI.
  static const unsigned nof_oh_prb = 0;
  // As per TS 38.214, Table 5.1.3.2-2.
  // TODO: TBS scaling is assumed to be 0. Need to set correct value.
  static const unsigned tbs_scaling = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info =
      make_dmrs_info_common(pdsch_td_alloc_list, pdsch_time_res, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.pg.paging_mcs_index);
  const sch_prbs_tbs        paging_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{
      msg_size, nof_symb_sh, calculate_nof_dmrs_per_rb(dmrs_info), nof_oh_prb, mcs_descr, nof_layers, tbs_scaling});

  // Find available RBs in PDSCH for Paging grant.
  crb_interval paging_crbs;
  {
    const unsigned    nof_paging_rbs = paging_prbs_tbs.nof_prbs;
    const crb_bitmap& used_crbs      = res_grid[pdsch_td_cfg.k0].dl_res_grid.used_crbs(bwp_cfg, pdsch_td_cfg.symbols);
    paging_crbs                      = rb_helper::find_empty_interval_of_length(used_crbs, nof_paging_rbs);
    if (paging_crbs.length() < nof_paging_rbs) {
      return false;
    }
  }

  return true;
}

bool paging_scheduler::allocate_paging(cell_resource_allocator&              res_grid,
                                       unsigned                              pdsch_time_res,
                                       span<const sched_paging_information*> ues_paging_info,
                                       search_space_id                       ss_id)
{
  // NOTE:
  // - [Implementation defined] Need to take into account PDSCH Time Domain Resource Allocation in UE's active DL BWP,
  //   for now only initial DL BWP is considered for simplification in this function.

  const pdsch_time_domain_resource_allocation& pdsch_td_cfg = pdsch_td_alloc_list[pdsch_time_res];
  static const unsigned                        nof_symb_sh  = pdsch_td_cfg.symbols.length();
  static const unsigned                        nof_layers   = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by P-RNTI.
  static const unsigned nof_oh_prb = 0;
  // As per TS 38.214, Table 5.1.3.2-2.
  // TODO: TBS scaling is assumed to be 0. Need to set correct value.
  static const unsigned tbs_scaling = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info =
      make_dmrs_info_common(pdsch_td_alloc_list, pdsch_time_res, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.pg.paging_mcs_index);
  const sch_prbs_tbs        paging_prbs_tbs =
      get_nof_prbs(prbs_calculator_sch_config{get_accumulated_paging_msg_size(ues_paging_info),
                                              nof_symb_sh,
                                              calculate_nof_dmrs_per_rb(dmrs_info),
                                              nof_oh_prb,
                                              mcs_descr,
                                              nof_layers,
                                              tbs_scaling});

  // > Find available RBs in PDSCH for Paging grant.
  crb_interval paging_crbs;
  {
    const unsigned    nof_paging_rbs = paging_prbs_tbs.nof_prbs;
    const crb_bitmap& used_crbs      = res_grid[pdsch_td_cfg.k0].dl_res_grid.used_crbs(bwp_cfg, pdsch_td_cfg.symbols);
    paging_crbs                      = rb_helper::find_empty_interval_of_length(used_crbs, nof_paging_rbs);
    if (paging_crbs.length() < nof_paging_rbs) {
      logger.warning("Not enough PDSCH space for Paging");
      return false;
    }
  }

  // > Allocate DCI_1_0 for Paging on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(res_grid[0], rnti_t::P_RNTI, ss_id, expert_cfg.pg.paging_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("Could not allocate Paging's DCI in PDCCH");
    return false;
  }

  // > Now that we are sure there is space in both PDCCH and PDSCH, set Paging CRBs as used.
  res_grid[pdsch_td_cfg.k0].dl_res_grid.fill(
      grant_info{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, pdsch_td_cfg.symbols, paging_crbs});

  // > Delegate filling Paging grants to helper function.
  fill_paging_grant(res_grid[pdsch_td_cfg.k0].result.dl.paging_grants.emplace_back(),
                    *pdcch,
                    paging_crbs,
                    pdsch_time_res,
                    ues_paging_info,
                    dmrs_info,
                    paging_prbs_tbs.tbs_bytes);

  return true;
}

void paging_scheduler::fill_paging_grant(dl_paging_allocation&                 pg_grant,
                                         pdcch_dl_information&                 pdcch,
                                         crb_interval                          crbs_grant,
                                         unsigned                              time_resource,
                                         span<const sched_paging_information*> ues_paging_info,
                                         const dmrs_information&               dmrs_info,
                                         unsigned                              tbs)
{
  // Fill Paging DCI.
  build_dci_f1_0_p_rnti(
      pdcch.dci, cell_cfg.dl_cfg_common.init_dl_bwp, crbs_grant, time_resource, expert_cfg.pg.paging_mcs_index);

  // Add Paging UE info to list of Paging information to pass to lower layers.
  for (const auto& pg_info : ues_paging_info) {
    pg_grant.paging_ue_list.emplace_back();
    pg_grant.paging_ue_list.back().paging_type_indicator =
        pg_info->paging_type_indicator == paging_identity_type::cn_ue_paging_identity
            ? paging_ue_info::cn_ue_paging_identity
            : paging_ue_info::ran_ue_paging_identity;
    pg_grant.paging_ue_list.back().paging_identity = pg_info->paging_identity;
  }

  // Fill PDSCH configuration.
  pdsch_information& pdsch = pg_grant.pdsch_cfg;
  build_pdsch_f1_0_p_rnti(pdsch,
                          cell_cfg,
                          tbs,
                          pdcch.dci.p_rnti_f1_0,
                          crbs_grant,
                          pdsch_td_alloc_list[pdcch.dci.p_rnti_f1_0.time_resource].symbols,
                          dmrs_info);
}
