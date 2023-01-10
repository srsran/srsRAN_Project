/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "paging_scheduler.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdcch/pdcch_type0_helpers.h"
#include "../support/prbs_calculator.h"
#include "../support/ssb_helpers.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"

using namespace srsgnb;

paging_scheduler::paging_scheduler(const scheduler_expert_config&                  expert_cfg_,
                                   const cell_configuration&                       cell_cfg_,
                                   pdcch_resource_allocator&                       pdcch_sch_,
                                   const sched_cell_configuration_request_message& msg) :
  expert_cfg(expert_cfg_), cell_cfg(cell_cfg_), pdcch_sch(pdcch_sch_), logger(srslog::fetch_basic_logger("MAC"))
{
  switch (cell_cfg.dl_cfg_common.pcch_cfg.default_paging_cycle) {
    case paging_cycle::rf32:
      default_paging_cycle = 32;
      break;
    case paging_cycle::rf64:
      default_paging_cycle = 64;
      break;
    case paging_cycle::rf128:
      default_paging_cycle = 128;
      break;
    case paging_cycle::rf256:
      default_paging_cycle = 256;
      break;
  }

  switch (cell_cfg.dl_cfg_common.pcch_cfg.nof_pf) {
    case pcch_config::nof_pf_per_drx_cycle::oneT:
      nof_pf_per_drx_cycle = 1;
      break;
    case pcch_config::nof_pf_per_drx_cycle::halfT:
      nof_pf_per_drx_cycle = 2;
      break;
    case pcch_config::nof_pf_per_drx_cycle::quarterT:
      nof_pf_per_drx_cycle = 4;
      break;
    case pcch_config::nof_pf_per_drx_cycle::oneEighthT:
      nof_pf_per_drx_cycle = 8;
      break;
    case pcch_config::nof_pf_per_drx_cycle::oneSixteethT:
      nof_pf_per_drx_cycle = 16;
      break;
  }

  paging_frame_offset = cell_cfg.dl_cfg_common.pcch_cfg.paging_frame_offset;

  switch (cell_cfg.dl_cfg_common.pcch_cfg.ns) {
    case pcch_config::nof_po_per_pf::four:
      nof_po_per_pf = 4;
      break;
    case pcch_config::nof_po_per_pf::two:
      nof_po_per_pf = 2;
      break;
    case pcch_config::nof_po_per_pf::one:
      nof_po_per_pf = 1;
      break;
  }

  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value()) {
    if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value() == 0) {
      // PDCCH monitoring occasions for paging are same as for RMSI. See TS 38.304, clause 7.1.
      sib1_period = std::max(ssb_periodicity_to_value(cell_cfg.ssb_cfg.ssb_period),
                             sib1_rtx_periodicity_to_value(expert_cfg.si.sib1_retx_period));
      for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
        if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
          continue;
        }
        type0_pdcch_css_n0_slots[i_ssb] =
            precompute_type0_pdcch_css_n0(msg.searchspace0, msg.coreset0, cell_cfg, msg.scs_common, i_ssb);
      }
      // Define a BWP configuration limited by CORESET#0 RBs.
      coreset_bwp_cfg      = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
      coreset_bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
    } else {
      for (const auto& cfg : cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces) {
        if (cfg.id != cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value()) {
          continue;
        }
        ss_cfg = cfg;
      }
      if (ss_cfg.cs_id != to_coreset_id(0) &&
          ((not cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value()) ||
           (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().id != ss_cfg.cs_id))) {
        srsgnb_assertion_failure("CORESET configuration for Paging Search Space not configured in DL BWP.");
      }
      if (ss_cfg.cs_id == to_coreset_id(0) &&
          (not cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value())) {
        srsgnb_assertion_failure("CORESET0 configuration for Paging Search Space not configured in DL BWP.");
      }

      coreset_bwp_cfg = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
      if (ss_cfg.cs_id == to_coreset_id(0)) {
        coreset_bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
      }

      precompute_type2_pdcch_slots(msg.scs_common);
    }
  } else {
    srsgnb_assertion_failure("Paging Search Space not configured in DL BWP.");
  }
}

