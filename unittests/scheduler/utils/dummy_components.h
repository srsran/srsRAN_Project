/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

class dummy_pdcch_resource_allocator : public pdcch_resource_allocator
{
public:
  pdcch_dl_information* alloc_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                           rnti_t                        rnti,
                                           search_space_id               ss_id,
                                           aggregation_level             aggr_lvl) override
  {
    TESTASSERT_EQ(ss_id, slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.ra_search_space_id);
    slot_alloc.result.dl.dl_pdcchs.emplace_back();
    slot_alloc.result.dl.dl_pdcchs.back().ctx.rnti    = rnti;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.bwp_cfg = &slot_alloc.cfg.dl_cfg_common.init_dl_bwp.generic_params;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.coreset_cfg =
        &*slot_alloc.cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
    slot_alloc.result.dl.dl_pdcchs.back().ctx.cces = {get_ncce(slot_alloc.slot), srsgnb::aggregation_level::n4};
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    srsgnb_terminate("UE-dedicated PDCCHs not supported");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          bwp_id_t                      bwp_id,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    srsgnb_terminate("UE-dedicated PDCCHs not supported");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    srsgnb_terminate("Common UL PDCCHs not supported");
    return nullptr;
  }

private:
  unsigned get_ncce(slot_point pdcch_sl)
  {
    if (not last_sl.valid() or pdcch_sl != last_sl) {
      next_ncce = to_nof_cces(srsgnb::aggregation_level::n4);
      last_sl   = pdcch_sl;
      return 0;
    }
    unsigned ret = next_ncce;
    next_ncce += to_nof_cces(srsgnb::aggregation_level::n4);
    return ret;
  }

  unsigned   next_ncce = 0;
  slot_point last_sl;
};

} // namespace srsgnb