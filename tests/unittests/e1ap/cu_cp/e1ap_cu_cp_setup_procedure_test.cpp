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
#include "e1ap_cu_cp_test_helpers.h"
#include "lib/e1ap/common/e1ap_asn1_utils.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/////////////////////////////////////
// CU-UP initiated E1 Setup Procedure
/////////////////////////////////////

/// Test the successful CU-UP initiated e1 setup procedure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_valid_then_connect_cu_up)
{
  // Action 1: Receive CuUpE1SetupRequest message
  test_logger.info("TEST: Receive CuUpE1SetupRequest message...");

  // Generate E1SetupRequest
  e1ap_message e1_setup_msg = generate_valid_cu_up_e1_setup_request();
  e1ap->handle_message(e1_setup_msg);

  // Action 2: Check if CuUpE1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "srsCU-UP");

  // Action 3: Transmit CuUpE1SetupResponse message
  test_logger.info("TEST: Transmit CuUpE1SetupResponse message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = true;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup response
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::options::gnb_cu_up_e1_setup_resp,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.successful_outcome().value.type());
}

/// Test the e1 setup failure
TEST_F(e1ap_cu_cp_test, when_received_cu_up_e1_setup_request_invalid_then_reject_cu_up)
{
  // Generate CuUpE1SetupRequest
  e1ap_message e1_setup_msg = generate_cu_up_e1_setup_request_base();
  e1ap->handle_message(e1_setup_msg);

  // Action 2 : Check if E1SetupRequest was forwarded to NGAP
  ASSERT_EQ(cu_up_processor_notifier.last_cu_up_e1_setup_request.gnb_cu_up_name.value(), "srsCU-UP");

  // Action 3: Transmit E1SetupFailure message
  test_logger.info("TEST: Transmit CuUpE1SetupFailure message...");
  cu_up_e1_setup_response msg = {};
  msg.success                 = false;
  msg.cause                   = cause_radio_network_t::unspecified;
  e1ap->handle_cu_up_e1_setup_response(msg);

  // Check the generated PDU is indeed the E1 Setup failure
  ASSERT_EQ(asn1::e1ap::e1ap_pdu_c::types_opts::options::unsuccessful_outcome,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.type());
  ASSERT_EQ(asn1::e1ap::e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::gnb_cu_up_e1_setup_fail,
            e1ap_pdu_notifier.last_e1ap_msg.pdu.unsuccessful_outcome().value.type());
}
