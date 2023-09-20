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

#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/scheduler_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/pcch/pcch_configuration.h"
#include <gtest/gtest.h>
#include <random>
#include <unordered_map>

using namespace srsran;

std::random_device rd;
std::mt19937       g(rd());

uint64_t get_random_uint(uint64_t min, uint64_t max)
{
  srsran_assert(min <= max, "Minimum value is greater than maximum value");
  return std::uniform_int_distribution<uint64_t>{min, max}(g);
}

// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct paging_sched_test_bench {
  // Approximation of PCCH-PCH Paging message size in Bytes.
  static constexpr unsigned pcch_pch_paging_msg_size = 11;

  scheduler_expert_config       expert_cfg;
  cell_configuration            cell_cfg;
  cell_resource_allocator       res_grid;
  pdcch_resource_allocator_impl pdcch_sch;
  paging_scheduler              pg_sch;

  explicit paging_sched_test_bench(const scheduler_expert_config&                  expert_cfg_,
                                   const sched_cell_configuration_request_message& cell_req =
                                       test_helpers::make_default_sched_cell_configuration_request()) :
    expert_cfg{expert_cfg_},
    cell_cfg{expert_cfg, cell_req},
    res_grid{cell_cfg},
    pdcch_sch{cell_cfg},
    pg_sch{expert_cfg, cell_cfg, pdcch_sch, cell_req}
  {
  }
};

class base_paging_sched_tester
{
public:
  using five_g_s_tmsi = uint64_t;

  slot_point                        current_slot{0, 0};
  srslog::basic_logger&             mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&             test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<paging_sched_test_bench> bench;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned                max_k_value = 0;
  scheduler_result_logger sched_res_logger;

  base_paging_sched_tester() = default;

