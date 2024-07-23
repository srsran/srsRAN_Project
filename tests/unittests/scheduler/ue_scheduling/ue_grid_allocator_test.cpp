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
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdcch/search_space.h"
#include <gtest/gtest.h>
#include <tests/test_doubles/scheduler/scheduler_result_test.h>

using namespace srsran;

class ue_grid_allocator_tester : public ::testing::TestWithParam<duplex_mode>
{
protected:
  ue_grid_allocator_tester(
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config()) :
    sched_cfg(sched_cfg_),
    cell_cfg(*[this]() {
      cfg_builder_params.dl_arfcn = GetParam() == duplex_mode::FDD ? 530000 : 520002;
      cfg_builder_params.scs_common =
          GetParam() == duplex_mode::FDD ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz30;
      cfg_builder_params.band           = band_helper::get_band_from_dl_arfcn(cfg_builder_params.dl_arfcn);
      cfg_builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      auto* cfg = cfg_mng.add_cell(test_helpers::make_default_sched_cell_configuration_request(cfg_builder_params));
      srsran_assert(cfg != nullptr, "Cell configuration failed");
      return cfg;
    }()),
    current_slot(cfg_builder_params.scs_common, 0)
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

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
    logger.set_context(current_slot.sfn(), current_slot.slot_index());

    res_grid.slot_indication(current_slot);
    pdcch_alloc.slot_indication(current_slot);
    pucch_alloc.slot_indication(current_slot);
    uci_alloc.slot_indication(current_slot);
    ues.slot_indication(current_slot);

    // Log scheduler results.
    res_logger.on_scheduler_result(res_grid[0].result);
  }

  bool run_until(unique_function<bool()> condition, unsigned max_slot_count = 1000)
  {
    for (unsigned count = 0; count != max_slot_count; ++count) {
      if (condition()) {
        return true;
      }
      run_slot();
    }
    return false;
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

  scheduler_expert_config                 sched_cfg;
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

  srslog::basic_logger&   logger{srslog::fetch_basic_logger("SCHED")};
  scheduler_result_logger res_logger{false, cell_cfg.pci};

  ue_repository          ues;
  ue_cell_grid_allocator alloc{expert_cfg, ues, logger};

  slot_point current_slot;
};

TEST_P(ue_grid_allocator_tester,
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

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_TRUE(crb_lims.contains(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1()));
}

TEST_P(ue_grid_allocator_tester, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
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

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam256);
}

TEST_P(ue_grid_allocator_tester, allocates_pdsch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  static const unsigned sched_bytes             = 2000U;
  const unsigned        max_nof_rbs_to_schedule = 10U;

  const ue_pdsch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = sched_bytes,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH corresponding to the grant.
  ASSERT_GE(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            grant1.max_nof_rbs);
}

TEST_P(ue_grid_allocator_tester, allocates_pusch_restricted_to_recommended_max_nof_rbs)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  const unsigned recommended_nof_bytes_to_schedule = 2000U;
  const unsigned max_nof_rbs_to_schedule           = 10U;

  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), grant1.max_nof_rbs);
}

TEST_P(ue_grid_allocator_tester, does_not_allocate_pusch_with_all_remaining_rbs_if_its_a_sr_indication)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  ue& u1                   = add_ue(ue_creation_req);
  // Trigger a SR indication.
  u1.handle_sr_indication();

  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = u1.pending_ul_newtx_bytes()};

  const crb_interval cell_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                  cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.stop()};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_LT(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), cell_crbs.length());
}

TEST_P(ue_grid_allocator_tester, no_two_pdschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  // Second PDSCH grant for the UE.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() {
    return alloc.allocate_dl_grant(grant1).status == alloc_status::success or
           alloc.allocate_dl_grant(grant2).status == alloc_status::success;
  }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));

  // Only one PDSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.size(), 1);
}

TEST_P(ue_grid_allocator_tester, no_two_puschs_are_allocated_in_same_slot_for_a_ue)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PUSCH grant for the UE.
  const ue_pusch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  // Second PUSCH grant for the UE.
  const ue_pusch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() {
    return alloc.allocate_ul_grant(grant1).status == alloc_status::success or
           alloc.allocate_ul_grant(grant2).status == alloc_status::success;
  }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));

  // Only one PUSCH per slot per UE.
  ASSERT_EQ(res_grid[0].result.ul.puschs.size(), 1);
}

TEST_P(ue_grid_allocator_tester, consecutive_puschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PUSCH grant for the UE.
  const ue_pusch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));
  slot_point last_pusch_alloc_slot = current_slot;

  run_slot();

  // Second PUSCH grant for the UE.
  const ue_pusch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant2).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u.crnti, res_grid[0].result.ul) != nullptr; }));
  ASSERT_GT(current_slot, last_pusch_alloc_slot);
}

