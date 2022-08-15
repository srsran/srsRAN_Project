/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../common/f1_du_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_du;

/// Test successful f1 setup procedure
TEST_F(f1ap_du_test, when_f1_setup_response_received_then_du_connected)
{
  // Action 1: Launch F1 setup procedure
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 setup response received.
  unsigned    transaction_id    = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  f1c_message f1_setup_response = generate_f1_setup_response_message(transaction_id);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().msg->gnb_cu_rrc_version.value.latest_rrc_version.to_number(), 2U);
}

/// Test unsuccessful f1 setup procedure with time to wait and successful retry
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_with_success)
{
  // Action 1: Launch F1 setup procedure
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 setup failure with time to wait received.
  unsigned    transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  f1c_message f1_setup_failure =
      generate_f1_setup_failure_message_with_time_to_wait(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1ap->handle_message(f1_setup_failure);

  // Status: CU received F1 Setup Request again.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Successful outcome after reinitiated F1 Setup
  f1c_message f1_setup_response = generate_f1_setup_response_message(transaction_id2);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  EXPECT_TRUE(t.ready());
  EXPECT_TRUE(t.get().success);
  EXPECT_EQ(t.get().msg->gnb_cu_rrc_version.value.latest_rrc_version.to_number(), 2U);
}

/// Test unsuccessful f1 setup procedure with time to wait and unsuccessful retry
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_without_success)
{
  // Action 1: Launch F1 setup procedure
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 setup failure with time to wait received.
  unsigned    transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  f1c_message f1_setup_failure =
      generate_f1_setup_failure_message_with_time_to_wait(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1ap->handle_message(f1_setup_failure);

  // Status: CU received F1 Setup Request again.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Unsuccessful outcome after reinitiated F1 Setup
  f1_setup_failure = generate_f1_setup_failure_message(transaction_id2);
  test_logger.info("Injecting F1SetupFailure");
  f1ap->handle_message(f1_setup_failure);

  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
}

/// Test the f1 setup procedure
TEST_F(f1ap_du_test, when_retry_limit_reached_then_du_not_connected)
{
  // Action 1: Launch F1 setup procedure
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Lauch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1ap_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.type().value, asn1::f1ap::f1_ap_pdu_c::types_opts::init_msg);
  EXPECT_EQ(msg_notifier.last_f1c_msg.pdu.init_msg().value.type().value,
            asn1::f1ap::f1_ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: F1 setup failure received.
  unsigned    transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
  f1c_message f1_setup_response_msg =
      generate_f1_setup_failure_message_with_time_to_wait(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);

  for (unsigned i = 0; i < request_msg.max_setup_retries + 1; i++) {
    f1ap->handle_message(f1_setup_response_msg);

    // Update transaction id.
    transaction_id = get_transaction_id(msg_notifier.last_f1c_msg.pdu).value();
    f1_setup_response_msg.pdu.unsuccessful_outcome().value.f1_setup_fail()->transaction_id.value = transaction_id;
  }

  EXPECT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
}
