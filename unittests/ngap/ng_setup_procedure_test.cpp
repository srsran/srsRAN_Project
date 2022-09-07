/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test successful ng setup procedure
TEST_F(ngap_test, when_ng_setup_response_received_then_amf_connected)
{
  // Action 1: Launch NG setup procedure
  ng_setup_request_message request_msg = generate_ng_setup_request_message();
  test_logger.info("Lauch ng setup request procedure...");
  async_task<ng_setup_response_message>         t = ngap->handle_ngap_setup_request(request_msg);
  lazy_task_launcher<ng_setup_response_message> t_launcher(t);

  // Status: AMF received NG Setup Request.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: NG setup response received.
  ngap_msg ng_setup_response = generate_ng_setup_response_message();
  test_logger.info("Injecting NGSetupResponse");
  ngap->handle_message(ng_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
  ASSERT_EQ(t.get().msg->amf_name.value.to_string(), "open5gs-amf0");
}

/// Test unsuccessful ng setup procedure with time to wait and successful retry
TEST_F(ngap_test, when_ng_setup_failure_with_time_to_wait_received_then_retry_with_success)
{
  // Action 1: Launch NG setup procedure
  ng_setup_request_message request_msg = generate_ng_setup_request_message();
  test_logger.info("Lauch ng setup request procedure...");
  async_task<ng_setup_response_message>         t = ngap->handle_ngap_setup_request(request_msg);
  lazy_task_launcher<ng_setup_response_message> t_launcher(t);

  // Status: AMF received NG Setup Request.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: NG setup failure with time to wait received.
  ngap_msg ng_setup_failure = generate_ng_setup_failure_message_with_time_to_wait(asn1::ngap::time_to_wait_opts::v10s);
  test_logger.info("Injecting NGSetupFailure with time to wait");
  ngap->handle_message(ng_setup_failure);

  // Status: AMF received NG Setup Request again.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Successful outcome after reinitiated NG Setup
  ngap_msg ng_setup_response = generate_ng_setup_response_message();
  test_logger.info("Injecting NGSetupResponse");
  ngap->handle_message(ng_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
  ASSERT_EQ(t.get().msg->amf_name.value.to_string(), "open5gs-amf0");
}

/// Test unsuccessful ng setup procedure with time to wait and unsuccessful retry
TEST_F(ngap_test, when_ng_setup_failure_with_time_to_wait_received_then_retry_without_success)
{
  // Action 1: Launch NG setup procedure
  ng_setup_request_message request_msg = generate_ng_setup_request_message();
  test_logger.info("Lauch ng setup request procedure...");
  async_task<ng_setup_response_message>         t = ngap->handle_ngap_setup_request(request_msg);
  lazy_task_launcher<ng_setup_response_message> t_launcher(t);

  // Status: AMF received NG Setup Request.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: NG setup failure with time to wait received.
  ngap_msg ng_setup_failure = generate_ng_setup_failure_message_with_time_to_wait(asn1::ngap::time_to_wait_opts::v10s);
  test_logger.info("Injecting NGSetupFailure with time to wait");
  ngap->handle_message(ng_setup_failure);

  // Status: AMF received NG Setup Request again.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Unsuccessful outcome after reinitiated NG Setup
  ng_setup_failure = generate_ng_setup_failure_message();
  test_logger.info("Injecting NGSetupFailure");
  ngap->handle_message(ng_setup_failure);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

/// Test the ng setup procedure
TEST_F(ngap_test, when_retry_limit_reached_then_amf_not_connected)
{
  // Action 1: Launch NG setup procedure
  ng_setup_request_message request_msg = generate_ng_setup_request_message();
  test_logger.info("Lauch f1 setup request procedure...");
  async_task<ng_setup_response_message>         t = ngap->handle_ngap_setup_request(request_msg);
  lazy_task_launcher<ng_setup_response_message> t_launcher(t);

  // Status: AMF received NG Setup Request.
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ng_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: NG setup failure received.
  ngap_msg ng_setup_response_msg =
      generate_ng_setup_failure_message_with_time_to_wait(asn1::ngap::time_to_wait_opts::v10s);

  for (unsigned i = 0; i < request_msg.max_setup_retries + 1; i++) {
    ngap->handle_message(ng_setup_response_msg);
  }

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}
