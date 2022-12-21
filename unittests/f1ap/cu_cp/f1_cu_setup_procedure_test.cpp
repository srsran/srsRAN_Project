/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_cu_test_helpers.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the successful f1 setup procedure
TEST_F(f1ap_cu_test, when_f1_setup_request_valid_then_connect_du)
{
  // Action 1: Receive F1SetupRequest message
  test_logger.info("TEST: Receive F1SetupRequest message...");

  // Generate F1SetupRequest
  f1c_message f1setup_msg = generate_f1_setup_request();

  f1c->handle_message(f1setup_msg);

  // Action 2: Check if F1SetupRequest was forwarded to DU processor
  ASSERT_EQ(du_processor_notifier.last_f1_setup_request_msg.request->gnb_du_id.value, 0x11U);

  // Action 3: Transmit F1SetupResponse message
  test_logger.info("TEST: Transmit F1SetupResponse message...");
  f1_setup_response_message msg = {};
  msg.success                   = true;
  f1c->handle_f1_setup_response(msg);

  // Check the generated PDU is indeed the F1 Setup response
  ASSERT_EQ(asn1::f1ap::f1_ap_pdu_c::types_opts::options::successful_outcome, f1c_pdu_notifier.last_f1c_msg.pdu.type());
  ASSERT_EQ(asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp,
            f1c_pdu_notifier.last_f1c_msg.pdu.successful_outcome().value.type());
}

/// Test the f1 setup failure
TEST_F(f1ap_cu_test, when_f1_setup_request_invalid_then_reject_du)
{
  // Generate Invalid F1SetupRequest
  f1c_message f1setup_msg                     = generate_f1_setup_request();
  auto&       setup_req                       = f1setup_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = false;
  setup_req->gnb_du_served_cells_list.value.clear();

  f1c->handle_message(f1setup_msg);

  // Action 2: Check if F1SetupRequest was forwarded to DU processor
  ASSERT_EQ(du_processor_notifier.last_f1_setup_request_msg.request->gnb_du_id.value, 0x11U);

  // Action 3: Transmit F1SetupFailure message
  test_logger.info("TEST: Transmit F1SetupFailure message...");
  f1_setup_response_message msg = {};
  msg.success                   = false;
  f1c->handle_f1_setup_response(msg);

  // Check the generated PDU is indeed the F1 Setup failure
  ASSERT_EQ(asn1::f1ap::f1_ap_pdu_c::types_opts::options::unsuccessful_outcome,
            f1c_pdu_notifier.last_f1c_msg.pdu.type());
  ASSERT_EQ(asn1::f1ap::f1_ap_elem_procs_o::unsuccessful_outcome_c::types_opts::f1_setup_fail,
            f1c_pdu_notifier.last_f1c_msg.pdu.unsuccessful_outcome().value.type());
}
