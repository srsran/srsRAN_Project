/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/cu_cp/cu_cp.h"
#include "test_helpers.h"
#include "unittests/e1/common/test_helpers.h"
#include "unittests/f1ap/common/test_helpers.h"
#include "unittests/f1ap/cu_cp/f1_cu_test_helpers.h"
#include "unittests/ngap/ngc_test_helpers.h"
#include "unittests/ngap/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
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

    f1c_pdu_notifier = std::make_unique<dummy_f1c_pdu_notifier>();
    e1_pdu_notifier  = std::make_unique<dummy_e1_pdu_notifier>(nullptr);
    ngc_amf_notifier = std::make_unique<dummy_ngc_amf_notifier>(nullptr);

    // create CU-CP config
    cu_cp_configuration cfg;
    cfg.cu_executor  = task_executor.get();
    cfg.f1c_notifier = f1c_pdu_notifier.get();
    cfg.e1_notifier  = e1_pdu_notifier.get();
    cfg.ngc_notifier = ngc_amf_notifier.get();

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
  std::unique_ptr<dummy_e1_pdu_notifier>  e1_pdu_notifier;
  std::unique_ptr<dummy_ngc_amf_notifier> ngc_amf_notifier;
  srslog::basic_logger&                   test_logger = srslog::fetch_basic_logger("TEST");
};

//////////////////////////////////////////////////////////////////////////////////////
/* DU connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the DU connection
TEST_F(cu_cp_test, when_new_du_connection_then_du_added)
{
  // Connect DU
  cu_cp_obj->on_new_du_connection();

  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1U);
}

/// Test the DU removal
TEST_F(cu_cp_test, when_du_remove_request_received_then_du_removed)
{
  // Connect DU
  cu_cp_obj->on_new_du_connection();

  // Check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1U);

  // Remove DU
  cu_cp_obj->handle_du_remove_request(MIN_DU_INDEX);

  // Check that DU has been removed
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 0U);
}

/// Test exeeding the maximum number of connected DUs
TEST_F(cu_cp_test, when_max_nof_dus_connected_then_reject_new_connection)
{
  for (int it = MIN_DU_INDEX; it < MAX_NOF_DUS; it++) {
    cu_cp_obj->on_new_du_connection();
  }

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->on_new_du_connection();

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* CU-UP connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the CU-UP connection
TEST_F(cu_cp_test, when_new_cu_up_connection_then_cu_up_added)
{
  // Connect CU-UP
  cu_cp_obj->on_new_cu_up_connection();

  // check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);
}

/// Test the CU-UP removal
TEST_F(cu_cp_test, when_cu_up_remove_request_received_then_cu_up_removed)
{
  // Connect CU-UP
  cu_cp_obj->on_new_cu_up_connection();

  // Check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);

  // Remove CU-UP
  // FIXME: This is scheduled but never run
  cu_cp_obj->handle_cu_up_remove_request(MIN_CU_UP_INDEX);

  // Check that CU-UP has been removed
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 0U);
}

/// Test exeeding the maximum number of connected CU-UPs
TEST_F(cu_cp_test, when_max_nof_cu_ups_connected_then_reject_new_connection)
{
  for (int it = MIN_CU_UP_INDEX; it < MAX_NOF_CU_UPS; it++) {
    cu_cp_obj->on_new_cu_up_connection();
  }

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);

  cu_cp_obj->on_new_cu_up_connection();

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF connection handling                                                          */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_ng_setup_response_received_then_amf_connected)
{
  // Connect AMF by injecting a ng_setup_response
  ngc_message ngc_msg = generate_ng_setup_response_message();
  cu_cp_obj->get_ngc_message_handler().handle_message(ngc_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());
}

TEST_F(cu_cp_test, when_amf_connected_then_ue_added)
{
  // Connect AMF by injecting a ng_setup_response
  ngc_message ngc_msg = generate_ng_setup_response_message();
  cu_cp_obj->get_ngc_message_handler().handle_message(ngc_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU
  cu_cp_obj->on_new_du_connection();

  // Generate F1SetupRequest
  f1c_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Inject Initial UL RRC message
  f1c_message init_ul_rrc_msg =
      generate_init_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255));
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // Inject UL RRC message containing RRC Setup Complete
  f1c_message ul_rrc_msg = generate_ul_rrc_message_transfer(
      int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255), srb_id_t::srb1, generate_rrc_setup_complete());
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ul_rrc_msg);

  // check that UE has been added
  ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);

  // check that the Initial UE Message was sent to the AMF
  ASSERT_EQ(ngc_amf_notifier->last_ngc_msg.pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngc_amf_notifier->last_ngc_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngc_amf_notifier->last_ngc_msg.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id.value.value, 0);
}

TEST_F(cu_cp_test, when_amf_not_connected_then_ue_rejected)
{
  // Connect DU
  cu_cp_obj->on_new_du_connection();

  // Generate F1SetupRequest
  f1c_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Inject Initial UL RRC message
  f1c_message init_ul_rrc_msg =
      generate_init_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255));
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // check that UE has been added
  ASSERT_EQ(cu_cp_obj->get_nof_ues(), 0U);

  // check that the Initial UE Message was not send to the AMF
  ASSERT_NE(ngc_amf_notifier->last_ngc_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
}

/// Test the f1 initial UL RRC message transfer procedure
TEST_F(cu_cp_test, when_amf_connection_drop_then_reject_ue)
{
  // Connect AMF by injecting a ng_setup_response
  ngc_message ngc_msg = generate_ng_setup_response_message();
  cu_cp_obj->get_ngc_message_handler().handle_message(ngc_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU
  cu_cp_obj->on_new_du_connection();

  // Generate F1SetupRequest
  f1c_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(f1setup_msg);

  // Attach first UE (successful)
  {
    gnb_cu_ue_f1ap_id_t first_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t first_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
    rnti_t              crnti             = to_rnti(0x4601);

    // Inject Initial UL RRC message
    f1c_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(first_ue_cu_ue_id, first_ue_du_ue_id, crnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Inject UL RRC message containing RRC Setup Complete
    f1c_message ul_rrc_msg = generate_ul_rrc_message_transfer(
        first_ue_cu_ue_id, first_ue_du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ul_rrc_msg);

    // check that UE has been added
    ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);
  }

  // Disconnect AMF
  cu_cp_obj->on_amf_connection_drop();

  ASSERT_FALSE(cu_cp_obj->amf_is_connected());

  // Attach second UE (failure)
  {
    gnb_cu_ue_f1ap_id_t second_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t second_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41256);
    rnti_t              crnti              = to_rnti(0x4602);

    // Inject Initial UL RRC message
    f1c_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(second_ue_cu_ue_id, second_ue_du_ue_id, crnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Check that UE has been added to F1C (the AMF connection drop isn't visible to F1C)
    ASSERT_EQ(cu_cp_obj->get_f1c_statistics_handler(int_to_du_index(0)).get_nof_ues(), 2);

    // The UE should not exists in the CU-CP though
    ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);

    // Inject UE Context Release Complete message
    f1c_message ue_context_release_complete_msg =
        generate_ue_context_release_complete(second_ue_cu_ue_id, second_ue_du_ue_id);
    test_logger.info("Injecting UE Context Release Complete message");
    cu_cp_obj->get_f1c_message_handler(int_to_du_index(0)).handle_message(ue_context_release_complete_msg);

    // Check that UE has also been removed from F1C
    ASSERT_EQ(cu_cp_obj->get_f1c_statistics_handler(int_to_du_index(0)).get_nof_ues(), 1);
  }
}
