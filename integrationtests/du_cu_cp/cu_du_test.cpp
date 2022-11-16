/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/du_high/du_high.h"
#include "../../lib/du_high/du_high_executor_strategies.h"
#include "unittests/f1c/common/test_helpers.h"
#include "unittests/ngap/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_factory.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// Dummy classes required by DU
struct phy_cell_dummy : public mac_cell_result_notifier {
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override {}
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override {}
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}
};

struct phy_dummy : public mac_result_notifier {
public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
  phy_cell_dummy            cell;
};

struct du_high_worker_manager {
  static const uint32_t task_worker_queue_size = 10000;

  void stop()
  {
    for (auto& w : dl_workers) {
      w.stop();
    }
    for (auto& w : ul_workers) {
      w.stop();
    }
  }

  manual_task_worker ctrl_worker{task_worker_queue_size};
  task_worker        dl_workers[2] = {{"DU-DL#0", task_worker_queue_size}, {"DU-DL#1", task_worker_queue_size}};
  task_worker        ul_workers[2] = {{"DU-UL#0", task_worker_queue_size}, {"DU-UL#1", task_worker_queue_size}};
  static_vector<task_worker_executor, 2> dl_execs{{dl_workers[0]}, {dl_workers[1]}};
  static_vector<task_worker_executor, 2> ul_execs{{ul_workers[0]}, {ul_workers[1]}};
  pcell_ul_executor_mapper               ul_exec_mapper{{&ul_execs[0], &ul_execs[1]}};
  cell_dl_executor_mapper                dl_exec_mapper{{&dl_execs[0], &dl_execs[1]}};
};

/// Fixture class for successful F1Setup
class cu_du_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create worker thread and executer
    task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
    std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

    // create message handler for CU and DU to relay messages back and forth
    dummy_cu_cp_f1c_pdu_notifier      cu_msg_handler(nullptr, nullptr);
    dummy_f1c_pdu_notifier            du_msg_handler(nullptr);
    srs_cu_cp::dummy_ngc_amf_notifier ngc_amf_notifier(nullptr);

    // create CU-CP config
    srs_cu_cp::cu_cp_configuration cu_cfg;
    cu_cfg.cu_executor  = task_executor.get();
    cu_cfg.f1c_notifier = &cu_msg_handler;
    cu_cfg.ngc_notifier = &ngc_amf_notifier;

    // create and start CU
    cu_cp_obj = create_cu_cp(cu_cfg);

    // create and start DU
    phy_dummy phy;

    srsgnb::srs_du::du_high_configuration du_cfg{};
    du_cfg.du_mng_executor = &workers.ctrl_worker;
    du_cfg.dl_executors    = &workers.dl_exec_mapper;
    du_cfg.ul_executors    = &workers.ul_exec_mapper;
    du_cfg.f1c_notifier    = &du_msg_handler;
    du_cfg.phy_adapter     = &phy;
    du_cfg.cells           = {config_helpers::make_default_du_cell_config()};

    // create DU object
    du_obj = std::make_unique<srs_du::du_high>(std::move(du_cfg));

    // attach DU msg handler to CU message handler and vice-versa (in this order)
    cu_msg_handler.attach_handler(cu_cp_obj.get(), &du_obj->get_f1c_message_handler());
    du_msg_handler.attach_handler(&cu_cp_obj->get_f1c_message_handler(srs_cu_cp::int_to_du_index(0)));

    // start CU and DU
    du_obj->start();
  }

public:
  std::unique_ptr<srs_cu_cp::cu_cp_interface> cu_cp_obj;
  std::unique_ptr<srs_du::du_high>            du_obj;
  du_high_worker_manager                      workers;
  srslog::basic_logger&                       test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test the f1 setup procedure was successful
TEST_F(cu_du_test, when_f1setup_successful_then_du_connected)
{
  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1);
}
