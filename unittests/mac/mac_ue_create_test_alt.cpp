/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/mac/mac_ctrl/ue_creation_procedure.h"
#include "mac_ctrl_test_dummies.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

class mac_ue_create_procedure_test : public testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::info);
    srslog::init();

    // Launch procedure
    msg.ue_index   = to_du_ue_index(1);
    msg.crnti      = to_rnti(0x4601);
    msg.cell_index = to_du_cell_index(0);
    msg.bearers.emplace_back();
    msg.bearers[0].lcid      = LCID_SRB0;
    msg.bearers[0].dl_bearer = nullptr; // procedure should not use bearers
    msg.bearers[0].ul_bearer = nullptr; // procedure should not use bearers

    proc = launch_async<mac_ue_create_request_procedure>(msg, mac_cfg, mac_ctrl, mac_ul, mac_dl, sched_cfg_adapter);
  }

  // Run all async tasks in same thread.
  manual_task_worker        worker{128};
  dummy_ul_executor_mapper  ul_exec_mapper{worker};
  dummy_dl_executor_mapper  dl_exec_mapper{&worker};
  dummy_mac_event_indicator du_mng_notif;
  dummy_mac_result_notifier phy_notifier;

  // Create a MAC config object.
  mac_common_config_t         mac_cfg{du_mng_notif, ul_exec_mapper, dl_exec_mapper, worker, phy_notifier};
  mac_ctrl_dummy_configurer   mac_ctrl;
  mac_ul_dummy_configurer     mac_ul;
  mac_dl_dummy_configurer     mac_dl;
  mac_scheduler_dummy_adapter sched_cfg_adapter;

  // MAC UE Creation Request Command.
  mac_ue_create_request_message msg{};

  // Procedure objects.
  async_task<mac_ue_create_response_message> proc;
};

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_requests_and_awaits_mac_ul_configurator_result)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  EXPECT_TRUE(mac_ul.last_ue_create_request.has_value());
  EXPECT_EQ(mac_ul.last_ue_create_request->ue_index, msg.ue_index);
  EXPECT_FALSE(mac_dl.last_ue_create_request.has_value());
  EXPECT_FALSE(proc.ready());
}

TEST_F(mac_ue_create_procedure_test,
       ue_creation_procedure_calls_and_awaits_mac_dl_configurator_after_mac_ul_result_is_received)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  mac_ul.ue_created_ev.set();
  EXPECT_TRUE(mac_dl.last_ue_create_request.has_value());
  EXPECT_EQ(mac_dl.last_ue_create_request->ue_index, msg.ue_index);
  EXPECT_FALSE(proc.ready());
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_completes_after_mac_dl_configurator_is_complete)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  mac_ul.ue_created_ev.set();
  mac_dl.ue_created_ev.set();
  EXPECT_TRUE(proc.ready());
  EXPECT_TRUE(proc.get().result);
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_completes_when_there_is_no_coroutine_suspension)
{
  // The procedure coroutine will not suspend while it waits for MAC DL and UL results.
  mac_ul.ue_created_ev.set();
  mac_dl.ue_created_ev.set();
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  EXPECT_TRUE(proc.ready());
  EXPECT_TRUE(proc.get().result);
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_fails_if_mac_ul_configurator_fails)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  mac_ul.expected_result = false;
  mac_ul.ue_created_ev.set();
  EXPECT_TRUE(proc.ready());
  EXPECT_FALSE(proc.get().result);
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
  EXPECT_TRUE(not mac_dl.last_ue_create_request.has_value());
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_fails_if_mac_dl_configurator_fails)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  mac_dl.expected_result = false;
  mac_ul.ue_created_ev.set();
  mac_dl.ue_created_ev.set();
  EXPECT_TRUE(proc.ready());
  EXPECT_FALSE(proc.get().result);
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
  EXPECT_TRUE(mac_dl.last_ue_create_request.has_value());
}
