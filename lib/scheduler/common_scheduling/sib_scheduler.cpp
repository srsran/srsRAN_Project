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

#include "sib_scheduler.h"
#include "../support/dci_builder.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdcch/pdcch_type0_helpers.h"
#include "../support/pdsch/pdsch_default_time_allocation.h"
#include "../support/pdsch/pdsch_resource_allocation.h"
#include "../support/prbs_calculator.h"
#include "../support/sch_pdu_builder.h"
#include "../support/ssb_helpers.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"
#include "srsran/ran/sib/sib_configuration.h"

using namespace srsran;

//  ------   Public methods   ------ .

sib1_scheduler::sib1_scheduler(const scheduler_si_expert_config&               expert_cfg_,
                               const cell_configuration&                       cfg_,
                               pdcch_resource_allocator&                       pdcch_sch,
                               const sched_cell_configuration_request_message& msg) :
  expert_cfg{expert_cfg_},
  cell_cfg{cfg_},
  pdcch_sched{pdcch_sch},
  coreset0{msg.coreset0},
  searchspace0{msg.searchspace0},
  sib1_payload_size{msg.sib1_payload_size}
{
  // Compute derived SIB1 parameters.
  sib1_period = std::max(ssb_periodicity_to_value(cfg_.ssb_cfg.ssb_period),
                         sib1_rtx_periodicity_to_value(expert_cfg.sib1_retx_period));

  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
      continue;
    }
    // NOTE:
    // - [Implementation defined] Use (n0 + 1) slot to avoid collisions between SSB and SIB1.
    sib1_type0_pdcch_css_slots[i_ssb] =
        precompute_type0_pdcch_css_n0_plus_1(searchspace0, coreset0, cell_cfg, msg.scs_common, i_ssb);
  }

  // Define a BWP configuration limited by CORESET#0 RBs.
  coreset0_bwp_cfg      = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  coreset0_bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
};

void sib1_scheduler::schedule_sib1(cell_slot_resource_allocator& res_grid, slot_point sl_point)
{
  // NOTE:
  // - [Implementation defined]
  //   The UE monitors the SearchSpaceSet 0 for SIB1 in 2 consecutive slots, starting from n0.
  //   In this function, it is assumed that the GNB only allocates the PDCCH/DCI_1_0 for SIB1 in the first slot, i.e.,
  //   in n0 if channel BW > 10 Mhz. Otherwise, in (n0 + 1) slot to make space for SSB.
  // - [Implementation defined] We assume the SIB1 is (re)transmitted every 20ms if the SSB periodicity <= 20ms.
  //   Else, we set the (re)transmission periodicity as the SSB's.

  // The sib1_period_slots is expressed in unit of slots.
  // NOTE: As sib1_period_slots is expressed in milliseconds or subframes, we need to this these into slots.
  const unsigned sib1_period_slots = sib1_period * static_cast<unsigned>(sl_point.nof_slots_per_subframe());

  // For each beam, check if the SIB1 needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the SIB1 for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    if (sl_point.to_uint() % sib1_period_slots == sib1_type0_pdcch_css_slots[ssb_idx].to_uint()) {
      // Ensure slot for SIB1 has DL enabled.
      if (not cell_cfg.is_dl_enabled(sl_point)) {
        logger.error("Could not allocate SIB1 for beam idx {} as slot is not DL enabled.", ssb_idx);
        return;
      }

      unsigned                          time_resource = 0;
      const search_space_configuration& ss_cfg =
          cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common
              .search_spaces[cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id];
      const unsigned coreset_duration        = cell_cfg.get_common_coreset(ss_cfg.get_coreset_id()).duration;
      const auto&    pdsch_td_res_alloc_list = get_si_rnti_pdsch_time_domain_list(
          cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp, cell_cfg.dmrs_typeA_pos);
      bool is_sib1_scheduled = false;
      for (const auto& pdsch_td_res : pdsch_td_res_alloc_list) {
        // Check whether PDSCH time domain resource fits in DL symbols of the slot.
        if (pdsch_td_res.symbols.stop() > cell_cfg.get_nof_dl_symbol_per_slot(sl_point)) {
          continue;
        }
        if (pdsch_td_res.map_type == sch_mapping_type::typeB) {
          continue;
        }
        if (pdsch_td_res.symbols.start() >= ss_cfg.get_first_symbol_index(ssb_idx) + coreset_duration) {
          time_resource = std::distance(pdsch_td_res_alloc_list.begin(), &pdsch_td_res);
          if (allocate_sib1(res_grid, ssb_idx, time_resource)) {
            // SIB1 Allocation successful.
            is_sib1_scheduled = true;
            break;
          }
        }
      }
      if (not is_sib1_scheduled) {
        logger.error("No valid PDSCH time domain resource exists to schedule SIB1");
      }
    }
  }
}

//  ------   Private methods   ------ .