TEST_P(ue_grid_allocator_tester, consecutive_pdschs_for_a_ue_are_allocated_in_increasing_order_of_time)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_slot = current_slot;

  run_slot();

  // Second PDSCH grant in the same slot for the UE.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant2).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot, last_pdsch_slot);
}

TEST_P(ue_grid_allocator_tester,
       ack_slot_of_consecutive_pdschs_for_a_ue_must_be_greater_than_or_equal_to_last_ack_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule = 400U;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  slot_point last_pdsch_ack_slot = current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1;

  run_slot();

  // Second PDSCH grant in the same slot for the UE.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant2).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  ASSERT_GE(current_slot + find_ue_pdsch(u.crnti, res_grid[0].result.dl.ue_grants)->context.k1, last_pdsch_ack_slot);
}

TEST_P(ue_grid_allocator_tester, successfully_allocated_pdsch_even_with_large_gap_to_last_pdsch_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule                       = 8U;
  const unsigned        nof_slot_until_pdsch_is_allocated_threshold = SCHEDULER_MAX_K0;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    run_slot();
  }

  // First PDSCH grant for the UE.
  const ue_pdsch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));

  // Ensure next PDSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PDSCH slot)
  // and current slot value is less than last allocated PDSCH slot. e.g. next PDSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_system_frame() / 2 + current_slot.nof_slots_per_frame(); ++i) {
    run_slot();
  }

  // Next PDSCH grant to be allocated.
  const ue_pdsch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant2).status == alloc_status::success; },
                        nof_slot_until_pdsch_is_allocated_threshold));
}

TEST_P(ue_grid_allocator_tester, successfully_allocated_pusch_even_with_large_gap_to_last_pusch_slot_allocated)
{
  static const unsigned nof_bytes_to_schedule                       = 400U;
  const unsigned        nof_slot_until_pusch_is_allocated_threshold = SCHEDULER_MAX_K2;

  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);

  const ue& u = add_ue(ue_creation_req);

  // Ensure current slot is the middle of 1024 SFNs. i.e. current slot=511.0
  while (current_slot.sfn() != NOF_SFNS / 2) {
    run_slot();
  }

  // First PUSCH grant for the UE.
  const ue_pusch_grant grant1{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));

  // Ensure next PUSCH to be allocated slot is after wrap around of 1024 SFNs (large gap to last allocated PUSCH slot)
  // and current slot value is less than last allocated PUSCH slot. e.g. next PUSCH to be allocated slot=SFN 2, slot 2
  // after wrap around of 1024 SFNs.
  for (unsigned i = 0; i < current_slot.nof_slots_per_system_frame() / 2 + current_slot.nof_slots_per_frame(); ++i) {
    run_slot();
  }

  // Second PUSCH grant for the UE.
  const ue_pusch_grant grant2{.user                  = &u,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(1),
                              .recommended_nof_bytes = nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant2).status == alloc_status::success; },
                        nof_slot_until_pusch_is_allocated_threshold));
}

class ue_grid_allocator_remaining_rbs_alloc_tester : public ue_grid_allocator_tester
{
public:
  ue_grid_allocator_remaining_rbs_alloc_tester() :
    ue_grid_allocator_tester(([]() {
      scheduler_expert_config sched_cfg_   = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.max_ul_grants_per_slot = 2;
      sched_cfg_.ue.max_pucchs_per_slot    = 2;
      return sched_cfg_;
    }()))
  {
  }
};

TEST_P(ue_grid_allocator_remaining_rbs_alloc_tester, remaining_dl_rbs_are_allocated_if_max_pucch_per_slot_is_reached)
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

  // Since UE dedicated SearchSpace is a UE specific SearchSpace (Not CSS). Entire BWP CRBs can be used for allocation.
  const unsigned       total_crbs = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs.length();
  const ue_pdsch_grant grant2{
      .user = &u2, .cell_index = to_du_cell_index(0), .h_id = to_harq_id(0), .recommended_nof_bytes = sched_bytes};

  ASSERT_TRUE(run_until([&]() {
    return alloc.allocate_dl_grant(grant1).status == alloc_status::success and
           alloc.allocate_dl_grant(grant2).status == alloc_status::success;
  }));
  ASSERT_TRUE(run_until([&]() {
    return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr and
           find_ue_pdsch(u2.crnti, res_grid[0].result.dl.ue_grants) != nullptr;
  }));
  // Successfully allocates PDSCH corresponding to the grant.
  ASSERT_GE(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.codewords.back().tb_size_bytes,
            sched_bytes);

  // Since UE dedicated SearchSpace is a UE specific SearchSpace (Not CSS). Entire BWP CRBs can be used for allocation.
  const unsigned crbs_allocated =
      find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length();

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(find_ue_pdsch(u2.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            (total_crbs - crbs_allocated));
}

