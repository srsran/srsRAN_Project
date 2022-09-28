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
  // Schedule pending SRB0 messages
  dl_bs.slot_indication();
  for (const auto& bs : dl_bs.get_events()) {
    if (not ues.contains(bs.ue_index)) {
      continue;
    }
    const auto& u = ues[bs.ue_index];

    for (const ue_carrier* ue_cc : u.ue_carriers()) {
      // Search available HARQ.
      const dl_harq_process* h = ue_cc->harqs.find_pending_dl_retx();
      if (h == nullptr) {
        h = ue_cc->harqs.find_empty_dl_harq();
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

          const pdsch_time_domain_resource_allocation& pdsch       = pdsch_list[time_res];
          static const unsigned                        nof_symb_sh = pdsch.symbols.length();
          static const unsigned                        mod_order   = get_bits_per_symbol(modulation_scheme::QPSK);
          static const unsigned                        nof_layers  = 1; // Assumption
          // TODO: As per Section 5.1.3.2, TS 38.214, need to derive xOverhead in PDSCH-ServingCellconfig.
          // Assumed to be not configured hence set to 0 as per spec.
          static const unsigned nof_oh_prb       = 0;
          static const float    target_code_rate = 379.0F;
          pdsch_prbs_tbs        srb0_prbs_tbs =
              get_nof_prbs(prbs_calculator_pdsch_config{bs.bs,
                                                        nof_symb_sh,
                                                        calculate_nof_dmrs_per_rb(dmrs_info),
                                                        nof_oh_prb,
                                                        mod_order,
                                                        static_cast<float>(target_code_rate / 1024),
                                                        nof_layers});

          const cell_slot_resource_grid& grid      = pdsch_alloc.dl_resource_grid(ue_cc->cell_index, pdsch.k0);
          const bwp_configuration&       bwp_cfg   = bwp_dl.generic_params;
          prb_bitmap                     used_crbs = grid.used_crbs(bwp_cfg, pdsch.symbols);
          crb_interval ue_grant_crbs = find_empty_interval_of_length(used_crbs, srb0_prbs_tbs.nof_prbs, 0);
          if (ue_grant_crbs.length() < srb0_prbs_tbs.nof_prbs) {
            logger.error("SCHED: Not enough PDSCH space for SRB0 message. Will re-try in next slot");
            // TODO: Keep copy of unscheduled buffer states
            continue;
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
  }
}

void ue_srb0_scheduler::handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs)
{
  dl_bs.emplace(bs);
}
