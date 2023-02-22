/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/mac/mac_ctrl/ue_creation_procedure.h"
#include "mac_ctrl_test_dummies.h"
#include "tests/unittests/mac/mac_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/async/manual_event.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_ue_create_procedure_test : public testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::info);
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

  bool procedure_is_complete() const { return proc.ready(); }

  bool is_procedure_successful() const { return proc.get().result; }

  bool mac_ul_unit_received_ue_create_command() const { return mac_ul.last_ue_create_request.has_value(); }

  bool mac_dl_unit_received_ue_create_command() const { return mac_dl.last_ue_create_request.has_value(); }

  void set_mac_ul_unit_result(bool result)
  {
    mac_ul.expected_result = result;
    mac_ul.ue_created_ev.set();
  }

  void set_mac_dl_unit_result(bool result)
  {
    mac_dl.expected_result = result;
    mac_dl.ue_created_ev.set();
  }

  // Run all async tasks in same thread.
  manual_task_worker           worker{128};
  dummy_ue_executor_mapper     ul_exec_mapper{worker};
  dummy_dl_executor_mapper     dl_exec_mapper{&worker};
  dummy_mac_event_indicator    du_mng_notif;
  dummy_mac_result_notifier    phy_notifier;
  test_helpers::dummy_mac_pcap pcap;

  // Create a MAC config object.
  mac_common_config_t         mac_cfg{du_mng_notif, ul_exec_mapper, dl_exec_mapper, worker, phy_notifier, pcap};
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
  EXPECT_TRUE(mac_ul_unit_received_ue_create_command());
  EXPECT_EQ(mac_ul.last_ue_create_request->ue_index, msg.ue_index);
  EXPECT_FALSE(mac_dl_unit_received_ue_create_command());
  EXPECT_FALSE(procedure_is_complete());
}

TEST_F(mac_ue_create_procedure_test,
       ue_creation_procedure_calls_and_awaits_mac_dl_configurator_after_mac_ul_result_is_received)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  set_mac_ul_unit_result(true);
  EXPECT_TRUE(mac_dl_unit_received_ue_create_command());
  EXPECT_EQ(mac_dl.last_ue_create_request->ue_index, msg.ue_index);
  EXPECT_FALSE(procedure_is_complete());
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_completes_after_mac_dl_configurator_is_complete)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  set_mac_ul_unit_result(true);
  set_mac_dl_unit_result(true);
  EXPECT_TRUE(procedure_is_complete());
  EXPECT_TRUE(is_procedure_successful());
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_completes_when_there_is_no_mac_ul_and_dl_suspension)
{
  // The procedure coroutine will not suspend while it waits for MAC DL and UL results. We achieve this by setting,
  // the result of MAC UL and DL units before we start the procedure.
  set_mac_ul_unit_result(true);
  set_mac_dl_unit_result(true);
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};

  EXPECT_TRUE(procedure_is_complete());
  EXPECT_TRUE(is_procedure_successful());
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_fails_if_mac_ul_configurator_fails)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  set_mac_ul_unit_result(false);

  EXPECT_TRUE(not mac_dl_unit_received_ue_create_command());
  EXPECT_TRUE(procedure_is_complete());
  EXPECT_FALSE(is_procedure_successful());
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}

TEST_F(mac_ue_create_procedure_test, ue_creation_procedure_fails_if_mac_dl_configurator_fails)
{
  lazy_task_launcher<mac_ue_create_response_message> proc_launcher{proc};
  set_mac_ul_unit_result(true);
  set_mac_dl_unit_result(false);

  EXPECT_TRUE(procedure_is_complete());
  EXPECT_FALSE(is_procedure_successful());
  EXPECT_EQ(msg.ue_index, proc.get().ue_index);
}
