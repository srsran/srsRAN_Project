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
#include "../support/dmrs_helpers.h"
#include "../support/prbs_calculator.h"

using namespace srsgnb;

ue_srb0_scheduler::ue_srb0_scheduler(srslog::basic_logger& logger) : logger(logger) {}

void ue_srb0_scheduler::run_slot(ue_pdsch_allocator& pdsch_alloc, const ue_list& ues)
{
  if (ues.empty()) {
    return;
  }

  // Queue of UEs with failed SRB0 scheduling attempt.
  std::queue<du_ue_index_t> failed_ues;
  // Schedule SRB0 messages
  while (not pending_ues.empty()) {
    // Check if UE exists
    if (not ues.contains(pending_ues.front())) {
      pending_ues.pop();
      continue;
    }

    const auto& u = ues[pending_ues.front()];
    if (not schedule_srb0(pdsch_alloc, u)) {
      failed_ues.push(pending_ues.front());
    }
    pending_ues.pop();
  }
  failed_ues.swap(pending_ues);
}

void ue_srb0_scheduler::handle_dl_buffer_state_indication(du_ue_index_t ue_index)
{
  pending_ues.push(ue_index);
}

bool ue_srb0_scheduler::schedule_srb0(ue_pdsch_allocator& pdsch_alloc, const ue& u)
{
  const auto& dl_lc_mgr  = u.get_dl_logical_channel_manager();
  const auto  srb0_bytes = dl_lc_mgr.pending_bytes(LCID_SRB0);
  for (const ue_carrier* ue_cc : u.ue_carriers()) {
    // Search for empty HARQ.
    const dl_harq_process* h = ue_cc->harqs.find_empty_dl_harq();
    if (h == nullptr) {
      logger.error("SCHED: No empty HARQ found.");
      return false;
    }

    // Search for available symbolxRB resources in different SearchSpaces.
    const bwp_downlink_common& bwp_dl = ue_cc->cfg().dl_bwp_common(ue_cc->active_bwp_id());
    for (const search_space_configuration* ss_cfg : ue_cc->cfg().get_dl_search_spaces(ue_cc->active_bwp_id())) {
      span<const pdsch_time_domain_resource_allocation> pdsch_list =
          ue_cc->cfg().get_pdsch_time_domain_list(ss_cfg->id);

      for (unsigned time_res = 0; time_res != pdsch_list.size(); ++time_res) {
        const auto&      cell_cfg  = ue_cc->cfg().cell_cfg_common;
        dmrs_information dmrs_info = make_dmrs_info_common(
            cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_res, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

        const auto&           pdsch           = pdsch_list[time_res];
        const unsigned        nof_symb_sh     = pdsch.symbols.length();
        static const unsigned mod_order       = get_bits_per_symbol(modulation_scheme::QPSK);
        static const unsigned srb0_mcs_index  = 0; // TODO: Need to parameterize
        sch_mcs_description   srb0_msc_config = pdsch_mcs_get_config(pdsch_mcs_table::qam64, srb0_mcs_index);
        static const unsigned nof_layers      = 1; // Assumption
        // TODO: As per Section 5.1.3.2, TS 38.214, need to derive xOverhead from PDSCH-ServingCellconfig.
        // Assumed to be not configured hence set to 0 as per spec.
        static const unsigned nof_oh_prb = 0;
        pdsch_prbs_tbs        srb0_prbs_tbs =
            get_nof_prbs(prbs_calculator_pdsch_config{srb0_bytes,
                                                      nof_symb_sh,
                                                      calculate_nof_dmrs_per_rb(dmrs_info),
                                                      nof_oh_prb,
                                                      mod_order,
                                                      static_cast<float>(srb0_msc_config.target_code_rate / 1024),
                                                      nof_layers});

        const cell_slot_resource_grid& grid      = pdsch_alloc.dl_resource_grid(ue_cc->cell_index, pdsch.k0);
        const bwp_configuration&       bwp_cfg   = bwp_dl.generic_params;
        prb_bitmap                     used_crbs = grid.used_crbs(bwp_cfg, pdsch.symbols);
        crb_interval ue_grant_crbs               = find_empty_interval_of_length(used_crbs, srb0_prbs_tbs.nof_prbs, 0);
        if (ue_grant_crbs.length() < srb0_prbs_tbs.nof_prbs) {
          logger.error("SCHED: Not enough PDSCH space for SRB0 message. Will re-try in next slot");
          return false;
        }
        // Successful allocation
        pdsch_alloc.allocate_pdsch(ue_pdsch_grant{&u,
                                                  ue_cc->cell_index,
                                                  h->pid,
                                                  ss_cfg->id,
                                                  time_res,
                                                  ue_grant_crbs,
                                                  dci_dl_format::f1_0,
                                                  aggregation_level::n4});
      }
    }
  }
  return false;
}