TEST_P(ue_grid_allocator_remaining_rbs_alloc_tester, remaining_ul_rbs_are_allocated_if_max_ul_grant_per_slot_is_reached)
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

  const crb_interval   cell_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                    cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.stop()};
  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule};
  const ue_pusch_grant grant2{.user                  = &u2,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule};

  ASSERT_TRUE(run_until([&]() {
    return alloc.allocate_ul_grant(grant1).status == alloc_status::success and
           alloc.allocate_ul_grant(grant2).status == alloc_status::success;
  }));
  ASSERT_TRUE(run_until([&]() {
    return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr and find_ue_pusch(u2.crnti, res_grid[0].result.ul);
  }));
  // Successfully allocates PUSCH corresponding to the grant.
  ASSERT_GE(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.tb_size_bytes, grant1.recommended_nof_bytes);

  const unsigned remaining_crbs =
      cell_crbs.length() - find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length();

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(find_ue_pusch(u2.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(), remaining_crbs);
}

class ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester : public ue_grid_allocator_tester
{
public:
  ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester() :
    ue_grid_allocator_tester(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_nof_rbs        = {20, 40};
      sched_cfg_.ue.pusch_nof_rbs        = {20, 40};
      return sched_cfg_;
    }()))
  {
  }
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20U;
  const unsigned        max_nof_rbs_to_schedule = 10U;

  const ue_pdsch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = sched_bytes,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pdsch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20000U;
  const unsigned        max_nof_rbs_to_schedule = 273U;

  const ue_pdsch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = sched_bytes,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_min_nof_rbs_even_if_rbs_required_to_schedule_is_low)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is low enough such that < 20 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 20U;
  const unsigned max_nof_rbs_to_schedule           = 10U;

  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::max(expert_cfg.pdsch_nof_rbs.start(), max_nof_rbs_to_schedule));
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
       allocates_pusch_with_expert_cfg_max_nof_rbs_even_if_rbs_required_to_schedule_is_high)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 40 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 200000U;
  const unsigned max_nof_rbs_to_schedule           = 273U;

  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1().length(),
            std::min(expert_cfg.pdsch_nof_rbs.stop(), max_nof_rbs_to_schedule));
}

class ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester : public ue_grid_allocator_tester
{
public:
  ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester() :
    ue_grid_allocator_tester(([]() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();
      sched_cfg_.ue.pdsch_crb_limits     = {20, 40};
      sched_cfg_.ue.pusch_crb_limits     = {20, 40};
      return sched_cfg_;
    }()))
  {
    // Assume SS#2 is USS configured with DCI format 1_1/0_1 and is the only SS used for UE PDSCH/PUSCH scheduling.
    const prb_interval pdsch_prbs =
        crb_to_prb(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.crbs, sched_cfg.ue.pdsch_crb_limits);
    pdsch_vrb_limits = vrb_interval{pdsch_prbs.start(), pdsch_prbs.stop()};
    pusch_vrb_limits = rb_helper::crb_to_vrb_ul_non_interleaved(
        sched_cfg.ue.pusch_crb_limits, cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start());
  }

protected:
  vrb_interval pdsch_vrb_limits;
  vrb_interval pusch_vrb_limits;
};

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pdsch_within_expert_cfg_pdsch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  static const unsigned sched_bytes             = 20000U;
  const unsigned        max_nof_rbs_to_schedule = 273U;

  const ue_pdsch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = sched_bytes,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_dl_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants) != nullptr; }));
  // Successfully allocates PDSCH within RB limits.
  ASSERT_EQ(find_ue_pdsch(u1.crnti, res_grid[0].result.dl.ue_grants)->pdsch_cfg.rbs.type1(), pdsch_vrb_limits);
}

TEST_P(ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester, allocates_pdsch_within_expert_cfg_pusch_rb_limits)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);

  // Ensure the buffer status is high enough such that > 20 RBs (configured in constructor) are required to schedule.
  const unsigned recommended_nof_bytes_to_schedule = 200000U;
  const unsigned max_nof_rbs_to_schedule           = 273U;

  const ue_pusch_grant grant1{.user                  = &u1,
                              .cell_index            = to_du_cell_index(0),
                              .h_id                  = to_harq_id(0),
                              .recommended_nof_bytes = recommended_nof_bytes_to_schedule,
                              .max_nof_rbs           = max_nof_rbs_to_schedule};

  ASSERT_TRUE(run_until([&]() { return alloc.allocate_ul_grant(grant1).status == alloc_status::success; }));
  ASSERT_TRUE(run_until([&]() { return find_ue_pusch(u1.crnti, res_grid[0].result.ul) != nullptr; }));
  // Successfully allocates PUSCH within RB limits.
  ASSERT_EQ(find_ue_pusch(u1.crnti, res_grid[0].result.ul)->pusch_cfg.rbs.type1(), pusch_vrb_limits);
}

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_remaining_rbs_alloc_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_nof_rbs_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));

INSTANTIATE_TEST_SUITE_P(ue_grid_allocator_test,
                         ue_grid_allocator_expert_cfg_pxsch_crb_limits_tester,
                         testing::Values(duplex_mode::FDD, duplex_mode::TDD));
