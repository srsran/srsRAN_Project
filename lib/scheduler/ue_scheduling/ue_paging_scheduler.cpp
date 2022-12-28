/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_paging_scheduler.h"
#include "../support/config_helpers.h"
#include "../support/pdcch/pdcch_type0_helpers.h"

#include "srsgnb/ran/pdcch/pdcch_type0_css_occasions.h"

using namespace srsgnb;

ue_paging_scheduler::ue_paging_scheduler(const scheduler_ue_expert_config&               expert_cfg_,
                                         const cell_configuration&                       cell_cfg_,
                                         pdcch_resource_allocator&                       pdcch_sch_,
                                         ue_list&                                        ues_,
                                         const sched_cell_configuration_request_message& msg) :
  expert_cfg(expert_cfg_),
  cell_cfg(cell_cfg_),
  pdcch_sch(pdcch_sch_),
  ues(ues_),
  initial_active_dl_bwp(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params),
  logger(srslog::fetch_basic_logger("MAC"))
{
  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    type0_pdcch_css_n0_slots.emplace_back(
        precompute_type0_pdcch_css_n0(msg.searchspace0, msg.coreset0, cell_cfg, msg.scs_common, i_ssb));
  }

  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.has_value()) {
    // See 38.212, clause 7.3.1.2.1 - N^{DL,BWP}_RB is the size of CORESET 0 for TC-RNTI.
    initial_active_dl_bwp.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
  }
}
