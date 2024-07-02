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

#include "../test_utils/config_generators.h"
#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
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
    expert_cfg([this]() {
      auto ue_expert_cfg                   = sched_cfg.ue;
      ue_expert_cfg.max_ul_grants_per_slot = 2;
      ue_expert_cfg.max_pucchs_per_slot    = 2;
      return ue_expert_cfg;
    }()),
    cell_cfg(*[this]() {
      cfg_builder_params.dl_arfcn       = 536020;
      cfg_builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      auto* cfg = cfg_mng.add_cell(test_helpers::make_default_sched_cell_configuration_request(cfg_builder_params));
      srsran_assert(cfg != nullptr, "Cell configuration failed");
      return cfg;
    }()),
    current_slot(cfg_builder_params.scs_common, 0)
  {
    // Initialize resource grid.
    res_grid.slot_indication(current_slot);
    pdcch_alloc.slot_indication(current_slot);
    pucch_alloc.slot_indication(current_slot);
    uci_alloc.slot_indication(current_slot);

    alloc.add_cell(to_du_cell_index(0), pdcch_alloc, uci_alloc, res_grid);
  }

  void run_slot()
  {
    ++current_slot;
    res_grid.slot_indication(current_slot);
    pdcch_alloc.slot_indication(current_slot);
    pucch_alloc.slot_indication(current_slot);
    uci_alloc.slot_indication(current_slot);
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        test_helpers::create_default_sched_ue_creation_request(cfg_builder_params);
    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }

    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    auto ev = cfg_mng.add_ue(ue_creation_req);
    ues.add_ue(std::make_unique<ue>(
        ue_creation_command{ev.next_config(), ue_creation_req.starts_in_fallback, harq_timeout_handler}));
    return ues[ue_creation_req.ue_index];
  }

  scheduler_expert_config                 sched_cfg = config_helpers::make_default_scheduler_expert_config();
  scheduler_ue_expert_config              expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                mac_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;
  scheduler_harq_timeout_dummy_handler    harq_timeout_handler;

  cell_config_builder_params cfg_builder_params;
  sched_config_manager       cfg_mng{scheduler_config{sched_cfg, mac_notif, metrics_notif}, metrics_ue_handler};
  const cell_configuration&  cell_cfg;

  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl pucch_alloc{cell_cfg, expert_cfg.max_pucchs_per_slot, expert_cfg.max_ul_grants_per_slot};
  uci_allocator_impl   uci_alloc{pucch_alloc};

  ue_repository          ues;
  ue_cell_grid_allocator alloc{expert_cfg, ues, srslog::fetch_basic_logger("SCHED")};

  slot_point current_slot;
};

TEST_F(ue_grid_allocator_tester,
       when_ue_dedicated_ss_is_css_then_allocation_is_within_coreset_start_crb_and_coreset0_end_crb)
{
  static const unsigned nof_bytes_to_schedule = 40U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change SS type to common.
  (*ue_creation_req.cfg.cells)[0]
      .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
      .set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);

  ue& u = add_ue(ue_creation_req);

  const crb_interval crbs =
      get_coreset_crbs((*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().coresets.back());
  const crb_interval crb_lims = {
      crbs.start(), crbs.start() + cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()};

  ue_pdsch_grant grant{.user                  = &u,
                       .cell_index            = to_du_cell_index(0),
                       .h_id                  = to_harq_id(0),
                       .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_dl_grant(grant).status, alloc_status::success);
  ASSERT_TRUE(crb_lims.contains(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1()));
}

TEST_F(ue_grid_allocator_tester, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
{
  static const unsigned nof_bytes_to_schedule = 40U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                       = to_du_ue_index(0);
  ue_creation_req.crnti                                                          = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#2 uses non-fallback DCI format hence the MCS table set in dedicated PDSCH configuration must be used.
  const ue_pdsch_grant grant{.user                  = &u,
                             .cell_index            = to_du_cell_index(0),
                             .h_id                  = to_harq_id(0),
                             .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_dl_grant(grant).status, alloc_status::success);
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam256);
}

TEST_F(ue_grid_allocator_tester, remaining_dl_rbs_are_allocated_if_max_pucch_per_slot_is_reached)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);
  ue_creation_req.ue_index = to_du_ue_index(1);
  ue_creation_req.crnti    = to_rnti(0x4602);
  const ue& u2             = add_ue(ue_creation_req);

  static const unsigned sched_bytes = 20U;
  const ue_pdsch_grant  grant1{
       .user = &u1, .cell_index = to_du_cell_index(0), .h_id = to_harq_id(0), .recommended_nof_bytes = sched_bytes};

  // Successfully allocates RBs corresponding to the grant.
  ASSERT_EQ(alloc.allocate_dl_grant(grant1).status, alloc_status::success);
  ASSERT_GE(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().tb_size_bytes, sched_bytes);

  // Since UE dedicated SearchSpace is a UE specific SearchSpace (Not CSS). Entire BWP CRBs can be used for allocation.
  const unsigned total_crbs     = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
  const unsigned crbs_allocated = res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().length();

  const ue_pdsch_grant grant2{
      .user = &u2, .cell_index = to_du_cell_index(0), .h_id = to_harq_id(0), .recommended_nof_bytes = sched_bytes};

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(alloc.allocate_dl_grant(grant2).status, alloc_status::success);
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().length(), (total_crbs - crbs_allocated));
}

