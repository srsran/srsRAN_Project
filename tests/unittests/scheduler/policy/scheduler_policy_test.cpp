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
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/policy/scheduler_time_rr.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

enum class policy_type { time_rr };

class base_scheduler_policy_test
{
protected:
  base_scheduler_policy_test(
      policy_type             policy,
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config(),
      const sched_cell_configuration_request_message& msg =
          test_helpers::make_default_sched_cell_configuration_request()) :
    logger(srslog::fetch_basic_logger("SCHED", true)), sched_cfg(sched_cfg_), cell_cfg(*[this, &msg]() {
      return cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(sched_cfg, msg)).get();
    }())
  {
    logger.set_level(srslog::basic_levels::debug);
    srslog::init();

    grid_alloc.add_cell(to_du_cell_index(0), pdcch_alloc, uci_alloc, res_grid);
    ue_res_grid.add_cell(res_grid);

    switch (policy) {
      case policy_type::time_rr:
        sched = std::make_unique<scheduler_time_rr>(sched_cfg.ue);
        break;
      default:
        report_fatal_error("Invalid policy");
    }
  }

  ~base_scheduler_policy_test() { srslog::flush(); }

  void run_slot()
  {
    grid_alloc.slot_indication(next_slot);

    res_grid.slot_indication(next_slot);
    pdcch_alloc.slot_indication(next_slot);
    pucch_alloc.slot_indication(next_slot);
    uci_alloc.slot_indication(next_slot);

    if (cell_cfg.is_dl_enabled(next_slot)) {
      sched->dl_sched(grid_alloc, ue_res_grid, ues);
      sched->ul_sched(grid_alloc, ue_res_grid, ues);
    }

    next_slot++;
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        make_ue_create_req(ue_index, to_rnti(0x4601 + (unsigned)ue_index), lcids_to_activate, uint_to_lcg_id(0));
    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_req)
  {
    ue_ded_cell_cfg_list.push_back(
        std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, ue_req.cfg));
    ues.add_ue(std::make_unique<ue>(
        ue_creation_command{*ue_ded_cell_cfg_list.back(), ue_req.starts_in_fallback, harq_timeout_handler}));
    return ues[ue_req.ue_index];
  }

  sched_ue_creation_request_message make_ue_create_req(du_ue_index_t                        ue_index,
                                                       rnti_t                               rnti,
                                                       const std::initializer_list<lcid_t>& lcids_to_activate,
                                                       lcg_id_t                             lcg_id)
  {
    sched_ue_creation_request_message req = test_helpers::create_default_sched_ue_creation_request();
    req.ue_index                          = ue_index;
    req.crnti                             = rnti;
    auto default_lc_cfg                   = config_helpers::create_default_logical_channel_config(uint_to_lcid(0));
    default_lc_cfg.lc_group               = lcg_id;
    req.cfg.lc_config_list.emplace();
    for (lcid_t lcid : lcids_to_activate) {
      default_lc_cfg.lcid = lcid;
      req.cfg.lc_config_list->push_back(default_lc_cfg);
    }
    return req;
  }

  void push_dl_bs(du_ue_index_t ue_index, lcid_t lcid, unsigned bytes)
  {
    dl_buffer_state_indication_message ind{};
    ind.ue_index = ue_index;
    ind.lcid     = lcid;
    ind.bs       = bytes;
    ues[ue_index].handle_dl_buffer_state_indication(ind);
  }

  void notify_ul_bsr(du_ue_index_t ue_index, lcg_id_t lcg_id, unsigned bytes)
  {
    // Assumptions:
    // - Only one LCG is assumed to have data to send.
    // - BSR is Short BSR.
    ul_bsr_indication_message msg{
        to_du_cell_index(0), ue_index, to_rnti(0x4601 + (unsigned)ue_index), bsr_format::SHORT_BSR, {}};
    msg.reported_lcgs.push_back(ul_bsr_lcg_report{lcg_id, bytes});

    ues[ue_index].handle_bsr_indication(msg);
  }

  srslog::basic_logger&                          logger;
  scheduler_expert_config                        sched_cfg;
  cell_common_configuration_list                 cell_cfg_list;
  std::vector<std::unique_ptr<ue_configuration>> ue_ded_cell_cfg_list;

  const cell_configuration&            cell_cfg;
  sched_cfg_dummy_notifier             dummy_mac_notif;
  scheduler_ue_metrics_dummy_notifier  metrics_notif;
  scheduler_harq_timeout_dummy_handler harq_timeout_handler;

  cell_resource_allocator       res_grid{cell_cfg};
  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl   pucch_alloc{cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot};
  uci_allocator_impl     uci_alloc{pucch_alloc};
  ue_resource_grid_view  ue_res_grid;
  ue_repository          ues;
  ue_cell_grid_allocator grid_alloc{sched_cfg.ue, ues, logger};
  std::unique_ptr<scheduler_policy> sched;
  slot_point                        next_slot{0, test_rgen::uniform_int<unsigned>(0, 10239)};
};

