/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_environment.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_connectivity_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_connectivity_test() : cu_cp_test_environment(cu_cp_test_env_params{create_manual_amf_stub()}) {}
};

TEST_F(cu_cp_connectivity_test, when_cu_cp_is_created_then_it_is_not_connected_to_any_other_node)
{
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf().try_pop_rx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  e1ap_message e1ap_pdu;
  ASSERT_FALSE(try_pop_e1ap_tx_pdu(e1ap_pdu))
      << "The CU-CP should not send a message to the E1 interface before being started";

  ASSERT_FALSE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}

TEST_F(cu_cp_connectivity_test, when_cu_cp_starts_then_it_initiates_ng_setup_procedure_and_waits_for_response)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_resp = generate_ng_setup_response();
  get_amf().enqueue_next_tx_pdu(ng_setup_resp);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  get_cu_cp().start();

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}

TEST_F(cu_cp_connectivity_test, when_ng_setup_fails_then_cu_cp_is_not_amf_connected_state)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_fail = generate_ng_setup_failure();
  get_amf().enqueue_next_tx_pdu(ng_setup_fail);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  get_cu_cp().start();

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_FALSE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}
