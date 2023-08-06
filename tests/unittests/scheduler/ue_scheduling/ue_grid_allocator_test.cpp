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

#include "../test_utils/config_generators.h"
#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdcch/search_space.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_grid_allocator_tester : public ::testing::Test
{
protected:
  ue_grid_allocator_tester() :
    cell_cfg([]() {
      cell_config_builder_params params;
      params.dl_arfcn       = 536020;
      params.coreset0_index = 13;
      params.band           = band_helper::get_band_from_dl_arfcn(params.dl_arfcn);
      params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

      unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          params.channel_bw_mhz,
          params.scs_common,
          params.band.has_value() ? band_helper::get_freq_range(params.band.value()) : frequency_range::FR1);

      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          params.dl_arfcn, *params.band, nof_crbs, params.scs_common, params.scs_common, 0);
      params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      return test_helpers::make_default_sched_cell_configuration_request(params);
    }()),
    ues(mac_notif)
  {
    alloc.add_cell(to_du_cell_index(0), dummy_pdcch_alloc, dummy_uci_alloc, res_grid);
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
    ue_creation_req.ue_index                          = ue_index;
    ue_creation_req.crnti                             = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list.push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ues.add_ue(std::make_unique<ue>(expert_cfg, cell_cfg, ue_creation_req, harq_timeout_handler));

    return ues[ue_index];
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    ues.add_ue(std::make_unique<ue>(expert_cfg, cell_cfg, ue_creation_req, harq_timeout_handler));
    return ues[ue_creation_req.ue_index];
  }

  void set_allocator_responses(const ue_pdsch_grant& grant)
  {
    const ue_cell& ue_cc = *grant.user->find_cell(grant.cell_index);

    dummy_uci_alloc.next_uci_allocation.alloc_successful = true;
    // TODO: Fix k1 assigned to UCI allocation results when there is more than one entry in k1 list.
    dummy_uci_alloc.next_uci_allocation.k1 = ue_cc.cfg().find_search_space(grant.ss_id)->get_k1_candidates().back();

    dummy_pdcch_alloc.next_ue_pdcch_alloc.ctx.rnti = grant.user->crnti;
    dummy_pdcch_alloc.next_ue_pdcch_alloc.ctx.bwp_cfg =
        &ue_cc.cfg().bwp(ue_cc.active_bwp_id()).dl_common->generic_params;
    dummy_pdcch_alloc.next_ue_pdcch_alloc.ctx.coreset_cfg =
        &*ue_cc.cfg().bwp(ue_cc.active_bwp_id()).dl_common->pdcch_common.coreset0;
  }

  scheduler_ue_expert_config           expert_cfg = config_helpers::make_default_scheduler_expert_config().ue;
  cell_configuration                   cell_cfg{test_helpers::make_default_sched_cell_configuration_request()};
  sched_cfg_dummy_notifier             mac_notif;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;

  dummy_pdcch_resource_allocator dummy_pdcch_alloc;
  dummy_uci_allocator            dummy_uci_alloc;
  cell_resource_allocator        res_grid{cell_cfg};

  ue_repository          ues;
  ue_cell_grid_allocator alloc{expert_cfg, ues, srslog::fetch_basic_logger("SCHED")};
};

TEST_F(ue_grid_allocator_tester, when_coreset0_grant_inside_coreset0_rb_lims_then_allocation_is_successful)
{
  ue& u = add_ue(to_du_ue_index(0), {});

  ue_pdsch_grant grant{.user           = &u,
                       .cell_index     = to_du_cell_index(0),
                       .h_id           = to_harq_id(0),
                       .ss_id          = to_search_space_id(1),
                       .time_res_index = 0,
                       .crbs           = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                       .aggr_lvl       = aggregation_level::n4};
  set_allocator_responses(grant);

  ASSERT_TRUE(alloc.allocate_dl_grant(grant));
}

TEST_F(ue_grid_allocator_tester,
       when_grant_inside_coreset_start_and_coreset0_end_rb_lims_for_css_then_allocation_is_successful)
{
  sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
  // Change SS type to common.
  ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0].set_non_ss0_monitored_dci_formats(
      search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);

  ue& u = add_ue(ue_creation_req);

  const crb_interval crbs =
      get_coreset_crbs(ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().coresets.back());

  ue_pdsch_grant grant{
      .user           = &u,
      .cell_index     = to_du_cell_index(0),
      .h_id           = to_harq_id(0),
      .ss_id          = to_search_space_id(2),
      .time_res_index = 0,
      .crbs           = {crbs.start(),
                         crbs.start() + cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()},
      .aggr_lvl       = aggregation_level::n4};
  set_allocator_responses(grant);

  ASSERT_TRUE(alloc.allocate_dl_grant(grant));
}

TEST_F(ue_grid_allocator_tester, when_using_fallback_dci_format_only_64_qam_mcs_table_is_used)
{
  sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                    = to_du_ue_index(0);
  ue_creation_req.crnti                                                       = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#1 uses fallback DCI format hence the 64QAM MCS table must be used.
  const ue_pdsch_grant grant{.user           = &u,
                             .cell_index     = to_du_cell_index(0),
                             .h_id           = to_harq_id(0),
                             .ss_id          = to_search_space_id(1),
                             .time_res_index = 0,
                             .crbs     = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                             .aggr_lvl = aggregation_level::n4};
  set_allocator_responses(grant);

  ASSERT_TRUE(alloc.allocate_dl_grant(grant));
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam64);
}

TEST_F(ue_grid_allocator_tester, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
{
  sched_ue_creation_request_message ue_creation_req = test_helpers::create_default_sched_ue_creation_request();
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  ue_creation_req.cfg.cells[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                    = to_du_ue_index(0);
  ue_creation_req.crnti                                                       = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#2 uses non-fallback DCI format hence the MCS table set in dedicated PDSCH configuration must be used.
  const ue_pdsch_grant grant{.user           = &u,
                             .cell_index     = to_du_cell_index(0),
                             .h_id           = to_harq_id(0),
                             .ss_id          = to_search_space_id(2),
                             .time_res_index = 0,
                             .crbs     = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                             .aggr_lvl = aggregation_level::n4};
  set_allocator_responses(grant);

  ASSERT_TRUE(alloc.allocate_dl_grant(grant));
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam256);
}
