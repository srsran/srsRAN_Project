/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../common/test_helpers.h"
#include "e1ap_cu_up_test_helpers.h"
#include "srsgnb/e1/cu_up/e1ap_cu_up.h"
#include "srsgnb/e1/cu_up/e1ap_cu_up_factory.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome
  msg_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still set_init_msg)
  EXPECT_EQ(msg_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  msg_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(msg_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  msg_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(msg_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of E1 Setup                                                             */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful CU-CP initiated e1 setup
TEST_F(e1ap_cu_up_test, when_cu_cp_e1_setup_request_valid_then_connect_cu_cp)
{
  // Receive E1SetupRequest message
  test_logger.info("TEST: Receive E1SetupRequest message...");

  // Generate E1SetupRequest
  e1_message cu_cp_e1_setup = generate_cu_cp_e1_setup_request();

  e1ap->handle_message(cu_cp_e1_setup);

  // Check if E1SetupRequest was forwarded to UE manager
  ASSERT_EQ(cu_up_notifier.last_cu_cp_e1_setup_request.request->gnb_cu_cp_name.value.to_string(), "srsCU-CP");

  // Action 3: Transmit E1SetupResponse message
  test_logger.info("TEST: Transmit E1SetupResponse message...");
  cu_cp_e1_setup_response msg = {};
  msg.success                 = true;
  e1ap->handle_cu_cp_e1_setup_response(msg);

  // Check the generated PDU is indeed the F1 Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_cu_cp_e1_setup_resp,
            msg_notifier.last_e1_msg.pdu.successful_outcome().value.type());
}

/// Test the f1 setup failure
TEST_F(e1ap_cu_up_test, when_cu_cp_e1_setup_request_cant_be_handled_then_reject_cu_cp)
{
  // Generate E1SetupRequest
  e1_message cu_cp_e1_setup = generate_cu_cp_e1_setup_request();

  e1ap->handle_message(cu_cp_e1_setup);

  // Check if E1SetupRequest was forwarded to UE manager
  ASSERT_EQ(cu_up_notifier.last_cu_cp_e1_setup_request.request->gnb_cu_cp_name.value.to_string(), "srsCU-CP");

  // Action 3: Transmit E1SetupFailure message
  test_logger.info("TEST: Transmit E1SetupFailure message...");
  cu_cp_e1_setup_response msg = {};
  msg.success                 = false;
  e1ap->handle_cu_cp_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail,
            msg_notifier.last_e1_msg.pdu.unsuccessful_outcome().value.type());
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of Bearer Context Messages                                              */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_setup_received_then_bearer_context_setup_response_is_sent)
{
  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1_message e1_bearer_context_setup = generate_bearer_context_setup_request(9);

  e1ap->handle_message(e1_bearer_context_setup);

  // Check if BearerContextSetupRequest was forwarded to UE manager
  ASSERT_EQ(cu_up_notifier.last_bearer_context_setup_request.serving_plmn.to_string(), "02f899");

  // Check the generated PDU is indeed the Bearer Context Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_setup_resp,
            msg_notifier.last_e1_msg.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_invalid_bearer_context_setup_received_then_bearer_context_setup_failure_is_sent)
{
  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1_message e1_bearer_context_setup = generate_invalid_bearer_context_setup_request(9);

  e1ap->handle_message(e1_bearer_context_setup);

  // Check the generated PDU is indeed the Bearer Context Setup Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_setup_fail,
            msg_notifier.last_e1_msg.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_valid_bearer_context_modification_received_then_bearer_context_modification_response_is_sent)
{
  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Generate BearerContextModificationRequest
  e1_message e1_bearer_context_modification = generate_bearer_context_modification_request(9, 0);

  e1ap->handle_message(e1_bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_mod_resp,
            msg_notifier.last_e1_msg.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_invalid_bearer_context_modification_received_then_bearer_context_modification_failure_is_sent)
{
  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextModificationRequest message
  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Generate BearerContextModificationRequest
  e1_message e1_bearer_context_modification = generate_invalid_bearer_context_modification_request(9, 0);

  e1ap->handle_message(e1_bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_mod_fail,
            msg_notifier.last_e1_msg.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_release_command_received_then_bearer_context_release_complete_is_sent)
{
  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextReleaseCommand message...");

  // Generate BearerContextReleaseCommand
  e1_message e1_bearer_context_release_cmd = generate_bearer_context_release_command(9, 0);

  e1ap->handle_message(e1_bearer_context_release_cmd);

  // Check the generated PDU is indeed the Bearer Context Modification Response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, msg_notifier.last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_release_complete,
            msg_notifier.last_e1_msg.pdu.successful_outcome().value.type());
}
