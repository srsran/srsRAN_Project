/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "unittests/e1/common/e1_cu_up_test_helpers.h"
#include "unittests/e1/common/test_helpers.h"
#include "srsgnb/e1/cu_up/e1_cu_up.h"
#include "srsgnb/e1/cu_up/e1_cu_up_factory.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_up;

/// Test the successful CU-CP initiated e1 setup procedure
TEST_F(e1_cu_up_test, when_valid_bearer_context_setup_received_then_bearer_context_response_is_send)
{
  // Action 1: Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1_message e1_bearer_context_setup_msg = generate_bearer_context_setup_request_msg(9);

  e1->handle_message(e1_bearer_context_setup_msg);

  // Action 2: Check if BearerContextSetupRequest was forwarded to UE manager
  ASSERT_EQ(ue_manager_notifier->last_bearer_context_setup_request_msg.serving_plmn.to_string(), "02f899");

  // Check the generated PDU is indeed the Bearer Context Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, msg_notifier->last_e1_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_setup_resp,
            msg_notifier->last_e1_msg.pdu.successful_outcome().value.type());
}
