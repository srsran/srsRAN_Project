/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "f1ap_du_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

/// Test successful f1 setup procedure
TEST_F(f1ap_du_test, when_f1_setup_response_received_then_du_connected)
{
  // Action: Launch F1 setup procedure
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);
  const auto& f1_setup_req = f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.f1_setup_request();
  ASSERT_FALSE(
      f1_setup_req->gnb_du_served_cells_list[0]->gnb_du_served_cells_item().served_cell_info.meas_timing_cfg.empty());

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action: F1 setup response received.
  unsigned     transaction_id    = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  f1ap_message f1_setup_response = generate_f1_setup_response_message(transaction_id);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// Test unsuccessful f1 setup procedure with time to wait and successful retry
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_with_success)
{
  // Action: Launch F1 setup procedure
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  // Action: F1 setup failure with time to wait received.
  unsigned     transaction_id = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  f1ap_message f1_setup_failure =
      generate_f1_setup_failure_message(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1c_gw.last_tx_f1ap_pdu = {};
  f1ap->handle_message(f1_setup_failure);

  // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::nulltype);

    this->tick();
  }

  // Status: CU received F1 Setup Request again.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Successful outcome after reinitiated F1 Setup
  f1ap_message f1_setup_response = generate_f1_setup_response_message(transaction_id2);
  test_logger.info("Injecting F1SetupResponse");
  f1ap->handle_message(f1_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

/// Test unsuccessful f1 setup procedure with time to wait and unsuccessful retry
TEST_F(f1ap_du_test, when_f1_setup_failure_with_time_to_wait_received_then_retry_without_success)
{
  // Action: Launch F1 setup procedure
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  EXPECT_FALSE(t.ready());

  // Action: F1 setup failure with time to wait received.
  unsigned     transaction_id = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  f1ap_message f1_setup_failure =
      generate_f1_setup_failure_message(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
  test_logger.info("Injecting F1SetupFailure with time to wait");
  f1c_gw.last_tx_f1ap_pdu = {};
  f1ap->handle_message(f1_setup_failure);

  // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
  for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::nulltype);

    this->tick();
  }

  // Status: CU received F1 Setup Request again.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  unsigned transaction_id2 = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  EXPECT_NE(transaction_id, transaction_id2);

  // Unsuccessful outcome after reinitiated F1 Setup
  f1_setup_failure = generate_f1_setup_failure_message(transaction_id2);
  test_logger.info("Injecting F1SetupFailure");
  f1ap->handle_message(f1_setup_failure);

  ASSERT_TRUE(t.ready());
  EXPECT_FALSE(t.get().success);
}

/// Test the f1 setup procedure
TEST_F(f1ap_du_test, when_retry_limit_reached_then_du_not_connected)
{
  // Action : Launch F1 setup procedure
  ASSERT_TRUE(f1ap->connect_to_cu_cp());
  f1_setup_request_message request_msg = generate_f1_setup_request_message();
  test_logger.info("Launch f1 setup request procedure...");
  async_task<f1_setup_response_message>         t = f1ap->handle_f1_setup_request(request_msg);
  lazy_task_launcher<f1_setup_response_message> t_launcher(t);

  // Status: CU received F1 Setup Request.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());

  for (unsigned i = 0; i < request_msg.max_setup_retries; i++) {
    // Status: F1 setup failure received.
    unsigned     transaction_id = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
    f1ap_message f1_setup_response_msg =
        generate_f1_setup_failure_message(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
    f1c_gw.last_tx_f1ap_pdu = {};
    f1ap->handle_message(f1_setup_response_msg);

    // Status: CU does not receive new F1 Setup Request until time-to-wait has ended.
    for (unsigned msec_elapsed = 0; msec_elapsed < 10000; ++msec_elapsed) {
      ASSERT_FALSE(t.ready());
      ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::nulltype);

      this->tick();
    }
  }

  // Status: F1 setup failure received.
  unsigned     transaction_id = get_transaction_id(f1c_gw.last_tx_f1ap_pdu.pdu).value();
  f1ap_message f1_setup_response_msg =
      generate_f1_setup_failure_message(transaction_id, asn1::f1ap::time_to_wait_opts::v10s);
  f1c_gw.last_tx_f1ap_pdu = {};
  f1ap->handle_message(f1_setup_response_msg);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::nulltype);
}