  ~base_paging_sched_tester()
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void setup_sched(const scheduler_expert_config&                  expert_cfg,
                   const sched_cell_configuration_request_message& msg =
                       test_helpers::make_default_sched_cell_configuration_request())
  {
    current_slot = slot_point{to_numerology_value(msg.scs_common), 0};

    const auto& dl_lst = msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
    for (const auto& pdsch : dl_lst) {
      if (pdsch.k0 > max_k_value) {
        max_k_value = pdsch.k0;
      }
    }
    const auto& ul_lst = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    for (const auto& pusch : ul_lst) {
      if (pusch.k2 > max_k_value) {
        max_k_value = pusch.k2;
      }
    }

    bench.emplace(expert_cfg, msg);

    // Initialize.
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
    sched_res_logger.on_slot_start();

    bench->res_grid.slot_indication(current_slot);
    bench->pdcch_sch.slot_indication(current_slot);
    bench->pg_sch.schedule_paging(bench->res_grid);

    // Log scheduling results.
    sched_res_logger.on_scheduler_result(bench->res_grid[0].result);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  static scheduler_expert_config create_expert_config(sch_mcs_index paging_mcs_index_, unsigned max_paging_retries_)
  {
    auto cfg                  = config_helpers::make_default_scheduler_expert_config();
    cfg.pg.paging_mcs_index   = paging_mcs_index_;
    cfg.pg.max_paging_retries = max_paging_retries_;
    return cfg;
  }

  sched_cell_configuration_request_message
  create_custom_cell_config_request(duplex_mode              duplx_mode,
                                    subcarrier_spacing       scs        = srsran::subcarrier_spacing::kHz30,
                                    bs_channel_bandwidth_fr1 carrier_bw = srsran::bs_channel_bandwidth_fr1::MHz20) const
  {
    cell_config_builder_params cell_cfg{};
    if (duplx_mode == duplex_mode::TDD) {
      // Band 40.
      cell_cfg.dl_arfcn       = 465000;
      cell_cfg.scs_common     = scs;
      cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
      cell_cfg.channel_bw_mhz = carrier_bw;
    }
    return test_helpers::make_default_sched_cell_configuration_request(cell_cfg);
  }

  uint64_t generate_five_g_s_tmsi()
  {
    // 5G-S-TMSI: Concatenation of the AMF Set Identity (10 bits), the AMF Pointer (6 bits) and the 5G-TMSI (32 bits).

    // Values of AMF Set Id and AMF pointer are assumed. Lower 32-bits are masked to fill randomly generated 5G-TMSI.
    static const uint64_t five_g_s_tmsi_with_5g_tmsi_masked = 0x010100000000;
    return five_g_s_tmsi_with_5g_tmsi_masked | get_random_uint(0, 0xfffffffe);
  }

  void push_paging_information(five_g_s_tmsi s_tmsi, unsigned paging_drx_)
  {
    // UE_ID: 5G-S-TMSI mod 1024. See TS 38.304, clause 7.1.
    const unsigned ue_id = s_tmsi % 1024;

    const sched_paging_information info{.paging_type_indicator   = paging_identity_type::cn_ue_paging_identity,
                                        .paging_identity         = s_tmsi,
                                        .ue_identity_index_value = ue_id,
                                        .paging_drx              = paging_drx_};

    bench->pg_sch.handle_paging_information(info);
  }

  bool is_ue_allocated_paging_grant(five_g_s_tmsi s_tmsi)
  {
    return std::any_of(bench->res_grid[0].result.dl.paging_grants.begin(),
                       bench->res_grid[0].result.dl.paging_grants.end(),
                       [&s_tmsi](const auto& grant) {
                         return std::any_of(grant.paging_ue_list.begin(),
                                            grant.paging_ue_list.end(),
                                            [&s_tmsi](const auto& ue) { return ue.paging_identity == s_tmsi; });
                       });
  }
};

// Parameters to be passed to test.
struct paging_scheduler_test_params {
  unsigned    max_paging_mcs;
  unsigned    max_paging_retries;
  uint16_t    drx_cycle_in_nof_rf;
  duplex_mode duplx_mode;
};

class paging_sched_tester : public base_paging_sched_tester,
                            public ::testing::TestWithParam<paging_scheduler_test_params>
{
protected:
  paging_sched_tester() : params{GetParam()} {}

  paging_scheduler_test_params params;
};

TEST_P(paging_sched_tester, successfully_allocated_paging_grant_ss_gt_0)
{
  const std::vector<srsran::pcch_config::nof_po_per_pf> possible_ns_values = {srsran::pcch_config::nof_po_per_pf::one,
                                                                              srsran::pcch_config::nof_po_per_pf::two,
                                                                              srsran::pcch_config::nof_po_per_pf::four};
  const std::vector<srsran::pcch_config::nof_pf_per_drx_cycle> possible_nof_pf_per_drx_values = {
      srsran::pcch_config::nof_pf_per_drx_cycle::oneT,
      srsran::pcch_config::nof_pf_per_drx_cycle::halfT,
      srsran::pcch_config::nof_pf_per_drx_cycle::quarterT,
      srsran::pcch_config::nof_pf_per_drx_cycle::oneEighthT,
      srsran::pcch_config::nof_pf_per_drx_cycle::oneSixteethT};

  auto cell_cfg_request = create_custom_cell_config_request(params.duplx_mode);
  // Modify to have more than one Paging occasion per PF.
  cell_cfg_request.dl_cfg_common.pcch_cfg.ns = possible_ns_values[get_random_uint(0, possible_ns_values.size() - 1)];
  cell_cfg_request.dl_cfg_common.pcch_cfg.nof_pf =
      possible_nof_pf_per_drx_values[get_random_uint(0, possible_nof_pf_per_drx_values.size() - 1)];
  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries), cell_cfg_request);

  // Notify scheduler of paging message.
  const uint64_t fiveg_s_tmsi = generate_five_g_s_tmsi();
  push_paging_information(fiveg_s_tmsi, params.drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0; i != (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf *
                                bench->res_grid.slot_tx().nof_slots_per_frame();
       ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(fiveg_s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, params.max_paging_retries);
}

TEST_P(paging_sched_tester, successfully_allocated_paging_grant_ss_eq_0)
{
  const std::vector<srsran::pcch_config::nof_pf_per_drx_cycle> possible_nof_pf_per_drx_values = {
      srsran::pcch_config::nof_pf_per_drx_cycle::halfT,
      srsran::pcch_config::nof_pf_per_drx_cycle::quarterT,
      srsran::pcch_config::nof_pf_per_drx_cycle::oneEighthT,
      srsran::pcch_config::nof_pf_per_drx_cycle::oneSixteethT};
  auto sched_cell_cfg = create_custom_cell_config_request(params.duplx_mode);
  // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
  sched_cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
  // Since we support CORESET multiplexing pattern 1. The value of N (Number of Paging Frames per DRX Cycle) can be 2,
  // 4, 8, 16).
  sched_cell_cfg.dl_cfg_common.pcch_cfg.nof_pf =
      possible_nof_pf_per_drx_values[get_random_uint(0, possible_nof_pf_per_drx_values.size() - 1)];

  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries), sched_cell_cfg);

  // Notify scheduler of paging message.
  const five_g_s_tmsi s_tmsi = generate_five_g_s_tmsi();
  push_paging_information(s_tmsi, params.drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0; i != (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf *
                                bench->res_grid.slot_tx().nof_slots_per_frame();
       ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, params.max_paging_retries);
}

