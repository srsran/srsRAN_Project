/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator.h"
#include "lib/scheduler/uci_scheduling/uci_allocator.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/test_utils.h"

namespace srsran {

class dummy_pdcch_resource_allocator : public pdcch_resource_allocator
{
public:
  rnti_t               last_dl_pdcch_rnti;
  pdcch_dl_information next_ue_pdcch_alloc;
  pdcch_ul_information next_ue_ul_pdcch_alloc;

  pdcch_dl_information* alloc_dl_pdcch_common(cell_slot_resource_allocator& slot_alloc,
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
    slot_alloc.result.dl.dl_pdcchs.back().ctx.cces = {get_ncce(slot_alloc.slot), srsran::aggregation_level::n4};
    return &slot_alloc.result.dl.dl_pdcchs[0];
  }

  pdcch_dl_information* alloc_dl_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    last_dl_pdcch_rnti = rnti;
    return &next_ue_pdcch_alloc;
  }

  pdcch_ul_information* alloc_ul_pdcch_ue(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        rnti,
                                          const ue_cell_configuration&  user,
                                          search_space_id               ss_id,
                                          aggregation_level             aggr_lvl) override
  {
    srsran_terminate("UE-dedicated PDCCHs not supported");
    return nullptr;
  }

  pdcch_ul_information* alloc_ul_pdcch_common(cell_slot_resource_allocator& slot_alloc,
                                              rnti_t                        rnti,
                                              search_space_id               ss_id,
                                              aggregation_level             aggr_lvl) override
  {
    return &next_ue_ul_pdcch_alloc;
  }

  bool cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc) override
  {
    srsran_terminate("Cancellations not supported");
    return false;
  }

private:
  unsigned get_ncce(slot_point pdcch_sl)
  {
    if (not last_sl.valid() or pdcch_sl != last_sl) {
      next_ncce = to_nof_cces(srsran::aggregation_level::n4);
      last_sl   = pdcch_sl;
      return 0;
    }
    unsigned ret = next_ncce;
    next_ncce += to_nof_cces(srsran::aggregation_level::n4);
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
                                   unsigned                     k0,
                                   span<const uint8_t>          k1_list,
                                   const pdcch_dl_information*  fallback_dci_info = nullptr) override
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
                                   const ue_cell_configuration&  ue_cell_cfg,
                                   bool                          is_fallback_mode = false) override
  {
  }

  void uci_allocate_csi_opportunity(cell_slot_resource_allocator& slot_alloc,
                                    rnti_t                        crnti,
                                    const ue_cell_configuration&  ue_cell_cfg,
                                    bool                          is_fallback_mode = false) override
  {
  }

  uint8_t get_scheduled_pdsch_counter_in_ue_uci(cell_slot_resource_allocator& slot_alloc, rnti_t crnti) override
  {
    return 0;
  }
};

class sched_cfg_dummy_notifier : public sched_configuration_notifier
{
public:
  optional<du_ue_index_t> last_ue_index_cfg;
  optional<du_ue_index_t> last_ue_index_deleted;

  void on_ue_config_complete(du_ue_index_t ue_index, bool ue_creation_result) override { last_ue_index_cfg = ue_index; }
  void on_ue_delete_response(du_ue_index_t ue_index) override { last_ue_index_deleted = ue_index; }
};

class scheduler_ue_metrics_dummy_notifier : public scheduler_ue_metrics_notifier
{
public:
  void report_metrics(span<const scheduler_ue_metrics> ue_metrics) override {}
};

class scheduler_harq_timeout_dummy_handler : public harq_timeout_handler
{
public:
  du_ue_index_t last_ue_idx = INVALID_DU_UE_INDEX;

  void handle_harq_timeout(du_ue_index_t ue_index, bool is_dl) override { last_ue_idx = ue_index; }
};

} // namespace srsran
