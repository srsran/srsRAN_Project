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

#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/duplex_mode.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

static cell_config_builder_params test_builder_params(duplex_mode duplx_mode)
{
  cell_config_builder_params builder_params{};
  if (duplx_mode == duplex_mode::TDD) {
    // Band 40.
    builder_params.dl_arfcn       = 474000;
    builder_params.scs_common     = srsran::subcarrier_spacing::kHz30;
    builder_params.band           = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
    builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

    const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
        builder_params.channel_bw_mhz,
        builder_params.scs_common,
        builder_params.band.has_value() ? band_helper::get_freq_range(builder_params.band.value())
                                        : frequency_range::FR1);

    optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc =
        band_helper::get_ssb_coreset0_freq_location(builder_params.dl_arfcn,
                                                    *builder_params.band,
                                                    nof_crbs,
                                                    builder_params.scs_common,
                                                    builder_params.scs_common,
                                                    builder_params.search_space0_index,
                                                    builder_params.max_coreset0_duration);
    builder_params.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
    builder_params.k_ssb             = ssb_freq_loc->k_ssb;
    builder_params.coreset0_index    = ssb_freq_loc->coreset0_idx;
  } else {
    builder_params.band = band_helper::get_band_from_dl_arfcn(builder_params.dl_arfcn);
  }

  return builder_params;
}

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  // Maximum number of slots to run per UE in order to validate the results of scheduler. Implementation defined.
  static constexpr unsigned max_test_run_slots_per_ue = 40;

  const scheduler_expert_config           sched_cfg;
  const scheduler_ue_expert_config&       expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                dummy_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_harq_timeout_dummy_handler    harq_timeout_handler;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;
  cell_config_builder_params              builder_params;

  sched_config_manager      cfg_mng{scheduler_config{sched_cfg, dummy_notif, metrics_notif}, metrics_ue_handler};
  const cell_configuration& cell_cfg;

  cell_resource_allocator       res_grid{cell_cfg};
  pdcch_resource_allocator_impl pdcch_sch{cell_cfg};
  pucch_allocator_impl          pucch_alloc{cell_cfg, 31U, 32U};
  uci_allocator_impl            uci_alloc{pucch_alloc};
  ue_repository                 ue_db;
  ue_cell_grid_allocator        ue_alloc;
  ue_srb0_scheduler             srb0_sched;

  explicit test_bench(const scheduler_expert_config&                  sched_cfg_,
                      const cell_config_builder_params&               builder_params_,
                      const sched_cell_configuration_request_message& cell_req) :
    sched_cfg{sched_cfg_},
    builder_params{builder_params_},
    cell_cfg{*[&]() { return cfg_mng.add_cell(cell_req); }()},
    ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED", true)),
    srb0_sched(expert_cfg, cell_cfg, pdcch_sch, pucch_alloc, ue_db)
  {
    ue_alloc.add_cell(cell_cfg.cell_index, pdcch_sch, uci_alloc, res_grid);
  }

  bool add_ue(const sched_ue_creation_request_message create_req)
  {
    auto ev = cfg_mng.add_ue(create_req);
    if (not ev.valid()) {
      return false;
    }

    // Add UE to UE DB.
    auto u = std::make_unique<ue>(
        ue_creation_command{ev.next_config(), create_req.starts_in_fallback, harq_timeout_handler});
    if (ue_db.contains(create_req.ue_index)) {
      // UE already exists.
      ev.abort();
      return false;
    }
    ue_db.add_ue(std::move(u));
    return true;
  }
};

class base_srb0_scheduler_tester
{
protected:
  slot_point                 current_slot{0, 0};
  srslog::basic_logger&      mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&      test_logger = srslog::fetch_basic_logger("TEST", true);
  scheduler_result_logger    result_logger;
  optional<test_bench>       bench;
  duplex_mode                duplx_mode;
  cell_config_builder_params builder_params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  base_srb0_scheduler_tester(duplex_mode duplx_mode_) :
    duplx_mode(duplx_mode_), builder_params(test_builder_params(duplx_mode))
  {
  }