TEST_P(paging_sched_tester, successfully_paging_multiple_ues)
{
  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries),
              create_custom_cell_config_request(params.duplx_mode));

  std::map<five_g_s_tmsi, unsigned> fiveg_s_tmsi_to_paging_attempts_lookup;

  static const unsigned nof_ues = 5;

  // Notify scheduler of paging message.
  for (unsigned ue_num = 0; ue_num < nof_ues; ue_num++) {
    const uint64_t fiveg_s_tmsi_ue = generate_five_g_s_tmsi();
    push_paging_information(fiveg_s_tmsi_ue, params.drx_cycle_in_nof_rf);
    fiveg_s_tmsi_to_paging_attempts_lookup[fiveg_s_tmsi_ue] = 0;
  }

  for (unsigned i = 0; i != nof_ues * (params.max_paging_retries + 1) * params.drx_cycle_in_nof_rf *
                                bench->res_grid.slot_tx().nof_slots_per_frame();
       ++i) {
    run_slot();
    for (auto& it : fiveg_s_tmsi_to_paging_attempts_lookup) {
      if (is_ue_allocated_paging_grant(it.first)) {
        it.second++;
      }
    }
  }

  for (auto& it : fiveg_s_tmsi_to_paging_attempts_lookup) {
    ASSERT_EQ(it.second, params.max_paging_retries);
  }
}

class paging_sched_special_case_tester : public base_paging_sched_tester, public ::testing::Test
{};

TEST_F(paging_sched_special_case_tester, successfully_allocated_paging_grant_5mhz_carrier_bw)
{
  const unsigned max_paging_mcs      = 3;
  const unsigned max_paging_retries  = 3;
  const uint16_t drx_cycle_in_nof_rf = 128;

  auto sched_cell_cfg = create_custom_cell_config_request(
      srsran::duplex_mode::FDD, subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz5);

  // Shuffle between SearchSpace#0 and SearchSpace#1.
  const auto ss_id = to_search_space_id(get_random_uint(0, 1));
  if (ss_id == to_search_space_id(0)) {
    // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
    sched_cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
    // Since we support CORESET multiplexing pattern 1. The value of N (Number of Paging Frames per DRX Cycle) can be 2,
    // 4, 8, 16).
    sched_cell_cfg.dl_cfg_common.pcch_cfg.nof_pf = srsran::pcch_config::nof_pf_per_drx_cycle::halfT;
  }

  setup_sched(create_expert_config(max_paging_mcs, max_paging_retries), sched_cell_cfg);

  // Notify scheduler of paging message.
  const five_g_s_tmsi s_tmsi = generate_five_g_s_tmsi();
  push_paging_information(s_tmsi, drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0;
       i != (max_paging_retries + 1) * drx_cycle_in_nof_rf * bench->res_grid.slot_tx().nof_slots_per_frame();
       ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, max_paging_retries);
}

class paging_sched_partial_slot_tester : public base_paging_sched_tester, public ::testing::Test
{};