void paging_scheduler::schedule_paging(cell_slot_resource_allocator& res_grid, slot_point sl_point)
{
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

  for (const auto& pg_ind : paging_pending_ues) {
    const unsigned drx_cycle = pg_ind.paging_drx.has_value() ? pg_ind.paging_drx.value() : default_paging_cycle;

    const unsigned t_div_n     = drx_cycle / nof_pf_per_drx_cycle;
    const unsigned ue_id_mod_n = pg_ind.ue_identity_index_value % nof_pf_per_drx_cycle;

    // Check for paging frame.
    // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See Ts 38.304, clause 7.1.
    if (((sl_point.sfn() + paging_frame_offset) % drx_cycle) != (t_div_n * ue_id_mod_n)) {
      continue;
    }

    // Index (i_s), indicating the index of the PO.
    // i_s = floor (UE_ID/N) mod Ns.
    const unsigned i_s = static_cast<unsigned>(std::floor(static_cast<double>(pg_ind.ue_identity_index_value) /
                                                          static_cast<double>(nof_pf_per_drx_cycle))) %
                         nof_po_per_pf;

    for (unsigned time_res_idx = 0;
         time_res_idx != cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list.size();
         ++time_res_idx) {
      if (paging_search_space == 0 &&
          schedule_paging_in_search_space0(
              res_grid, sl_point, time_res_idx, pg_ind.ue_identity_index_value, i_s, pg_ind.paging_msg_size)) {
        break;
      }
      if (paging_search_space > 0 &&
          schedule_paging_in_search_space_id_gt_0(
              res_grid, sl_point, time_res_idx, pg_ind.ue_identity_index_value, i_s, pg_ind.paging_msg_size)) {
        break;
      }
    }
  }
}

void paging_scheduler::handle_paging_indication_message(paging_indication_message paging_indication)
{
  paging_pending_ues.push_back(paging_indication);
}

bool paging_scheduler::schedule_paging_in_search_space_id_gt_0(cell_slot_resource_allocator& res_grid,
                                                               slot_point                    sl_point,
                                                               unsigned                      pdsch_time_res,
                                                               unsigned                      ue_id,
                                                               unsigned                      i_s,
                                                               unsigned                      paging_msg_size)
{
  // Following values for Ns are valid as per TS 38.304, clause 7.1 and TS 38.331, clause 5.2.1 when
  // pagingSearchSpace > 0
  // - Ns can be 1, 2 or 4 provided SearchSpace has sufficient PDCCH Monitoring Occasions to match nof. PO(s).

  // - [Implementation defined] Currently, PDCCH allocator does not support more than one monitoring occasion per slot
  //   and the monitoring occasion should start at the first symbol of the slot. Therefore, DU must configuring paging
  //   search space taking this into account when defining SearchSpace > 0 for paging.

  // For each beam, check if the paging needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the paging for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    // TODO: Support multi-beam operations. As per TS 38.304, clause 7.1, In multi-beam operations, the same paging
    //  message are repeated in all transmitted beams.

    if (sl_point.slot_index() == type2_pdcch_css_slots[ssb_idx][i_s].slot_index()) {
      return allocate_paging(res_grid,
                             pdsch_time_res,
                             ue_id,
                             ssb_idx,
                             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value(),
                             paging_msg_size);
    }
  }

  return false;
}

bool paging_scheduler::schedule_paging_in_search_space0(cell_slot_resource_allocator& res_grid,
                                                        slot_point                    sl_point,
                                                        unsigned                      pdsch_time_res,
                                                        unsigned                      ue_id,
                                                        unsigned                      i_s,
                                                        unsigned                      paging_msg_size)
{
  // Following values for Ns are valid as per TS 38.304, clause 7.1 and TS 38.331, clause 5.2.1 when
  // pagingSearchSpace = 0
  // - Ns can be 1 when multiplexing pattern 1.
  // - Ns can be 2 when multiplexing pattern is 2 or 3 and SS/PBCH periodicity is 5ms

  // TS 38.304, clause 7.1, When SearchSpaceId = 0 is configured for pagingSearchSpace and Ns = 2, PO is either in the
  // first half frame (i_s = 0) or the second half frame (i_s = 1) of the PF.
  if ((i_s == 0 && sl_point.is_odd_hrf()) || (i_s == 1 && (not sl_point.is_odd_hrf()))) {
    return false;
  }

  // - [Implementation defined] When pagingSearchSpace = 0, the UE monitors the SearchSpaceSet 0 for Paging in 2
  //   consecutive slots, starting from n0 for multiplexing pattern 1.
  //   In this function, it is assumed that the GNB only allocates the PDCCH/DCI_1_0 for Paging in the first slot, i.e.,
  //   in n0. This simplification is taken from SIB1 scheduler.

  // The paging_period_slots is expressed in unit of slots.
  // NOTE: As paging_period_slots is expressed in milliseconds or subframes, we need to this these into slots.
  const unsigned paging_period_slots = sib1_period * static_cast<unsigned>(sl_point.nof_slots_per_subframe());

  // For each beam, check if the paging needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the paging for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    if (sl_point.to_uint() % paging_period_slots == type0_pdcch_css_n0_slots[ssb_idx].to_uint()) {
      // Ensure slot for Paging has DL enabled.
      if (not cell_cfg.is_dl_enabled(sl_point)) {
        logger.error("SCHED: Could not allocated paging for beam idx {} as slot is not DL enabled.", ssb_idx);
        return false;
      }

      return allocate_paging(res_grid,
                             pdsch_time_res,
                             ue_id,
                             ssb_idx,
                             cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id,
                             paging_msg_size);
    }
  }

  return false;
}

