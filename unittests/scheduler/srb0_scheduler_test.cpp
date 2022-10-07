/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generators.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "lib/scheduler/ue_scheduling/ue_srb0_scheduler.h"
#include "scheduler_test_suite.h"
#include "srsgnb/ran/duplex_mode.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

std::random_device rd;
std::mt19937       g(rd());

unsigned get_random_uint(unsigned min, unsigned max)
{
  return std::uniform_int_distribution<unsigned>{min, max}(g);
}

// k0 and list of k2 values.
using test_params = std::tuple<uint8_t, std::vector<uint8_t>>;

/// Helper class to initialize and store relevant objects for the test and provide helper methods.
struct test_bench {
  cell_configuration            cell_cfg;
  cell_resource_allocator       res_grid;
  pdcch_resource_allocator_impl pdcch_sch;
  pucch_allocator_impl          pucch_sch;
  slot_point                    sl_tx;
  ue_list                       ue_db;
  ue_cell_grid_allocator        ue_alloc;
  ue_srb0_scheduler             srb0_sched;

  explicit test_bench(
      const sched_cell_configuration_request_message& cell_req = make_default_sched_cell_configuration_request()) :
    cell_cfg{cell_req},
    res_grid{cell_cfg},
    pdcch_sch{cell_cfg},
    pucch_sch{cell_cfg},
    sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0},
    ue_alloc(ue_db, srslog::fetch_basic_logger("MAC")),
    srb0_sched(cell_cfg, pdcch_sch, pucch_sch, ue_db)
  {
    // Add UE to UE DB
    auto ue_creation_msg = make_scheduler_ue_creation_request(test_helpers::make_default_ue_creation_request());
    auto u               = std::make_unique<ue>(cell_cfg, ue_creation_msg);
    ue_db.insert(to_du_ue_index(0), std::move(u));
  }
};

class srb0_scheduler_tester : public ::testing::TestWithParam<test_params>
{
protected:
  slot_point            next_slot{0, 0};
  srslog::basic_logger& mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("MAC");
  optional<test_bench>  bench;
  // We use this value to account for the case when the PDSCH or PUSCH is allocated several slots in advance.
  unsigned max_k_value = 0;

  srb0_scheduler_tester() = default;

  ~srb0_scheduler_tester() override
  {
    // Log pending allocations before finishing test.
    for (unsigned i = 0; i != max_k_value; ++i) {
      run_slot();
    }
    srslog::flush();
  }

  void
  setup_sched(const sched_cell_configuration_request_message& msg = make_default_sched_cell_configuration_request())
  {
    bench.emplace(msg);

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
  }

  void run_slot()
  {
    mac_logger.set_context(next_slot.to_uint());
    test_logger.set_context(next_slot.to_uint());

    bench->res_grid.slot_indication(next_slot);
    // TODO: Fill some parts of the resource grid here
    next_slot++;

    bench->srb0_sched.run_slot(bench->res_grid);

    // Check sched result consistency.
    test_scheduler_result_consistency(bench->cell_cfg, bench->res_grid);
  }

  const pusch_time_domain_resource_allocation& get_pusch_td_resource(uint8_t time_resource) const
  {
    return bench->cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list[time_resource];
  }

  static sched_cell_configuration_request_message create_random_cell_config_request(duplex_mode        mode,
                                                                                    const test_params& params)
  {
    sched_cell_configuration_request_message msg = make_default_sched_cell_configuration_request();
    msg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0 = std::get<0>(params);

    const auto& k2s        = std::get<1>(params);
    auto&       pusch_list = msg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list;
    if (k2s.empty()) {
      pusch_list[0].k2 = get_random_uint(2, 4);
    } else {
      pusch_list.resize(k2s.size(), pusch_list[0]);
      for (unsigned i = 0; i != k2s.size(); ++i) {
        pusch_list[i].k2 = k2s[i];
      }
    }
    if (mode == duplex_mode::TDD) {
      msg.tdd_ul_dl_cfg_common = config_helpers::make_default_tdd_ul_dl_config_common();
    }
    return msg;
  }

  span<const pdcch_dl_information> scheduled_dl_pdcchs() { return bench->res_grid[0].result.dl.dl_pdcchs; }

  slot_point result_slot_tx() const { return bench->res_grid[0].slot; }
};

TEST_P(srb0_scheduler_tester, successfully_allocated_resources)
{
  setup_sched(
      create_random_cell_config_request(get_random_uint(0, 1) == 0 ? duplex_mode::FDD : duplex_mode::TDD, GetParam()));
  run_slot();
  ASSERT_TRUE(true);
}

TEST_P(srb0_scheduler_tester, asserts_true_test2)
{
  ASSERT_TRUE(true);
}

INSTANTIATE_TEST_SUITE_P(srb0_scheduler,
                         srb0_scheduler_tester,
                         ::testing::Combine(::testing::Range<uint8_t>(1, 4), // k0
                                            ::testing::Values(std::vector<uint8_t>{2},
                                                              std::vector<uint8_t>{2, 4}))); // k2

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
