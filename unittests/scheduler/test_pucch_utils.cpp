/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_test_utils.h"
#include "scheduler_test_suite.h"
#include <gtest/gtest.h>

using namespace srsgnb;

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  test_bench t_bench;
  t_bench.slot_indication(t_bench.sl_tx);
  t_bench.get_ue();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