bool paging_scheduler::allocate_paging(cell_slot_resource_allocator& res_grid,
                                       unsigned                      pdsch_time_res,
                                       unsigned                      ue_id,
                                       unsigned                      beam_idx,
                                       search_space_id               ss_id,
                                       unsigned                      paging_msg_size)
{
  // NOTE:
  // - [Implementation defined] Need to take into account PDSCH Time Domain Resource Allocation in UE's active DL BWP,
  //   for now only initial DL BWP is considered for simplification in this function.

  const pdsch_time_domain_resource_allocation& pdsch_td_cfg =
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[pdsch_time_res];
  static const unsigned nof_symb_sh = pdsch_td_cfg.symbols.length();
  static const unsigned nof_layers  = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by P-RNTI.
  static const unsigned nof_oh_prb = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, pdsch_time_res, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.pg.paging_mcs_index);
  const pdsch_prbs_tbs      paging_prbs_tbs = get_nof_prbs(prbs_calculator_pdsch_config{
      paging_msg_size, nof_symb_sh, calculate_nof_dmrs_per_rb(dmrs_info), nof_oh_prb, mcs_descr, nof_layers});

  // 1. Find available RBs in PDSCH for Paging grant.
  crb_interval paging_crbs;
  {
    const unsigned    nof_paging_rbs = paging_prbs_tbs.nof_prbs;
    const prb_bitmap& used_crbs      = res_grid.dl_res_grid.used_crbs(coreset_bwp_cfg, pdsch_td_cfg.symbols);
    paging_crbs                      = find_empty_interval_of_length(used_crbs, nof_paging_rbs, 0);
    if (paging_crbs.length() < nof_paging_rbs) {
      logger.error("SCHED: Not enough PDSCH space for Paging in beam idx: {}", beam_idx);
      return false;
    }
  }

  // 2. Allocate DCI_1_0 for Paging on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_pdcch_common(res_grid, rnti_t::P_RNTI, ss_id, expert_cfg.pg.paging_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("SCHED: Could not allocated Paging's DCI in PDCCH for beam idx: {}", beam_idx);
    return false;
  }

  // 3. Now that we are sure there is space in both PDCCH and PDSCH, set Paging CRBs as used.
  res_grid.dl_res_grid.fill(
      grant_info{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, pdsch_td_cfg.symbols, paging_crbs});

  // 4. Delegate filling Paging grants to helper function.
  fill_paging_grant(res_grid, paging_crbs, pdsch_time_res, ue_id, dmrs_info, paging_prbs_tbs.tbs_bytes);

  logger.info("SCHED: Paging, cell={}, SSB beam idx: {}, crbs={}", res_grid.cfg.cell_index, beam_idx, paging_crbs);
  return true;
}

