/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/converters/f1ap_configuration_helpers.h"
#include "lib/scheduler/logging/scheduler_result_logger.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/scheduler_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "srsran/mac/mac_configuration_helpers.h"
#include "srsran/ran/duplex_mode.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
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

// Parameters to be passed to test.
struct paging_scheduler_test_params {
  unsigned    max_paging_mcs;
  unsigned    max_paging_retries;
  uint16_t    drx_cycle_in_nof_rf;
  duplex_mode duplx_mode;
};

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
    cell_cfg{cell_req},
    res_grid{cell_cfg},
    pdcch_sch{cell_cfg},
    pg_sch{expert_cfg, cell_cfg, pdcch_sch, cell_req}
  {
  }
};

class paging_sched_tester : public ::testing::TestWithParam<paging_scheduler_test_params>
{
public:
  using five_g_s_tmsi = uint64_t;

protected:
  slot_point                        current_slot{0, 0};
  srslog::basic_logger&             mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&             test_logger = srslog::fetch_basic_logger("TEST", true);
  optional<paging_sched_test_bench> bench;
  paging_scheduler_test_params      params;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned                max_k_value = 0;
  scheduler_result_logger sched_res_logger;

  paging_sched_tester() : params{GetParam()} {};

  ~paging_sched_tester() override
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
  create_custom_cell_config_request(subcarrier_spacing       scs        = srsran::subcarrier_spacing::kHz30,
                                    bs_channel_bandwidth_fr1 carrier_bw = srsran::bs_channel_bandwidth_fr1::MHz20) const
  {
    cell_config_builder_params cell_cfg{};
    if (params.duplx_mode == duplex_mode::TDD) {
      // Band 40.
      cell_cfg.dl_arfcn       = 465000;
      cell_cfg.scs_common     = scs;
      cell_cfg.band           = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_arfcn);
      cell_cfg.channel_bw_mhz = carrier_bw;

      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          cell_cfg.channel_bw_mhz,
          cell_cfg.scs_common,
          cell_cfg.band.has_value() ? band_helper::get_freq_range(cell_cfg.band.value()) : frequency_range::FR1);

      optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          cell_cfg.dl_arfcn, *cell_cfg.band, nof_crbs, cell_cfg.scs_common, cell_cfg.scs_common, 0);
      srsran_assert(ssb_freq_loc.has_value(), "Invalid cell config parameters");
      cell_cfg.offset_to_point_a = ssb_freq_loc->offset_to_point_A;
      cell_cfg.k_ssb             = ssb_freq_loc->k_ssb;
      cell_cfg.coreset0_index    = ssb_freq_loc->coreset0_idx;
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

  void push_paging_indication_message(five_g_s_tmsi s_tmsi, unsigned paging_drx_)
  {
    // UE_ID: 5G-S-TMSI mod 1024. See TS 38.304, clause 7.1.
    const unsigned ue_id = s_tmsi % 1024;

    const paging_indication_message msg{.paging_type_indicator =
                                            paging_indication_message::paging_identity_type::cn_ue_paging_identity,
                                        .paging_identity         = s_tmsi,
                                        .cell_index              = to_du_cell_index(0),
                                        .ue_identity_index_value = ue_id,
                                        .paging_drx              = paging_drx_,
                                        .paging_msg_size         = paging_sched_test_bench::pcch_pch_paging_msg_size};

    bench->pg_sch.handle_paging_indication_message(msg);
  }

  bool is_ue_allocated_paging_grant(five_g_s_tmsi s_tmsi)
  {
    return std::any_of(bench->res_grid[0].result.dl.paging_grants.begin(),
                       bench->res_grid[0].result.dl.paging_grants.end(),
                       [&s_tmsi](const auto& grant) { return grant.paging_identity == s_tmsi; });
  }
};

TEST_P(paging_sched_tester, successfully_allocated_paging_grant_ss_gt_0)
{
  auto cell_cfg_request = create_custom_cell_config_request();
  // Modify to have more than one Paging occasion per PF.
  cell_cfg_request.dl_cfg_common.pcch_cfg.ns = srsran::pcch_config::nof_po_per_pf::four;
  // >> PDCCH-Config.
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.emplace();
  // >> Add CORESET#1.
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value() =
      config_helpers::make_default_coreset_config();
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().id = to_coreset_id(1);
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().pdcch_dmrs_scrambling_id.emplace();
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().pdcch_dmrs_scrambling_id.value() = 0;
  // >> Add SearchSpace#2.
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.search_spaces.push_back(
      config_helpers::make_default_ue_search_space_config());
  // >> Set Paging to use SS#2.
  cell_cfg_request.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(2);
  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries), cell_cfg_request);

  // Notify scheduler of paging message.
  const uint64_t fiveg_s_tmsi = generate_five_g_s_tmsi();
  push_paging_indication_message(fiveg_s_tmsi, params.drx_cycle_in_nof_rf);

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
  auto sched_cell_cfg = create_custom_cell_config_request();
  // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
  sched_cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
  // Since we support CORESET multiplexing pattern 1. The value of N (Number of Paging Frames per DRX Cycle) can be 2,
  // 4, 8, 16).
  sched_cell_cfg.dl_cfg_common.pcch_cfg.nof_pf = srsran::pcch_config::nof_pf_per_drx_cycle::halfT;

  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries), sched_cell_cfg);

  // Notify scheduler of paging message.
  const five_g_s_tmsi s_tmsi = generate_five_g_s_tmsi();
  push_paging_indication_message(s_tmsi, params.drx_cycle_in_nof_rf);

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

TEST_P(paging_sched_tester, successfully_allocated_paging_grant_ss_eq_0_5mhz_carrier_bw)
{
  auto sched_cell_cfg = create_custom_cell_config_request(subcarrier_spacing::kHz15, bs_channel_bandwidth_fr1::MHz5);
  // In default config Paging Search Space is set to 1. Therefore, modify it to be equal to 0 for this test case.
  sched_cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id = to_search_space_id(0);
  // Since we support CORESET multiplexing pattern 1. The value of N (Number of Paging Frames per DRX Cycle) can be 2,
  // 4, 8, 16).
  sched_cell_cfg.dl_cfg_common.pcch_cfg.nof_pf = srsran::pcch_config::nof_pf_per_drx_cycle::halfT;

  setup_sched(create_expert_config(params.max_paging_mcs, params.max_paging_retries), sched_cell_cfg);

  // Notify scheduler of paging message.
  const five_g_s_tmsi s_tmsi = generate_five_g_s_tmsi();
  push_paging_indication_message(s_tmsi, params.drx_cycle_in_nof_rf);

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
              create_custom_cell_config_request());

  std::map<five_g_s_tmsi, unsigned> fiveg_s_tmsi_to_paging_attempts_lookup;

  static const unsigned nof_ues = 5;

  // Notify scheduler of paging message.
  for (unsigned ue_num = 0; ue_num < nof_ues; ue_num++) {
    const uint64_t fiveg_s_tmsi_ue = generate_five_g_s_tmsi();
    push_paging_indication_message(fiveg_s_tmsi_ue, params.drx_cycle_in_nof_rf);
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
