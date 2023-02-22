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

#include "lib/mac/du_rnti_table.h"
#include "lib/mac/mac_ctrl/mac_controller.h"
#include "mac_ctrl_test_dummies.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include "tests/unittests/mac/mac_test_helpers.h"

using namespace srsran;

void test_mac_ctrl_ue_procedures()
{
  test_delimit_logger delimiter{"Test UE procedures"};

  manual_task_worker              worker{128};
  dummy_ue_executor_mapper        ul_exec_mapper{worker};
  dummy_dl_executor_mapper        dl_exec_mapper{&worker};
  dummy_mac_event_indicator       du_mng_notifier;
  dummy_mac_result_notifier       phy_notifier;
  test_helpers::dummy_mac_pcap    pcap;
  mac_common_config_t             cfg{du_mng_notifier, ul_exec_mapper, dl_exec_mapper, worker, phy_notifier, pcap};
  mac_ul_dummy_configurer         ul_unit;
  mac_dl_dummy_configurer         dl_unit;
  mac_scheduler_dummy_adapter     sched_cfg_adapter;
  du_rnti_table                   rnti_table;
  dummy_rach_handler_configurator rach_hdl;

  mac_controller mac_ctrl(cfg, ul_unit, dl_unit, rach_hdl, rnti_table, sched_cfg_adapter);

  // Action 1: Create UE
  mac_ue_create_request_message ue_create_msg{};
  ue_create_msg.ue_index                               = to_du_ue_index(1);
  ue_create_msg.cell_index                             = to_du_cell_index(0);
  ue_create_msg.crnti                                  = to_rnti(0x4601);
  async_task<mac_ue_create_response_message>         t = mac_ctrl.handle_ue_create_request(ue_create_msg);
  lazy_task_launcher<mac_ue_create_response_message> t_launcher(t);

  // Status: UE creation started in MAC UL but not in MAC DL
  TESTASSERT(ul_unit.last_ue_create_request.has_value());
  TESTASSERT_EQ(ue_create_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  TESTASSERT_EQ(ue_create_msg.crnti, ul_unit.last_ue_create_request->crnti);
  TESTASSERT(not dl_unit.last_ue_create_request.has_value());
  TESTASSERT(not t.ready());

  // Action 2: MAC UL UE Creation finishes
  ul_unit.expected_result = true;
  ul_unit.ue_created_ev.set();

  // Status: MAC DL UE Creation starts
  TESTASSERT(dl_unit.last_ue_create_request.has_value());
  TESTASSERT_EQ(ue_create_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  TESTASSERT_EQ(ue_create_msg.crnti, dl_unit.last_ue_create_request->crnti);
  TESTASSERT(not t.ready());

  // Action 3: MAC DL UE Creation finishes
  dl_unit.expected_result = true;
  dl_unit.ue_created_ev.set();

  // Status: MAC DL UE Creation finished. MAC CTRL UE Creation finished as well.
  TESTASSERT(dl_unit.last_ue_create_request.has_value());
  TESTASSERT(t.ready());
  TESTASSERT_EQ(ue_create_msg.ue_index, t.get().ue_index);
  TESTASSERT(t.get().result);
  TESTASSERT(mac_ctrl.find_ue(ue_create_msg.ue_index) != nullptr);
  TESTASSERT(mac_ctrl.find_by_rnti(ue_create_msg.crnti) != nullptr);
  TESTASSERT_EQ(ue_create_msg.ue_index, mac_ctrl.find_ue(ue_create_msg.ue_index)->du_ue_index);

  // Action 4: Delete UE
  mac_ue_delete_request_message ue_delete_msg{};
  ue_delete_msg.ue_index                                = to_du_ue_index(1);
  ue_delete_msg.rnti                                    = to_rnti(0x4601);
  ue_delete_msg.cell_index                              = to_du_cell_index(0);
  async_task<mac_ue_delete_response_message>         t2 = mac_ctrl.handle_ue_delete_request(ue_delete_msg);
  lazy_task_launcher<mac_ue_delete_response_message> t_launcher2(t2);

  // Status: UE deleted from MAC DL, UL and CTRL
  TESTASSERT(dl_unit.last_ue_delete_request.has_value());
  TESTASSERT_EQ(ue_delete_msg.ue_index, dl_unit.last_ue_create_request->ue_index);
  TESTASSERT(ul_unit.last_ue_delete_request.has_value());
  TESTASSERT_EQ(ue_delete_msg.ue_index, ul_unit.last_ue_create_request->ue_index);
  TESTASSERT(t2.ready());
  TESTASSERT(mac_ctrl.find_ue(ue_create_msg.ue_index) == nullptr);
  TESTASSERT(mac_ctrl.find_by_rnti(ue_create_msg.crnti) == nullptr);
}

int main()
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);

  srslog::init();

  test_mac_ctrl_ue_procedures();
}