  ~base_srb0_scheduler_tester()
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_expert_config& sched_cfg, const sched_cell_configuration_request_message& msg)
  {
    current_slot = slot_point{to_numerology_value(msg.scs_common), 0};

    bench.emplace(sched_cfg, builder_params, msg);

    const auto& dl_lst = bench->cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);
    bench->pucch_alloc.slot_indication(current_slot);
  }

  void run_slot()
  {
    current_slot++;

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    result_logger.on_slot_start();

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);

    bench->srb0_sched.run_slot(bench->res_grid);

    result_logger.on_scheduler_result(bench->res_grid[0].result);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  scheduler_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index) const
  {
    scheduler_expert_config     cfg   = config_helpers::make_default_scheduler_expert_config();
    scheduler_ue_expert_config& uecfg = cfg.ue;
    uecfg.dl_mcs                      = {10, 10};
    uecfg.ul_mcs                      = {10, 10};
    uecfg.max_nof_harq_retxs          = 4;
    uecfg.max_msg4_mcs                = max_msg4_mcs_index;
    return cfg;
  }

  sched_cell_configuration_request_message
  create_custom_cell_config_request(unsigned k0, const optional<tdd_ul_dl_config_common>& tdd_cfg = {})
  {
    if (duplx_mode == srsran::duplex_mode::TDD and tdd_cfg.has_value()) {
      builder_params.tdd_ul_dl_cfg_common = *tdd_cfg;
    }
    sched_cell_configuration_request_message msg =
        test_helpers::make_default_sched_cell_configuration_request(builder_params);
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = k0;
    return msg;
  }

  bool ue_is_allocated_pdcch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.dl.dl_pdcchs.begin(),
                       bench->res_grid[0].result.dl.dl_pdcchs.end(),
                       [&u](const auto& pdcch) { return pdcch.ctx.rnti == u.crnti; });
  }

  bool ue_is_allocated_pdsch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.dl.ue_grants.begin(),
                       bench->res_grid[0].result.dl.ue_grants.end(),
                       [&u](const auto& grant) { return grant.pdsch_cfg.rnti == u.crnti; });
  }

  bool ue_is_allocated_pucch(const ue& u)
  {
    return std::any_of(bench->res_grid[0].result.ul.pucchs.begin(),
                       bench->res_grid[0].result.ul.pucchs.end(),
                       [&u](const auto& pucch) { return pucch.crnti == u.crnti; });
  }

  bool tbs_scheduled_bytes_matches_given_size(const ue& u, unsigned exp_size)
  {
    unsigned total_cw_tb_size_bytes = 0;

    // Fetch PDSCH resource grid allocators.
    const cell_slot_resource_allocator& pdsch_alloc = bench->res_grid[0];
    // Search for PDSCH UE grant.
    for (const auto& grant : pdsch_alloc.result.dl.ue_grants) {
      if (grant.pdsch_cfg.rnti != u.crnti) {
        continue;
      }
      for (const auto& cw : grant.pdsch_cfg.codewords) {
        total_cw_tb_size_bytes += cw.tb_size_bytes;
      }
    }
    return total_cw_tb_size_bytes >= exp_size;
  }

  bool add_ue(rnti_t tc_rnti, du_ue_index_t ue_index)
  {
    // Add cell to UE cell grid allocator.
    auto ue_create_req     = test_helpers::create_default_sched_ue_creation_request(bench->builder_params);
    ue_create_req.crnti    = tc_rnti;
    ue_create_req.ue_index = ue_index;
    return bench->add_ue(ue_create_req);
  }

  void push_buffer_state_to_dl_ue(du_ue_index_t ue_idx, unsigned buffer_size)
  {
    // Notification from upper layers of DL buffer state.
    const dl_buffer_state_indication_message msg{ue_idx, LCID_SRB0, buffer_size};
    bench->ue_db[ue_idx].handle_dl_buffer_state_indication(msg);

    // Notify scheduler of DL buffer state.
    bench->srb0_sched.handle_dl_buffer_state_indication(ue_idx);
  }

  unsigned get_pending_bytes(du_ue_index_t ue_idx) { return bench->ue_db[ue_idx].pending_dl_srb0_newtx_bytes(); }

  const ue& get_ue(du_ue_index_t ue_idx) { return bench->ue_db[ue_idx]; }
};

// Parameters to be passed to test.
struct srb0_test_params {
  uint8_t     k0;
  duplex_mode duplx_mode;
};

