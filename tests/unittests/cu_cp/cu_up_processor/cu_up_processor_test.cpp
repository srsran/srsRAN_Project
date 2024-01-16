/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cu_up_processor_test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/e1ap/cu_up/e1ap_cu_up_test_helpers.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::e1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* E1 setup                                                                         */
//////////////////////////////////////////////////////////////////////////////////////

/// Test the cu-up processor start
TEST_F(cu_up_processor_test, when_valid_e1_setup_received_then_e1_setup_response_sent)
{
  // Generate valid CuUpE1SetupRequest
  cu_up_e1_setup_request e1_setup_request = srsran::srs_cu_up::generate_cu_up_e1_setup_request();

  // Pass message to CU-UP processor
  cu_up_processor_obj->handle_cu_up_e1_setup_request(e1_setup_request);

  // check that CU-UP E1 setup response was sent
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(e1ap_notifier.last_msg.pdu.successful_outcome().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::successful_outcome_c::types_opts::gnb_cu_up_e1_setup_resp);
}