class scheduler_policy_test : public base_scheduler_policy_test, public ::testing::TestWithParam<policy_type>
{
protected:
  scheduler_policy_test() : base_scheduler_policy_test(GetParam()) {}
};

TEST_P(scheduler_policy_test, when_coreset0_used_then_dl_grant_is_within_bounds_of_coreset0_rbs)
{
  auto  ue_req  = make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(0));
  auto& ss_list = (*ue_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces;
  ss_list.clear();
  ss_list.push_back(config_helpers::make_default_common_search_space_config());
  ss_list.back().set_non_ss0_id(to_search_space_id(2));
  // Note: We use Aggregation Level 2 to avoid collisions with CORESET#0 PDCCH candidates.
  ss_list.back().set_non_ss0_nof_candidates({0, 2, 0, 0, 0});
  ue& u = add_ue(ue_req);
  // Note: set CQI=15 to use low aggregation level.
  u.get_pcell().handle_csi_report(
      csi_report_data{std::nullopt, std::nullopt, std::nullopt, std::nullopt, cqi_value{15U}});
  push_dl_bs(u.ue_index, uint_to_lcid(4), 100000000);

  run_slot();

  ASSERT_FALSE(this->res_grid[0].result.dl.ue_grants.empty());
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.back().context.ue_index, u.ue_index);
  crb_interval crbs         = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs();
  prb_interval alloced_prbs = prb_interval{this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().start(),
                                           this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().stop()};
  ASSERT_EQ(prb_to_crb(crbs, alloced_prbs), crbs);
}

TEST_P(scheduler_policy_test, scheduler_uses_only_specific_searchspaces_defined_in_dedicated_configuration)
{
  sched_ue_creation_request_message ue_req =
      make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(4)}, uint_to_lcg_id(1));

  const ue& u = add_ue(ue_req);
  push_dl_bs(u.ue_index, uint_to_lcid(4), 1053);
  notify_ul_bsr(u.ue_index, uint_to_lcg_id(1), 1053);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.size(), 1);
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.size(), 1);
  // SearchSpace#2 is the only SearchSpace defined in UE dedicated configuration.
  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.back().ctx.context.ss_id, to_search_space_id(2));
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.back().ctx.context.ss_id, to_search_space_id(2));
}

TEST_P(scheduler_policy_test, scheduler_allocates_more_than_one_ue_in_case_their_bsr_is_low)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  notify_ul_bsr(u1.ue_index, lcg_id, 10);
  notify_ul_bsr(u2.ue_index, lcg_id, 10);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.size(), 2);
  ASSERT_NE(this->res_grid[0].result.dl.ul_pdcchs[0].ctx.rnti, this->res_grid[0].result.dl.ul_pdcchs[1].ctx.rnti);
  // NOTE: Both UEs have same PUSCH time domain resources configured.
  span<const pusch_time_domain_resource_allocation> pusch_td_res_list =
      u1.get_pcell().cfg().cell_cfg_common.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
  // In default UE dedicated configuration, SearchSpace#2 is configured to use DCI format 1_1/0_1.
  if (this->res_grid[0].result.dl.ul_pdcchs[0].dci.c_rnti_f0_1.time_resource ==
      this->res_grid[0].result.dl.ul_pdcchs[1].dci.c_rnti_f0_1.time_resource) {
    const unsigned k2 = pusch_td_res_list[this->res_grid[0].result.dl.ul_pdcchs[0].dci.c_rnti_f0_1.time_resource].k2;
    const auto&    pusch_res_grid = this->res_grid[k2];
    ASSERT_EQ(pusch_res_grid.result.ul.puschs.size(), 2);
    ASSERT_FALSE(pusch_res_grid.result.ul.puschs[0].pusch_cfg.rbs.type1().overlaps(
        pusch_res_grid.result.ul.puschs[1].pusch_cfg.rbs.type1()));
  }
}

TEST_P(scheduler_policy_test, scheduler_allocates_more_than_one_ue_in_case_their_dl_buffer_state_is_low)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  push_dl_bs(u1.ue_index, uint_to_lcid(5), 10);
  push_dl_bs(u2.ue_index, uint_to_lcid(5), 10);

  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.dl_pdcchs.size(), 2);
  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 2);
  ASSERT_NE(this->res_grid[0].result.dl.dl_pdcchs[0].ctx.rnti, this->res_grid[0].result.dl.dl_pdcchs[1].ctx.rnti);
  ASSERT_FALSE(this->res_grid[0].result.dl.ue_grants[0].pdsch_cfg.rbs.type1().overlaps(
      this->res_grid[0].result.dl.ue_grants[1].pdsch_cfg.rbs.type1()));
}