class srb0_scheduler_tester : public base_srb0_scheduler_tester, public ::testing::TestWithParam<srb0_test_params>
{
protected:
  srb0_scheduler_tester() : base_srb0_scheduler_tester(GetParam().duplx_mode), params{GetParam()} {}

  srb0_test_params params;
};

TEST_P(srb0_scheduler_tester, successfully_allocated_resources)
{
  setup_sched(create_expert_config(2), create_custom_cell_config_request(params.k0));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  const unsigned mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  const unsigned exp_size = get_pending_bytes(to_du_ue_index(0));

  // Test the following:
  // 1. Check for DCI_1_0 allocation for SRB0 on PDCCH.
  // 2. Check for PDSCH allocation.
  // 3. Check whether CW TB bytes matches with pending bytes to be sent.
  const auto& test_ue = get_ue(to_du_ue_index(0));
  bool        is_ue_allocated_pdcch{false};
  bool        is_ue_allocated_pdsch{false};
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    if (ue_is_allocated_pdcch(test_ue)) {
      is_ue_allocated_pdcch = true;
    }
    if (ue_is_allocated_pdsch(test_ue)) {
      is_ue_allocated_pdsch = true;
      ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(test_ue, exp_size));
    }
  }
  ASSERT_TRUE(is_ue_allocated_pdcch);
  ASSERT_TRUE(is_ue_allocated_pdsch);
}

TEST_P(srb0_scheduler_tester, failed_allocating_resources)
{
  setup_sched(create_expert_config(0), create_custom_cell_config_request(params.k0));

  // Add UE 1.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 101 bytes.
  unsigned ue1_mac_srb0_sdu_size = 101;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), ue1_mac_srb0_sdu_size);

  // Add UE 2.
  add_ue(to_rnti(0x4602), to_du_ue_index(1));
  // Notify about SRB0 message in DL of size 350 bytes. i.e. big enough to not get allocated with the max. mcs chosen.
  unsigned ue2_mac_srb0_sdu_size = 350;
  push_buffer_state_to_dl_ue(to_du_ue_index(1), ue2_mac_srb0_sdu_size);

  run_slot();

  // Allocation for UE2 should fail.
  const auto& test_ue = get_ue(to_du_ue_index(1));
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
    ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
  }
}

TEST_P(srb0_scheduler_tester, test_large_srb0_buffer_size)
{
  setup_sched(create_expert_config(27), create_custom_cell_config_request(params.k0));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 458 bytes.
  const unsigned mac_srb0_sdu_size = 458;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  const unsigned exp_size = get_pending_bytes(to_du_ue_index(0));

  const auto& test_ue = get_ue(to_du_ue_index(0));
  bool        is_ue_allocated_pdcch{false};
  bool        is_ue_allocated_pdsch{false};
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    if (ue_is_allocated_pdcch(test_ue)) {
      is_ue_allocated_pdcch = true;
    }
    if (ue_is_allocated_pdsch(test_ue)) {
      is_ue_allocated_pdsch = true;
      ASSERT_TRUE(tbs_scheduled_bytes_matches_given_size(test_ue, exp_size));
    }
  }
  ASSERT_TRUE(is_ue_allocated_pdcch);
  ASSERT_TRUE(is_ue_allocated_pdsch);
}

TEST_P(srb0_scheduler_tester, test_srb0_buffer_size_exceeding_max_msg4_mcs_index)
{
  setup_sched(create_expert_config(3), create_custom_cell_config_request(params.k0));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Notify about SRB0 message in DL of size 360 bytes which requires MCS index > 3.
  const unsigned mac_srb0_sdu_size = 360;
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  // Allocation for UE should fail.
  const auto& test_ue = get_ue(to_du_ue_index(0));
  for (unsigned sl_idx = 0; sl_idx < bench->max_test_run_slots_per_ue * (1U << current_slot.numerology()); sl_idx++) {
    run_slot();
    ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
    ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
  }
}

TEST_P(srb0_scheduler_tester, sanity_check_with_random_max_mcs_and_payload_size)
{
  const sch_mcs_index max_msg4_mcs = get_random_uint(0, 27);
  setup_sched(create_expert_config(max_msg4_mcs), create_custom_cell_config_request(params.k0));
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Random payload size.
  const unsigned mac_srb0_sdu_size = get_random_uint(1, 458);
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  srslog::basic_logger& logger(srslog::fetch_basic_logger("TEST"));
  logger.info("SRB0 scheduler sanity test params PDU size ({}), max msg4 mcs ({}).", mac_srb0_sdu_size, max_msg4_mcs);

  run_slot();
}

