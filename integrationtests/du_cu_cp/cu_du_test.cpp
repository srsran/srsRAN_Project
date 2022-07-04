/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/cu_cp/cu_cp.h"
#include "../../lib/du_high/du_high.h"
#include "../../lib/du_high/du_high_executor_strategies.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/test_helpers.h"

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

/// Test the f1 setup procedure
void test_f1_setup()
{
  test_delimit_logger delimiter{"Test F1 setup procedure between DU and CU-CP"};

  // create worker thread and executer
  task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

  // create message handler for CU and DU to relay messages back and forth
  dummy_f1c_pdu_notifier cu_msg_handler(nullptr);
  dummy_f1c_pdu_notifier du_msg_handler(nullptr);

  // create CU-CP config
  srs_cu_cp::cu_cp_configuration cu_cfg;
  cu_cfg.cu_executor  = task_executor.get();
  cu_cfg.f1c_notifier = &cu_msg_handler;

  // create and start CU
  srs_cu_cp::cu_cp cu_cp_obj(cu_cfg);

  // create and start DU
  du_high_worker_manager workers;
  phy_dummy              phy;

  srsgnb::srs_du::du_high_configuration du_cfg{};
  du_cfg.du_mng_executor = &workers.ctrl_worker;
  du_cfg.dl_executors    = &workers.dl_exec_mapper;
  du_cfg.ul_executors    = &workers.ul_exec_mapper;
  du_cfg.f1c_notifier    = &du_msg_handler;
  du_cfg.phy_adapter     = &phy;
  du_cfg.cells           = {du_config_helpers::make_default_du_cell_config()};

  // create DU object
  srsgnb::srs_du::du_high du_obj(du_cfg);

  // attach CU msg handler to DU message handler and vice-versa
  du_msg_handler.attach_handler(&cu_cp_obj.get_f1c_message_handler());
  cu_msg_handler.attach_handler(&du_obj.get_f1c_message_handler());

  // start CU and DU
  cu_cp_obj.start();
  du_obj.start();

  // TODO: check that DU has been added
  // TESTASSERT_EQ(1, cu_cp_obj.get_nof_dus());

  workers.stop();

  return;
}

/// Test the f1 initial UL RRC message transfer procedure
void test_rrc_message_transfer_procedure()
{
  test_delimit_logger   delimiter{"Test F1 initial UL RRC message transfer procedure between DU and CU-CP"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  // create worker thread and executer
  task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

  // create message handler for CU and DU to relay messages back and forth
  dummy_f1c_pdu_notifier cu_msg_handler(nullptr);
  dummy_f1c_pdu_notifier du_msg_handler(nullptr);

  // create CU-CP config
  srs_cu_cp::cu_cp_configuration cu_cfg;
  cu_cfg.cu_executor  = task_executor.get();
  cu_cfg.f1c_notifier = &cu_msg_handler;

  // create and start CU
  srs_cu_cp::cu_cp cu_cp_obj(cu_cfg);

  // create and start DU
  du_high_worker_manager workers;
  phy_dummy              phy;

  srsgnb::srs_du::du_high_configuration du_cfg{};
  du_cfg.du_mng_executor = &workers.ctrl_worker;
  du_cfg.dl_executors    = &workers.dl_exec_mapper;
  du_cfg.ul_executors    = &workers.ul_exec_mapper;
  du_cfg.f1c_notifier    = &du_msg_handler;
  du_cfg.phy_adapter     = &phy;
  du_cfg.cells           = {du_config_helpers::make_default_du_cell_config()};

  // create DU object
  srsgnb::srs_du::du_high du_obj(du_cfg);

  // attach CU msg handler to DU message handler and vice-versa
  du_msg_handler.attach_handler(&cu_cp_obj.get_f1c_message_handler());
  cu_msg_handler.attach_handler(&du_obj.get_f1c_message_handler());

  // start CU and DU
  cu_cp_obj.start();
  du_obj.start();

  // Handling of Initial UL RRC message transfer
  {
    asn1::f1ap::f1_ap_pdu_c pdu;

    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_ULRRC_MSG_TRANSFER);

    auto& init_ul_rrc                     = pdu.init_msg().value.init_ulrrc_msg_transfer();
    init_ul_rrc->gnb_du_ue_f1_ap_id.value = 41255; // same as C-RNTI

    init_ul_rrc->nrcgi.value.nrcell_id.from_string("000000000000101111000110000101001110"); // 12345678 in decimal
    init_ul_rrc->nrcgi.value.plmn_id.from_string("02f899");
    init_ul_rrc->c_rnti.value = 41255;

    init_ul_rrc->rrc_container.value.from_string("1dec89d05766");
    init_ul_rrc->duto_currc_container_present = true;
    init_ul_rrc->duto_currc_container.value.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");

    // Pass PDU to CU-CP
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj.get_f1c_message_handler().handle_message(pdu);
  }

  // TODO: check that DU has received the RRCSetup

  // Handling of UL RRC message transfer
  {
    asn1::f1ap::f1_ap_pdu_c pdu;

    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_ULRRC_MSG_TRANSFER);

    auto& ul_rrc                     = pdu.init_msg().value.ulrrc_msg_transfer();
    ul_rrc->gnb_cu_ue_f1_ap_id.value = 22;
    ul_rrc->gnb_du_ue_f1_ap_id.value = 41255; // same as C-RNTI
    ul_rrc->srbid.value              = 1;
    ul_rrc->rrc_container.value.from_string(
        "000010c01000082727e01c3ff100c047e004139000bf202f8998000410000000f2e04f070f0707100517e004139000bf202f8998000410"
        "000000f1001032e04f070f0702f1b08010027db00000000080101b669000000000801000001000000005202f8990000011707f070c0401"
        "980b018010174000090530101000000000");

    // Pass PDU to CU-CP
    test_logger.info("Injecting UL RRC message");
    cu_cp_obj.get_f1c_message_handler().handle_message(pdu);
  }

  // TODO: check that CU has received the RRCSetupComplete

  workers.stop();

  return;
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_f1_setup();
  test_rrc_message_transfer_procedure();

  return 0;
}