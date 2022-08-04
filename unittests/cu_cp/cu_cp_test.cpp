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
#include "../common_test_helpers/f1_cu_test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* F1 Setup Successful                                                              */
//////////////////////////////////////////////////////////////////////////////////////

/// Fixture class for successful F1Setup
class cu_cp_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    // create worker thread and executer
    task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
    std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

    f1c_pdu_notifier = std::make_unique<dummy_f1c_pdu_notifier>(nullptr);

    // create CU-CP config
    cu_cp_configuration cfg;
    cfg.cu_executor  = task_executor.get();
    cfg.f1c_notifier = f1c_pdu_notifier.get();

    // create and start DUT
    cu_cp_obj = std::make_unique<cu_cp>(std::move(cfg));
    cu_cp_obj->start();
  }

  std::unique_ptr<cu_cp>                  cu_cp_obj;
  std::unique_ptr<dummy_f1c_pdu_notifier> f1c_pdu_notifier;
  srslog::basic_logger&                   test_logger = srslog::fetch_basic_logger("TEST");
};

/// Test the f1 setup procedure
TEST_F(cu_cp_test, when_valid_f1setup_sent_then_du_connected)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_valid_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // check that DU has been added
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 1);

  // Check response is F1SetupResponse
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), f1_ap_pdu_c::types_opts::options::successful_outcome);
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.successful_outcome().value.type(),
            f1_ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp);

  cu_cp_obj->stop();
}

/// Test exeeding the maximum number of connected DUs
TEST_F(cu_cp_test, when_max_nof_dus_connected_then_reject_new_connection)
{
  for (int it = MIN_DU_INDEX; it < MAX_NOF_DUS; it++) {
    cu_cp_obj->on_new_connection();
  }

  // check that MAX_NOF_DUS are connected
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->on_new_connection();

  // check that MAX_NOF_DUS are connected
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->stop();
}

//////////////////////////////////////////////////////////////////////////////////////
/* F1 Setup Failure                                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_du_served_cells_list_missing_then_f1setup_rejected)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_f1_setup_request_base();

  auto& setup_req                             = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = false;

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // check that DU has not been added
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 0);

  // Check the generated PDU is indeed the F1 Setup failure
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), f1_ap_pdu_c::types_opts::options::unsuccessful_outcome);
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.unsuccessful_outcome().value.type(),
            f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

TEST_F(cu_cp_test, when_gnb_du_sys_info_missing_then_f1setup_rejected)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_valid_f1_setup_request();

  auto& setup_req = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list.value[0].value().gnb_du_served_cells_item().gnb_du_sys_info_present = false;

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // check that DU has not been added
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 0);

  // Check the generated PDU is indeed the F1 Setup failure
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type(), f1_ap_pdu_c::types_opts::options::unsuccessful_outcome);
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.unsuccessful_outcome().value.type(),
            f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Initial UL RRC message transfer                                                  */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the f1 initial UL RRC message transfer procedure
TEST_F(cu_cp_test, when_valid_init_ul_rrc_msg_sent_then_ue_added)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Connect AMF
  cu_cp_obj->on_amf_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_valid_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Inject Initial UL RRC message
  f1c_msg init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(41255);
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // Inject UL RRC message containing RRC Setup Complete
  f1c_msg ul_rrc_msg = generate_valid_rrc_setup_complete_msg(0, 41255);
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ul_rrc_msg);

  // check that UE has been added
  EXPECT_EQ(cu_cp_obj->get_nof_ues(), 1);
}

/// Test the f1 initial UL RRC message transfer procedure
TEST_F(cu_cp_test, when_amf_connection_drop_then_reject_ue)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Connect AMF
  cu_cp_obj->on_amf_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_valid_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Attach first UE (successful)
  {
    unsigned int first_ue_c_rnti   = 41255;
    unsigned int first_ue_cu_ue_id = 0;

    // Inject Initial UL RRC message
    f1c_msg init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(first_ue_c_rnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Inject ULL RRC message containing RRC Setup Complete
    f1c_msg ul_rrc_msg = generate_valid_rrc_setup_complete_msg(first_ue_cu_ue_id, first_ue_c_rnti);
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ul_rrc_msg);

    // check that UE has been added
    EXPECT_EQ(cu_cp_obj->get_nof_ues(), 1);
  }

  // Disconnect AMF
  cu_cp_obj->on_amf_connection_drop();

  // Attach second UE (failure)
  {
    unsigned int second_ue_c_rnti   = 41256;
    unsigned int second_ue_cu_ue_id = 1;

    // Inject Initial UL RRC message
    f1c_msg init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(second_ue_c_rnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Check that UE has been added to F1AP (the AMF connection drop isn't visible to F1AP)
    EXPECT_EQ(cu_cp_obj->get_f1c_statistics_handler(int_to_du_index(0)).get_nof_ues(), 2);

    // The UE should not exists in the CU-CP though
    EXPECT_EQ(cu_cp_obj->get_nof_ues(), 1);

    // FIXME: release injection should be done automatically by DU processor

    // Inject UE Context Release Complete message
    f1c_msg ue_context_release_complete_msg =
        generate_f1_ue_context_release_complete_msg(second_ue_cu_ue_id, second_ue_c_rnti);

    test_logger.info("Injecting UE Context Release Complete message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ue_context_release_complete_msg);

    // Check that UE has also been removed from F1AP
    EXPECT_EQ(cu_cp_obj->get_f1c_statistics_handler(int_to_du_index(0)).get_nof_ues(), 1);
  }
}
