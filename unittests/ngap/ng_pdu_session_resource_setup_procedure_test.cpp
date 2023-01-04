/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngc_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test Initial Context Setup Request
TEST_F(ngc_test, when_valid_pdu_session_resource_setup_request_received_then_pdu_session_setup_succeeds)
{
  ASSERT_EQ(ngc->get_nof_ues(), 0);

  // Inject UE creation at NGC
  ngc->create_ngc_ue(MIN_DU_INDEX, MIN_UE_INDEX, *rrc_ue_notifier, *rrc_ue_notifier, *du_processor_notifier);

  // generate and inject valid initial ue message to create UE context
  ngap_initial_ue_message msg = generate_initial_ue_message();
  ngc->handle_initial_ue_message(msg);

  // inject DL NAS transport to set AMF UE ID
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message();
  ngc->handle_ul_nas_transport_message(ul_nas_transport);

  // Inject Initial Context Setup Request
  ngc_message init_context_setup_request = generate_valid_initial_context_setup_request_message();
  ngc->handle_message(init_context_setup_request);

  // Inject PDU Session Resource Setup Request
  ngc_message pdu_session_resource_setup_request = generate_valid_pdu_session_resource_setup_request_message();
  ngc->handle_message(pdu_session_resource_setup_request);

  // Check conversion in adapter
  ASSERT_EQ(pdu_session_resource_setup_request.pdu.init_msg()
                .value.pdu_session_res_setup_request()
                ->pdu_session_res_setup_list_su_req.value.size(),
            du_processor_notifier->last_request.pdu_session_res_setup_items.size());

  ASSERT_EQ(du_processor_notifier->last_request.pdu_session_res_setup_items[0].pdu_session_type, "ipv4");

  // Check that AMF notifier was called with right type
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.successful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp);

  // Check that response contains PDU Session Resource Setup List
  ASSERT_TRUE(msg_notifier->last_ngc_msg.pdu.successful_outcome()
                  .value.pdu_session_res_setup_resp()
                  ->pdu_session_res_setup_list_su_res_present);
}

/// Test Initial Context Setup Request
TEST_F(ngc_test, when_invalid_pdu_session_resource_setup_request_received_then_pdu_session_setup_failed)
{
  ASSERT_EQ(ngc->get_nof_ues(), 0);

  // Inject UE creation at NGC
  ngc->create_ngc_ue(MIN_DU_INDEX, MIN_UE_INDEX, *rrc_ue_notifier, *rrc_ue_notifier, *du_processor_notifier);

  // generate and inject valid initial ue message to create UE context
  ngap_initial_ue_message msg = generate_initial_ue_message();
  ngc->handle_initial_ue_message(msg);

  // inject DL NAS transport to set AMF UE ID
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message();
  ngc->handle_ul_nas_transport_message(ul_nas_transport);

  // Inject Initial Context Setup Request
  ngc_message init_context_setup_request = generate_valid_initial_context_setup_request_message();
  ngc->handle_message(init_context_setup_request);

  // Inject invalid PDU Session Resource Setup Request
  ngc_message pdu_session_resource_setup_request = generate_invalid_pdu_session_resource_setup_request_message();
  ngc->handle_message(pdu_session_resource_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.successful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::pdu_session_res_setup_resp);

  // Check that response does not contains PDU Session Resource Setup List
  ASSERT_FALSE(msg_notifier->last_ngc_msg.pdu.successful_outcome()
                   .value.pdu_session_res_setup_resp()
                   ->pdu_session_res_setup_list_su_res_present);

  // Check that response contains PDU Session Resource Failed to Setup List
  ASSERT_TRUE(msg_notifier->last_ngc_msg.pdu.successful_outcome()
                  .value.pdu_session_res_setup_resp()
                  ->pdu_session_res_failed_to_setup_list_su_res_present);
}
