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

#include "du_manager_procedure_test_helpers.h"
#include "lib/du_manager/procedures/du_ue_ric_configuration_procedure.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

class du_ue_ric_config_tester : public du_manager_proc_tester, public ::testing::Test
{
protected:
  du_ue_ric_config_tester() :
    du_manager_proc_tester(std::vector<du_cell_config>{config_helpers::make_default_du_cell_config()})
  {
    test_ue = &create_ue(to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX)));
  }

  void start_procedure(const ric_control_config& request)
  {
    proc = launch_async<du_ue_ric_configuration_procedure>(request, ue_mng, params);
    proc_launcher.emplace(proc);
  }

  optional<ric_control_config_response> proc_result()
  {
    return proc_launcher->ready() ? optional<ric_control_config_response>{proc_launcher->get()} : nullopt;
  }

  du_ue*                                                    test_ue = nullptr;
  async_task<ric_control_config_response>                   proc;
  optional<lazy_task_launcher<ric_control_config_response>> proc_launcher;
};

TEST_F(du_ue_ric_config_tester,
       when_new_ric_config_is_started_then_du_manager_starts_mac_config_and_waits_for_mac_response)
{
  start_procedure(ric_control_config{(uint64_t)test_ue->f1ap_ue_id, nullopt, nullopt, 5, 10});

  ASSERT_TRUE(mac.last_ue_reconf_msg.has_value()) << "MAC should have received new configuration";
  ASSERT_EQ(mac.last_ue_reconf_msg->ue_index, test_ue->ue_index);
  ASSERT_TRUE(mac.last_ue_reconf_msg->sched_cfg.res_alloc_cfg.has_value());
  prb_interval expected_prbs{5, 10};
  ASSERT_EQ(mac.last_ue_reconf_msg->sched_cfg.res_alloc_cfg->pdsch_grant_size_limits, expected_prbs);
  ASSERT_FALSE(mac.last_ue_reconf_msg->sched_cfg.cells.has_value()) << "Cells should not have been configured";
  ASSERT_FALSE(mac.last_ue_reconf_msg->sched_cfg.lc_config_list.has_value())
      << "Logical channels should not have been configured";
  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";
}

TEST_F(du_ue_ric_config_tester, when_mac_finished_configuration_then_procedure_finishes)
{
  start_procedure(ric_control_config{(uint64_t)test_ue->f1ap_ue_id, nullopt, nullopt, 5, 10});

  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";
  mac.wait_ue_reconf.result = mac_ue_reconfiguration_response{test_ue->ue_index, true};
  mac.wait_ue_reconf.ready_ev.set();
  ASSERT_TRUE(proc_result().has_value()) << "The procedure should have finished after receiving MAC response";
  ASSERT_TRUE(proc_result()->max_prb_alloc_result);
  ASSERT_TRUE(proc_result()->min_prb_alloc_result);
}

TEST_F(du_ue_ric_config_tester,
       when_new_ric_config_is_started_but_another_ue_procedure_is_running_then_ric_config_waits)
{
  // Schedule a dummy procedure in the UE task loop.
  manual_event_flag previous_event_complete;
  ue_mng.schedule_async_task(test_ue->ue_index,
                             launch_async([&previous_event_complete](coro_context<async_task<void>>& ctx) {
                               CORO_BEGIN(ctx);
                               CORO_AWAIT(previous_event_complete);
                               CORO_RETURN();
                             }));

  // Start RIC UE config.
  start_procedure(ric_control_config{(uint64_t)test_ue->f1ap_ue_id, nullopt, nullopt, 5, 10});

  // Status: RIC config procedure is waiting for previous procedure.
  ASSERT_FALSE(mac.last_ue_reconf_msg.has_value())
      << "RIC configuration should not start until previous procedure finishes";

  // Previous procedure finishes.
  previous_event_complete.set();
  ASSERT_TRUE(mac.last_ue_reconf_msg.has_value())
      << "RIC configuration should not start until previous procedure finishes";
  ASSERT_FALSE(proc_result().has_value()) << "The procedure should wait for MAC response";

  // MAC finishes configuration and RIC procedure finishes.
  mac.wait_ue_reconf.result = mac_ue_reconfiguration_response{test_ue->ue_index, true};
  mac.wait_ue_reconf.ready_ev.set();
  ASSERT_TRUE(proc_result().has_value()) << "The procedure should have finished after receiving MAC response";
}