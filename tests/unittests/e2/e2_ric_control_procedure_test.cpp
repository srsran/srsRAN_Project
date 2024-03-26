/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST_F(e2_test_setup, ric_control_procedure_setup)
{
  e2_message request_msg = generate_ric_control_request_style2_action6(test_logger);
  e2->handle_message(request_msg);
  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_ctrl_ack);
  auto ack = msg.pdu.successful_outcome().value.ric_ctrl_ack();
  ASSERT_EQ(ack->ran_function_id, 1);
}

TEST_F(e2_test_setup, ric_control_procedure_setup2)
{
  e2_message request_msg = generate_ric_control_request(test_logger, 2, 6, 12, 12);
  e2->handle_message(request_msg);
  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg.pdu.successful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::successful_outcome_c::types_opts::ric_ctrl_ack);
  auto ack = msg.pdu.successful_outcome().value.ric_ctrl_ack();
  ASSERT_EQ(ack->ran_function_id, 1);
}

TEST_F(e2_test_setup, ric_control_procedure_fail)
{
  e2_message request_msg = generate_ric_control_request(test_logger, 2, 6, 1, 12);
  e2->handle_message(request_msg);
  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg.pdu.unsuccessful_outcome().value.type(),
            asn1::e2ap::e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::ric_ctrl_fail);
  auto ack = msg.pdu.unsuccessful_outcome().value.ric_ctrl_fail();
  ASSERT_EQ(ack->ran_function_id, 1);
}
