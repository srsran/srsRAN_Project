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
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/e1ap/common/e1ap_message.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_connectivity_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_connectivity_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, create_mock_amf()}) {}

  void run_ng_setup()
  {
    ngap_message ng_setup_resp = generate_ng_setup_response();
    get_amf().enqueue_next_tx_pdu(ng_setup_resp);
    EXPECT_TRUE(get_cu_cp().start());
  }
};

//----------------------------------------------------------------------------------//
// CU-CP to AMF connection handling                                                 //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_cu_cp_is_created_then_it_is_not_connected_to_amf)
{
  ngap_message ngap_pdu;
  ASSERT_FALSE(get_amf().try_pop_rx_pdu(ngap_pdu))
      << "The CU-CP should not send a message to the NG interface before being started";

  ASSERT_FALSE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}

TEST_F(cu_cp_connectivity_test, when_cu_cp_starts_then_it_initiates_ng_setup_procedure_and_blocks_waiting_for_response)
{
  // Enqueue AMF NG Setup Response as an auto-reply to CU-CP.
  ngap_message ng_setup_resp = generate_ng_setup_response();
  get_amf().enqueue_next_tx_pdu(ng_setup_resp);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_TRUE(get_cu_cp().start());

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_TRUE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}

TEST_F(cu_cp_connectivity_test, when_ng_setup_fails_then_cu_cp_is_not_in_amf_connected_state)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_fail = generate_ng_setup_failure();
  get_amf().enqueue_next_tx_pdu(ng_setup_fail);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_FALSE(get_cu_cp().start());

  ngap_message ngap_pdu;
  ASSERT_TRUE(get_amf().try_pop_rx_pdu(ngap_pdu)) << "CU-CP did not send the NG Setup Request to the AMF";
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::ng_setup_request))
      << "CU-CP did not setup the AMF connection";

  ASSERT_FALSE(get_cu_cp().get_cu_cp_ngap_connection_interface().amf_is_connected());
}

//----------------------------------------------------------------------------------//
// DU connection handling                                                           //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_new_f1_setup_request_is_received_and_ng_is_setup_then_f1_setup_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection between DU and CU-CP.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;

  // Send F1 Setup Request.
  get_du(du_idx).push_tx_pdu(generate_f1_setup_request());

  // Ensure the F1 Setup Response is received and correct.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}))
      << "F1 Setup Response was not received by the DU";
  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(f1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::successful_outcome_c::types_opts::f1_setup_resp);
}

TEST_F(cu_cp_connectivity_test, when_max_nof_dus_connected_reached_then_cu_cp_rejects_new_du_connections)
{
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_du();
    ASSERT_TRUE(ret.has_value());
  }

  auto ret = connect_new_du();
  ASSERT_FALSE(ret.has_value());
}

TEST_F(
    cu_cp_connectivity_test,
    when_max_nof_dus_connected_reached_and_du_connection_drops_then_du_is_removed_from_cu_cp_and_new_du_connection_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection and F1 Setup for max number of DUs.
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_du();
    ASSERT_TRUE(ret.has_value());
    unsigned du_idx = *ret;
    get_du(du_idx).push_tx_pdu(generate_f1_setup_request());
    f1ap_message f1ap_pdu;
    ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));
  }

  // Drop DU connection.
  ASSERT_TRUE(this->drop_du_connection(0));

  // A new DU can now be created.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
}

TEST_F(cu_cp_connectivity_test, when_ng_setup_is_not_successful_then_f1_setup_is_rejected)
{
  // Enqueue AMF NG Setup Response as an auto reply to CU-CP.
  ngap_message ng_setup_fail = generate_ng_setup_failure();
  get_amf().enqueue_next_tx_pdu(ng_setup_fail);

  // This call is blocking. When it returns, the CU-CP should have finished its attempt at AMF connection.
  ASSERT_FALSE(get_cu_cp().start());

  // Establish TNL connection between DU and CU-CP and start F1 setup procedure.
  auto ret = connect_new_du();
  ASSERT_TRUE(ret.has_value());
  unsigned du_idx = *ret;
  get_du(du_idx).push_tx_pdu(generate_f1_setup_request());
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000}));

  // The CU-CP should reject F1 setup.
  //  ASSERT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::unsuccessful_outcome);
  // TODO: Fix F1AP procedure handling.
}

//----------------------------------------------------------------------------------//
// CU-UP connection handling                                                        //
//----------------------------------------------------------------------------------//

TEST_F(cu_cp_connectivity_test, when_new_e1_setup_request_is_received_and_ng_is_setup_then_e1_setup_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection between CU-CP and CU-UP.
  auto ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
  unsigned cu_up_idx = *ret;

  // CU-UP sends E1 Setup Request.
  get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());

  // Ensure the E1 Setup Response is received and correct.
  e1ap_message e1ap_pdu;
  ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}))
      << "E1 Setup Response was not received by the CU-UP";
  ASSERT_EQ(e1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(e1ap_pdu.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp);
}

TEST_F(cu_cp_connectivity_test, when_max_nof_cu_ups_connected_reached_then_cu_cp_rejects_new_cu_up_connections)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection between CU-CP and CU-UP.
  for (unsigned i = 0; i != this->get_test_env_params().max_nof_cu_ups; ++i) {
    auto ret = connect_new_cu_up();
    ASSERT_TRUE(ret.has_value());
  }

  // The last one is rejected.
  auto ret = connect_new_cu_up();
  ASSERT_FALSE(ret.has_value());
}

TEST_F(
    cu_cp_connectivity_test,
    when_max_nof_cu_ups_connected_reached_and_cu_up_connection_drops_then_cu_up_is_removed_from_cu_cp_and_new_cu_up_connection_is_accepted)
{
  // Run NG setup to completion.
  run_ng_setup();

  // Establish TNL connection and E1 Setup for max number of CU-UPs.
  for (unsigned idx = 0; idx < this->get_test_env_params().max_nof_dus; idx++) {
    auto ret = connect_new_cu_up();
    ASSERT_TRUE(ret.has_value());
    unsigned cu_up_idx = *ret;
    get_cu_up(cu_up_idx).push_tx_pdu(generate_valid_cu_up_e1_setup_request());
    e1ap_message e1ap_pdu;
    ASSERT_TRUE(this->wait_for_e1ap_tx_pdu(cu_up_idx, e1ap_pdu, std::chrono::milliseconds{1000}));
  }

  // Drop DU connection.
  ASSERT_TRUE(this->drop_cu_up_connection(0));

  // A new CU-UP can now be created.
  auto ret = connect_new_cu_up();
  ASSERT_TRUE(ret.has_value());
}
