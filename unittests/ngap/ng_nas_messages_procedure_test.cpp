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

  // generate and inject valid initial ue message
  ngap_initial_ue_message msg = generate_initial_ue_message();
  ngc->handle_initial_ue_message(msg);

  // check that initial UE message is sent to AMF and that UE objects has been created
  ASSERT_EQ(msg_notifier->last_ngc_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(ngc->get_nof_ues(), 1);
}

/// Test DL NAS transport handling
TEST_F(ngc_test, when_no_ue_present_dl_nas_transport_is_dropped)
{
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  // TODO: how to check dropped message?
}

TEST_F(ngc_test, when_ue_present_dl_nas_transport_is_forwarded)
{
  ngc_message dl_nas_transport = generate_downlink_nas_transport_message();
  ngc->handle_message(dl_nas_transport);

  // TODO: add check for RRC notifier called
}
