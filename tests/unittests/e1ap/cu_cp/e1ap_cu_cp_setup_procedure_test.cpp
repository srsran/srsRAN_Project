/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "../common/test_helpers.h"
#include "e1ap_cu_cp_test_helpers.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

///////////////////////////////////////
// CU-CP initiated E1 Setup Procedure
///////////////////////////////////////

/// Test successful cu-cp initiated e1 setup procedure
TEST_F(e1ap_cu_cp_test, when_cu_cp_e1_setup_response_received_then_cu_cp_connected)
{
  // Action 1: Launch E1 setup procedure
  cu_cp_e1_setup_request request_msg = generate_cu_cp_e1_setup_request();
  test_logger.info("Launch e1 setup request procedure...");
  async_task<cu_cp_e1_setup_response>         t = e1ap->handle_cu_cp_e1_setup_request(request_msg);
  lazy_task_launcher<cu_cp_e1_setup_response> t_launcher(t);

  // Status: CU-UP received E1 Setup Request.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: E1 setup response received.
  unsigned     transaction_id    = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  e1ap_message e1_setup_response = generate_cu_cp_e1_setup_respose(transaction_id);
  test_logger.info("Injecting CuCpE1SetupResponse");
  e1ap->handle_message(e1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
  ASSERT_EQ(t.get().gnb_cu_up_name.value(), "srsCU-UP");
}

/// Test unsuccessful f1 setup procedure with time to wait and successful retry
TEST_F(e1ap_cu_cp_test, when_cu_cp_e1_setup_failure_with_time_to_wait_received_then_retry_with_success)
{
  // Action 1: Launch E1 setup procedure
  cu_cp_e1_setup_request request_msg = generate_cu_cp_e1_setup_request();
  test_logger.info("Launch e1 setup request procedure...");
  async_task<cu_cp_e1_setup_response>         t = e1ap->handle_cu_cp_e1_setup_request(request_msg);
  lazy_task_launcher<cu_cp_e1_setup_response> t_launcher(t);

  // Status: CU-UP received E1 Setup Request.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action 2: E1 setup failure with time to wait received.
  unsigned     transaction_id = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  e1ap_message e1_setup_failure =
      generate_cu_cp_e1_setup_failure_with_time_to_wait(transaction_id, asn1::e1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting CuCpE1SetupFailure with time to wait");
  e1ap_pdu_notifier.last_e1ap_msg = {};
  e1ap->handle_message(e1_setup_failure);

  // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::nulltype);

    this->tick();
  }

  // Status: CU-UP received E1 Setup Request again.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  unsigned transaction_id2 = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Successful outcome after reinitiated E1 Setup
  e1ap_message e1_setup_response = generate_cu_cp_e1_setup_respose(transaction_id2);
  test_logger.info("Injecting CuCpE1SetupResponse");
  e1ap->handle_message(e1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
  ASSERT_EQ(t.get().gnb_cu_up_name.value(), "srsCU-UP");
}

/// Test unsuccessful e1 setup procedure with time to wait and unsuccessful retry
TEST_F(e1ap_cu_cp_test, when_cu_cp_e1_setup_failure_with_time_to_wait_received_then_retry_without_success)
{
  // Action 1: Launch E1 setup procedure
  cu_cp_e1_setup_request request_msg = generate_cu_cp_e1_setup_request();
  test_logger.info("Launch e1 setup request procedure...");
  async_task<cu_cp_e1_setup_response>         t = e1ap->handle_cu_cp_e1_setup_request(request_msg);
  lazy_task_launcher<cu_cp_e1_setup_response> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action 2: E1 setup failure with time to wait received.
  unsigned     transaction_id = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  e1ap_message e1_setup_failure =
      generate_cu_cp_e1_setup_failure_with_time_to_wait(transaction_id, asn1::e1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting CuCpE1SetupFailure with time to wait");
  e1ap_pdu_notifier.last_e1ap_msg = {};
  e1ap->handle_message(e1_setup_failure);

  // Status: CU-UP does not receive new E1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::nulltype);

    this->tick();
  }

  // Status: CU-UP received E1 Setup Request again.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  unsigned transaction_id2 = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Unsuccessful outcome after reinitiated E1 Setup
  e1_setup_failure = generate_cu_cp_e1_setup_failure(transaction_id2);
  test_logger.info("Injecting CuCpE1SetupFailure");
  e1ap->handle_message(e1_setup_failure);

  ASSERT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
}

/// Test the e1 setup procedure
TEST_F(e1ap_cu_cp_test, when_retry_limit_reached_then_cu_cp_not_connected)
{
  // Action 1: Launch E1 setup procedure
  cu_cp_e1_setup_request request_msg = generate_cu_cp_e1_setup_request();
  test_logger.info("Launch e1 setup request procedure...");
  async_task<cu_cp_e1_setup_response>         t = e1ap->handle_cu_cp_e1_setup_request(request_msg);
  lazy_task_launcher<cu_cp_e1_setup_response> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  for (unsigned i = 0; i < request_msg.max_setup_retries; i++) {
    // Status: F1 setup failure received.
    unsigned     transaction_id = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
    e1ap_message e1_setup_response_msg =
        generate_cu_cp_e1_setup_failure_with_time_to_wait(transaction_id, asn1::e1ap::time_to_wait_opts::v10s);
    e1ap_pdu_notifier.last_e1ap_msg = {};
    e1ap->handle_message(e1_setup_response_msg);

    // Status: CU-UP does not receive new E1 Setup Request until time-to-wait has ended.
    for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
      ASSERT_FALSE(t.ready());
      ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::nulltype);

      this->tick();
    }
  }

  // Status: E1 setup failure received.
  unsigned     transaction_id = get_transaction_id(e1ap_pdu_notifier.last_e1ap_msg.pdu).value();
  e1ap_message e1_setup_response_msg =
      generate_cu_cp_e1_setup_failure_with_time_to_wait(transaction_id, asn1::e1ap::time_to_wait_opts::v10s);
  e1ap_pdu_notifier.last_e1ap_msg = {};
  e1ap->handle_message(e1_setup_response_msg);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type().value, asn1::e1ap::e1ap_pdu_c::types_opts::nulltype);
}

/////////////////////////////////////
// CU-UP initiated E1 Setup Procedure
/////////////////////////////////////

/// Test the successful CU-UP initiated e1 setup procedure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_valid_then_connect_cu_up)
{
  // Action 1: Receive CuUpE1SetupRequest message
  test_logger.info("TEST: Receive CuUpE1SetupRequest message...");

  // Generate E1SetupRequest
  e1ap_message e1_setup_msg = generate_valid_cu_up_e1_setup_request();
  e1ap->handle_message(e1_setup_msg);

  // Action 2: Check if CuUpE1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "srsCU-UP");

  // Action 3: Transmit CuUpE1SetupResponse message
  test_logger.info("TEST: Transmit CuUpE1SetupResponse message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = true;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_cu_up_e1_setup_resp,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.successful_outcome().value.type());
}

/// Test the e1 setup failure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_invalid_then_reject_cu_up)
{
  // Generate CuUpE1SetupRequest
  e1ap_message e1_setup_msg = generate_cu_up_e1_setup_request_base();
  e1ap->handle_message(e1_setup_msg);

  // Action 2 : Check if E1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "srsCU-UP");

  // Action 3: Transmit E1SetupFailure message
  test_logger.info("TEST: Transmit CuUpE1SetupFailure message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = false;
  msg.cause                   = cause_t::radio_network;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.unsuccessful_outcome().value.type());
}
