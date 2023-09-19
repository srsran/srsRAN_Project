/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "../common/test_helpers.h"
#include "e1ap_cu_up_test_helpers.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_up;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of unsupported messages                                                 */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_unsupported_init_msg_received_then_message_ignored)
{
  // Set last message of PDU notifier to successful outcome
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_init_msg();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still set_init_msg)
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_successful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_successful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

TEST_F(e1ap_cu_up_test, when_unsupported_unsuccessful_outcome_received_then_message_ignored)
{
  // Set last message of PDU notifier to init_msg
  e1ap_gw.last_tx_e1ap_pdu.pdu.set_init_msg();

  // Generate unupported E1AP PDU
  e1ap_message unsupported_msg = {};
  unsupported_msg.pdu.set_unsuccessful_outcome();

  e1ap->handle_message(unsupported_msg);

  // Check that PDU has not been forwarded (last PDU is still init_msg)
  EXPECT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of E1 Setup                                                             */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the successful CU-UP initiated E1 setup
TEST_F(e1ap_cu_up_test, when_cu_up_started_then_e1_setup_request_sent)
{
  run_e1_setup_procedure();

  // Check the generated PDU is indeed the E1 Setup Request
  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.init_msg().value.type(),
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::options::gnb_cu_up_e1_setup_request);
}

//////////////////////////////////////////////////////////////////////////////////////
/* Handling of Bearer Context Messages                                              */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_setup_received_then_bearer_context_setup_response_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_bearer_context_setup_request(9);

  e1ap->handle_message(bearer_context_setup);

  // Check if BearerContextSetupRequest was forwarded to UE manager
  ASSERT_EQ(cu_up_notifier.last_bearer_context_setup_request.serving_plmn, "20899"); // this is the human readable PLMN

  // Check the generated PDU is indeed the Bearer Context Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_setup_resp,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());

  ASSERT_EQ(e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome()
                .value.bearer_context_setup_resp()
                ->sys_bearer_context_setup_resp.ng_ran_bearer_context_setup_resp()
                .pdu_session_res_setup_list[0]
                .drb_setup_list_ng_ran.size(),
            1);
}

TEST_F(e1ap_cu_up_test, when_invalid_bearer_context_setup_received_then_bearer_context_setup_failure_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_invalid_bearer_context_setup_request(9);

  e1ap->handle_message(bearer_context_setup);

  // Check the generated PDU is indeed the Bearer Context Setup Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_setup_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_invalid_bearer_context_setup_inactivity_timer_received_then_bearer_context_setup_failure_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextSetupRequest message...");

  // Generate BearerContextSetupRequest
  e1ap_message bearer_context_setup = generate_invalid_bearer_context_setup_request_inactivity_timer(9);

  e1ap->handle_message(bearer_context_setup);

  // Check the generated PDU is indeed the Bearer Context Setup Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_setup_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_valid_bearer_context_modification_received_then_bearer_context_modification_response_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Generate BearerContextModificationRequest
  e1ap_message bearer_context_modification = generate_bearer_context_modification_request(9, 0);

  e1ap->handle_message(bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_mod_resp,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test,
       when_invalid_bearer_context_modification_received_then_bearer_context_modification_failure_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextModificationRequest message
  test_logger.info("TEST: Receive BearerContextModificationRequest message...");

  // Generate BearerContextModificationRequest
  e1ap_message bearer_context_modification = generate_invalid_bearer_context_modification_request(9, 0);

  e1ap->handle_message(bearer_context_modification);

  // Check the generated PDU is indeed the Bearer Context Modification Failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::options::bearer_context_mod_fail,
            e1ap_gw.last_tx_e1ap_pdu.pdu.unsuccessful_outcome().value.type());
}

TEST_F(e1ap_cu_up_test, when_valid_bearer_context_release_command_received_then_bearer_context_release_complete_is_sent)
{
  // Connect CU-CP
  run_e1_setup_procedure();

  // Test preamble
  this->setup_bearer(9);

  // Receive BearerContextSetupRequest message
  test_logger.info("TEST: Receive BearerContextReleaseCommand message...");

  // Generate BearerContextReleaseCommand
  e1ap_message bearer_context_release_cmd = generate_bearer_context_release_command(9, 0);

  e1ap->handle_message(bearer_context_release_cmd);

  // Check the generated PDU is indeed the Bearer Context Modification Response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome, e1ap_gw.last_tx_e1ap_pdu.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::bearer_context_release_complete,
            e1ap_gw.last_tx_e1ap_pdu.pdu.successful_outcome().value.type());
}
