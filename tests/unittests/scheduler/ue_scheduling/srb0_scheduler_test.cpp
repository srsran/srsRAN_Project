/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "srsran/ran/duplex_mode.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

// Parameters to be passed to test.
struct srb0_test_params {
  uint8_t     k0;
  duplex_mode duplx_mode;
};

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  // Maximum number of slots to run per UE in order to validate the results of scheduler. Implementation defined.
  static constexpr unsigned max_test_run_slots_per_ue = 40;

  scheduler_ue_expert_config    expert_cfg;
  cell_configuration            cell_cfg;
  cell_resource_allocator       res_grid;
  pdcch_resource_allocator_impl pdcch_sch;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  ue_list                       ue_db;
  ue_cell_grid_allocator        ue_alloc;
  ue_srb0_scheduler             srb0_sched;

  explicit test_bench(const scheduler_ue_expert_config&               expert_cfg_,
                      const sched_cell_configuration_request_message& cell_req) :
    expert_cfg{expert_cfg_},
    cell_cfg{cell_req},
    res_grid{cell_cfg},
    pdcch_sch{cell_cfg},
    pucch_alloc{cell_cfg},
    uci_alloc{pucch_alloc},
    ue_alloc(expert_cfg, ue_db, srslog::fetch_basic_logger("SCHED", true)),
    srb0_sched(expert_cfg, cell_cfg, pdcch_sch, pucch_alloc, ue_db)
  {
  }
};

class srb0_scheduler_tester : public ::testing::TestWithParam<srb0_test_params>
{
protected:
  slot_point            current_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<test_bench>  bench;
  srb0_test_params      params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  srb0_scheduler_tester() : params{GetParam()} {};

  ~srb0_scheduler_tester() override
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_ue_expert_config& expert_cfg, const sched_cell_configuration_request_message& msg)
  {
    current_slot = slot_point{to_numerology_value(msg.scs_common), 0};

    bench.emplace(expert_cfg, msg);

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
  }

  void run_slot()
  {
    current_slot++;

    mac_logger.set_context(current_slot.sfn(), current_slot.slot_index());
    test_logger.set_context(current_slot.sfn(), current_slot.slot_index());

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);

    bench->srb0_sched.run_slot(bench->res_grid);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  scheduler_ue_expert_config create_expert_config(sch_mcs_index max_msg4_mcs_index) const
  {
    scheduler_ue_expert_config cfg{};
    cfg.fixed_dl_mcs.emplace(10);
    cfg.fixed_ul_mcs.emplace(10);
    cfg.max_nof_harq_retxs = 4;
    cfg.max_msg4_mcs       = max_msg4_mcs_index;
    return cfg;
  }

  sched_cell_configuration_request_message create_custom_cell_config_request() const
  {
    cell_config_builder_params cell_cfg{};
    cell_cfg.band = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
    if (params.duplx_mode == duplex_mode::TDD) {
      // Band 40.
      cell_cfg.dl_arfcn       = 474000;
      cell_cfg.scs_common     = srsran::subcarrier_spacing::kHz30;
      cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
      cell_cfg.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          cell_cfg.channel_bw_mhz,
          cell_cfg.scs_common,
          cell_cfg.band.has_value() ? band_helper::get_freq_range(cell_cfg.band.value()) : frequency_range::FR1);

      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          cell_cfg.dl_arfcn, *cell_cfg.band, nof_crbs, cell_cfg.scs_common, cell_cfg.scs_common, 0);
      cell_cfg.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      cell_cfg.k_ssb             = ssb_freq_loc->k_ssb;
      cell_cfg.coreset0_index    = ssb_freq_loc->coreset0_idx;
    }
    sched_cell_configuration_request_message msg =
        test_helpers::make_default_sched_cell_configuration_request(cell_cfg);
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = params.k0;

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
    cell_config_builder_params cell_cfg{};
    cell_cfg.band = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
    if (params.duplx_mode == duplex_mode::TDD) {
      // Band 40.
      cell_cfg.dl_arfcn       = 474000;
      cell_cfg.scs_common     = srsran::subcarrier_spacing::kHz30;
      cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
      cell_cfg.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          cell_cfg.channel_bw_mhz,
          cell_cfg.scs_common,
          cell_cfg.band.has_value() ? band_helper::get_freq_range(cell_cfg.band.value()) : frequency_range::FR1);

      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          cell_cfg.dl_arfcn, *cell_cfg.band, nof_crbs, cell_cfg.scs_common, cell_cfg.scs_common, 0);
      cell_cfg.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      cell_cfg.k_ssb             = ssb_freq_loc->k_ssb;
      cell_cfg.coreset0_index    = ssb_freq_loc->coreset0_idx;
    }
    // Add cell to UE cell grid allocator.
    bench->ue_alloc.add_cell(bench->cell_cfg.cell_index, bench->pdcch_sch, bench->uci_alloc, bench->res_grid);
    auto ue_create_req     = test_helpers::create_default_sched_ue_creation_request(cell_cfg);
    ue_create_req.crnti    = tc_rnti;
    ue_create_req.ue_index = ue_index;
    // Add UE to UE DB.
    auto u = std::make_unique<ue>(bench->expert_cfg, bench->cell_cfg, ue_create_req);
    if (bench->ue_db.contains(ue_index)) {
      // UE already exists.
      return false;
    }
    bench->ue_db.insert(ue_index, std::move(u));
    return true;
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

TEST_P(srb0_scheduler_tester, successfully_allocated_resources)
{
  setup_sched(create_expert_config(1), create_custom_cell_config_request());
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
  setup_sched(create_expert_config(0), create_custom_cell_config_request());

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
  setup_sched(create_expert_config(27), create_custom_cell_config_request());
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
  setup_sched(create_expert_config(3), create_custom_cell_config_request());
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
  setup_sched(create_expert_config(max_msg4_mcs), create_custom_cell_config_request());
  // Add UE.
  add_ue(to_rnti(0x4601), to_du_ue_index(0));
  // Random payload size.
  const unsigned mac_srb0_sdu_size = get_random_uint(1, 458);
  push_buffer_state_to_dl_ue(to_du_ue_index(0), mac_srb0_sdu_size);

  srslog::basic_logger& logger(srslog::fetch_basic_logger("TEST"));
  logger.info("SRB0 scheduler sanity test params PDU size ({}), max msg4 mcs ({}).", mac_srb0_sdu_size, max_msg4_mcs);

  run_slot();
}

TEST_P(srb0_scheduler_tester, test_allocation_in_appropriate_slots_in_tdd)
{
  // Set test params to TDD.
  params.duplx_mode = duplex_mode::TDD;

  auto cell_cfg = create_custom_cell_config_request();
  setup_sched(create_expert_config(1), cell_cfg);

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
    if (bench->cell_cfg.is_ul_enabled(current_slot)) {
      // Check whether PUCCH/PDSCH is not scheduled in UL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pdcch(test_ue));
        ASSERT_FALSE(ue_is_allocated_pdsch(test_ue));
      }
    }
    if (bench->cell_cfg.is_dl_enabled(current_slot)) {
      // Check whether PUCCH HARQ is not scheduled in DL slots for any of the UEs.
      for (unsigned ue_idx = 0; ue_idx < MAX_UES; ue_idx++) {
        const auto& test_ue = get_ue(to_du_ue_index(ue_idx));
        ASSERT_FALSE(ue_is_allocated_pucch(test_ue));
      }
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
