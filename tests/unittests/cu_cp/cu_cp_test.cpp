/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "common/test_helpers.h"
#include "lib/cu_cp/cu_cp.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/e1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace srsran;
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

    // create CU-CP config
    cu_cp_configuration cfg;
    cfg.cu_cp_executor = &ctrl_worker;
    cfg.f1ap_notifier  = &f1ap_pdu_notifier;
    cfg.e1ap_notifier  = &e1ap_pdu_notifier;
    cfg.ngap_notifier  = &ngap_amf_notifier;

    cfg.ngap_config.ran_node_name = "srsgnb01";
    cfg.ngap_config.plmn          = "00101";
    cfg.ngap_config.tac           = 7;

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

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  dummy_f1ap_pdu_notifier f1ap_pdu_notifier;
  dummy_e1ap_pdu_notifier e1ap_pdu_notifier;
  dummy_ngap_amf_notifier ngap_amf_notifier;

  manual_task_worker ctrl_worker{128};

  std::unique_ptr<cu_cp> cu_cp_obj;
};

//////////////////////////////////////////////////////////////////////////////////////
/* Initial CU-CP routine manager with connected CU-UPs                              */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the initail cu-cp routine
TEST_F(cu_cp_test, when_new_cu_ups_conneced_then_cu_up_e1_setup_request_send)
{
  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_cp_executor = &ctrl_worker;
  cfg.f1ap_notifier  = &f1ap_pdu_notifier;
  cfg.e1ap_notifier  = &e1ap_pdu_notifier;
  cfg.ngap_notifier  = &ngap_amf_notifier;

  cfg.ngap_config.ran_node_name = "srsgnb01";
  cfg.ngap_config.plmn          = "00101";
  cfg.ngap_config.tac           = 7;

  // create and start DUT
  auto dummy_cu_cp = std::make_unique<cu_cp>(std::move(cfg));
  dummy_cu_cp->handle_new_cu_up_connection();

  dummy_cu_cp->start();

  // check that CU-UP has been added
  ASSERT_EQ(dummy_cu_cp->get_nof_cu_ups(), 1U);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);
}

//////////////////////////////////////////////////////////////////////////////////////
/* DU connection handling                                                           */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the DU connection
TEST_F(cu_cp_test, when_new_du_connection_then_du_added)
{
  // Connect DU
  cu_cp_obj->handle_new_du_connection();

  // check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1U);
}

/// Test the DU removal
TEST_F(cu_cp_test, when_du_remove_request_received_then_du_removed)
{
  // Connect DU
  cu_cp_obj->handle_new_du_connection();

  // Check that DU has been added
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 1U);

  // Remove DU
  cu_cp_obj->handle_du_remove_request(du_index_t::min);

  // Check that DU has been removed
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), 0U);
}

/// Test exeeding the maximum number of connected DUs
TEST_F(cu_cp_test, when_max_nof_dus_connected_then_reject_new_connection)
{
  for (int it = du_index_to_uint(du_index_t::min); it < MAX_NOF_DUS; it++) {
    cu_cp_obj->handle_new_du_connection();
  }

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);

  cu_cp_obj->handle_new_du_connection();

  // Check that MAX_NOF_DUS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_dus(), MAX_NOF_DUS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* CU-UP connection handling                                                        */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the CU-UP connection
TEST_F(cu_cp_test, when_new_cu_up_connection_then_cu_up_added)
{
  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();

  // check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);
}

/// Test the CU-UP removal
TEST_F(cu_cp_test, when_cu_up_remove_request_received_then_cu_up_removed)
{
  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();

  // Check that CU-UP has been added
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 1U);

  // Remove CU-UP
  // FIXME: This is scheduled but never run
  cu_cp_obj->handle_cu_up_remove_request(cu_up_index_t::min);

  // Check that CU-UP has been removed
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), 0U);
}

/// Test exeeding the maximum number of connected CU-UPs
TEST_F(cu_cp_test, when_max_nof_cu_ups_connected_then_reject_new_connection)
{
  for (int it = cu_up_index_to_uint(cu_up_index_t::min); it < MAX_NOF_CU_UPS; it++) {
    cu_cp_obj->handle_new_cu_up_connection();
  }

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);

  cu_cp_obj->handle_new_cu_up_connection();

  // Check that MAX_NOF_CU_UPS are connected
  ASSERT_EQ(cu_cp_obj->get_nof_cu_ups(), MAX_NOF_CU_UPS);
}

//////////////////////////////////////////////////////////////////////////////////////
/* AMF connection handling                                                          */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_ng_setup_response_received_then_amf_connected)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());
}

