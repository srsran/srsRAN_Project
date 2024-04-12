/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../common/test_helpers.h"
#include "e1ap_cu_cp_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_cp_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome
  e1ap_pdu_notifier.last_e1ap_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_cp_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1ap_pdu_notifier.last_e1ap_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_cp_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1ap_pdu_notifier.last_e1ap_msg.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  ASSERT_EQ(e1ap_pdu_notifier.last_e1ap_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Inactivity Notification                                                          */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_cp_test, when_valid_inactivity_message_received_then_message_is_forwarded_to_cu_cp)
{
  // Test Preamble.
  auto& ue = create_ue();

  // Generate Bearer Context Inactivity Notification with UE activity level
  e1ap_message inactivity_notification = generate_bearer_context_inactivity_notification_with_ue_level(
      ue.cu_cp_ue_e1ap_id.value(), ue.cu_up_ue_e1ap_id.value());

  e1ap->handle_message(inactivity_notification);

  // Check that PDU has been forwarded to CU-CP
  ASSERT_TRUE(cu_cp_notifier.last_msg.ue_inactive);
}

TEST_F(e1ap_cu_cp_test, when_invalid_inactivity_message_received_then_message_is_not_forwarded_to_cu_cp)
{
  // Test Preamble.
  auto& ue = create_ue();

  // Generate Invalid Bearer Context Inactivity Notification
  e1ap_message inactivity_notification =
      generate_invalid_bearer_context_inactivity_notification(ue.cu_cp_ue_e1ap_id.value(), ue.cu_up_ue_e1ap_id.value());

  e1ap->handle_message(inactivity_notification);

  // Check that PDU has not been forwarded to CU-CP
  ASSERT_FALSE(cu_cp_notifier.last_msg.ue_inactive);
}