TEST_F(paging_sched_partial_slot_tester, successfully_allocated_paging_grant_ss_gt_0_in_partial_slot_tdd)
{
  const unsigned                max_paging_mcs      = 3;
  const unsigned                max_paging_retries  = 3;
  const uint16_t                drx_cycle_in_nof_rf = 128;
  const tdd_ul_dl_config_common tdd_cfg{.ref_scs  = subcarrier_spacing::kHz30,
                                        .pattern1 = {.dl_ul_tx_period_nof_slots = 5,
                                                     .nof_dl_slots              = 2,
                                                     .nof_dl_symbols            = 8,
                                                     .nof_ul_slots              = 2,
                                                     .nof_ul_symbols            = 0}};

  auto cell_cfg_request                 = create_custom_cell_config_request(duplex_mode::TDD);
  cell_cfg_request.tdd_ul_dl_cfg_common = tdd_cfg;
  // Generate PDSCH Time domain allocation based on the partial slot TDD configuration.
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list =
      config_helpers::make_pdsch_time_domain_resource(cell_cfg_request.searchspace0,
                                                      cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common,
                                                      nullopt,
                                                      cell_cfg_request.tdd_ul_dl_cfg_common);

  // Set Paging configuration to a particular value in order to derive the 5G-S-TMSI, such that Paging Occasion
  // of UE fall in partial slot of above set TDD configuration. i.e. we would like to force Paging Occasion index of UE
  // to be 3rd PDCCH Monitoring Occasion which corresponds to 3rd in SearchSpace#1 used for Paging (SS#1 is monitored in
  // every DL slot).
  cell_cfg_request.dl_cfg_common.pcch_cfg.ns     = srsran::pcch_config::nof_po_per_pf::four;
  cell_cfg_request.dl_cfg_common.pcch_cfg.nof_pf = srsran::pcch_config::nof_pf_per_drx_cycle::oneT;
  setup_sched(create_expert_config(max_paging_mcs, max_paging_retries), cell_cfg_request);

  // N value used in equation found at TS 38.304, clause 7.1.
  const unsigned N = static_cast<unsigned>(cell_cfg_request.dl_cfg_common.pcch_cfg.default_paging_cycle) /
                     static_cast<unsigned>(cell_cfg_request.dl_cfg_common.pcch_cfg.nof_pf);

  unsigned i_s          = 0;
  uint64_t fiveg_s_tmsi = 0;
  while (i_s != tdd_cfg.pattern1.nof_dl_slots) {
    fiveg_s_tmsi = generate_five_g_s_tmsi();

    // UE_ID: 5G-S-TMSI mod 1024. See TS 38.304, clause 7.1.
    const unsigned ue_id = fiveg_s_tmsi % 1024;

    // Index (i_s), indicating the index of the PO.
    // i_s = floor (UE_ID/N) mod Ns.
    i_s = static_cast<unsigned>(std::floor(static_cast<double>(ue_id) / static_cast<double>(N))) %
          static_cast<unsigned>(cell_cfg_request.dl_cfg_common.pcch_cfg.ns);
  }

  // Notify scheduler of paging message.
  push_paging_information(fiveg_s_tmsi, drx_cycle_in_nof_rf);

  unsigned paging_attempts = 0;
  for (unsigned i = 0;
       i != (max_paging_retries + 1) * drx_cycle_in_nof_rf * bench->res_grid.slot_tx().nof_slots_per_frame();
       ++i) {
    run_slot();
    if (is_ue_allocated_paging_grant(fiveg_s_tmsi)) {
      paging_attempts++;
    }
  }

  ASSERT_EQ(paging_attempts, max_paging_retries);
}

INSTANTIATE_TEST_SUITE_P(paging_sched_tester,
                         paging_sched_tester,
                         testing::Values(paging_scheduler_test_params{.max_paging_mcs      = 3,
                                                                      .max_paging_retries  = 3,
                                                                      .drx_cycle_in_nof_rf = 128,
                                                                      .duplx_mode          = duplex_mode::FDD},
                                         paging_scheduler_test_params{.max_paging_mcs      = 3,
                                                                      .max_paging_retries  = 3,
                                                                      .drx_cycle_in_nof_rf = 128,
                                                                      .duplx_mode          = duplex_mode::TDD}));
