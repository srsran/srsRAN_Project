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

#include "lib/du_high/du_high_executor_strategies.h"
#include "tests/integrationtests/du_high/test_utils/du_high_worker_manager.h"
#include "tests/test_doubles/f1ap/f1c_test_local_gateway.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du_high/du_high_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

// Dummy classes required by DU
struct phy_cell_dummy : public mac_cell_result_notifier {
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override {}
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override {}
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}
  void on_cell_results_completion(slot_point slot) override {}
};

struct phy_dummy : public mac_result_notifier {
public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
  phy_cell_dummy            cell;
};

/// Fixture class for successful F1Setup
class cu_du_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create message handler for CU and DU to relay messages back and forth
    srs_cu_cp::dummy_ngap_amf_notifier ngap_amf_notifier;
    // create CU-CP config
    srs_cu_cp::cu_cp_configuration cu_cfg;
    cu_cfg.cu_cp_executor = &workers.ctrl_exec;
    cu_cfg.ngap_notifier  = &ngap_amf_notifier;

    // create and start CU
    cu_cp_obj = create_cu_cp(cu_cfg);

    // Attach F1-C gateway to CU-CP.
    f1c_gw.attach_cu_cp_du_repo(cu_cp_obj->get_connected_dus());

    // create and start DU
    phy_dummy phy;

    srsran::srs_du::du_high_configuration du_cfg{};
    du_cfg.exec_mapper = &workers.exec_mapper;
    du_cfg.f1c_client  = &f1c_gw;
    du_cfg.phy_adapter = &phy;
    du_cfg.timers      = &timers;
    du_cfg.cells       = {config_helpers::make_default_du_cell_config()};
    du_cfg.sched_cfg   = config_helpers::make_default_scheduler_expert_config();

    // create DU object
    du_obj = make_du_high(std::move(du_cfg));

    // start CU and DU
    du_obj->start();
  }

public:
  du_high_worker_manager                      workers;
  timer_manager                               timers;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
  f1c_test_local_gateway                      f1c_gw{};
  std::unique_ptr<srs_cu_cp::cu_cp_interface> cu_cp_obj;
  std::unique_ptr<du_high>                    du_obj;
};

/// Test the f1 setup procedure was successful
TEST_F(cu_du_test, when_f1setup_successful_then_du_connected)
{
  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_connected_dus().get_nof_dus(), 1);
}