TEST_P(scheduler_policy_test, scheduler_allocates_ues_with_sr_opportunity_first_than_ues_with_only_ul_data)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  ue&       u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));

  notify_ul_bsr(u1.ue_index, lcg_id, 200);
  u2.handle_sr_indication();

  run_slot();

  ASSERT_GE(this->res_grid[0].result.dl.ul_pdcchs.size(), 1);
  ASSERT_EQ(this->res_grid[0].result.dl.ul_pdcchs.front().ctx.rnti, to_rnti(0x4602))
      << fmt::format("UE with SR opportunity should have been scheduled first.");
}

class scheduler_policy_partial_slot_tdd_test : public base_scheduler_policy_test,
                                               public ::testing::TestWithParam<policy_type>
{
protected:
  scheduler_policy_partial_slot_tdd_test() :
    base_scheduler_policy_test(GetParam(), config_helpers::make_default_scheduler_expert_config(), []() {
      cell_config_builder_params builder_params{};
      // Band 40.
      builder_params.dl_arfcn       = 465000;
      builder_params.scs_common     = subcarrier_spacing::kHz30;
      builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
      builder_params.channel_bw_mhz = srsran::bs_channel_bandwidth_fr1::MHz20;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          builder_params.channel_bw_mhz,
          builder_params.scs_common,
          builder_params.band.has_value() ? band_helper::get_freq_range(builder_params.band.value())
                                          : frequency_range::FR1);

      std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
          band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                      *builder_params.band,
                                                      nof_crbs,
                                                      builder_params.scs_common,
                                                      builder_params.scs_common,
                                                      builder_params.search_space0_index,
                                                      builder_params.max_coreset0_duration);
      srsran_assert(ssb_freq_loc.has_value(), "Invalid cell config parameters");
      builder_params.offset_to_point_a    = ssb_freq_loc->offset_to_point_A;
      builder_params.k_ssb                = ssb_freq_loc->k_ssb;
      builder_params.coreset0_index       = ssb_freq_loc->coreset0_idx;
      builder_params.tdd_ul_dl_cfg_common = tdd_ul_dl_config_common{.ref_scs  = subcarrier_spacing::kHz30,
                                                                    .pattern1 = {.dl_ul_tx_period_nof_slots = 10,
                                                                                 .nof_dl_slots              = 5,
                                                                                 .nof_dl_symbols            = 5,
                                                                                 .nof_ul_slots              = 4,
                                                                                 .nof_ul_symbols            = 0}};
      return test_helpers::make_default_sched_cell_configuration_request(builder_params);
    }())
  {
    next_slot = {to_numerology_value(subcarrier_spacing::kHz30), 0};
  }
};

TEST_P(scheduler_policy_partial_slot_tdd_test, scheduler_allocates_in_partial_slot_tdd)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation is extended over partial slot.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 100000);

  // Run until one slot before partial slot.
  while (cell_cfg.is_fully_dl_enabled(next_slot) or cell_cfg.is_fully_ul_enabled(next_slot)) {
    run_slot();
  }
  // Run for partial slot.
  run_slot();

  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 1);
}

class scheduler_round_robin_test : public base_scheduler_policy_test, public ::testing::Test
{
protected:
  scheduler_round_robin_test() : base_scheduler_policy_test(policy_type::time_rr) {}
};

TEST_F(scheduler_round_robin_test, round_robin_does_not_account_ues_with_empty_buffers)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));
  const ue& u2     = add_ue(make_ue_create_req(to_du_ue_index(1), to_rnti(0x4602), {uint_to_lcid(5)}, lcg_id));
  const ue& u3     = add_ue(make_ue_create_req(to_du_ue_index(2), to_rnti(0x4603), {uint_to_lcid(5)}, lcg_id));

  push_dl_bs(u1.ue_index, uint_to_lcid(5), 1000000);
  push_dl_bs(u3.ue_index, uint_to_lcid(5), 1000000);

  std::array<du_ue_index_t, 2> rr_ues = {u1.ue_index, u3.ue_index};
  unsigned                     offset = 0;
  for (unsigned i = 0; i != 10; ++i) {
    run_slot();
    ASSERT_GE(this->res_grid[0].result.dl.ue_grants.size(), 1);
    if (i == 0) {
      offset = this->res_grid[0].result.dl.ue_grants[0].context.ue_index == u1.ue_index ? 0 : 1;
      ASSERT_NE(this->res_grid[0].result.dl.ue_grants[0].context.ue_index, u2.ue_index);
    } else {
      ASSERT_EQ(this->res_grid[0].result.dl.ue_grants[0].context.ue_index, rr_ues[(i + offset) % rr_ues.size()]);
    }
  }
}

