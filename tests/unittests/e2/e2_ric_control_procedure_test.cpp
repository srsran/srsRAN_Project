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
  ASSERT_EQ(ack->ran_function_id, 3);
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
  ASSERT_EQ(ack->ran_function_id, 3);
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
  ASSERT_EQ(ack->ran_function_id, 3);
}

TEST_F(e2_test_setup, ric_control_procedure_packed)
{
  uint8_t e2ap_ctrl_req[] = {
      0x00, 0x04, 0x00, 0x76, 0x00, 0x00, 0x05, 0x00, 0x1d, 0x00, 0x05, 0x00, 0x03, 0xfd, 0x00, 0x00, 0x00, 0x05,
      0x00, 0x02, 0x00, 0x03, 0x00, 0x16, 0x00, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0x10, 0x00,
      0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x05, 0x00, 0x17, 0x00, 0x46, 0x45, 0x00, 0x00, 0x01, 0x00, 0x00, 0x60,
      0x00, 0x00, 0x40, 0x00, 0x03, 0x00, 0x02, 0x44, 0x00, 0x00, 0x00, 0x04, 0x60, 0x00, 0x00, 0x40, 0x00, 0x01,
      0x00, 0x06, 0x2a, 0x00, 0x05, 0x30, 0x30, 0x31, 0x30, 0x31, 0x00, 0x07, 0x44, 0x00, 0x01, 0x00, 0x08, 0x2a,
      0x00, 0x01, 0x31, 0x00, 0x09, 0x2a, 0x00, 0x01, 0x30, 0x00, 0x0a, 0x28, 0x80, 0x01, 0x05, 0x00, 0x0b, 0x28,
      0x80, 0x01, 0x19, 0x00, 0x0c, 0x28, 0x80, 0x01, 0x64, 0x00, 0x15, 0x00, 0x01, 0x40};
  byte_buffer e2ap_ctrl_req_buf = byte_buffer::create(e2ap_ctrl_req, e2ap_ctrl_req + sizeof(e2ap_ctrl_req)).value();
  packer->handle_packed_pdu(std::move(e2ap_ctrl_req_buf));

  asn1::cbit_ref bref(gw->last_pdu);
  e2_message     msg = {};
  if (msg.pdu.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Couldn't unpack E2 PDU");
  }
  ASSERT_EQ(msg.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::successful_outcome);
}