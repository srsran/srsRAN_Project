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
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/common_test_helpers/f1_cu_test_helpers.h"
#include "unittests/f1_interface/common/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

/// Fixture class for CU-CP test
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

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();

    cu_cp_obj->stop();
  }

  std::unique_ptr<cu_cp>                  cu_cp_obj;
  std::unique_ptr<dummy_f1c_pdu_notifier> f1c_pdu_notifier;
  srslog::basic_logger&                   test_logger = srslog::fetch_basic_logger("TEST");
};

//////////////////////////////////////////////////////////////////////////////////////
/* DU connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the DU connection
TEST_F(cu_cp_test, when_new_connection_then_du_added)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // check that DU has been added
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 1);
}

/// Test the DU connection
TEST_F(cu_cp_test, when_du_remove_request_received_then_du_removed)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Check that DU has been added
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 1);

  // Remove DU
  cu_cp_obj->handle_du_remove_request(MIN_DU_INDEX);

  // Check that DU has been removed
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), 0);
}

/// Test exeeding the maximum number of connected DUs
TEST_F(cu_cp_test, when_max_nof_dus_connected_then_reject_new_connection)
{
  for (int it = MIN_DU_INDEX; it < MAX_NOF_DUS; it++) {
    cu_cp_obj->on_new_connection();
  }

  // Check that MAX_NOF_DUS are connected
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->on_new_connection();

  // Check that MAX_NOF_DUS are connected
  EXPECT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->stop();
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF connection handling                                                          */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_amf_connected_then_ue_added)
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

TEST_F(cu_cp_test, when_amf_not_connected_then_ue_rejected)
{
  // Connect DU
  cu_cp_obj->on_new_connection();

  // Generate F1SetupRequest
  f1c_msg f1setup_msg = generate_valid_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Inject Initial UL RRC message
  f1c_msg init_ul_rrc_msg = generate_valid_f1_init_ul_rrc_msg(41255);
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // check that UE has been added
  EXPECT_EQ(cu_cp_obj->get_nof_ues(), 0);
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

    // Inject UE Context Release Complete message
    f1c_msg ue_context_release_complete_msg =
        generate_f1_ue_context_release_complete_msg(second_ue_cu_ue_id, second_ue_c_rnti);
    test_logger.info("Injecting UE Context Release Complete message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ue_context_release_complete_msg);

    // Check that UE has also been removed from F1AP
    EXPECT_EQ(cu_cp_obj->get_f1c_statistics_handler(int_to_du_index(0)).get_nof_ues(), 1);
  }
}
