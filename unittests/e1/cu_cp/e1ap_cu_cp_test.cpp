/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e1ap_cu_cp_test_helpers.h"
#include "unittests/e1/common/test_helpers.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp_factory.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_cp_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome
  e1_pdu_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(e1_pdu_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_cp_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1_pdu_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(e1_pdu_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_cp_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1_pdu_notifier.last_e1_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(e1_pdu_notifier.last_e1_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}
