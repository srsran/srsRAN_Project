/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

//#include "lib/scheduler/cell/resource_grid.h"
//#include "lib/scheduler/pucch_scheduling/pucch_guardbands_sched.h"
//#include "unittests/scheduler/test_utils/scheduler_test_suite.h"
//#include "unittests/scheduler/test_utils/config_generators.h"
#include <gtest/gtest.h>

// using namespace srsgnb;

#if 0
// Test bench with all that is needed for the PUCCH.
class test_bench
{
public:
  explicit test_bench() :
    cell_cfg{make_default_sched_cell_configuration_request()},
    res_grid{cell_cfg},
    pucch_guard_sched{cell_cfg},
    sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
  {
    srsgnb_assert(
        not cell_cfg,
        "sched_ue_creation_request_message initialization is not complete.");


    slot_indication(sl_tx);
  }

  // Add an extra UE, whose RNTI will have RNTI +1 with respect to the last_allocated_rnti.
  void add_ue();

  void slot_indication(slot_point slot_tx)
  {
    mac_logger.set_context(slot_tx.to_uint());
    test_logger.set_context(slot_tx.to_uint());
    res_grid.slot_indication(slot_tx);
  }

  cell_configuration      cell_cfg;
  cell_resource_allocator res_grid;
  pucch_guardbands_sched  pucch_guard_sched;
  slot_point             sl_tx;
  srslog::basic_logger&  mac_logger  = srslog::fetch_basic_logger("MAC");
  srslog::basic_logger&  test_logger = srslog::fetch_basic_logger("TEST");
};
#endif


// Tests the PUCCH scheduler with SR opportunities and HARQ-ACK.
TEST(test_pucch_output_sr_harq, test_pucch_sr_harq) {
  printf("Hello");
}

int main(int argc, char** argv)
{
  /*
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();
   */

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