TEST_F(scheduler_round_robin_test, round_robin_must_not_attempt_to_allocate_twice_for_same_ue_in_one_slot)
{
  const lcg_id_t lcg_id = uint_to_lcg_id(2);
  ue&            u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  auto get_pdsch_grant_count_for_ue = [&](const rnti_t crnti) {
    return std::count_if(this->res_grid[0].result.dl.ue_grants.begin(),
                         this->res_grid[0].result.dl.ue_grants.end(),
                         [&](const dl_msg_alloc& grant) { return crnti == grant.pdsch_cfg.rnti; });
  };
  auto get_pusch_grant_count_for_ue = [&](const rnti_t crnti) {
    return std::count_if(this->res_grid[0].result.ul.puschs.begin(),
                         this->res_grid[0].result.ul.puschs.end(),
                         [&](const ul_sched_info& grant) { return crnti == grant.pusch_cfg.rnti; });
  };

  // Action: Push buffer status notification for DL + Ul and a SR indication.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 20000000);
  notify_ul_bsr(u1.ue_index, lcg_id, 2000000);
  u1.handle_sr_indication();

  // Action: Run for at least 256 slots or more so that there are some HARQs with pending reTx.
  // Status: Policy scheduler should not allocate reTx and new Tx for the same UE at the same time.
  for (unsigned i = 0; i != 512; ++i) {
    ASSERT_LE(get_pdsch_grant_count_for_ue(u1.crnti), 1) << fmt::format("At slot={}", next_slot);
    ASSERT_LE(get_pusch_grant_count_for_ue(u1.crnti), 1) << fmt::format("At slot={}", next_slot);
    run_slot();
  }
}

class scheduler_policy_alloc_bounds_test : public base_scheduler_policy_test,
                                           public ::testing::TestWithParam<policy_type>
{
protected:
  scheduler_policy_alloc_bounds_test() :
    base_scheduler_policy_test(GetParam(), []() {
      scheduler_expert_config sched_cfg_ = config_helpers::make_default_scheduler_expert_config();

      // Modify boundaries within which PDSCH and PUSCH needs to be allocated to UEs.
      sched_cfg_.ue.pdsch_crb_limits = {10, 15};
      sched_cfg_.ue.pusch_crb_limits = {10, 15};

      return sched_cfg_;
    }())
  {
  }
};

TEST_P(scheduler_policy_alloc_bounds_test, scheduler_allocates_pdsch_within_configured_boundaries)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation does not stop.
  push_dl_bs(u1.ue_index, uint_to_lcid(5), 100000);

  // Run for partial slot.
  run_slot();

  // Expected CRBs is based on the expert configuration passed to scheduler during class initialization.
  crb_interval expected_crb_allocation{10, 15};
  prb_interval expected_prb_interval = crb_to_prb(
      u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).dl_common->generic_params.crbs, expected_crb_allocation);
  vrb_interval expected_vrb_interval{expected_prb_interval.start(), expected_prb_interval.stop()};

  ASSERT_EQ(this->res_grid[0].result.dl.ue_grants.size(), 1);
  ASSERT_TRUE(this->res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1() == expected_vrb_interval);
}

TEST_P(scheduler_policy_alloc_bounds_test, scheduler_allocates_pusch_within_configured_boundaries)
{
  lcg_id_t  lcg_id = uint_to_lcg_id(2);
  const ue& u1     = add_ue(make_ue_create_req(to_du_ue_index(0), to_rnti(0x4601), {uint_to_lcid(5)}, lcg_id));

  // Buffer has to be large enough so that the allocation does not stop.
  notify_ul_bsr(u1.ue_index, lcg_id, 2000000);

  // Run until one slot before partial slot.
  while (this->res_grid[0].result.ul.puschs.empty()) {
    run_slot();
  }

  // Expected CRBs is based on the expert configuration passed to scheduler during class initialization.
  crb_interval expected_crb_allocation{10, 15};
  prb_interval expected_prb_interval = crb_to_prb(
      u1.get_pcell().cfg().bwp(u1.get_pcell().active_bwp_id()).ul_common->generic_params.crbs, expected_crb_allocation);
  vrb_interval expected_vrb_interval{expected_prb_interval.start(), expected_prb_interval.stop()};

  ASSERT_TRUE(this->res_grid[0].result.ul.puschs.back().pusch_cfg.rbs.type1() == expected_vrb_interval);
}

INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_policy_test, testing::Values(policy_type::time_rr));
INSTANTIATE_TEST_SUITE_P(scheduler_policy,
                         scheduler_policy_partial_slot_tdd_test,
                         testing::Values(policy_type::time_rr));
INSTANTIATE_TEST_SUITE_P(scheduler_policy, scheduler_policy_alloc_bounds_test, testing::Values(policy_type::time_rr));
