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
#include "unittests/f1c/common/f1_cu_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the successful UE context modification procedure (gNB-CU initiated)
TEST_F(f1c_cu_test, when_ue_modification_response_received_then_procedure_successful)
{
  // Action 1: Launch UE Context modification procedure
  f1ap_ue_context_modification_request_message request_msg = {};
  test_logger.info("Launch ue context modification procedure...");
  async_task<f1ap_ue_context_modification_response_message> t =
      f1c->handle_ue_context_modification_request(request_msg);
  lazy_task_launcher<f1ap_ue_context_modification_response_message> t_launcher(t);

  // Status: DU received UE Context modification Request message.
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 UE Context modification Response received.
  f1c_message ue_context_modification_response = generate_ue_context_modification_response_msg(0, 41255);
  test_logger.info("Injecting UEContextModificationResponse");
  f1c->handle_message(ue_context_modification_response);

  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().response->gnb_du_ue_f1_ap_id.value, 41255U);
}

/// Test the unsuccessful UE context modification procedure (gNB-CU initiated)
TEST_F(f1c_cu_test, when_ue_modification_failure_received_then_procedure_unsuccessful)
{
  // Action 1: Launch UE Context modification procedure
  f1ap_ue_context_modification_request_message request_msg = {};
  test_logger.info("Launch ue context modification procedure...");
  async_task<f1ap_ue_context_modification_response_message> t =
      f1c->handle_ue_context_modification_request(request_msg);
  lazy_task_launcher<f1ap_ue_context_modification_response_message> t_launcher(t);

  // Status: DU received UE Context modification Request message.
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(f1c_pdu_notifier->last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::ue_context_mod_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 UE Context modification Failure received.
  f1c_message ue_context_modification_failure = generate_ue_context_modification_failure_msg(0, 41255);
  test_logger.info("Injecting UEContextModificationFailure");
  f1c->handle_message(ue_context_modification_failure);

  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
  EXPECT_EQ(t.get().failure->gnb_du_ue_f1_ap_id.value, 41255U);
}