void paging_scheduler::fill_paging_grant(cell_slot_resource_allocator& res_grid,
                                         crb_interval                  crbs_grant,
                                         unsigned                      time_resource,
                                         unsigned                      ue_id,
                                         const dmrs_information&       dmrs_info,
                                         unsigned                      tbs)
{
  // Add DCI to list to dl_pdcch.
  srsgnb_assert(not res_grid.result.dl.dl_pdcchs.empty(), "No DL PDCCH grant found in the DL sched results.");
  auto&              paging_pdcch = res_grid.result.dl.dl_pdcchs.back();
  const prb_interval paging_prbs  = crb_to_prb(coreset_bwp_cfg, crbs_grant);

  // Fill Paging DCI.
  paging_pdcch.dci.type             = dci_dl_rnti_config_type::p_rnti_f1_0;
  paging_pdcch.dci.p_rnti_f1_0      = {};
  dci_1_0_p_rnti_configuration& dci = paging_pdcch.dci.p_rnti_f1_0;
  dci.N_rb_dl_bwp                   = coreset_bwp_cfg.crbs.length();
  dci.short_messages_indicator      = dci_1_0_p_rnti_configuration::payload_info::scheduling_information;
  dci.frequency_resource            = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{dci.N_rb_dl_bwp, paging_prbs.start(), paging_prbs.length()});
  dci.time_resource = time_resource;
  // As per Table 7.3.1.2.2-5, TS 38.212, 0 = non-interleaved, 1 = interleaved.
  // TODO: Verify if interleaved is suitable for Paging.
  dci.vrb_to_prb_mapping       = 0;
  dci.modulation_coding_scheme = expert_cfg.pg.paging_mcs_index.to_uint();

  // Add Paging to list of Paging information to pass to lower layers.
  dl_paging_allocation& paging = res_grid.result.dl.paging_grants.emplace_back();
  paging.ue_id                 = ue_id;

  // Fill PDSCH configuration.
  pdsch_information& pdsch = paging.pdsch_cfg;
  pdsch.rnti               = paging_pdcch.ctx.rnti;
  pdsch.bwp_cfg            = paging_pdcch.ctx.bwp_cfg;
  pdsch.coreset_cfg        = paging_pdcch.ctx.coreset_cfg;
  pdsch.symbols = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[dci.time_resource].symbols;
  pdsch.prbs    = paging_prbs;
  // As per TS 38.211, Section 7.3.1.1, n_ID is set to Physical Cell ID.
  pdsch.n_id = cell_cfg.pci;

  pdsch_codeword& cw   = pdsch.codewords.emplace_back();
  cw.mcs_index         = dci.modulation_coding_scheme;
  cw.mcs_table         = pdsch_mcs_table::qam64;
  cw.mcs_descr         = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.tb_size_bytes     = static_cast<uint32_t>(tbs);
  pdsch.dmrs           = dmrs_info;
  pdsch.is_interleaved = paging_pdcch.dci.si_f1_0.vrb_to_prb_mapping > 0;
  pdsch.ss_set_type    = search_space_set_type::type0;
  pdsch.dci_fmt        = dci_dl_format::f1_0;
}

void paging_scheduler::precompute_type2_pdcch_slots(subcarrier_spacing scs_common)
{
  const auto& ss_periodicity = ss_cfg.monitoring_slot_period;
  const auto& ss_slot_offset = ss_cfg.monitoring_slot_offset;
  const auto& ss_duration    = ss_cfg.duration;

  std::vector<slot_point> pdcch_monitoring_occasions;
  // Initialize slot point to 0.
  slot_point sl = slot_point{to_numerology_value(scs_common), 0};
  // Compute all PDCCH Monitoring Occasions in a frame.
  for (unsigned slot_num = 0; slot_num < sl.nof_slots_per_frame(); slot_num++) {
    const slot_point ref_sl = sl + slot_num;
    // Ensure slot for Paging has DL enabled.
    if (not cell_cfg.is_dl_enabled(sl)) {
      continue;
    }
    if ((slot_num - ss_slot_offset) % ss_periodicity == 0) {
      // - [Implementation defined] Currently, PDCCH allocator does not support more than one monitoring occasion
      //   per slot.
      for (unsigned duration = 0; duration < ss_duration; duration++) {
        const slot_point add_pmo_slot = ref_sl + duration;

        // Ensure slot for Paging has DL enabled.
        if (not cell_cfg.is_dl_enabled(add_pmo_slot)) {
          continue;
        }
        pdcch_monitoring_occasions.push_back(add_pmo_slot);
        slot_num += ss_duration;
      }
    }
  }

  unsigned nof_ssb_transmitted{};
  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
      continue;
    }
    nof_ssb_transmitted++;
  }

  const auto& first_pmo_of_po = cell_cfg.dl_cfg_common.pcch_cfg.first_pdcch_monitoring_occasion_of_po_value;
  for (unsigned po_idx = 0; po_idx < nof_po_per_pf; po_idx++) {
    for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
      if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
        continue;
      }
      // See TS 38.304, clause 7.1.
      const auto pmo_idx =
          first_pmo_of_po.empty()
              ? (po_idx * nof_ssb_transmitted * pcch_config::NR_OF_PDCCH_MONITORING_OCCASION_PER_SSB_IN_PO)
              : first_pmo_of_po[po_idx];
      srsgnb_assert(pmo_idx < pdcch_monitoring_occasions.size(),
                    "Not enough PDCCH Monitoring Occasions for Paging Occasion");
      type2_pdcch_css_slots[i_ssb].push_back(pdcch_monitoring_occasions[pmo_idx]);
    }
  }
}