bool sib1_scheduler::allocate_sib1(cell_slot_resource_allocator& res_grid, unsigned beam_idx, unsigned time_resource)
{
  const auto& pdsch_td_res_alloc_list =
      get_si_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp, cell_cfg.dmrs_typeA_pos);
  const ofdm_symbol_range sib1_ofdm_symbols = pdsch_td_res_alloc_list[time_resource].symbols;
  const unsigned          nof_symb_sh       = sib1_ofdm_symbols.length();
  static const unsigned   nof_layers        = 1;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by SI-RNTI.
  static const unsigned nof_oh_prb = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  const dmrs_information dmrs_info =
      make_dmrs_info_common(pdsch_td_res_alloc_list, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  const sch_mcs_description mcs_descr     = pdsch_mcs_get_config(pdsch_mcs_table::qam64, expert_cfg.sib1_mcs_index);
  const sch_prbs_tbs        sib1_prbs_tbs = get_nof_prbs(prbs_calculator_sch_config{
      sib1_payload_size, nof_symb_sh, calculate_nof_dmrs_per_rb(dmrs_info), nof_oh_prb, mcs_descr, nof_layers});

  // 1. Find available RBs in PDSCH for SIB1 grant.
  crb_interval sib1_crbs;
  {
    const crb_interval crb_lims =
        pdsch_helper::get_ra_crb_limits_common(cell_cfg.dl_cfg_common.init_dl_bwp, to_search_space_id(0));
    const unsigned    nof_sib1_rbs = sib1_prbs_tbs.nof_prbs;
    const prb_bitmap& used_crbs    = res_grid.dl_res_grid.used_crbs(
        cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, crb_lims, sib1_ofdm_symbols);
    sib1_crbs = rb_helper::find_empty_interval_of_length(used_crbs, nof_sib1_rbs, 0);
    if (sib1_crbs.length() < nof_sib1_rbs) {
      // early exit
      logger.error("Not enough PDSCH space for SIB1 in beam idx: {}", beam_idx);
      return false;
    }
  }

  // 2. Allocate DCI_1_0 for SIB1 on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sched.alloc_dl_pdcch_common(res_grid,
                                        rnti_t::SI_RNTI,
                                        cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id,
                                        expert_cfg.sib1_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("Could not allocated SIB1's DCI in PDCCH for beam idx: {}", beam_idx);
    return false;
  }

  // 3. Now that we are sure there is space in both PDCCH and PDSCH, set SIB1 CRBs as used.
  // NOTE:
  // - ofdm_symbol_range{2, 14} is a temporary hack. The OFDM symbols should be derived from the SIB1 size and
  //   frequency allocation.
  res_grid.dl_res_grid.fill(
      grant_info{cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, sib1_ofdm_symbols, sib1_crbs});

  // 4. Delegate filling SIB1 grants to helper function.
  fill_sib1_grant(res_grid, sib1_crbs, time_resource, dmrs_info, sib1_prbs_tbs.tbs_bytes);
  return true;
}

void sib1_scheduler::fill_sib1_grant(cell_slot_resource_allocator& res_grid,
                                     crb_interval                  sib1_crbs_grant,
                                     unsigned                      time_resource,
                                     const dmrs_information&       dmrs_info,
                                     unsigned                      tbs)
{
  // System information indicator for SIB1, in DCI 1_0. Refer to Section 7.3.1.2.1 and Table 7.3.1.2.1-2, TS 38.212.
  static const unsigned sib1_si_indicator = 0;

  // Add DCI to list to dl_pdcch.
  srsran_assert(res_grid.result.dl.dl_pdcchs.size() > 0, "No DL PDCCH grant found in the DL sched results.");

  const auto& pdsch_td_res_alloc_list =
      get_si_rnti_pdsch_time_domain_list(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp, cell_cfg.dmrs_typeA_pos);

  // Fill SIB1 DCI.
  auto& sib1_pdcch = res_grid.result.dl.dl_pdcchs.back();
  build_dci_f1_0_si_rnti(sib1_pdcch.dci,
                         cell_cfg.dl_cfg_common.init_dl_bwp,
                         sib1_crbs_grant,
                         time_resource,
                         expert_cfg.sib1_mcs_index,
                         sib1_si_indicator);

  // Add SIB1 to list of SIB1 information to pass to lower layers.
  sib_information& sib1 = res_grid.result.dl.bc.sibs.emplace_back();
  sib1.si_indicator     = sib_information::si_indicator_type::sib1;

  // Fill PDSCH configuration.
  pdsch_information& pdsch = sib1.pdsch_cfg;
  build_pdsch_f1_0_si_rnti(pdsch,
                           cell_cfg,
                           tbs,
                           sib1_pdcch.dci.si_f1_0,
                           sib1_crbs_grant,
                           pdsch_td_res_alloc_list[sib1_pdcch.dci.si_f1_0.time_resource].symbols,
                           dmrs_info);
}