TEST_F(cu_cp_test, when_amf_connected_then_ue_added)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU
  cu_cp_obj->handle_new_du_connection();
  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(f1setup_msg);

  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(int_to_gnb_du_ue_f1ap_id(41255));
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // Inject UL RRC message containing RRC Setup Complete
  f1ap_message ul_rrc_msg = generate_ul_rrc_message_transfer(
      int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(41255), srb_id_t::srb1, generate_rrc_setup_complete());
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(ul_rrc_msg);

  // check that UE has been added
  ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);

  // check that the Initial UE Message was sent to the AMF
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msg.pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
  ASSERT_EQ(ngap_amf_notifier.last_ngap_msg.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id.value.value, 0);
}

TEST_F(cu_cp_test, when_amf_not_connected_then_ue_rejected)
{
  // Connect DU
  cu_cp_obj->handle_new_du_connection();
  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(f1setup_msg);

  gnb_cu_ue_f1ap_id_t cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
  gnb_du_ue_f1ap_id_t du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
  rnti_t              crnti    = to_rnti(0x4601);

  // Inject Initial UL RRC message
  f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(du_ue_id, crnti);
  test_logger.info("Injecting Initial UL RRC message");
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(init_ul_rrc_msg);

  // Inject UE Context Release Complete message
  f1ap_message ue_context_release_complete_msg = generate_ue_context_release_complete(cu_ue_id, du_ue_id);
  test_logger.info("Injecting UE Context Release Complete message");
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(ue_context_release_complete_msg);

  // check that UE has not been added
  ASSERT_EQ(cu_cp_obj->get_nof_ues(), 0U);

  // check that the Initial UE Message was not send to the AMF
  ASSERT_NE(ngap_amf_notifier.last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg);
}

/// Test the f1 initial UL RRC message transfer procedure
TEST_F(cu_cp_test, when_amf_connection_drop_then_reject_ue)
{
  // Connect AMF by injecting a ng_setup_response
  ngap_message ngap_msg = generate_ng_setup_response();
  cu_cp_obj->get_ngap_message_handler().handle_message(ngap_msg);

  ASSERT_TRUE(cu_cp_obj->amf_is_connected());

  // Connect DU
  cu_cp_obj->handle_new_du_connection();
  // Connect CU-UP
  cu_cp_obj->handle_new_cu_up_connection();

  // Generate F1SetupRequest
  f1ap_message f1setup_msg = generate_f1_setup_request();

  // Pass message to CU-CP
  cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(f1setup_msg);

  // Attach first UE (successful)
  {
    gnb_cu_ue_f1ap_id_t first_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(0);
    gnb_du_ue_f1ap_id_t first_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41255);
    rnti_t              crnti             = to_rnti(0x4601);

    // Inject Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(first_ue_du_ue_id, crnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Inject UL RRC message containing RRC Setup Complete
    f1ap_message ul_rrc_msg = generate_ul_rrc_message_transfer(
        first_ue_cu_ue_id, first_ue_du_ue_id, srb_id_t::srb1, generate_rrc_setup_complete());
    test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
    cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(ul_rrc_msg);

    // check that UE has been added
    ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);
  }

  // Disconnect AMF
  cu_cp_obj->handle_amf_connection_drop();

  ASSERT_FALSE(cu_cp_obj->amf_is_connected());

  // Attach second UE (failure)
  {
    gnb_cu_ue_f1ap_id_t second_ue_cu_ue_id = int_to_gnb_cu_ue_f1ap_id(1);
    gnb_du_ue_f1ap_id_t second_ue_du_ue_id = int_to_gnb_du_ue_f1ap_id(41256);
    rnti_t              crnti              = to_rnti(0x4602);

    // Inject Initial UL RRC message
    f1ap_message init_ul_rrc_msg = generate_init_ul_rrc_message_transfer(second_ue_du_ue_id, crnti);
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(init_ul_rrc_msg);

    // Inject UE Context Release Complete message
    f1ap_message ue_context_release_complete_msg =
        generate_ue_context_release_complete(second_ue_cu_ue_id, second_ue_du_ue_id);
    test_logger.info("Injecting UE Context Release Complete message");
    cu_cp_obj->get_f1ap_message_handler(uint_to_du_index(0)).handle_message(ue_context_release_complete_msg);

    // The UE should not exists in the CU-CP
    ASSERT_EQ(cu_cp_obj->get_nof_ues(), 1U);

    // Check that UE has also been removed from F1AP
    ASSERT_EQ(cu_cp_obj->get_f1ap_statistics_handler(uint_to_du_index(0)).get_nof_ues(), 1);
  }
}
