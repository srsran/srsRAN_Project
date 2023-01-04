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
TEST_F(ngc_test, when_valid_initial_context_setup_request_received_then_response_send)
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

  // Check that AMF notifier was called with right type
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.successful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::init_context_setup_resp);

  ASSERT_EQ(ngc->get_nof_ues(), 1);
}

/// Test Initial Context Setup Request
TEST_F(ngc_test, when_invalid_initial_context_setup_request_received_then_failure_send)
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
  ngc_message init_context_setup_request = generate_invalid_initial_context_setup_request_message();
  ngc->handle_message(init_context_setup_request);

  // Check that AMF notifier was called with right type
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::unsuccessful_outcome);
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.unsuccessful_outcome().value.type(),
            asn1::ngap::ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::init_context_setup_fail);

  // TODO: Activate this when UE context release is added
  // ASSERT_EQ(ngc->get_nof_ues(), 0);
}
