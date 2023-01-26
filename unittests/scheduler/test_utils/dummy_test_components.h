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
#include "lib/scheduler/uci_scheduling/uci_allocator.h"
#include "srsgnb/scheduler/scheduler_metrics.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

class dummy_pdcch_resource_allocator : public pdcch_resource_allocator
{
public:
  rnti_t               last_dl_pdcch_rnti;
  pdcch_dl_information next_ue_pdcch_alloc;
  pdcch_ul_information next_ue_ul_pdcch_alloc;

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
    last_dl_pdcch_rnti = rnti;
    return &next_ue_pdcch_alloc;
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
    return &next_ue_ul_pdcch_alloc;
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

class dummy_uci_allocator : public uci_allocator
{
public:
  uci_allocation next_uci_allocation;

  void slot_indication(slot_point sl_tx) override {}

  uci_allocation alloc_uci_harq_ue(cell_resource_allocator&     res_alloc,
                                   rnti_t                       crnti,
                                   const ue_cell_configuration& ue_cell_cfg,
                                   unsigned                     pdsch_time_domain_resource,
                                   unsigned                     k1) override
  {
    return next_uci_allocation;
  }

  void multiplex_uci_on_pusch(ul_sched_info&                pusch_grant,
                              cell_slot_resource_allocator& slot_alloc,
                              const ue_cell_configuration&  ue_cell_cfg,
                              rnti_t                        crnti) override
  {
  }

  void uci_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                   rnti_t                        crnti,
                                   const ue_cell_configuration&  ue_cell_cfg) override
  {
  }
};

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  optional<du_ue_index_t> last_ue_index_cfg;
  optional<du_ue_index_t> last_ue_index_deleted;

  void on_ue_config_complete(du_ue_index_t ue_index) override { last_ue_index_cfg = ue_index; }
  void on_ue_delete_response(du_ue_index_t ue_index) override { last_ue_index_deleted = ue_index; }
};

class scheduler_ue_metrics_dummy_notifier : public scheduler_ue_metrics_notifier
{
public:
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override {}
};

} // namespace srsgnb