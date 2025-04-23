/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

/// Test the initial e2ap setup procedure with own task worker
TEST_F(e2_entity_test, on_start_send_e2ap_setup_request)
{
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2agent->start();

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  e2_setup_response.pdu.successful_outcome()
      .value.e2setup_resp()
      ->ran_functions_accepted[0]
      ->ran_function_id_item()
      .ran_function_id = e2sm_kpm_asn1_packer::ran_func_id;
  test_logger.info("Injecting E2SetupResponse");
  e2agent->get_e2_interface().handle_message(e2_setup_response);
  e2agent->stop();
}

/// Test successful E2 setup procedure
TEST_F(e2_test, when_e2_setup_response_received_then_e2_connected)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  e2_message request_msg = generate_e2_setup_request_message("1.3.6.1.4.1.53148.1.2.2.2");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message request;
  request.request                                 = request_msg.pdu.init_msg().value.e2setup_request();
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

TEST_F(e2_test, when_e2_setup_failure_received_then_e2_setup_failed)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  e2_message request_msg = generate_e2_setup_request_message("1.3.6.1.4.1.53148.1.2.2.2");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message request;
  request.request                                 = request_msg.pdu.init_msg().value.e2setup_request();
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_failure(transaction_id);
  test_logger.info("Injecting E2SetupFailure");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_FALSE(t.get().success);
}

TEST_F(e2_test_setup, e2_sends_correct_kpm_ran_function_definition)
{
  using namespace asn1::e2sm;
  using namespace asn1::e2ap;
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  e2_message request_msg = generate_e2_setup_request_message("1.3.6.1.4.1.53148.1.2.2.2");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message request;
  request.request                                 = request_msg.pdu.init_msg().value.e2setup_request();
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  ran_function_item_s& ran_func_added1 = e2_client->last_tx_e2_pdu.pdu.init_msg()
                                             .value.e2setup_request()
                                             ->ran_functions_added[0]
                                             .value()
                                             .ran_function_item();
  asn1::cbit_ref                                  bref1(ran_func_added1.ran_function_definition);
  asn1::e2sm::e2sm_kpm_ran_function_description_s ran_func_def = {};
  if (ran_func_def.unpack(bref1) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  // check contents of E2SM-KPM-RANfunction-Description
  ASSERT_EQ(ran_func_def.ran_function_name.ran_function_short_name.to_string(), "ORAN-E2SM-KPM");
  ric_report_style_item_s& ric_report_style = ran_func_def.ric_report_style_list[2];
  ASSERT_EQ(ric_report_style.ric_report_style_type, 3);
  meas_info_action_item_s& meas_cond_it = ric_report_style.meas_info_action_list[2];
  ASSERT_EQ(meas_cond_it.meas_name.to_string(), "RSRQ");

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

TEST_F(e2_test_setup, e2_sends_correct_rc_ran_function_definition)
{
  using namespace asn1::e2sm;
  using namespace asn1::e2ap;
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  e2_message request_msg = generate_e2_setup_request_message("1.3.6.1.4.1.53148.1.1.2.3");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message request;
  request.request                                 = request_msg.pdu.init_msg().value.e2setup_request();
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  ran_function_item_s& ran_func_added1 = e2_client->last_tx_e2_pdu.pdu.init_msg()
                                             .value.e2setup_request()
                                             ->ran_functions_added[0]
                                             .value()
                                             .ran_function_item();
  asn1::cbit_ref bref1(ran_func_added1.ran_function_definition);

  asn1::e2sm::e2sm_rc_ran_function_definition_s ran_func_def = {};
  if (ran_func_def.unpack(bref1) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }

  ASSERT_EQ(ran_func_def.ran_function_name.ran_function_short_name.to_string(), "ORAN-E2SM-RC");

  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0].ric_ctrl_style_type, 2);
  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0].ric_ctrl_style_name.to_string(),
            "Radio Resource Allocation Control");
  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0].ric_ctrl_action_list[0].ric_ctrl_action_id,
            6);
  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0]
                .ric_ctrl_action_list[0]
                .ric_ctrl_action_name.to_string(),
            "Slice-level PRB quota");
  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0]
                .ric_ctrl_action_list[0]
                .ran_ctrl_action_params_list[9]
                .ran_param_id,
            11);
  ASSERT_EQ(ran_func_def.ran_function_definition_ctrl.ric_ctrl_style_list[0]
                .ric_ctrl_action_list[0]
                .ran_ctrl_action_params_list[9]
                .ran_param_name.to_string(),
            "Min PRB Policy Ratio");

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  unsigned   transaction_id    = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message e2_setup_response = generate_e2_setup_response(transaction_id);
  test_logger.info("Injecting E2SetupResponse");
  e2->handle_message(e2_setup_response);

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}

TEST_F(e2_test, correctly_unpack_e2_response)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");
  // Action 1: Launch E2 setup procedure
  e2_message request_msg = generate_e2_setup_request_message("1.3.6.1.4.1.53148.1.2.2.2");
  test_logger.info("Launch e2 setup request procedure...");
  e2_setup_request_message request;
  request.request                                 = request_msg.pdu.init_msg().value.e2setup_request();
  async_task<e2_setup_response_message>         t = e2->handle_e2_setup_request(request);
  lazy_task_launcher<e2_setup_response_message> t_launcher(t);

  // Status: received E2 Setup Request.
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2setup_request);

  // Status: Procedure not yet ready.
  ASSERT_FALSE(t.ready());
  // Action 2: E2 setup response received.
  uint8_t     e2_resp[]   = {0x20, 0x01, 0x00, 0x38, 0x00, 0x00, 0x04, 0x00, 0x31, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04,
                             0x00, 0x07, 0x00, 0x00, 0xf1, 0x10, 0x00, 0x01, 0x90, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x00,
                             0x06, 0x40, 0x05, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x34, 0x00, 0x12, 0x00, 0x00, 0x00,
                             0x35, 0x00, 0x0c, 0x00, 0x00, 0xe0, 0x6e, 0x67, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x66, 0x00};
  byte_buffer e2_resp_buf = byte_buffer::create(e2_resp, e2_resp + sizeof(e2_resp)).value();
  packer->handle_packed_pdu(std::move(e2_resp_buf));

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}
