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

#include "lib/scheduler/cell/resource_grid.h"
#include "lib/scheduler/pucch_scheduling/pucch_guardbands_scheduler.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "tests/unittests/scheduler/uci_and_pucch/uci_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

// Test bench with all that is needed for the PUCCH guardbands scheduler.
class test_bench_guardbands
{
public:
  explicit test_bench_guardbands(subcarrier_spacing scs, bool is_tdd = false) :
    cell_cfg{sched_cfg, make_default_sched_cell_configuration_request_scs(scs, is_tdd)},
    sl_tx{to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs), 0}
  {
    srsran_assert(not cell_cfg.pucch_guardbands.empty(), "PUCCH guardbands list is empty.");

    slot_indication(sl_tx);
  }

  void slot_indication(slot_point slot_tx) { res_grid.slot_indication(slot_tx); }

  const scheduler_expert_config sched_cfg = config_helpers::make_default_scheduler_expert_config();
  cell_configuration            cell_cfg;
  cell_resource_allocator       res_grid{cell_cfg};
  pucch_guardbands_scheduler    pucch_guard_sched{cell_cfg};
  slot_point                    sl_tx;
};

// Class to test PUCCH schedule with SR occasions only.
class test_pucch_guard_sched : public ::testing::TestWithParam<std::pair<subcarrier_spacing, bool>>
{
public:
  test_pucch_guard_sched() : bwp_cfg{t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params}
  {
    res_grid_size  = t_bench.res_grid.RING_ALLOCATOR_SIZE;
    ul_bwp_scs     = t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs;
    test_tot_slots = test_tot_frames * static_cast<unsigned>(t_bench.sl_tx.nof_slots_per_frame());
  }

protected:
  test_bench_guardbands t_bench{GetParam().first, GetParam().second};
  // Run the test for more than a multiple of MAX system frame number, so as to test the resource grid wrap-around for
  // that val.
  const unsigned test_tot_frames{2049};
  // Helper variables.
  unsigned                 res_grid_size;
  const bwp_configuration& bwp_cfg;
  subcarrier_spacing       ul_bwp_scs;
  unsigned                 test_tot_slots;
};

TEST_P(test_pucch_guard_sched, test_first_slot)
{
  // Run scheduler for the first slot only.
  t_bench.pucch_guard_sched.run_slot(t_bench.res_grid);

  // Check if PUCCH guardbands have been allocated for all the slots in the grid.
  for (unsigned sl = 0; sl < res_grid_size; ++sl) {
    for (const auto& guard_b : t_bench.cell_cfg.pucch_guardbands) {
      if (t_bench.cell_cfg.is_fully_ul_enabled(t_bench.res_grid[sl].slot)) {
        const grant_info grant{ul_bwp_scs, guard_b.symbols, prb_to_crb(bwp_cfg, guard_b.prbs)};
        ASSERT_TRUE(test_res_grid_has_re_set(t_bench.res_grid, grant, sl));
      }
    }
  }
}

TEST_P(test_pucch_guard_sched, test_slots_after_initialization)
{
  // Run scheduler for the several slots.
  for (unsigned sl = 0; sl < test_tot_slots; ++sl) {
    t_bench.pucch_guard_sched.run_slot(t_bench.res_grid);

    // Check if, after initialization, PUCCH guardbands have been allocated for the last slot of the grid.
    if (sl > 0) {
      for (const auto& guard_b : t_bench.cell_cfg.pucch_guardbands) {
        if (t_bench.cell_cfg.is_fully_ul_enabled(t_bench.res_grid[res_grid_size - 1].slot)) {
          const grant_info grant{ul_bwp_scs, guard_b.symbols, prb_to_crb(bwp_cfg, guard_b.prbs)};
          ASSERT_TRUE(test_res_grid_has_re_set(t_bench.res_grid, grant, res_grid_size - 1));
        }
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_pucch_guard_sched_scs_values,
                         test_pucch_guard_sched,
                         testing::Values(
                             // FDD.
                             std::make_pair(subcarrier_spacing::kHz15, false),
                             std::make_pair(subcarrier_spacing::kHz30, false),
                             // TDD.
                             std::make_pair(subcarrier_spacing::kHz15, true),
                             std::make_pair(subcarrier_spacing::kHz30, true)));
