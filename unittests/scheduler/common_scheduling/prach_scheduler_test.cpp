/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/scheduler/common_scheduling/prach_scheduler.h"
#include "unittests/scheduler/test_utils/config_generators.h"
#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class prach_tester : public ::testing::Test
{
protected:
  prach_tester() :
    cell_cfg(test_helpers::make_default_sched_cell_configuration_request()),
    prach_sch(cell_cfg),
    sl(0, 0),
    prach_cfg(prach_configuration_get(
        frequency_range::FR1,
        duplex_mode::FDD,
        cell_cfg.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index)),
    res_grid(cell_cfg)
  {
  }

  void slot_indication()
  {
    ++sl;
    res_grid.slot_indication(sl);
    prach_sch.run_slot(res_grid);
    prach_counter += res_grid[0].result.ul.prachs.size();
  }

  bool is_prach_slot()
  {
    if (sl.sfn() % prach_cfg.x != prach_cfg.y) {
      return false;
    }
    if (std::find(prach_cfg.subframe.begin(), prach_cfg.subframe.end(), sl.subframe_index()) ==
        prach_cfg.subframe.end()) {
      return false;
    }
    return true;
  }

  unsigned nof_prach_occasions_allocated() const { return res_grid[0].result.ul.prachs.size(); }

  static const unsigned nof_slots_run = 1000;

  cell_configuration      cell_cfg;
  prach_scheduler         prach_sch;
  slot_point              sl;
  prach_configuration     prach_cfg;
  cell_resource_allocator res_grid;

  unsigned prach_counter = 0;
};

TEST_F(prach_tester, prach_sched_allocates_in_prach_configured_slots)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    if (is_prach_slot()) {
      ASSERT_GE(nof_prach_occasions_allocated(), 1);
    } else {
      ASSERT_EQ(0, nof_prach_occasions_allocated());
    }
  }
  ASSERT_GT(prach_counter, 0);
}

TEST_F(prach_tester, prach_sched_results_matches_config)
{
  for (unsigned i = 0; i != nof_slots_run; ++i) {
    slot_indication();
    test_scheduler_result_consistency(cell_cfg, res_grid[0].result);
  }
}
