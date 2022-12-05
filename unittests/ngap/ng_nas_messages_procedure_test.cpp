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

/// Initial UE message tests
TEST_F(ngc_test, when_initial_ue_message_is_received_then_ngap_ue_is_created)
{
  ASSERT_EQ(ngc->get_nof_ues(), 0);

  // Inject UE creation at NGC
  ngc->create_ngc_ue(MIN_DU_INDEX, MIN_UE_INDEX, *rrc_ue_notifier, *rrc_ue_notifier);

  // generate and inject valid initial ue message
  ngap_initial_ue_message msg = generate_initial_ue_message();
  ngc->handle_initial_ue_message(msg);

  // check that initial UE message is sent to AMF and that UE objects has been created
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngc->get_nof_ues(), 1);
}

/// Test DL NAS transport handling
TEST_F(ngc_test, when_ue_present_dl_nas_transport_is_forwarded)
{
  // Inject UE creation at NGC
  ngc->create_ngc_ue(MIN_DU_INDEX, MIN_UE_INDEX, *rrc_ue_notifier, *rrc_ue_notifier);

  // Inject DL NAS transport message from AMF
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  // Check that RRC notifier was called
  ASSERT_EQ(rrc_ue_notifier->last_nas_pdu.length(), nas_pdu_len);
}

TEST_F(ngc_test, when_no_ue_present_dl_nas_transport_is_dropped)
{
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  // Check that no message has been sent to RRC
  ASSERT_EQ(rrc_ue_notifier->last_nas_pdu.length(), 0);
}

/// Test UL NAS transport handling
TEST_F(ngc_test, when_ue_present_and_amf_set_ul_nas_transport_is_forwared)
{
  ASSERT_EQ(ngc->get_nof_ues(), 0);

  // Inject UE creation at NGC
  ngc->create_ngc_ue(MIN_DU_INDEX, MIN_UE_INDEX, *rrc_ue_notifier, *rrc_ue_notifier);

  // generate and inject valid initial ue message to create UE context
  ngap_initial_ue_message msg = generate_initial_ue_message();
  ngc->handle_initial_ue_message(msg);

  // inject DL NAS transport to set AMF UE ID
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  ngap_ul_nas_transport_message ul_nas_transport = generate_ul_nas_transport_message();
  ngc->handle_ul_nas_transport_message(ul_nas_transport);

  // Check that AMF notifier was called with right type
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.init_msg().value.type(),
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ul_nas_transport);
}