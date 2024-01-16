/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "lib/mac/mac_ctrl/mac_controller.h"
#include "lib/mac/rnti_manager.h"
#include "mac_ctrl_test_dummies.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;

class mac_controller_test : public ::testing::Test
{
protected:
  mac_controller_test()
  {
    srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
    srslog::init();
  }

  void start_ue_creation(mac_ue_create_request req)
  {
    t = mac_ctrl.handle_ue_create_request(req);
    t_launcher.emplace(t);
  }

  manual_task_worker          worker{128};
  dummy_ue_executor_mapper    ul_exec_mapper{worker};
  dummy_dl_executor_mapper    dl_exec_mapper{&worker};
  dummy_mac_event_indicator   du_mng_notifier;
  mac_control_config          maccfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, worker};
  mac_ul_dummy_configurer     ul_unit;
  mac_dl_dummy_configurer     dl_unit;
  mac_scheduler_dummy_adapter sched_cfg_adapter;
  rnti_manager                rnti_table;

  mac_controller mac_ctrl{maccfg, ul_unit, dl_unit, rnti_table, sched_cfg_adapter};

  async_task<mac_ue_create_response>                   t;
  optional<lazy_task_launcher<mac_ue_create_response>> t_launcher;
};

TEST_F(mac_controller_test, ue_procedures)
{
  // Action 1: Create UE
  mac_ue_create_request ue_create_msg{to_du_cell_index(0), to_du_ue_index(1), to_rnti(0x4601)};
  start_ue_creation(ue_create_msg);

  // Status: UE creation started in MAC UL but not in MAC DL
  ASSERT_TRUE(ul_unit.last_ue_create_request.has_value());
  ASSERT_EQ(ue_create_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  ASSERT_EQ(ue_create_msg.crnti, ul_unit.last_ue_create_request->crnti);
  ASSERT_FALSE(dl_unit.last_ue_create_request.has_value());
  ASSERT_FALSE(t.ready());

  // Action 2: MAC UL UE Creation finishes
  ul_unit.expected_result = true;
  ul_unit.ue_created_ev.set();

  // Status: MAC DL UE Creation starts
  ASSERT_TRUE(dl_unit.last_ue_create_request.has_value());
  ASSERT_EQ(ue_create_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  ASSERT_EQ(ue_create_msg.crnti, dl_unit.last_ue_create_request->crnti);
  ASSERT_FALSE(t.ready());

  // Action 3: MAC DL UE Creation finishes
  dl_unit.expected_result = true;
  dl_unit.ue_created_ev.set();

  // Status: Scheduler UE Creation starts.
  ASSERT_TRUE(sched_cfg_adapter.last_ue_create_request.has_value());
  ASSERT_EQ(sched_cfg_adapter.last_ue_create_request->ue_index, ue_create_msg.ue_index);
  ASSERT_FALSE(t.ready());

  // Action 4: Scheduler UE Creation finishes
  sched_cfg_adapter.ue_created_ev.set(true);

  // Status: MAC DL UE Creation finished. MAC CTRL UE Creation finished as well.
  ASSERT_TRUE(t.ready());
  ASSERT_EQ(ue_create_msg.ue_index, t.get().ue_index);
  ASSERT_EQ(t.get().allocated_crnti, ue_create_msg.crnti);
  ASSERT_TRUE(mac_ctrl.find_ue(ue_create_msg.ue_index) != nullptr);
  ASSERT_TRUE(mac_ctrl.find_by_rnti(ue_create_msg.crnti) != nullptr);
  ASSERT_EQ(ue_create_msg.ue_index, mac_ctrl.find_ue(ue_create_msg.ue_index)->du_ue_index);

  // Action 4: Delete UE
  mac_ue_delete_request ue_delete_msg{};
  ue_delete_msg.ue_index                        = to_du_ue_index(1);
  ue_delete_msg.rnti                            = to_rnti(0x4601);
  ue_delete_msg.cell_index                      = to_du_cell_index(0);
  async_task<mac_ue_delete_response>         t2 = mac_ctrl.handle_ue_delete_request(ue_delete_msg);
  lazy_task_launcher<mac_ue_delete_response> t_launcher2(t2);

  // Status: UE deleted from MAC DL, UL and CTRL
  ASSERT_TRUE(dl_unit.last_ue_delete_request.has_value());
  ASSERT_EQ(ue_delete_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  ASSERT_TRUE(ul_unit.last_ue_delete_request.has_value());
  ASSERT_EQ(ue_delete_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  ASSERT_TRUE(t2.ready());
  ASSERT_TRUE(mac_ctrl.find_ue(ue_create_msg.ue_index) == nullptr);
  ASSERT_TRUE(mac_ctrl.find_by_rnti(ue_create_msg.crnti) == nullptr);
}
