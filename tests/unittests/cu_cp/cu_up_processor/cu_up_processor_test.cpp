/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_processor_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* E1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the cu-up processor start
TEST_F(cu_up_processor_test, when_cu_up_processor_is_started_then_e1_setup_request_is_sent)
{
  // Start CU-UP processor object
  cu_up_processor_obj->start();

  // check that CU-CP E1 setup request has been sent
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // inject CU-CP E1 setup response
  e1ap_message e1ap_msg = generate_cu_cp_e1_setup_respose(0);
  cu_up_processor_obj->get_e1ap_message_handler().handle_message(e1ap_msg);

  // check that CU-UP has been added
  ASSERT_EQ(cu_up_processor_obj->get_context().id, 1);
  ASSERT_EQ(cu_up_processor_obj->get_context().cu_up_name, "srsCU-UP");
}

/// Test the cu-up processor start
TEST_F(cu_up_processor_test, when_cu_cp_e1_setup_response_is_received_then_context_is_updated)
{
  // Start CU-UP processor object
  cu_up_processor_obj->start();

  // check that CU-CP E1 setup request has been sent
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::gnb_cu_cp_e1_setup_request);

  // inject CU-CP E1 setup response
  e1ap_message e1ap_msg = generate_cu_cp_e1_setup_respose(0);
  cu_up_processor_obj->get_e1ap_message_handler().handle_message(e1ap_msg);

  // check that CU-UP has been added and context has been updated
  ASSERT_EQ(cu_up_processor_obj->get_context().id, 1);
  ASSERT_EQ(cu_up_processor_obj->get_context().cu_up_name, "srsCU-UP");
}
