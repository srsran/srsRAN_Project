/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/e1_interface/cu_up/e1_cu_up.h"
#include "srsgnb/e1_interface/cu_up/e1_cu_up_factory.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/e1_interface/common/e1_cu_up_test_helpers.h"
#include "unittests/e1_interface/common/test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;

/// Test the successful CU-CP initiated e1 setup procedure
TEST_F(e1_cu_up_test, when_cu_cp_e1_setup_request_valid_then_connect_cu_cp)
{
  // Action 1: Receive E1SetupRequest message
  test_logger.info("TEST: Receive E1SetupRequest message...");

  // Generate E1SetupRequest
  e1_message cu_cp_e1_setup_msg = generate_cu_cp_e1_setup_request_message();

  e1->handle_message(cu_cp_e1_setup_msg);

  // Action 2: Check if E1SetupRequest was forwarded to UE manager
  ASSERT_EQ(ue_manager_notifier->last_cu_cp_e1_setup_request_msg.request->gnb_cu_cp_name.value.to_string(), "srsCU-CP");

  // Action 3: Transmit E1SetupResponse message
  test_logger.info("TEST: Transmit E1SetupResponse message...");
  cu_cp_e1_setup_response_message msg = {};
  msg.success                         = true;
  e1->handle_cu_cp_e1_setup_response(msg);

  // Check the generated PDU is indeed the F1 Setup response
  ASSERT_EQ(asn1::e1ap::e1_ap_pdu_c::types_opts::options::successful_outcome, msg_notifier->last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1_ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_cu_cp_e1_setup_resp,
            msg_notifier->last_e1_msg.pdu.successful_outcome().value.type());
}

/// Test the f1 setup failure
TEST_F(e1_cu_up_test, when_cu_cp_e1_setup_request_cant_be_handled_then_reject_cu_cp)
{
  // Generate E1SetupRequest
  e1_message cu_cp_e1_setup_msg = generate_cu_cp_e1_setup_request_message();

  e1->handle_message(cu_cp_e1_setup_msg);

  // Action 2: Check if E1SetupRequest was forwarded to UE manager
  ASSERT_EQ(ue_manager_notifier->last_cu_cp_e1_setup_request_msg.request->gnb_cu_cp_name.value.to_string(), "srsCU-CP");

  // Action 3: Transmit E1SetupFailure message
  test_logger.info("TEST: Transmit E1SetupFailure message...");
  cu_cp_e1_setup_response_message msg = {};
  msg.success                         = false;
  e1->handle_cu_cp_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup failure
  ASSERT_EQ(asn1::e1ap::e1_ap_pdu_c::types_opts::options::unsuccessful_outcome, msg_notifier->last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_cp_e1_setup_fail,
            msg_notifier->last_e1_msg.pdu.unsuccessful_outcome().value.type());
}
