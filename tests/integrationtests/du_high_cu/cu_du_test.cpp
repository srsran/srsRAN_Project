/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "tests/integrationtests/du_high/test_utils/du_high_worker_manager.h"
#include "tests/test_doubles/f1ap/f1c_test_local_gateway.h"
#include "tests/unittests/cu_cp/test_doubles/mock_amf.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_configuration_helpers.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du/du_high/du_high_clock_controller.h"
#include "srsran/du/du_high/du_high_factory.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"
#include "srsran/support/io/io_broker_factory.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

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

    // create CU-CP config
    srs_cu_cp::cu_cp_configuration cu_cfg = config_helpers::make_default_cu_cp_config();
    cu_cfg.services.cu_cp_executor        = &workers.exec_mapper->du_control_executor(); // reuse du-high ctrl exec
    cu_cfg.services.timers                = &timers;
    cu_cfg.ngap.ngaps.push_back(srs_cu_cp::cu_cp_configuration::ngap_config{
        &*amf, {{7, {{plmn_identity::test_value(), {{slice_service_type{1}}}}}}}});
    cu_cfg.metrics.layers_cfg.enable_ngap = true;
    cu_cfg.metrics.layers_cfg.enable_rrc  = true;

    // create CU-CP.
    cu_cp_obj = create_cu_cp(cu_cfg);

    // Create AMF response to NG Setup.
    amf->enqueue_next_tx_pdu(srs_cu_cp::generate_ng_setup_response());

    // Start CU-CP.
    cu_cp_obj->start();

    // Attach F1-C gateway to CU-CP.
    f1c_gw.attach_cu_cp_du_repo(cu_cp_obj->get_f1c_handler());

    // create and start DU
    phy_dummy phy;

    du_high_configuration du_cfg{};
    du_cfg.ran.cells       = {config_helpers::make_default_du_cell_config()};
    du_cfg.ran.sched_cfg   = config_helpers::make_default_scheduler_expert_config();
    du_cfg.ran.gnb_du_name = "test_du";

    du_high_dependencies du_dependencies;
    du_dependencies.exec_mapper = workers.exec_mapper.get();
    du_dependencies.f1c_client  = &f1c_gw;
    du_dependencies.f1u_gw      = &f1u_gw;
    du_dependencies.phy_adapter = &phy;
    du_dependencies.timer_ctrl  = timer_ctrl.get();

    // create DU object
    du_obj = make_du_high(std::move(du_cfg), du_dependencies);

    // start CU and DU
    du_obj->start();
  }

  ~cu_du_test() override
  {
    // flush logger after each test
    srslog::flush();
  }

public:
  du_high_worker_manager                workers;
  timer_manager                         timers;
  std::unique_ptr<io_broker>            broker{create_io_broker(io_broker_type::epoll)};
  std::unique_ptr<mac_clock_controller> timer_ctrl{
      srs_du::create_du_high_clock_controller(timers, *broker, *workers.time_exec)};
  srslog::basic_logger&  test_logger = srslog::fetch_basic_logger("TEST");
  f1c_test_local_gateway f1c_gw{};
  f1u_test_local_gateway f1u_gw{};

  std::unique_ptr<srs_cu_cp::mock_amf> amf{srs_cu_cp::create_mock_amf()};
  std::unique_ptr<srs_cu_cp::cu_cp>    cu_cp_obj;
  std::unique_ptr<du_high>             du_obj;
};

/// Test the f1 setup procedure was successful
TEST_F(cu_du_test, when_f1setup_successful_then_du_connected)
{
  // check that DU has been added
  auto report = cu_cp_obj->get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.dus.size(), 1);
  ASSERT_EQ(report.dus[0].cells.size(), 1);
}
