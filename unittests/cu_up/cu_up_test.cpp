/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_test_helpers.h"
#include "srsgnb/cu_up/cu_up_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;
using namespace asn1::e1ap;

class dummy_e1_notifier : public e1_message_notifier
{
  void on_new_message(const e1_message& msg) override
  {
    // do nothing
  }
};

/// Fixture class for CU-UP test
class cu_up_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create worker thread and executer
    task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
    std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

    dummy_e1_notifier e1_message_notifier;

    // create config
    cu_up_configuration cfg;
    cfg.cu_up_executor = task_executor.get();
    cfg.e1_notifier    = &e1_message_notifier;

    // create and start DUT
    cu_up = create_cu_up(cfg);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

//////////////////////////////////////////////////////////////////////////////////////
/* E1 connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the E1 connection
TEST_F(cu_up_test, when_e1_connection_established_then_e1_connected)
{
  // Connect E1
  cu_up->on_e1_connection_establish();

  // check that E1 is in connected state
  ASSERT_TRUE(cu_up->e1_is_connected());
}