TEST_F(ue_grid_allocator_tester, remaining_ul_rbs_are_allocated_if_max_ul_grant_per_slot_is_reached)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);
  ue_creation_req.ue_index = to_du_ue_index(1);
  ue_creation_req.crnti    = to_rnti(0x4602);
  const ue& u2             = add_ue(ue_creation_req);

  const unsigned recommended_nof_bytes_to_schedule = 200U;
  const unsigned max_nof_rbs_to_schedule           = 10U;

  const crb_interval   cell_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                    cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.stop()};
  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  // Successfully allocates RBs corresponding to the grant.
  ASSERT_EQ(alloc.allocate_ul_grant(grant1).status, alloc_status::success);
  unsigned k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
                    ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
                    .k2;

  const unsigned remaining_crbs =
      cell_crbs.length() - res_grid[k2].result.ul.puschs.back().pusch_cfg.rbs.type1().length();
  const ue_pusch_grant grant2{.user                  = &u2,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(alloc.allocate_ul_grant(grant2).status, alloc_status::success);
  k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
           ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
           .k2;
  ASSERT_EQ(res_grid[k2].result.ul.puschs.back().pusch_cfg.rbs.type1().length(), remaining_crbs);
}

TEST_F(ue_grid_allocator_tester, no_two_pdschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant{.user                  = &u,
                             .cell_index            = to_du_cell_index(0),
                             .h_id                  = to_harq_id(0),
                             .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_dl_grant(grant).status, alloc_status::success);

  // Second PDSCH grant for the UE.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  // Second PDSCH grant should not be allocated.
  ASSERT_NE(alloc.allocate_dl_grant(grant2).status, alloc_status::success);
}

TEST_F(ue_grid_allocator_tester, no_two_puschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PUSCH grant for the UE.
  const ue_pusch_grant grant{.user                  = &u,
                             .cell_index            = to_du_cell_index(0),
                             .h_id                  = to_harq_id(0),
                             .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_ul_grant(grant).status, alloc_status::success);

  // Second PUSCH grant for the UE.
  const ue_pusch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  // Second PUSCH grant should not be allocated.
  ASSERT_NE(alloc.allocate_ul_grant(grant2).status, alloc_status::success);
}

TEST_F(ue_grid_allocator_tester, consecutive_puschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  slot_point last_pusch_alloc_slot;

  // First PUSCH grant for the UE.
  const ue_pusch_grant grant{.user                  = &u,
                             .cell_index            = to_du_cell_index(0),
                             .h_id                  = to_harq_id(0),
                             .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_ul_grant(grant).status, alloc_status::success);
  unsigned k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
                    ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
                    .k2;
  last_pusch_alloc_slot = current_slot + k2;

  // Second PUSCH grant in the same slot for the UE.
  const ue_pusch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  const auto outcome = alloc.allocate_ul_grant(grant2);
  if (outcome.status == alloc_status::success) {
    k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
             ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
             .k2;
    ASSERT_GT(current_slot + k2, last_pusch_alloc_slot);
    last_pusch_alloc_slot = current_slot + k2;
  }

  run_slot();

  // Third PUSCH grant in the next slot for the UE.
  const ue_pusch_grant grant3{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(2),
                              .recommended_nof_bytes = nof_bytes_to_schedule};
  ASSERT_EQ(alloc.allocate_ul_grant(grant3).status, alloc_status::success);
  k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
           ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
           .k2;
  ASSERT_GT(current_slot + k2, last_pusch_alloc_slot);
}

TEST_F(ue_grid_allocator_tester,
       ack_slot_of_consecutive_pdschs_for_a_ue_must_be_greater_than_or_equal_to_last_ack_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  slot_point last_pdsch_ack_slot;

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant{.user                  = &u,
                             .cell_index            = to_du_cell_index(0),
                             .h_id                  = to_harq_id(0),
                             .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_EQ(alloc.allocate_dl_grant(grant).status, alloc_status::success);
  const search_space_info* ss_info =
      u.get_pcell().cfg().find_search_space(res_grid[0].result.dl.dl_pdcchs.back().ctx.context.ss_id);
  unsigned k1 =
      ss_info
          ->get_k1_candidates()[*res_grid[0].result.dl.dl_pdcchs.back().dci.c_rnti_f1_1.pdsch_harq_fb_timing_indicator];
  last_pdsch_ack_slot = current_slot + k1;

  // Second PDSCH grant in the same slot for the UE.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  const auto outcome = alloc.allocate_dl_grant(grant2);
  if (outcome.status == srsran::alloc_status::success) {
    ss_info = u.get_pcell().cfg().find_search_space(res_grid[0].result.dl.dl_pdcchs.back().ctx.context.ss_id);
    k1      = ss_info->get_k1_candidates()
             [*res_grid[0].result.dl.dl_pdcchs.back().dci.c_rnti_f1_1.pdsch_harq_fb_timing_indicator];
    ASSERT_GE(current_slot + k1, last_pdsch_ack_slot);
    last_pdsch_ack_slot = current_slot + k1;
  }

  run_slot();

  // Third PDSCH grant in the next slot for the UE.
  const ue_pdsch_grant grant3{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(2),
                              .recommended_nof_bytes = nof_bytes_to_schedule};
  ASSERT_EQ(alloc.allocate_dl_grant(grant3).status, alloc_status::success);
  ss_info = u.get_pcell().cfg().find_search_space(res_grid[0].result.dl.dl_pdcchs.back().ctx.context.ss_id);
  k1 =
      ss_info
          ->get_k1_candidates()[*res_grid[0].result.dl.dl_pdcchs.back().dci.c_rnti_f1_1.pdsch_harq_fb_timing_indicator];
  ASSERT_GE(current_slot + k1, last_pdsch_ack_slot);
}
