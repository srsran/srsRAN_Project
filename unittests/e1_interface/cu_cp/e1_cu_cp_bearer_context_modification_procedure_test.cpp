/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/async_test_utils.h"
#include "unittests/e1_interface/common/e1_cu_cp_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the successful bearer context modification procedure (CU-CP initiated)
TEST_F(e1_cu_cp_test, when_bearer_modification_response_received_then_procedure_successful)
{
  // Action 1: Launch Bearer Context modification procedure
  e1ap_bearer_context_modification_request_message request_msg = {};
  test_logger.info("Launch bearer context modification procedure...");
  async_task<e1ap_bearer_context_modification_response_message> t =
      e1->handle_bearer_context_modification_request(request_msg);
  lazy_task_launcher<e1ap_bearer_context_modification_response_message> t_launcher(t);

  // Status: CU-UP received Bearer Context modification Request message.
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.type().value, asn1::e1ap::e1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: E1 Bearer Context modification Response received.
  e1_message bearer_context_modification_response = generate_bearer_context_modification_response_msg(1U, 2U);
  test_logger.info("Injecting BearerContextModificationResponse");
  e1->handle_message(bearer_context_modification_response);

  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().response->gnb_cu_cp_ue_e1_ap_id.value, 1U);
}

/// Test the unsuccessful bearer context modification procedure (CU-CP initiated)
TEST_F(e1_cu_cp_test, when_bearer_modification_failure_received_then_procedure_unsuccessful)
{
  // Action 1: Launch Bearer Context modification procedure
  e1ap_bearer_context_modification_request_message request_msg = {};
  test_logger.info("Launch bearer context modification procedure...");
  async_task<e1ap_bearer_context_modification_response_message> t =
      e1->handle_bearer_context_modification_request(request_msg);
  lazy_task_launcher<e1ap_bearer_context_modification_response_message> t_launcher(t);

  // Status: CU-UP received Bearer Context modification Request message.
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.type().value, asn1::e1ap::e1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier->last_e1_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1_ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: E1 Bearer Context modification Failure received.
  e1_message bearer_context_modification_failure = generate_bearer_context_modification_failure_msg(1U, 2U);
  test_logger.info("Injecting BearerContextModificationFailure");
  e1->handle_message(bearer_context_modification_failure);

  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().failure->gnb_cu_cp_ue_e1_ap_id.value, 1U);
}
