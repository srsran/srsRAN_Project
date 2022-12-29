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
#include "../support/pdcch/pdcch_type0_helpers.h"
#include "../support/ssb_helpers.h"

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
  logger(srslog::fetch_basic_logger("MAC"))
{
  if (cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.has_value() &&
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value() == 0) {
    for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
      if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, i_ssb)) {
        continue;
      }
      type0_pdcch_css_n0_slots[i_ssb] =
          precompute_type0_pdcch_css_n0(msg.searchspace0, msg.coreset0, cell_cfg, msg.scs_common, i_ssb);
    }
  }
}
