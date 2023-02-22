/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/du_high/du_high.h"
#include "../../../lib/du_high/du_high_executor_strategies.h"
#include "tests/unittests/du_high/test_utils/du_high_worker_manager.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/cu_cp/cu_cp_factory.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/executors/manual_task_worker.h"
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
    dummy_cu_cp_f1ap_pdu_notifier      cu_msg_handler(nullptr, nullptr);
    dummy_f1ap_pdu_notifier            du_msg_handler(nullptr);
    srs_cu_cp::dummy_ngap_amf_notifier ngap_amf_notifier;
    // create CU-CP config
    srs_cu_cp::cu_cp_configuration cu_cfg;
    cu_cfg.cu_cp_executor = &workers.ctrl_worker;
    cu_cfg.f1ap_notifier  = &cu_msg_handler;
    cu_cfg.ngap_notifier  = &ngap_amf_notifier;

    // create and start CU
    cu_cp_obj = create_cu_cp(cu_cfg);

    // create and start DU
    phy_dummy phy;

    srsran::srs_du::du_high_configuration du_cfg{};
    du_cfg.du_mng_executor = &workers.ctrl_worker;
    du_cfg.cell_executors  = &workers.cell_exec_mapper;
    du_cfg.ue_executors    = &workers.ue_exec_mapper;
    du_cfg.f1ap_notifier   = &du_msg_handler;
    du_cfg.phy_adapter     = &phy;
    du_cfg.timers          = &timers;
    du_cfg.cells           = {config_helpers::make_default_du_cell_config()};
    du_cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();

    // create DU object
    du_obj = std::make_unique<srs_du::du_high>(std::move(du_cfg));

    // attach DU msg handler to CU message handler and vice-versa (in this order)
    cu_msg_handler.attach_handler(cu_cp_obj.get(), &du_obj->get_f1ap_message_handler());
    du_msg_handler.attach_handler(&cu_cp_obj->get_f1ap_message_handler(srs_cu_cp::uint_to_du_index(0)));

    // start CU and DU
    du_obj->start();
  }

public:
  std::unique_ptr<srs_cu_cp::cu_cp_interface> cu_cp_obj;
  std::unique_ptr<srs_du::du_high>            du_obj;
  du_high_worker_manager                      workers;
  timer_manager                               timers;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test the f1 setup procedure was successful
TEST_F(cu_du_test, when_f1setup_successful_then_du_connected)
{
  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1);
}