class srb0_scheduler_tdd_tester : public base_srb0_scheduler_tester, public ::testing::Test
{
protected:
  srb0_scheduler_tdd_tester() : base_srb0_scheduler_tester(srsran::duplex_mode::TDD) {}
};

TEST_F(srb0_scheduler_tdd_tester, test_allocation_in_appropriate_slots_in_tdd)
{
  const unsigned      k0                 = 0;
  const sch_mcs_index max_msg4_mcs_index = 1;
  auto                cell_cfg           = create_custom_cell_config_request(k0);
  setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);

  const unsigned MAX_UES            = 4;
  const unsigned MAX_TEST_RUN_SLOTS = 40;
  const unsigned MAC_SRB0_SDU_SIZE  = 129;

  // Add UEs.
  for (unsigned idx = 0; idx < MAX_UES; idx++) {
    add_ue(to_rnti(0x4601 + idx), to_du_ue_index(idx));
    // Notify about SRB0 message in DL.
    push_buffer_state_to_dl_ue(to_du_ue_index(idx), MAC_SRB0_SDU_SIZE);
  }

  for (unsigned idx = 0; idx < MAX_UES * MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();
    if (not bench->cell_cfg.is_dl_enabled(current_slot)) {
      // Check whether PDCCH/PDSCH is not scheduled in UL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
        ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
      }
    }
    if (not bench->cell_cfg.is_ul_enabled(current_slot)) {
      // Check whether PUCCH HARQ is not scheduled in DL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pucch(test_ue));
      }
    }
  }
}

TEST_F(srb0_scheduler_tdd_tester, test_allocation_in_partial_slots_tdd)
{
  const unsigned                           k0                 = 0;
  const sch_mcs_index                      max_msg4_mcs_index = 8;
  const tdd_ul_dl_config_common            tdd_cfg{.ref_scs  = subcarrier_spacing::kHz30,
                                                   .pattern1 = {.dl_ul_tx_period_nof_slots = 5,
                                                                .nof_dl_slots              = 2,
                                                                .nof_dl_symbols            = 8,
                                                                .nof_ul_slots              = 2,
                                                                .nof_ul_symbols            = 0}};
  sched_cell_configuration_request_message cell_cfg = create_custom_cell_config_request(k0, tdd_cfg);
  // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
  cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list = config_helpers::make_pdsch_time_domain_resource(
      cell_cfg.searchspace0, cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common, nullopt, cell_cfg.tdd_ul_dl_cfg_common);
  setup_sched(create_expert_config(max_msg4_mcs_index), cell_cfg);

  const unsigned MAX_TEST_RUN_SLOTS = 40;
  const unsigned MAC_SRB0_SDU_SIZE  = 129;

  // Add a single UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));

  for (unsigned idx = 0; idx < MAX_TEST_RUN_SLOTS * (1U << current_slot.numerology()); idx++) {
    run_slot();
    // Notify about SRB0 message in DL one slot before partial slot in order for it to be scheduled in the next
    // (partial) slot.
    if (bench->cell_cfg.is_dl_enabled(current_slot + 1) and
        (not bench->cell_cfg.is_fully_dl_enabled(current_slot + 1))) {
      push_buffer_state_to_dl_ue(to_du_ue_index(0), MAC_SRB0_SDU_SIZE);
    }
    // Check SRB0 allocation in partial slot.
    if (bench->cell_cfg.is_dl_enabled(current_slot) and (not bench->cell_cfg.is_fully_dl_enabled(current_slot))) {
      const auto& test_ue = get_ue(to_du_ue_index(0));
      ASSERT_TRUE(ue_is_allocated_pdcch(test_ue));
      ASSERT_TRUE(ue_is_allocated_pdsch(test_ue));
      break;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(srb0_scheduler,
                         srb0_scheduler_tester,
                         testing::Values(srb0_test_params{.k0 = 0, .duplx_mode = duplex_mode::FDD},
                                         srb0_test_params{.k0 = 0, .duplx_mode = duplex_mode::TDD}));

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  (void)(::testing::GTEST_FLAG(death_test_style) = "fast");

  return RUN_ALL_TESTS();
}
